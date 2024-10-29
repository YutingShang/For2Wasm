//import fs from 'node:fs';     //instead of require('node:fs')
const fs = require('node:fs');
const memory = new WebAssembly.Memory({ initial: 1 }) //memory object with 1 page

/////// loggin function to print strings in wasm//////////////////////////////////

function consoleLogString(offset, length) {
    const bytes = new Uint8Array(memory.buffer, offset, length);
    const string = new TextDecoder("utf8").decode(bytes);
    console.log(string)
}

// Importing the prompting module - synchronous stdin input 
// prompt-sync module creates prompting functions, so need to call prompt-sync() to get actual prompting function
const prompt = require('prompt-sync')();  

////////Export JS functions to wasm/////////////////////////////////

const importObject = {
    js: { mem: memory },
    console: {
        logString: consoleLogString,
        log: (val) => console.log(val),
        promptSync : () => Number(prompt('')),
    },
    
};

/////runs wasm binary with import object, calls exported main function////////////////////////////////////

WebAssembly.instantiate(
    fs.readFileSync('./summation.wasm'), importObject
).then(obj => obj.instance.exports.main())
