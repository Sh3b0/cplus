mkdir -p build
cmake -S . -B build
cmake --build build

./build/cplus -d examples/sample.cp 