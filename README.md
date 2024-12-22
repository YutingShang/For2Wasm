# For2Wasm
Fortran to WebAssembly Compiler

**Building the Project**

To build the project, run `make` in the root directory. This will output the executable to the `build/bin` directory.

**Cleaning the Project**

To clean the project, run `make clean` in the root directory. This will remove the `build` directory and all its contents.

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
* `-DCE` - output the flowgraph after dead code elimination and convert to png
* `-DCE-ir` - output the IR tree after dead code elimination and convert to png
* `-DCE-WASM` - output the WASM code after dead code elimination and run the program

**Maintenance**

* Run `make clean` when necessary.
* Run `rm -rf dist/` to clean the dist (output) directory.
* Run `./build.sh -help` to get help on the build script.
* Run `time ./build.sh` to get the time taken to run the program.

