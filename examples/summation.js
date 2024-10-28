import fs from 'node:fs';     //instead of require('node:fs')

const memory = new WebAssembly.Memory({ initial: 1 }) //memory object with 1 page


///////////get synchronous (?) input from stdin, fill the array to sum up////////////////////////////////

import readlinePromise from 'node:readline/promises';
async function readNumberInputs() {

    const readline = readlinePromise.createInterface({   //node readline module to take inputs, callback style
        input: process.stdin,
        output: process.stdout,
    });

    let num;
    do {
        num = await readline.question("Add:");
        input_array.push(num)
    } while (num != 0)

    //typed view of the WebAssembly memory created above, copy array into it to prepopulate
    const arrayBuffer = new Int32Array(memory.buffer, array_base, input_array.length)
    arrayBuffer.set(input_array)
    readline.close()
}

/////// loggin function $log called from WebAss//////////////////////////////////


function consoleLogString(offset, length) {
    const bytes = new Uint8Array(memory.buffer, offset, length);
    const string = new TextDecoder("utf8").decode(bytes);
    console.log(string)
}

/////////////////////////////////////////


const importObject = {
    js: { mem: memory },
    console: {
        logString: consoleLogString,
        log: (val) => console.log(val),
    },
};

const input_array = []  //array of inputs to sum (TODO : take from stdin)
const array_base = 100;    //base address to store array in Memory after the fixed String outputs in Wasm

/////////////////////////////////////////


WebAssembly.instantiate(
    fs.readFileSync('./summation.wasm'), importObject
).then(obj => { obj.instance.exports.intro(); return readNumberInputs().then(() => obj); }) // Ensures `readNumberInputs()` completes before proceeding - needs to return a promise
    .then(obj => obj.instance.exports.main(array_base, input_array.length))
