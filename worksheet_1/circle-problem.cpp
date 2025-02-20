#include <iostream>
#include <cstdlib>
#include <cmath>

/*
* Gauss circle problem
* Calculate how many points (x, y), where x and y are integers,
* such that x^2 + y^2 <= r^2, for some integer r.
*/

// O(r)
long circle_problem(long r) {
    // iterate over all x, and find the integer part of y = sqrt(r^2 - x^2),
    // this is the number of points for the given x in one quadrant
    long n = 0;
    for (long x = 0; x <= r; ++x)
        n += std::floor(std::sqrt(r * r - x * x));
    // all 4 quadrants pluss center point
    return 4 * n + 1;
}

int main(int argc, char** argv){
    // raise error if no argument is given
    if (argc < 2) {
        std::cerr << "No argument given. Please provide an integer as argument.\n";
        return 1;
    }
    // convert argument to integer
    long r = std::atoi(argv[1]);
    long n = circle_problem(r);
    std::cout << "There are " << n << " integer points (x, y) such that "
              << "x^2 + y^2 <= " << r << "^2.\n";
    return 0;
}
