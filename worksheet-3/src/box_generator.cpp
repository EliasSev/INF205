#include "box_generator.h"

#include <string>    // std::string
#include <cmath>     // std::floor, std::ceil
#include <fstream>   // std::ofstream
#include <sstream>   // std::stringstream
#include <random>    // std::random_device, std::uniform_real_distribution
#include <numeric>   // std::iota
#include <algorithm> // std::shuffle

/*
* Calculate the volume and number of molecules in the box. In addition, initialize the 
* random number generator.
*/
BoxGenerator::BoxGenerator(double a, double rho)
    : a(a), V(a * a * a), N(std::floor(a * a * a * rho)) {

    // initialize pseudo-random number generator with random seed
    std::random_device rd;
    this->rng.seed(rd()); 
}

/*
* Create a .xyz file and write the N randomly generated molecules to the file.
*/
void BoxGenerator::write_to_file(const std::string& file_name) {
    // fill 'points' with random coordiantes
    this->generate_positions();

    // write number of molecules 'N' to stream
    std::stringstream stream;
    stream << this->N << "\n\n";

    // write N random points to stream
    std::vector<int> rand_idx = this->random_indices(this->points.size());
    for (int i = 0; i < this->N; ++i) {
        std::array<double, 3>& point = this->points[rand_idx[i]];  // random point
        stream << "C\t" << point[0] << "\t" << point[1] << "\t" << point[2] << "\n";
    }

    // write stream to file
    std::ofstream file(file_name);
    file << stream.str();
    file.close();
}

/*
* Fill up the vector 'points' with N randomly generated (x, y, z) positions. To fill the box evenly,
* ceil(N^(1/3))^3 points are generated. Only N of these are written to a file in 'write_to_file'.
*/
void BoxGenerator::generate_positions() {
    int K = std::ceil(std::cbrt(this->N));  // number of point per dimension, K^3 >= N
    double ds = this->a / K;         // step size
    this->points.resize(K * K * K);  // resize the vector to the known size

    // set up random number generator
    std::uniform_real_distribution<> uniform_dist(0.25 * ds, 0.75 * ds);  // uniform dist on [ds/4, 3ds/4]

    // place molecules randomly on a sub-cube which is 50% smaller than the cube [x0, x1]x[y0, y1]x[z0, z1]
    int idx = 0;
    double x0, y0, z0;
    for (double i = 0; i < K; ++i) {
        x0 = i * ds;
        for (double j = 0; j < K; ++j) {    
            y0 = j * ds;
            for (double k = 0; k < K; ++k) {
                z0 = k * ds;  // (x0, y0, z0) is the lower left of the cube
                // random (x, y, z) point within the sub-cube
                points[idx] = {x0 + uniform_dist(rng), y0 + uniform_dist(rng), z0 + uniform_dist(rng)};
                ++idx;
            }
        }
    }
}

/*
* Generate a vector (0, 1, ..., n_indicies-1), randomly shuffle it, and return it.
*/
std::vector<int> BoxGenerator::random_indices(int n_indicies) {
    std::vector<int> indicies(n_indicies); 
    std::iota(indicies.begin(), indicies.end(), 0);  // fill with {0, 1, ..., n_indicies-1}
    std::shuffle(indicies.begin(), indicies.end(), this->rng);  // shuffle the indices
    return indicies;
}
