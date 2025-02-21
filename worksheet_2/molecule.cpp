#include "molecule.h"  // Molecule, MoleculeSystem

#include <iostream>   // std::cout, std::endl
#include <string>     // std::string, std::getline
#include <fstream>    // std::ifstream
#include <vector>     // std::vector
#include <cmath>      // std::pow
#include <stdexcept>  // std::invalid_argument

/*
* Read in (x, y, z) coordiantes from a stream and store them in the array q.
*/
void Molecule::read_position(std::ifstream& stream) {
    std::string type{};
    stream >> type >> this->q[0] >> this->q[1] >> this->q[2];
    std::string skip;
    std::getline(stream, skip);
}

/*
* Read in (vx, vy, vz) velocities from a stream and store them in the array v.
*/
void Molecule::read_velocity(std::ifstream& stream) {
    std::string type{};
    stream >> type >> this->v[0] >> this->v[1] >> this->v[2];
    std::string skip;
    std::getline(stream, skip);
}

/*
* Print out the molecules id, its position, and its velocity.
*/
void Molecule::print_molecule() const {
    std::cout << "Molecule[id=" << id << ", ";
    std::cout << "q=(" << q[0] << ", " << q[1] << ", " << q[2] << "), ";
    std::cout << "v=(" << v[0] << ", " << v[1] << ", " << v[2] << ")]" << std::endl;
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

    // skip empty line
    std::string skip;
    std::getline(molecule_stream, skip);

    // set ids and read in position
    for (int i = 0; i < N_molecules; ++i) {
        molecules[i].set_id(i);
        molecules[i].read_position(molecule_stream);
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

    std::string skip;  // skip empty line
    std::getline(velocity_stream, skip);
    for (int i = 0; i < this->N; ++i) {  // read in velocities
        molecules[i].read_velocity(velocity_stream);
    }
}

/*
* print a nicely formatted list of every molecule.
*/
void MoleculeSystem::print_system() const {
    std::cout << "System containing N = " << this->N << " molecules:" << std::endl;
    for (int i = 0; i < this->N; ++i) {
        std::cout << "    ";
        molecules[i].print_molecule();
    }
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
* Comepute the potential energy of the system by summing the truncated shifted
* Lennard-Jones potentials over all the molecule pairs.
*/
double MoleculeSystem::potential_energy() const {
    double E_pot = 0;
    for (int i = 0; i < N; ++i) {
        const double* qi = molecules[i].get_position();
        for (int j = i + 1; j < N; ++j) {
            const double* qj = molecules[j].get_position();
            double rij = this->rij(qi, qj);
            E_pot += truncated_LJ_potential(rij);
        }
    }
    return E_pot;
}

/*
* Compute the distance between two molecules given their position arrays.
*/
double MoleculeSystem::rij(const double* qi, const double* qj) const {
    double norm2 = std::pow(qi[0] - qj[0], 2) + 
                    std::pow(qi[1] - qj[1], 2) +
                    std::pow(qi[2] - qj[2], 2);
    return std::sqrt(norm2);
}

/*
* Compute the truncated-shifted Lennard-Jones potential between a pair of molecules
* given the distance r between them.
*/
double MoleculeSystem::truncated_LJ_potential(double r) const {
    if (r < 2.5)
        return LJ_potential(r) - LJ_potential(2.5);
    return 0;
}

/*
* Compute the  Lennard-Jones potential between a pair of molecules
* given the distance r between them.
*/
double MoleculeSystem::LJ_potential(double r) const {
    return 4 * (1 / std::pow(r, 12) - 1 / std::pow(r, 6));
}

/*
* Compute and print out the potential, kinetic and total energy of the system.
*/
void MoleculeSystem::print_energy() const {
    const double Ek = kinetic_energy();
    const double Ep = potential_energy();
    const double Etot = Ep + Ek;
    std::cout << "E_kin = " << Ek << std::endl;
    std::cout << "E_pot = " << Ep << std::endl;
    std::cout << "E_tot = " << Etot << std::endl;
}
