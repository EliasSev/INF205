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

    // getters and setters
    int get_id() const { return id; }
    const double* get_position() const { return q; }
    const double* get_velocity() const { return v; }
    void set_id(int id) { this->id = id; }

    // reading in data
    void read_position(std::ifstream& stream);
    void read_velocity(std::ifstream& stream);
};
