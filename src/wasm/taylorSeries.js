
const memory = new WebAssembly.Memory({ initial: 1, maximum: 1});


/////// logging function to print strings in wasm//////////////////////////////////

function consoleLogString(offset, length) {
    const bytes = new Uint8Array(memory.buffer, offset, length);
    const string = new TextDecoder("utf8").decode(bytes);
    console.log(string)
}  

////////Export JS functions to wasm/////////////////////////////////

const importObject = {
    js: { mem: memory },
    console: {
        logString: consoleLogString,
        log: (val) => console.log(val),
        log_i64: (val) => console.log(BigInt(val).toString()),
        promptSync : () => Number(prompt('')),              //won't work for i64
    },
    math: {
        pow: (base, exponent) => Math.pow(base, exponent),   //won't work for i64
    }
};



/////runs wasm binary with import object, calls exported main function////////////////////////////////////

function hello(){
    WebAssembly.instantiateStreaming(fetch("taylorSeries.wasm"), importObject)
        .then(
            obj => {
                const start = performance.now();
                obj.instance.exports.main();
                const end = performance.now();
                console.log(`Time taken: ${end - start} milliseconds`);
            }
        );
}

