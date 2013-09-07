
CXXFLAGS := -std=c++11 -O2 -Wall -pedantic -Wextra

CXX := g++

src := \
	random_float.cpp \
	random_sign.cpp

obj := $(patsubst %.cpp, %.o, $(filter %.cpp, $(src)))

exe := random_float.x random_sign.x

all: $(exe)

%.x: %.o
	$(CXX) -o $@ $^
