#pragma once

#include "molecule.h"
#include "cell.h"

#include <vector>   // std::vector
#include <fstream>  // std::ifstream

/*
* The MoleculeSystem class represents a collection of molecules in 3D space, and consists
* of a vector of molecules and a grid which groups the molecules into cells.
*/
class MoleculeSystem{
    private:
        double a{};                       // side length of box
        int N{};                          // number of molecules
        std::vector<Molecule> molecules;  // vector of molecules
        int n_cells{};                    // n_cells x n_cells x n_cells grid
        std::vector<std::vector<std::vector<Cell>>> grid;  // grid of cells
    
    public:
        MoleculeSystem(double a);

        // getters
        int get_N() const { return N; };
        const std::vector<Molecule>& get_molecules() const { return molecules; }

        // reading in data
        void read_position_stream(std::ifstream& molecule_stream);
        void read_velocity_stream(std::ifstream& velocity_stream);

        // system energy calculations
        void system_energy(bool pure_output);
        void build_grid();
        double grid_potential();
        double cell_potential(int i, int j, int k) const;
        double intra_potential(int i, int j, int k) const;
        double potential_energy() const;
        double kinetic_energy() const;
        double truncated_LJ_potential(double r2) const;
        double rij2(const double* qi, const double* qj) const;

        // heuristics for choosing linked-cell data structure
        bool heuristic_structure_selection() const;
    };
