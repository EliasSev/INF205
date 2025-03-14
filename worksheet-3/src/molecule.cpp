#include "molecule.h"  // Molecule, MoleculeSystem

#include <string>     // std::string, std::getline
#include <fstream>    // std::ifstream

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