OUTPUT_DIR=dist

mkdir -p $OUTPUT_DIR


MAIN_PROGRAM=./build/bin/main
EXAMPLE_FORTRAN_FILE=${1:-examples/summation.f90}
FLAG1=${2}      # output format
OPTIMISATION_FLAGS="${@:3}"     # captured all optimisation flags, expand with ${FLAGS[@]}


if [[ "$FLAG1" == "-help" || "$EXAMPLE_FORTRAN_FILE" == "-help" ]]; then
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
    echo "  -simplify \t remove all empty control flow constructs\n"
    echo "  -const \t constant propagation\n"
    echo "  -CP \t copy propagation\n"
    echo "  -iterCSE-CP \t fix point iteration of CSE and copy propagation\n"
    echo "------------------------------------------------------------------------------\n"
    exit 0
else 
    make install #generates the main program
fi


if [ "$FLAG1" == "-WASM" ]; then
    WAT_FILE=$OUTPUT_DIR/output.wat
    WASM_FILE=$OUTPUT_DIR/output.wasm
    PROGRAM_FILE=src/wasm/program.js
    printf "%s" "$($MAIN_PROGRAM $EXAMPLE_FORTRAN_FILE $FLAG1 $OPTIMISATION_FLAGS)" > $WAT_FILE
    wat2wasm $WAT_FILE -o $WASM_FILE
    node $PROGRAM_FILE $WASM_FILE
fi

if [[ "$FLAG1" == "-irTree"  || "$FLAG1" == "-parseTree"  || "$FLAG1" == "-astTree" || "$FLAG1" == "-flowgraph" ]]; then
    DOT_FILE=$OUTPUT_DIR/tree.dot
    IMAGE_FILE=$OUTPUT_DIR/tree.png
    $MAIN_PROGRAM $EXAMPLE_FORTRAN_FILE $FLAG1 ${OPTIMISATION_FLAGS[@]} > $DOT_FILE
    dot -Tpng $DOT_FILE -o $IMAGE_FILE
    cursor $IMAGE_FILE     #open in vscode
fi

if [[ "$FLAG1" == "-irPrint" || "$FLAG1" == "-analysis" ]]; then          #just run with the original command line inputs, no extra processes
    $MAIN_PROGRAM $EXAMPLE_FORTRAN_FILE $FLAG1 ${OPTIMISATION_FLAGS[@]}
fi
