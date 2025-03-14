#include "box_generator.h"

#include <iostream>  // std::cout
#include <string>    // std::string
#include <cmath>     // std::floor

// arguments <box-size> <density> <file-name.xyz>
int main(int argc, char** argv) {
    if (argc < 4) {
        std::cout << "Usage: <box-size> <density> <file-name.xyz>\n";
        return EXIT_FAILURE;
    }

    // try converting strings to doubles
    double box_size, density;
    try {
        box_size = std::stod(argv[1]);
        density = std::stod(argv[2]);
    } catch (std::invalid_argument&) {
        std::cout << "<box-size> and <density> must both be numbers, not: '"
                  << argv[1] << "' and '" << argv[2] << "'\n";
        return EXIT_FAILURE;
    } catch (std::out_of_range&) {
        std::cout << "<box-size> and <density> can not be too large, not: '"
                  << argv[1] << "' and '" << argv[2] << "'\n";
        return EXIT_FAILURE;
    }

    // check parameter requirements
    if (box_size < 5) {
        std::cout << "<box-size> must be greater or equal to 5, not: " << box_size << "\n";
        return EXIT_FAILURE;
    }
    if (density <= 0) {
        std::cout << "<density> must be greater than 0, not: " << density << "\n";
        return EXIT_FAILURE;
    }

    std::string file_name = argv[3];
    BoxGenerator box_generator(box_size, density);
    box_generator.write_to_file(file_name);
    std::cout << "Box with of size " << box_size << " and density " << density << " saved as '" << file_name << "'\n";
    return 0;
}