#include "molecule_system.h"

#include <iostream>   // std::cout
#include <string>     // std::string, std::getline
#include <fstream>    // std::ifstream
#include <vector>     // std::vector
#include <cmath>      // std::pow
#include <stdexcept>  // std::invalid_argument
#include <chrono>     // time measurement
#include <algorithm>  // std::max, std::min

// compile time constants
namespace{
    constexpr double CUTOFF = 2.5;
    constexpr double CUTOFF_SQUARED = CUTOFF * CUTOFF;
    constexpr double LJ_SHIFT = 0.01631689114;
}

/*
* Initializer: calculate the number of cells needed for the grid.
*/
MoleculeSystem::MoleculeSystem(double a) : a(a) {
    // number of cells along 1 axis
    this->n_cells = std::floor(this->a / CUTOFF);
}

/*
* Read in the number of molecules (N), and sequence of molecule positions storing 
* the values in the molecules vector.
*/
void MoleculeSystem::read_position_stream(std::ifstream& molecule_stream) {
    // read in the number of molecules
    int N_molecules = 0;
    molecule_stream >> N_molecules;

    // make sure the number of molecules is valid
    if (N_molecules <= 0) {
        throw std::invalid_argument("Number of molecules invalid (must be greater than 0)");
    }
    this->N = N_molecules;

    // resize the molecules vector to N
    molecules.resize(N_molecules);

    std::string skip;  // skip rest of line
    std::getline(molecule_stream, skip);
    std::string line;  // skip next line
    std::getline(molecule_stream, line);

    // set ids and read in position
    for (int i = 0; i < N_molecules; ++i) {
        this->molecules[i].set_id(i);
        this->molecules[i].read_position(molecule_stream);

        // validate position
        const double* qi = this->molecules[i].get_position();
        double max = std::max(qi[0], std::max(qi[1], qi[2]));
        double min = std::min(qi[0], std::min(qi[1], qi[2]));
        if (max > this->a || min < 0) {
            throw std::invalid_argument("Molecule with coordinates outside box boundaries detected. Aborting.");
        }
    }
}

/*
* Read in a sequence of molecule velocities storing the values in the molecules vector.
*/
void MoleculeSystem::read_velocity_stream(std::ifstream& velocity_stream) {
    // assert that number of velocities and positions match
    int N_velocities = 0;
    velocity_stream >> N_velocities;
    if (N_velocities != this->N) {
        throw std::invalid_argument("The number of positions does not match the number of velocities");
    }

    std::string skip;  // skip rest of line
    std::getline(velocity_stream, skip);
    std::string line;  // skip next line
    std::getline(velocity_stream, line);
    for (int i = 0; i < this->N; ++i) {  // read in velocities
        molecules[i].read_velocity(velocity_stream);
    }
}

/*
* Compute and time the calculations, and print out the results to the terminal.
*/
void MoleculeSystem::system_energy(bool pure_output) {
    double Ek = this->kinetic_energy();

    // time direct iteration method
    auto t0 = std::chrono::high_resolution_clock::now();
    double Ep_iter = this->potential_energy();
    auto t1 = std::chrono::high_resolution_clock::now();
    long long dt_iter = std::chrono::duration_cast<std::chrono::nanoseconds>(t1-t0).count();

    // time grid method
    double Ep_grid = this->grid_potential();
    auto t2 = std::chrono::high_resolution_clock::now();
    long long dt_grid = std::chrono::duration_cast<std::chrono::nanoseconds>(t2-t1).count();

    std::string speedup = std::to_string(static_cast<double>(dt_iter) / dt_grid);

    // if --pure_output flag is given, print a simple output
    if (pure_output) {
        // <Ep_iter> <Ep_grid> <dt_iter> <dt_grid> <speedup>
        std::cout << Ep_iter << " " << Ep_grid << " " << dt_iter * 1e-6 << " " 
                  << dt_grid * 1e-6 << " " << speedup << "\n";
    } else {
        // recommended data structure based on box size and density
        std::string recommended = "iteration over all molecules";
        bool use_linked_cell = this->heuristic_structure_selection();
        if (use_linked_cell) recommended = "linked-cell data structure";

        // if n_cells < 3, tell the user that linked-cell data structure defaulted back to iteration
        std::string warning = "";
        std::string method = "linked-cell data structure";
        if (this->n_cells < 3) {
            warning = "WARNING: num. cells < 3, linked-cell defaulting back to iteration\n";
            speedup = "not avaiable";
            method = "defaulted back to iteration";
        }

        // print results
        std::string line = "-----------------------------------------------------\n";
        std::string bar  = "#####################################################\n";
        std::cout << bar <<"Linked cell " << this->n_cells << " x " << this->n_cells << " x " << this->n_cells
                << " grid set up (cell length = " << this->a / this->n_cells << ")\n" << warning
                << line << "E_pot = " << Ep_iter << " | time: " << dt_iter * 1e-6 << "ms"
                << " | method: iteration over all molecules\n"
                << "E_pot = " << Ep_grid << " | time: " << dt_grid * 1e-6 << "ms"
                << " | method: " << method << "\n"
                << line <<"Recommended data structure: " << recommended << "\n"
                << "Linked cell speedup factor: " << speedup << "\n"
                << line <<"E_kin = " << Ek << "\n"
                << "E_tot = " << Ep_iter + Ek << "\n" << bar;
    }
}

/*
* Set up the linked cell data structure.
*/
void MoleculeSystem::build_grid() {
    // resize grid to n_cells x n_cells x n_cells
    this->grid.resize(this->n_cells, std::vector<std::vector<Cell>>(
                      this->n_cells, std::vector<Cell>(
                      this->n_cells)));
    
    // fill up grid with molecules
    double cell_length = this->a / this->n_cells;
    for (int id = 0; id < N; ++id) {
        const Molecule& molecule = molecules[id];

        // calculate cell index i, j, k
        const double* q = molecules[id].get_position();
        int i = std::floor(q[0] / cell_length); 
        int j = std::floor(q[1] / cell_length);
        int k = std::floor(q[2] / cell_length);
        this->grid[i][j][k].add_molecule(id);
    }
}

/*
* Calculate the potential energy of the system using a grid of cells,
* known as a linked cell data structure, to speed up calculations for large systems.
*/
double MoleculeSystem::grid_potential() {
    // don't allow grids of size 2 or smaller
    if (this->n_cells < 3) return this->potential_energy();

    // set up the cell grid
    this->build_grid();

    // add up the potential of each cell in the grid
    double E_pot = 0;
    for (int i = 0; i < this->n_cells; ++i) {
        for (int j = 0; j < this->n_cells; ++j) {
            for (int k = 0; k < this->n_cells; ++k) {
                E_pot += this->cell_potential(i, j, k);
            }
        }
    }
    return E_pot;
}

/*
* Calculate the potential the cell (i, j, k)
*/
double MoleculeSystem::cell_potential(int i, int j, int k) const {   
    // compute the potential within the cell
    double E_pot = this->intra_potential(i, j, k);

    // array of (di, dj, dk) index differences for the 13 "right" neighbors
    int indices[3][13] = {
        { 1,  1,  1,  1,  1,  1,  1,  1,  1,  0,  0,  0,  0},
        {-1, -1, -1,  0,  0,  0,  1,  1,  1, -1,  0,  1, -1},
        {-1,  0,  1, -1,  0,  1, -1,  0,  1,  1,  1,  1,  0}
    };

    // go through the 13 neighbors "to the right"
    for (int ids = 0; ids < 13; ++ids) {
        int ni = (i + indices[0][ids] + this->n_cells) % this->n_cells;
        int nj = (j + indices[1][ids] + this->n_cells) % this->n_cells;
        int nk = (k + indices[2][ids] + this->n_cells) % this->n_cells;
        
        // go through cell (i, j, k)s molecules
        for (int id : this->grid[i][j][k].get_molecules()) {
            const double* q = this->molecules[id].get_position();

            // go through neighbor cell (ni, nj, nk)s molecules
            for (int id_ngh : this->grid[ni][nj][nk].get_molecules()) {
                const double* q_ngh = this->molecules[id_ngh].get_position();
                double rij2 = this->rij2(q, q_ngh);
                E_pot += this->truncated_LJ_potential(rij2);
            }
        }
    }
    return E_pot;
}

/*
* Calculate the potential energy within a cell.
*/
double MoleculeSystem::intra_potential(int i, int j, int k) const {
    double E_pot = 0;
    const Cell& cell = this->grid[i][j][k];
    int n_molecules = cell.get_n_molecules();

    // go through each molecule
    for (int i = 0; i < n_molecules; ++i) {
        int molecule_i_id = cell.get_molecules()[i];
        const double* qi = this->molecules[molecule_i_id].get_position();

        // go through all the other molecules
        for (int j = i + 1; j < n_molecules; ++j) {
            int molecule_j_id = cell.get_molecules()[j];
            const double* qj = this->molecules[molecule_j_id].get_position();

            // compute the distance and the potential
            double rij2 = this->rij2(qi, qj);
            E_pot += this->truncated_LJ_potential(rij2);
        }
    }
    return E_pot;
}

/*
* Compute the potential energy of the system by summing the truncated shifted
* Lennard-Jones potentials over all the molecule pairs.
*/
double MoleculeSystem::potential_energy() const {
    double E_pot = 0;
    for (int i = 0; i < N; ++i) {
        const double* qi = molecules[i].get_position();
        for (int j = i + 1; j < N; ++j) {
            const double* qj = molecules[j].get_position();
            double rij2 = this->rij2(qi, qj);
            E_pot += truncated_LJ_potential(rij2);
        }
    }
    return E_pot;
}

/*
* Compute the kinetic energy of the system (sum over all the molecules).
*/
double MoleculeSystem::kinetic_energy() const {
    double E_kin = 0;
    for (int i = 0; i < N; ++i) {
        const double* v = molecules[i].get_velocity();
        double norm2 = v[0] * v[0] + v[1] * v[1] + v[2] * v[2];
        E_kin += norm2 / 2;
    }
    return E_kin;
}

/*
* Compute the truncated-shifted Lennard-Jones potential between a pair of molecules
* given the squared distance r2 between them.
*/
double MoleculeSystem::truncated_LJ_potential(double r2) const {
    if (r2 >= CUTOFF_SQUARED) return 0;
    double inv_r2 = 1.0 / r2;
    double inv_r6 = inv_r2 * inv_r2 * inv_r2;
    return 4 * (inv_r6 * inv_r6 - inv_r6) + LJ_SHIFT;
}

/*
* Compute the squared distance, assuming periodic boundary conditions, 
* between two molecules, given their position arrays.
*/
double MoleculeSystem::rij2(const double* qi, const double* qj) const {
    double dx = std::remainder(qi[0] - qj[0], a);
    double dy = std::remainder(qi[1] - qj[1], a);
    double dz = std::remainder(qi[2] - qj[2], a);
    return dx * dx + dy * dy + dz * dz;
}

/*
* Applies heuristic-based selection criteria to determine whether 
* to use a cell-linked data structure (true) or to iterate over all the molecules (false).
*/
bool MoleculeSystem::heuristic_structure_selection() const {
    // n x n x n grid where n < 3 is not supported
    if (this->n_cells < 3) return false;

    // criteria based on empirical data
    double rho = this->N / (this->a * this->a * this->a);
    if (this->N == 1) return false;
    if (8 <= this->a && this->a < 13) {
        double f1 = 0.0018 * this->a - 0.0114;
        if (rho <= f1) return false;
    }
    if (13 <= this->a && this->a < 21) {
        double f2 = -0.00063 * this->a + 0.0201;
        if (rho <= f2) return false;
    }
    if (21 <= this->a && this->a < 43) {
        double f3 = -0.0002 * this->a + 0.011;
        if (rho <= f3) return false;
    }
    return true;
}