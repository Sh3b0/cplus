mkdir -p build
cmake -S . -B build
cmake --build build
cp build/cplus tests