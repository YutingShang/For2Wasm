#!/bin/bash

# Get the directory of the current script
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
MAKEFILE_DIR=$SCRIPT_DIR  # Adjust the path to point to your Makefile directory

OUTPUT_DIR=$SCRIPT_DIR/../../dist

mkdir -p $OUTPUT_DIR


MAIN_PROGRAM=$SCRIPT_DIR/../../build/bin/main
FLAG1=${1}     #should be the fortran file, or could be '-help' or 'install' or 'run'

NODE_FLAGS=()     #could be '--no-L' for no liftoff in wasm execution, or '--P' for print the compiled wasm code
START_IDX=1
# If FLAG1 is a command like 'install', 'run', etc., start collecting flags from position 2
###NOTE: also need to add flags here!!!
if [ "$FLAG1" == "run-benchmark" ] && [[ "${2}" =~ ^[0-9]+$ ]]; then      #if the second argument is a number, then it is the number of runs
    START_IDX=3
    NUM_RUNS=${2}
elif [[ "$FLAG1" == "install" || "$FLAG1" == "run" || "$FLAG1" == "compile" || "$FLAG1" == "test" || "$FLAG1" == "run-benchmark" ]]; then
    START_IDX=2
    #num runs is just default 1
fi
NEXT_IDX=$START_IDX    #after processing all the flags, this will be the index of the fortran file

for arg in "${@:$START_IDX}"; do
    if [[ "$arg" == --* ]]; then
        # Translate shorthand flags to their full versions
        if [[ "$arg" == "--no-L" ]]; then
            NODE_FLAGS+=("--no-liftoff")
        elif [[ "$arg" == "--P" ]]; then
            NODE_FLAGS+=("--print-wasm-code")
        else
            echo "Unknown flag: $arg"
            exit 1
        fi
        ((NEXT_IDX++))       #increment the end index
    else
        # If we encounter a non-flag argument, stop collecting flags
        break
    fi
done


#output file only works for -WASM flag
get_output_file() {
    if [ "${1}" = "-o" ]; then
        WAT_FILE="${2}"
        WASM_FILE="${2%.*}.wasm"
        NEXT_IDX=$((NEXT_IDX + 2))
    else
        WAT_FILE=$OUTPUT_DIR/output.wat
        WASM_FILE=$OUTPUT_DIR/output.wasm
    fi
}

parse_args() {
    EXAMPLE_FORTRAN_FILE="${1}"
    OUTPUT_FORMAT="${2}"
    OPTIMISATION_FLAGS="${@:3}"
}

run_node_with_flags() {
    # Check if there are any NODE_FLAGS
    # always have --no-wasm-tier-up to either force liftoff or not (then its turbofan)
    if [ ${#NODE_FLAGS[@]} -gt 0 ]; then
        node --no-wasm-tier-up "${NODE_FLAGS[@]}" $PROGRAM_FILE $WASM_FILE $WAT_FILE $NUM_RUNS    #hopefully num runs is usually just empty
    else
        node --no-wasm-tier-up --liftoff $PROGRAM_FILE $WASM_FILE $WAT_FILE $NUM_RUNS
    fi
}

get_output_file "${@:NEXT_IDX}"   #if a custom -o output.wat one exists for -WASM compile or run
PROGRAM_FILE=$SCRIPT_DIR/../../src/wasm/program.js

if [ "$FLAG1" == "-help" ]; then
    echo "\n------------------------------------------------------------------------------"
    echo "*                                FOR2WASM                                    *"
    echo "------------------------------------------------------------------------------"
    echo "Usage: \t ./build.sh <build_type>? <node_flags>? <fortran_file> <output_format> <optimisation_flags>?\n"
    echo "Build type: \n"
    echo "  None \t default compile and run fortran file (using built compiler)\n"
    echo "  compile \t compile the fortran file (does not run)\n"
    echo "  run \t \t runs the last compiled wasm file (can just ./build.sh run)\n"
    echo "  install \t builds the compiler (does not run tests)\n"
    echo "  test \t \t builds the compiler and runs the tests\n"
    echo "Node flags: \n"
    echo "  -no-L \t \t --no-liftoff in wasm execution\n"
    echo "  -P \t \t --print-wasm-code\n"
    echo "Output format: \n"
    echo "  -WASM \t WASM format -> executes the program\n"
    echo "  -irPrint \t IR tree -> text format\n"
    echo "  -irTree \t IR tree -> dot format + converts to png\n"
    echo "  -astTree \t AST tree -> dot format + converts to png [no optimisation]\n"
    echo "  -parseTree \t parse tree -> dot format + converts to png [no optimisation]\n"
    echo "  -flowgraph \t flowgraph -> dot format + converts to png\n"
    echo "Optimisation flags: \n"
    echo "  -DCE \t \t dead code elimination\n"
    echo "  -CSE \t \t common subexpression elimination\n"
    echo "  -simplify \t remove all empty control flow constructs and unused declare statements\n"
    echo "  -const \t constant propagation\n"
    echo "  -CP \t copy propagation\n"
    echo "  -iterCSE-CP \t fix point iteration of CSE and copy propagation\n"
    echo "  -PRE \t partial redundancy elimination\n"
    echo "  -iterPRE-CP \t fix point iteration of PRE and copy propagation\n"
    echo "  -tile \t loop tiling with a tile size of 4 bytes\n"
    echo "------------------------------------------------------------------------------\n"
    exit 0
elif [ "$FLAG1" == "install" ]; then
    make -C $MAKEFILE_DIR install #generates the main program without running tests, -C is to specify the makefile directory
    parse_args "${@:NEXT_IDX}"
elif [ "$FLAG1" == "test" ]; then
    make -C $MAKEFILE_DIR #generates the main program with unit tests
    parse_args "${@:NEXT_IDX}"
elif [ "$FLAG1" == "compile" ]; then
    #to work with the -WASM flag, just creates the wasm and wat files, but does not run node
    #does not work with the --no-L flag or the --P flag
    parse_args "${@:NEXT_IDX}"
elif [[ "$FLAG1" == "run" || "$FLAG1" == "run-benchmark" ]]; then
    #to work with the -WASM flag, just runs the wasm and wat files in the dist/ directory if it exists
    if [ -f "$WASM_FILE" ]; then
        if [ "$FLAG1" == "run-benchmark" ]; then
            PROGRAM_FILE=$SCRIPT_DIR/../../src/wasm/program_benchmark.js     #change the program file to the benchmark file
        fi
        run_node_with_flags
    else
        echo "Error: WASM file not found in dist/ directory"
        exit 1
    fi
else      #just ./build.sh - does not run make, will just run the main program executable
    parse_args "${@:NEXT_IDX}"
fi

if [ "$OUTPUT_FORMAT" == "-WASM" ]; then
    printf "%s" "$($MAIN_PROGRAM $EXAMPLE_FORTRAN_FILE $OUTPUT_FORMAT $OPTIMISATION_FLAGS)" > $WAT_FILE
    wat2wasm $WAT_FILE -o $WASM_FILE
    if [ "$FLAG1" == "compile" ]; then
        exit 0
    fi
    run_node_with_flags
fi

if [ "$FLAG1" == "compile" ]; then
    echo "compile flag works only with -WASM flag"
    exit 1
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
