# For2Wasm
Fortran to WebAssembly Compiler

### Building the Project

To build the project, run `make` in the root directory. This will output the executable to the **build/bin** directory and run the unit tests.

To build without tests, run `make install`.

### Cleaning the Project

To clean the project, run `make clean` in the root directory. This will remove the **build** directory and all its contents.

To clean the tests, run `make cleantest` in the root directory. This will remove the **build/tests** and **build/generated/gtest** directories and all their contents.

### Using the Build Script

You can simply run the script `./build.sh` and it will compile the program.

To compile a fortran program, with different output options and optimisation flags, you can use the following command:
```
./build.sh <build_type>? <node_flags>? <fortran_file> <output_format> <optimisation_flags>?
```

Input files are typically located in the **examples** directory.
Output files are typically located in the **dist** directory.

### Build Options

* `./build.sh test` - build the compiler with unit tests
* `./build.sh install` - build the compiler (no unit tests)
* `./build.sh` - compile and run the fortran program (using built compiler)
* `./build.sh compile` - compile the fortran program but does not run it
* `./build.sh run` - run the last compiled fortran program in the dist/ directory (error if files do not exist)

### Node Flags

* `-no-L` - do not use **Liftoff** for wasm execution (short for `--no-liftoff`)
* `-P` - print the wasm code (short for `--print-wasm-code`)

Note. The `--no-wasm-tier-up` flag is always used to either force Liftoff or not (then uses **Turbofan**)

### Output Options

The first flag determines what to output. The available options are:

* `-WASM` - output the IR tree in WASM format and run the program
* `-irPrint` - output the IR tree in text format
* `-irTree` - output the IR tree in dot format and convert to png
* `-astTree` - output the AST tree in dot format and convert to png [no optimisation]
* `-parseTree` - output the parse tree in dot format and convert to png [no optimisation]
* `-flowgraph` - output the flowgraph in dot format and convert to png

The second and third flags determine what optimisation to apply. The available options are:
* `-DCE` - run dead code elimination
* `-CSE` - run common subexpression elimination
* `-simplify` - run simplification (e.g. removes empty control flow constructs)
* `-const` - run constant propagation
* `-CP` - run copy propagation
* `-iterCSE-CP` - run copy propagation and common subexpression elimination iteratively until no more changes are made
* `-iterPRE-CP` - run copy propagation and partial redundancy elimination iteratively until no more changes are made
* `-tile` - run loop tiling with a tile size of 4

### Maintenance Recap

* Run `make clean` when necessary.
* Run `make cleantest` when necessary.
* Run `./build.sh -help` to get help on the command line arguments.
* Run `time ./build.sh` to get the time taken to run the program.

