OUTPUT_DIR=dist

mkdir -p $OUTPUT_DIR

WAT_FILE=$OUTPUT_DIR/output.wat
WASM_FILE=$OUTPUT_DIR/output.wasm
PROGRAM_FILE=src/wasm/program.js
MAIN_PROGRAM=./build/bin/main
EXAMPLE_FORTRAN_FILE=${1:-examples/summation.f90}

make    #generates the main program

$MAIN_PROGRAM $EXAMPLE_FORTRAN_FILE -irWASM > $WAT_FILE
wat2wasm $WAT_FILE -o $WASM_FILE
node $PROGRAM_FILE $WASM_FILE
