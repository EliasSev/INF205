#include "molecule_system.h"

#include <string>    // std::stod
#include <iostream>  // std::cout
#include <fstream>   // std::ifstream

int main(int argc, char** argv) {
    if (argc < 3) {
        std::cout << "Usage: <box-size> <position.xyz> [velocity.xyz] [--pure_output]\n";
        return EXIT_FAILURE;
    }

    // handling argument 1: try converting strings to double
    double a = 0;
    try {
        a = std::stod(argv[1]);
    } catch (std::invalid_argument&) {
        std::cout << "<box-size> must be a number, not: '" << argv[1] << "'\n";
        return EXIT_FAILURE;
    } catch (std::out_of_range&) {
        std::cout << "<box-size> is too large: '" << argv[1] << "'\n";
        return EXIT_FAILURE;
    }
    // check parameter requirement
    if (a < 5) {
        std::cout << "<box-size> must be larger than 5, not: " << a << "\n";
        return EXIT_FAILURE;
    }

    MoleculeSystem molecule_system(a);
    
    // handling argument 2: attempt to read in positions at argv[2]
	std::ifstream molecule_stream(argv[2]);
    if (!molecule_stream) {
		std::cout << "An error occurred when trying to open '" << argv[2] << "'\n";
		return EXIT_FAILURE;
	}
    molecule_system.read_position_stream(molecule_stream);
    molecule_stream.close();

    // handling optional argument 3 and 4
    bool flag_given = false;
    bool velocity_arg = false;
    std::string velocity_file = "";
    if (argc > 3) {
        // check if "--pure_output" is in position 3 or 4
        if (std::string(argv[3]) == "--pure_output") {
            flag_given = true;
        } else {
            velocity_arg = true;
            velocity_file = argv[3];
        }
        
        // if 5 arguments are given, find where the velocity stream is
        if (argc > 4 && std::string(argv[4]) == "--pure_output") {
            flag_given = true;
        } else if (argc > 4 && !flag_given) {
            velocity_arg = true;
            velocity_file = argv[4];
        }

        // if velocity file is given, read it
        if (velocity_arg) {
            std::ifstream velocity_stream(velocity_file);
            if (!velocity_stream) {
                std::cout << "An error occured when trying to open '" << velocity_file << "'\n";
                return EXIT_FAILURE;
            }
            molecule_system.read_velocity_stream(velocity_stream);
            velocity_stream.close();
        }
    }
    
    // output the results to the terminal
    molecule_system.system_energy(flag_given);
    return 0;
}