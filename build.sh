OUTPUT_DIR=dist

mkdir -p $OUTPUT_DIR


MAIN_PROGRAM=./build/bin/main
EXAMPLE_FORTRAN_FILE=${1:-examples/summation.f90}
FLAG=${2:--irWASM}


if [[ "$FLAG" == "-help" || "$EXAMPLE_FORTRAN_FILE" == "-help" ]]; then
    echo "\n------------------------------------------------------------------------------"
    echo "*                                FOR2WASM                                    *"
    echo "------------------------------------------------------------------------------"
    echo "Usage: \t ./build.sh <fortran_file> <flag>\n"
    echo "Flags:\n"
    echo "  -irWASM \t IR tree -> WASM format + executes the program (default)\n"
    echo "  -irPrint \t IR tree -> text format\n"
    echo "  -irDot \t IR tree -> dot format + converts to png\n"
    echo "  -astDot \t AST tree -> dot format + converts to png\n"
    echo "  -parseDot \t parse tree -> dot format + converts to png"
    echo "------------------------------------------------------------------------------\n"
    exit 0
else 
    make    #generates the main program
fi


if [ "$FLAG" == "-irWASM" ]; then
    WAT_FILE=$OUTPUT_DIR/output.wat
    WASM_FILE=$OUTPUT_DIR/output.wasm
    PROGRAM_FILE=src/wasm/program.js
    $MAIN_PROGRAM $EXAMPLE_FORTRAN_FILE -irWASM > $WAT_FILE
    wat2wasm $WAT_FILE -o $WASM_FILE
    node $PROGRAM_FILE $WASM_FILE
fi

if [[ "$FLAG" == "-irDot"  || "$FLAG" == "-parseDot"  || "$FLAG" == "-astDot" ]]; then
    DOT_FILE=$OUTPUT_DIR/tree.dot
    IMAGE_FILE=$OUTPUT_DIR/tree.png
    $MAIN_PROGRAM $EXAMPLE_FORTRAN_FILE $FLAG > $DOT_FILE
    dot -Tpng $DOT_FILE -o $IMAGE_FILE
    code $IMAGE_FILE     #open in vscode
fi

if [ "$FLAG" == "-irPrint" ]; then          #just run with the original command line inputs, no extra processes
    $MAIN_PROGRAM $EXAMPLE_FORTRAN_FILE $FLAG
fi
