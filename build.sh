#!/bin/bash

# removing previously built binary
rm parallel_concurrent_transformation

# cleaning old build
rm -rf build/

# creating build folder
mkdir build

cd build/

# preparation of Makefile
cmake ..

# compile & build
make

# copying the binary to the rott project folder
cp parallel_concurrent_transformation ../

# starting binary
./parallel_concurrent_transformation
