#include "molecule.h"

#include <iostream>  // std::cout, std::endl
#include <fstream>   // std::ifstream

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cout << "Command line argument(s) missing: "
                  << "please provide <position.xyz> (and optionally <velocity.xyz>)" << std::endl;
        return EXIT_FAILURE;
    }

    MoleculeSystem molecule_system;
    
    // attempt to read in positions at argv[1]
	std::ifstream molecule_stream(argv[1]);
    if (!molecule_stream) {
		std::cout << "An error occured when trying to open '" << argv[1] << "'" << std::endl;
		return EXIT_FAILURE;
	}
    molecule_system.read_position_stream(molecule_stream);
    molecule_stream.close();

    // optionally, attempt to read in velocities at argv[2]
    if (argc > 2) {
        std::ifstream velocity_stream(argv[2]);
        if (!velocity_stream) {
            std::cout << "An error occured when trying to open '" << argv[2] << "'" << std::endl;
            return EXIT_FAILURE;
        }
        molecule_system.read_velocity_stream(velocity_stream);
        velocity_stream.close();
    }

    // calculate and print out
    molecule_system.print_energy();
    return 0;
}