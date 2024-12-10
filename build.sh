OUTPUT_DIR=dist

mkdir -p $OUTPUT_DIR


MAIN_PROGRAM=./build/bin/main
EXAMPLE_FORTRAN_FILE=${1:-examples/summation.f90}
FLAG=${2:--irWASM}

make    #generates the main program

if [ "$FLAG" == "-irWASM" ]; then
    WAT_FILE=$OUTPUT_DIR/output.wat
    WASM_FILE=$OUTPUT_DIR/output.wasm
    PROGRAM_FILE=src/wasm/program.js
    $MAIN_PROGRAM $EXAMPLE_FORTRAN_FILE -irWASM > $WAT_FILE
    wat2wasm $WAT_FILE -o $WASM_FILE
    node $PROGRAM_FILE $WASM_FILE
fi

if [ "$FLAG" == "-irDot" ]; then
    DOT_FILE=$OUTPUT_DIR/tree.dot
    IMAGE_FILE=$OUTPUT_DIR/tree.png
    $MAIN_PROGRAM $EXAMPLE_FORTRAN_FILE -irDot > $DOT_FILE
    dot -Tpng $DOT_FILE -o $IMAGE_FILE
    # code $IMAGE_FILE     #open in vscode
fi
