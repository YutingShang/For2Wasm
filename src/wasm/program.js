import fs from 'node:fs';    
import promptSync from 'prompt-sync';    //for getting stdin input
const prompt = promptSync();   // need to call prompt-sync() to get actual prompting function

// Get WASM file path from command line arguments, e.g. node program.js ./output.wasm ./output.wat
const wasmFile = process.argv.length > 2 ? process.argv[2] : '';    
const watFile = process.argv.length > 3 ? process.argv[3] : '';

// Check if file exists
if (!fs.existsSync(wasmFile)) {
    console.error(`Error: WASM file '${wasmFile}' not found!`);
    process.exit(1);
}
if (!fs.existsSync(watFile)) {
    console.error(`Error: WAT file '${watFile}' not found!`);
    process.exit(1);
}

//Function to parse the WAT file to get the memory size
function extractMemorySize(watFilePath) {
    const watContent = fs.readFileSync(watFilePath, 'utf-8');

    // Regular expression to match the memory import - matches (memory 1) and extracts the number of pages
    const memoryRegex = /\(memory\s+(\d+)\)/;  
    const match = watContent.match(memoryRegex);

    if (match && match[1]) {    //a valid match is found
        return parseInt(match[1], 10); // Return the number of pages (e.g., 1 from (memory 1))
    }

    return 1;  // Default to 1 page if not found
}

// Extract memory pages size from the WAT file
const initialMemoryPages = extractMemorySize(watFile);
const memory = new WebAssembly.Memory({ initial: initialMemoryPages, maximum: initialMemoryPages});


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
        promptSync : () => Number(prompt('')),
    },
    math: {
        pow: (base, exponent) => Math.pow(base, exponent),   //won't work for i64
    }
};



/////runs wasm binary with import object, calls exported main function////////////////////////////////////

WebAssembly.instantiate(
    fs.readFileSync(wasmFile), importObject
).then(obj => obj.instance.exports.main());
