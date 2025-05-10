# Clear previous build
rm -rf build
mkdir build && cd build

# Configure with specific compilers
CXX=g++-14 CC=gcc-14 cmake ..

# Now build
cmake --build .