
#include <iostream>
#include <cstdlib>
#include <random>
#include <boost/lexical_cast.hpp>

int main(int argc, char* argv[])
{
   std::random_device device;
   double number;

   if (argc == 1) {
      std::uniform_real_distribution<double> dist(0., 1.);
      number = dist(device);
   } else if (argc == 3) {
      double start, stop;
      try {
         start = boost::lexical_cast<double>(argv[1]);
         stop  = boost::lexical_cast<double>(argv[2]);
      } catch (const boost::bad_lexical_cast&) {
         std::cout << "Error: cannot cast arguments to double: "
                   << argv[1] << ' ' << argv[2] << '\n';
         return 1;
      }
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
