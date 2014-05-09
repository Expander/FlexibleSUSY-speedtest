#!/bin/sh

root -l -b -q talk.C+(\"samples/data_intel_core2_duo_1core.dat\",\"Intel Core2 Duo (P8600, 1 core)\")

mv benchmark.pdf benchmark_intel_core2_duo_1core.pdf
mv benchmark.png benchmark_intel_core2_duo_1core.png

root -l -b -q talk.C+(\"samples/data_intel_core2_duo.dat\",\"Intel Core2 Duo (P8600, 2 cores)\")

mv benchmark.pdf benchmark_intel_core2_duo_2cores.pdf
mv benchmark.png benchmark_intel_core2_duo_2cores.png

root -l -b -q talk.C+(\"samples/data_intel_xeon.dat\",\"Intel Xeon (L5640, 6 cores)\")

mv benchmark.pdf benchmark_intel_xeon_6cores.pdf
mv benchmark.png benchmark_intel_xeon_6cores.png
