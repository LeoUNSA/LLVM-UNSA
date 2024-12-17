# Placeholder

This repository contains a tool for finding parallel patterns in the execution of sequential and multi-threaded C and C++ code. It instruments the code and generates execution traces; then it finds parallel patterns on the traces using high-level exploration and constraint-based graph matching techniques.

## Trace generation
The code instrumentation and trace generation component is implemented on top of LLVM's DataFlowSanitizer. Most of the additions can be found in the files `llvm/lib/Transforms/Instrumentation/DataFlowSanitizer.cpp` and `compiler-rt/lib/dfsan/dfsan.cc`.

### Dependencies

- clang 
- Boost 1.68 or later

On Arch, run:

```
sudo pacman -S clang cmake ninja boost boost-libs

```

### Compiling

Take the following steps (beware that compiling LLVM consumes a significant amount of resources):

```
git clone --recursive https://github.com/robcasloz/LLVM-UNSA.git
cd LLVM-UNSA
mkdir build
cd build
cmake -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++ -DBUILD_SHARED_LIBS=1 -DLLVM_ENABLE_PROJECTS="clang;compiler-rt" -DLLVM_TARGETS_TO_BUILD="X86" -DCMAKE_CXX_STANDARD=14 -DCMAKE_BUILD_TYPE=Release -G "Ninja" ../llvm
ninja compiler-rt clang
```

### Instrumenting a source file

Run the compiled Clang with the following arguments to generate a trace of the desired file in the working directory; it will be used as input lated on.

```
clang -g -fno-discard-value-names -Xclang -disable-lifetime-markers -fsanitize=dataflow -mllvm -dfsan-discovery llvm/tools/discovery/examples/hello-world.c
```

The optional argument `-mllvm -dfsan-discovery-commutativity-list=FILE` can be used to provide a [Sanitizer special case list](https://releases.llvm.org/7.0.0/tools/clang/docs/SanitizerSpecialCaseList.html) that lists external functions (for example from the standard C library) that can be part of parallel patterns.

## Discovery of parallel patterns

The parallel pattern discovery finding component is implemented as a collection of Python scripts and [MiniZinc](https://www.minizinc.org) graph matching models. Both can be found in the `llvm/tools/discovery` directory.

### Dependencies

- Python 3.8 or later

- MiniZinc 2.3.2 or later

On Ubuntu 20.04, just run:

```
sudo apt-get install python3 python3-networkx
mkdir -p $MINIZINC_INSTALL_DIR
wget -qO- https://github.com/MiniZinc/MiniZincIDE/releases/download/2.5.3/MiniZincIDE-2.5.3-bundle-linux-x86_64.tgz | tar -xvzf - --directory $MINIZINC_INSTALL_DIR
export PATH=$PATH:$MINIZINC_INSTALL_DIR/MiniZincIDE-2.5.3-bundle-linux-x86_64/bin

```

### Finding parallel patterns in a trace

To find patterns heuristically in a trace, run:

```
llvm/tools/discovery/find_patterns.py llvm/tools/discovery/examples/hello-world.trace
```

The script outputs a table in CSV format where each row corresponds to a found pattern (identifier, location, loop whenever applicable, and pattern type).

### Testing

To test the pattern finder on a set of small examples, set the environment variable `CC` to the compiled `clang` and run:
```
cd llvm/tools/discovery
./test.py
```

