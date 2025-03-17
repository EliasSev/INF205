#pragma once

#include <string>  // std::string
#include <vector>  // std::vector
#include <array>   // std::array
#include <random>  // std::mt19937

class BoxGenerator{
    private:
        double a{};  // box side length  
        double V{};  // box volume
        int N{};     // number of molecules
        std::vector<std::array<double, 3>> points = {};  // random coordinates
        std::mt19937 rng;  // pseudo-random number generator
    
    public:
        BoxGenerator(double a, double rho);
        int get_N() const { return this->N; }
        void write_to_file(const std::string& file_name);
        void generate_positions();
        std::vector<int> random_indices(int n_indicies);
    };
