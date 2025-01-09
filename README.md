# For2Wasm
Fortran to WebAssembly Compiler

**Building the Project**

To build the project, run `make` in the root directory. This will output the executable to the `build/bin` directory and run the unit tests.

If you just want to build the project without running the tests, run `make install`.

**Cleaning the Project**

To clean the project, run `make clean` in the root directory. This will remove the `build` directory and all its contents.

To clean the tests, run `make cleantest` in the root directory. This will remove the `build/tests` and `build/generated/gtest` directories and all their contents.

**Using the Build Script**

You can simply run the script `./build.sh` and it will compile the program and run it. For example:
```
./build.sh examples/summation.f90 -irWASM
```
**Output Options**

The third argument is a flag that determines what to output. The available options are:

* `-irWASM` - (*default*) output the IR tree in WASM format and run the program
* `-irPrint` - output the IR tree in text format
* `-irDot` - output the IR tree in dot format and convert to png
* `-astDot` - output the AST tree in dot format and convert to png
* `-parseDot` - output the parse tree in dot format and convert to png
* `-flowgraph` - output the flowgraph in dot format and convert to png
---
* `-DCE` - output the flowgraph after dead code elimination and convert to png
* `-DCE-ir` - output the IR tree after dead code elimination and convert to png
* `-DCE-WASM` - output the WASM code after dead code elimination and run the program
    * `-simplify` - fourth argument, if present, will run simplification on the IR tree after DCE (e.g. removes empty control flow constructs)
* `-CSE` - output the flowgraph after common subexpression elimination and convert to png
* `-CSE-ir` - output the IR tree after common subexpression elimination and convert to png
* `-CSE-irPrint` - output the IR tree after common subexpression elimination in text format

**Maintenance**

* Run `make clean` when necessary.
* Run `rm -rf dist/` to clean the dist (output) directory.
* Run `./build.sh -help` to get help on the build script.
* Run `time ./build.sh` to get the time taken to run the program.

