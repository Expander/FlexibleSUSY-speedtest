
CXXFLAGS := -std=c++11 -O2 -Wall -pedantic -Wextra

CXX := g++

src := \
	random_double.cpp \
	random_float.cpp \
	random_sign.cpp

obj := $(patsubst %.cpp, %.o, $(filter %.cpp, $(src)))

exe := random_float.x random_double.x random_sign.x

all: $(exe)

%.x: %.o
	$(CXX) -o $@ $^

clean:
	rm -f out.spc
	rm -f SPheno.spc SPheno.spc.MSSM SPheno.out Messages.out
