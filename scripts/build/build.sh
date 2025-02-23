#!/bin/bash

# Get the directory of the current script
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
MAKEFILE_DIR=$SCRIPT_DIR  # Adjust the path to point to your Makefile directory

OUTPUT_DIR=$SCRIPT_DIR/../../dist

mkdir -p $OUTPUT_DIR


MAIN_PROGRAM=$SCRIPT_DIR/../../build/bin/main
FLAG1=${1:-examples/summation.f90}     #should be the fortran file, or could be '-help' or 'install' or 'run'

if [ "$FLAG1" == "-help" ]; then
    echo "\n------------------------------------------------------------------------------"
    echo "*                                FOR2WASM                                    *"
    echo "------------------------------------------------------------------------------"
    echo "Usage: \t ./build.sh <fortran_file> <flag1> <flag2> <flag3>\n"
    echo "Flag 1: output format\n"
    echo "  -WASM \t WASM format -> executes the program\n"
    echo "  -irPrint \t IR tree -> text format\n"
    echo "  -irTree \t IR tree -> dot format + converts to png\n"
    echo "  -astTree \t AST tree -> dot format + converts to png [no optimisation]\n"
    echo "  -parseTree \t parse tree -> dot format + converts to png [no optimisation]\n"
    echo "  -flowgraph \t flowgraph -> dot format + converts to png\n"
    echo "Flag 2 & \nFlag 3: optimisation (default is no optimisation)\n"
    echo "  -DCE \t \t dead code elimination\n"
    echo "  -CSE \t \t common subexpression elimination\n"
    echo "  -simplify \t remove all empty control flow constructs and unused declare statements\n"
    echo "  -const \t constant propagation\n"
    echo "  -CP \t copy propagation\n"
    echo "  -iterCSE-CP \t fix point iteration of CSE and copy propagation\n"
    echo "  -PRE \t partial redundancy elimination\n"
    echo "  -iterPRE-CP \t fix point iteration of PRE and copy propagation\n"
    echo "------------------------------------------------------------------------------\n"
    exit 0
elif [ "$FLAG1" == "install" ]; then
    make -C $MAKEFILE_DIR install #generates the main program without running tests, -C is to specify the makefile directory
    EXAMPLE_FORTRAN_FILE="${2}"
    OUTPUT_FORMAT=${3}      # output format
    OPTIMISATION_FLAGS="${@:4}"     # captured all optimisation flags, expand with ${FLAGS[@]}
elif [ "$FLAG1" == "run" ]; then
    #do not run make, will just run the main program
    EXAMPLE_FORTRAN_FILE="${2}"
    OUTPUT_FORMAT=${3}      # output format
    OPTIMISATION_FLAGS="${@:4}"     # captured all optimisation flags, expand with ${FLAGS[@]}
else 
    make -C $MAKEFILE_DIR #generates the main program
    EXAMPLE_FORTRAN_FILE="${FLAG1}"
    OUTPUT_FORMAT=${2}      # output format
    OPTIMISATION_FLAGS="${@:3}"     # captured all optimisation flags, expand with ${FLAGS[@]}  
fi

if [ "$OUTPUT_FORMAT" == "-WASM" ]; then
    WAT_FILE=$OUTPUT_DIR/output.wat
    WASM_FILE=$OUTPUT_DIR/output.wasm
    PROGRAM_FILE=$SCRIPT_DIR/../../src/wasm/program.js
    printf "%s" "$($MAIN_PROGRAM $EXAMPLE_FORTRAN_FILE $OUTPUT_FORMAT $OPTIMISATION_FLAGS)" > $WAT_FILE
    wat2wasm $WAT_FILE -o $WASM_FILE
    node --no-wasm-tier-up --no-liftoff --no-turbofan $PROGRAM_FILE $WASM_FILE $WAT_FILE
fi

if [[ "$OUTPUT_FORMAT" == "-irTree"  || "$OUTPUT_FORMAT" == "-parseTree"  || "$OUTPUT_FORMAT" == "-astTree" || "$OUTPUT_FORMAT" == "-flowgraph" ]]; then
    DOT_FILE=$OUTPUT_DIR/tree.dot
    IMAGE_FILE=$OUTPUT_DIR/tree.png
    $MAIN_PROGRAM $EXAMPLE_FORTRAN_FILE $OUTPUT_FORMAT ${OPTIMISATION_FLAGS[@]} > $DOT_FILE
    dot -Tpng $DOT_FILE -o $IMAGE_FILE
    cursor $IMAGE_FILE     #open in vscode
fi

if [[ "$OUTPUT_FORMAT" == "-irPrint" || "$OUTPUT_FORMAT" == "-analysis" ]]; then          #just run with the original command line inputs, no extra processes
    $MAIN_PROGRAM $EXAMPLE_FORTRAN_FILE $OUTPUT_FORMAT ${OPTIMISATION_FLAGS[@]}
fi
