1. `g++ -std=c++11 main.cpp -o main` to compile the program 
which converts the `output.ir` file from the `src/antlr/Fortran` directory into WebAssembly

2. `./main` to run the program
    - Or `./main > output.wat` 

3. Then run `wat2wasm output.wat -o output.wasm` to convert from text based wat to binary wasm

4. Then ensure `program.js` has the `output.wasm` file, and install any node packages (e.g. `prompt-sync`)
   - `npm install prompt-sync`
   - Or if the `package.json` file exists, simply run `npm install` to get dependencies