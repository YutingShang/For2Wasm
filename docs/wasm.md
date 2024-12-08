### Compiling and Running the WebAssembly Program

1. **Compile the program**: `g++ -std=c++11 main.cpp -o main`
This compiles the program that converts the `output.ir` file from the `src/antlr/Fortran` directory into WebAssembly.

2. **Run the program**: `./main`
    - **Optional**: Redirect the output to a file: `./main > output.wat`

3. **Convert to binary wasm**: `wat2wasm output.wat -o output.wasm`
This converts the text-based WebAssembly file (`output.wat`) to a binary WebAssembly file (`output.wasm`).

4. **Prepare the Node.js program**:
   - Ensure `program.js` has access to the `output.wasm` file.
   - Install required Node.js packages, such as `prompt-sync`, using:
     - `npm install prompt-sync`
     - Or, if a `package.json` file exists, run `npm install` to install all dependencies.

5. **Run the Node.js program**: `node program.js`