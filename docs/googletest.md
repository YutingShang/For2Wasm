In the `third-party` directory, I have included the Google Test framework, by running the following command:
`git clone https://github.com/google/googletest.git -b v1.15.2`

Version 1.15.2 was the latest version at the time of the project. (Will be in detached HEAD state)

The google test files are `.cc` but my own test files are `.cpp`.

----

To create a new IR test case expected output file, run the following command (which will **not** add a newline character to the end of the file):
```
echo -n "$(./build/bin/main <input file> -irPrint)" >> <output file>
```

Output file should probably be in the `tests/expected_output/IR_strings` directory.

---

To create a new WASM test case:
```
printf "%s" "$(./build/bin/main <input file> -WASM)" > <output file>
```
Output file should probably be in the `tests/expected_output/WASM_files` directory.

Example:
```
printf "%s" "$(./build/bin/main examples/dead3.f90 -WASM -DCE -simplify)" > tests/expected_output/WASM_files/dead3_DCE_simplify.wat
```
