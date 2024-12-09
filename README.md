# For2Wasm
Fortran to WebAssembly Compiler 

To build the project, run `make` in the root directory.

This will output the executable to the `build/bin` directory.

To clean the project, run `make clean` in the root directory.
This will remove the `build` directory and all its contents.

-----

Currently my program works to compile Fortran to WASM by first compiling the main program, which you can run with the desired example program:

```
make
./build/bin/main examples/summation.f90 -irWASM > output.wat
wat2wasm output.wat -o output.wasm

```
*then move output.wasm to folder where program.js is, and `cd` to that directory*

```
node program.js
```

This will run the program and produce the execution behavior.

---

Okay I've put this all in a bash script, so you can just run `./build.sh` and it will compile the program and run it.

e.g. 
```
./build.sh examples/summation.f90
```

This will compile the program and run it.

The output will be in the `dist` directory.
And then it automatically runs `node program.js` on this output.

Run `make clean` when necessary.

