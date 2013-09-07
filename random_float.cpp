
#include <iostream>
#include <cstdlib>
#include <random>

int main(int argc, char* argv[])
{
   std::random_device device;
   double number;

   if (argc == 1) {
      std::uniform_real_distribution<double> dist(0., 1.);
      number = dist(device);
   } else if (argc == 3) {
      const double start = std::atof(argv[1]);
      const double stop  = std::atof(argv[2]);
      std::uniform_real_distribution<double> dist(start, stop);
      number = dist(device);
   } else {
      std::cout << "Usage: ./random_float.x [<start> <stop>]\n";
      return 1;
   }

   std::cout.precision(15);
   std::cout << number << '\n';

   return 0;
}
