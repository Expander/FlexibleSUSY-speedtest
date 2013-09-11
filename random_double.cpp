
#include <iostream>
#include <cstdlib>
#include <limits>
#include <boost/lexical_cast.hpp>
#include "random.hpp"

int main(int argc, char* argv[])
{
   typedef double scalar_t;
   scalar_t number;

   if (argc == 1) {
      number = random_number(0., 1.);
   } else if (argc == 3) {
      scalar_t start, stop;
      try {
         start = boost::lexical_cast<scalar_t>(argv[1]);
         stop  = boost::lexical_cast<scalar_t>(argv[2]);
      } catch (const boost::bad_lexical_cast&) {
         std::cout << "Error: cannot cast arguments to double: "
                   << argv[1] << ' ' << argv[2] << '\n';
         return 1;
      }
      number = random_number(start, stop);
   } else {
      std::cout << "Usage: " << argv[0] << " [<start> <stop>]\n";
      return 1;
   }

   std::cout.precision(std::numeric_limits<scalar_t>::digits10);
   std::cout << number << '\n';

   return 0;
}
