
#include <iostream>
#include <random>

int main()
{
   std::random_device device;
   std::uniform_real_distribution<double> dist(0., 1.);
   const double number = dist(device);

   std::cout << (number < 0.5 ? -1 : 1) << '\n';

   return 0;
}
