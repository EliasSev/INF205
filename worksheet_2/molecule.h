#pragma once

#include <fstream>  // std::ifstream
#include <vector>   // std::vector

/*
* The Molecule class represents molecules in 3D space, each with
* an unique id, a position and a velocity.
*/
class Molecule{
private:
    int id = -1;    // unique identifier (-1 to identify unititialized id)
    double q[3]{};  // (x, y, z) position
    double v[3]{};  // (vx, vy, vz) velocity

public:
    Molecule() = default;

    int get_id() const { return id; }
    const double* get_position() const { return q; }
    const double* get_velocity() const { return v; }
    void set_id(int id) { this->id = id; }

    void read_position(std::ifstream& stream);
    void read_velocity(std::ifstream& stream);
    void print_molecule() const;
};

/*
* The MoleculeSystem class represents a collection of molecules in 3D space, and
* stores the number of molecules (N) and a vector of molecules. The class provides methods
* to nicely print out the system, and to calculate the potential and kinetic energy.
*/
class MoleculeSystem{
private:
    int N{};  // number of molecules
    std::vector<Molecule> molecules;  // dynamic vector to store the molecules

public:
    MoleculeSystem() = default;

    int get_N() const { return N; };
    std::vector<Molecule> get_molecules() const { return molecules; }

    void read_position_stream(std::ifstream& molecule_stream);
    void read_velocity_stream(std::ifstream& velocity_stream);
    void print_system() const;
    double kinetic_energy() const;
    double potential_energy() const;
    double rij(const double* qi, const double* qj) const;
    double truncated_LJ_potential(double r) const;
    double LJ_potential(double r) const;
    void print_energy() const;
};
