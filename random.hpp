
#include <random>

template <class T>
T random_number(T start, T stop)
{
   std::random_device device;
   std::uniform_real_distribution<T> dist(start, stop);
   return dist(device);
}
