import fs from 'node:fs';    
import promptSync from 'prompt-sync';    //for getting stdin input
import { performance } from 'node:perf_hooks';  // Import performance for benchmarking
const prompt = promptSync();   // need to call prompt-sync() to get actual prompting function

// Get WASM file path from command line arguments, e.g. node program.js ./output.wasm ./output.wat [numRuns]
const wasmFile = process.argv.length > 2 ? process.argv[2] : '';    
const watFile = process.argv.length > 3 ? process.argv[3] : '';
const numRuns = 1 //process.argv.length > 4 ? parseInt(process.argv[4], 10) : 1;  // Default to 1 run if not specified
console.log(process.argv)
//is the --no-liftoff stuff here- NO, intercepted by node

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
        promptSync : () => Number(prompt('')),              //won't work for i64
    },
    math: {
        pow: (base, exponent) => Math.pow(base, exponent),   //won't work for i64
    }
};



/////runs wasm binary with import object, calls exported main function with benchmarking////////////////////////////////////

// Use path module to handle file paths properly
import path from 'node:path';

// Get the directory where the script is located
const SCRIPT_DIR = path.dirname(new URL(import.meta.url).pathname);

const EXP_NUM_PATH = path.resolve(path.join(SCRIPT_DIR, '..', '..', 'scripts', 'benchmark', 'EXP_NUM.txt'));
const RESULTS_DIR = path.resolve(path.join(SCRIPT_DIR, '..', '..', 'scripts', 'benchmark', 'results'));

const EXPERIMENT_NUMBER = Number(fs.readFileSync(EXP_NUM_PATH, 'utf-8').trim());     //read experiment number from file
// Read and compile the WebAssembly module
const wasmBytes = fs.readFileSync(wasmFile);
WebAssembly.compile(wasmBytes).then(module => {
    WebAssembly.instantiate(module, importObject).then(instance => {
        const main = instance.exports.main;

        //override console.log to capture output
        const originalLog = console.log;
        let capturedOutput = [];     //global variable to store the output

        function captureOutput(...args) {
            capturedOutput.push(args.join(' '));
            originalLog(...args);       //still displays it normally after capturing
        }

        function benchmark() {
            capturedOutput = [];
            console.log = captureOutput;       //override console.log to capture output

            const start = performance.now();
            main(); // Run the WebAssembly main function
            const end = performance.now();

            console.log = originalLog;       //restore original console.log

            return {
                time: end - start,
                output: capturedOutput.join('\n')
            };
        }

        function writeToFile(runTime) {

            fs.appendFileSync(`${RESULTS_DIR}/exp${EXPERIMENT_NUMBER}/benchmark_results_${EXPERIMENT_NUMBER}.txt`, `${runTime}\n`);   //should be consistent with the python script
        }
        
        if (numRuns <= 1) {       //could use this as just the normal program script then??
            // Just run once without benchmarking if numRuns is 1 or less
            // main();
            // console.log("Performing warmup runs...");
            // for (let i = 0; i < 3; i++) {
            //     benchmark();
            // }
            console.log("Starting timed RUN");
            const result = benchmark();
            const runTime = result.time;
            writeToFile(runTime.toFixed(6));
        } else {
            console.log(`Running benchmark ${numRuns} times...`);
            let totalTime = 0;
            let referenceOutput = null;
            // Perform 3 trial runs and discard results
            // console.log("Performing warmup runs...");
            // for (let i = 0; i < 3; i++) {
            //     benchmark();
            // }
            console.log("Starting timed runs...");
            for (let i = 0; i < numRuns; i++) {
                const result = benchmark();
                const runTime = result.time;
                writeToFile(runTime.toFixed(6));    //write timing to file to 6dp
                totalTime += runTime;

                // Verify output consistency
                if (i === 0) {
                    referenceOutput = result.output;
                } else if (result.output !== referenceOutput) {
                    console.error(`Warning: Run ${i} produced different output`);
                }
            }
            
            const avgTime = totalTime / numRuns;
            console.log(`\nAverage execution time: ${avgTime.toFixed(6)} ms`);      //just output to console (not actually storing)
        }
    });
});
