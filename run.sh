mkdir -p build
cmake -S . -B build
cmake --build build

./build/cplus -d examples/sample.cp
# ./build/cplus -d examples/ex1.cp
# ./build/cplus -d examples/ex2.cp
# ./build/cplus -d examples/ex3.cp

# ./a.out
