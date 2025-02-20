#include <iostream>
#include <cstdlib>

bool is_prime(int n)
{
   if(n < 2) return false;
   for(int i = 2; n >= i*i; i++)
      if((n % i) == 0) return false;
   return true;
}

int main(int argc, char** argv)
{
   // raise error if no argument is given
   if (argc < 2) {
      std::cerr << "No argument given. Please provide an integer as argument.\n";
      return 1;
   }

   int x = std::atoi(argv[1]);
   if(is_prime(x))
      std::cout << x << " is prime.\n";
   else std::cout << x << " is not prime.\n";
}