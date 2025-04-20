import subprocess
import time
import os
from typing import List, Dict
import json

ROOT_DIR = "../../"
BUILD_SCRIPT = "../build/build.sh"

EXPERIMENT_NUMBER = 50
results_file = None
        

def install_the_compiler():
    subprocess.run([BUILD_SCRIPT, 'install'], check=True)

def set_experiment_number():
    global EXPERIMENT_NUMBER
    with open('EXP_NUM.txt', 'r') as f:
        EXPERIMENT_NUMBER = int(f.read().strip())    #should be consistent with the JS script

def increment_experiment_number():
    global EXPERIMENT_NUMBER
    EXPERIMENT_NUMBER += 1
    with open('EXP_NUM.txt', 'w') as f:
        f.write(str(EXPERIMENT_NUMBER))

def prepare_results_file(results_file: str):

    #create the directory if it doesn't exist
    os.makedirs(os.path.dirname(results_file), exist_ok=True)
    
    if os.path.exists(results_file):
        response = input(f"File {results_file} already exists. Ok to delete? (y/n): ")
        if response.lower() != 'y':
            raise Exception("User cancelled - file already exists")
        os.remove(results_file)

    #then create the file
    with open(results_file, 'w') as f:
        f.write("")

def get_benchmark_name(benchmark_file: str):
    #removes the .f90 extension
    return os.path.basename(benchmark_file).split('.')[0]

def get_optimisation_name(opt_flag: str):
    #converts flag name to optimisation name

    # Map between optimization flags and names
    opt_mapping = {
        '': 'None',  # No optimization
        '-DCE': 'DCE',
        '-CSE': 'CSE', 
        '-PRE': 'PRE',
        '-CP': 'CP',
        '-const': 'const',
        '-simplify': 'simplify',
        '-iterCSE-CP': 'iterCSE-CP',
        '-iterPRE-CP': 'iterPRE-CP',
        '-tile': 'tile'
    }

    return opt_mapping.get(opt_flag, "Unknown")

#runs a list of optimisations for a benchmark
#gets the compile and run times and writes to a benchmark file
def run_single_benchmark_optimisation(benchmark_file: str, optimisation_flags: List[str], num_compile_runs: int = 10, v8_flags: List[str] = []):
    """NOTE: num_compile_runs is the number of times to compile the benchmark, each is then then run 5 times in the JS/

    The python script will return a list of compile times and the JS will save the each run time to a file 

    """

    compile_times = []
    overall_run_times = []
    output_wat_file = "results/exp" + str(EXPERIMENT_NUMBER)+"/dist/" + get_benchmark_name(benchmark_file) + "".join(optimisation_flags) + ".wat"
    os.makedirs(os.path.dirname(output_wat_file), exist_ok=True)

    # optimisation_names = [get_optimisation_name(flag) for flag in optimisation_flags]
    with open(results_file, 'a') as f:
        f.write("Benchmark: " + get_benchmark_name(benchmark_file) + "\n")
        f.write("Optimisation: " + " ".join(optimisation_flags) + "\n")

    for i in range(num_compile_runs):

        

        #compile the benchmark into wasm
        print("Compiling the benchmark into wasm")
        compile_start_time = time.time()*1000  #convert to ms
        subprocess.run([BUILD_SCRIPT, 'compile', '-o', output_wat_file,benchmark_file, '-WASM'] + optimisation_flags, check=True)  # check for errors
        compile_end_time = time.time()*1000
        compile_time = compile_end_time - compile_start_time
        compile_times.append(compile_time)

        #get the lines of code in the wasm file
        with open(output_wat_file, 'r') as f:
            if i == 0:
                code_size = len(f.readlines())
                print("Code size (lines):", code_size)
                #write the code size to the file, one per line
                with open(results_file, 'a') as f:
                    f.write("Lines of code: " + "\n")
                    f.write(str(code_size) + "\n")
                    f.write("Run times: " + "\n")
            else:
                new_code_size = len(f.readlines())
                if new_code_size != code_size:
                    raise Exception("Code size changed from", code_size, "to", new_code_size)

        #run time is monitored by the JS script - will write to the (same) file first
        print("Running the benchmark 1 times in JS")
        run_time = time.time()*1000
        # if single_opt == '-tile':
        #run everything with liftoff
        result = subprocess.run([BUILD_SCRIPT, 'run-benchmark', ] + v8_flags + ['-o', output_wat_file], capture_output=True, text=True, check=True)
        run_time = time.time()*1000 - run_time
        print("Run time:", run_time)
        # print("stdout:", result.stdout)
        print("stderr:", result.stderr)
        # else:
        #     result = subprocess.run([BUILD_SCRIPT, 'run-benchmark', '5'], capture_output=True, text=True, check=True)
        #     print("stdout:", result.stdout) 
        #     print("stderr:", result.stderr)
        
        overall_run_times.append(run_time)   #this is slightly different to the JS script runtime which does not include WebAssembly.compile() - parsing it 

        
        

    #write the compile times to the file, one per line
    with open(results_file, 'a') as f:
        # f.write("Run time including instantiation: " + "\n")
        # for run_time in overall_run_times:
        #     f.write(str(run_time) + "\n")

        f.write("Compile times: " + "\n")
        for c_time in compile_times:
            f.write(str(c_time) + "\n")
        

        f.write("--------------------------------\n")
    


    return compile_time

def run_gfortran_benchmark(benchmark_file: str, num_compile_runs: int = 10):
    #runs the benchmark with gfortran

    compile_times = []
    run_times = []

    

    for i in range(num_compile_runs):
        gfortran_output_file = "results/exp" + str(EXPERIMENT_NUMBER)+"/dist/" + get_benchmark_name(benchmark_file) +str(i)
        os.makedirs(os.path.dirname(gfortran_output_file), exist_ok=True)

        compile_start_time = time.time()*1000
        result = subprocess.run(['gfortran', benchmark_file, '-o', gfortran_output_file], capture_output=True, text=True, check=True)
        compile_end_time = time.time()*1000
        compile_time = compile_end_time - compile_start_time
        compile_times.append(compile_time)
        print("Compile time:", compile_time)
        print("stderr:", result.stderr)

        run_time = time.time()*1000
        result = subprocess.run([gfortran_output_file], capture_output=True, text=True, check=True)
        run_time = time.time()*1000 - run_time
        run_times.append(run_time)
        print("Run time:", run_time)
        print("stderr:", result.stderr)

    with open(results_file, 'a') as f:
        f.write("Benchmark: " + get_benchmark_name(benchmark_file) + "\n")
        f.write("Optimisation: " + "GFortran" + "\n")
        f.write("Run times: " + "\n")

        for run_time in run_times:
            f.write(str(run_time) + "\n")

        f.write("Compile times: " + "\n")
        for c_time in compile_times:
            f.write(str(c_time) + "\n")

        f.write("--------------------------------\n")

    

#runs a list of optimisations for a benchmark
def run_optimisations_for_benchmarks(benchmark_files: List[str], num_compile_runs: int = 10, optimisations: List[List[str]] = None):

    # optimisations = [["GFortran"], ["None"], ["-DCE"], ["-CSE"], ["-PRE"], ["-CP"], ["-const"], ["-simplify"], ["-DCE", "-simplify"], ["-iterCSE-CP"], ["-iterPRE-CP"], ["-iterCSE-CP", "-DCE"], ["-iterPRE-CP", "-DCE"], ["-iterCSE-CP", "-iterPRE-CP", "-const", "-DCE", "-simplify"]]
    # optimisations = [["None"], ["-iterCSE-CP", "-iterPRE-CP", "-const", "-DCE", "-simplify"], ["-iterPRE-CP", "-iterCSE-CP", "-const", "-DCE", "-simplify"], ["-const", "-DCE", "-simplify", "-iterCSE-CP", "-iterPRE-CP"], ["-simplify", "-DCE", "-const","-iterPRE-CP", "-iterCSE-CP"], ["-iterCSE-CP", "-DCE",  "-simplify", "-const", "-iterPRE-CP"], ["-iterCSE-CP", "-DCE", "-iterPRE-CP", "-const", "-DCE", "-simplify"]]
    # I think the None flag will just be ignored
    # optimisations = [["None"], ["-simplify"], ["-DCE", "-simplify"]]
    # optimisations = [["GFortran"]]#, ["None"]]#, ["-iterCSE-CP", "-iterPRE-CP", "-const", "-DCE", "-simplify"]]
    for benchmark_file in benchmark_files:
        for opt_flags in optimisations:
            if opt_flags == ["GFortran"]:
                run_gfortran_benchmark(benchmark_file, num_compile_runs)
            else:
                run_single_benchmark_optimisation(benchmark_file, opt_flags, num_compile_runs)

def run_micro_benchmarks(benchmark_optimisations: Dict[str, List[List[str]]], num_compile_runs: int = 10):
    for benchmark_file, benchmark_optimisations in benchmark_optimisations.items():
        if benchmark_file == ROOT_DIR + "examples/microbenchmarks/matrixMult.f90":
            v8_flags = ["--no-L"]
        else:
            v8_flags = []
        for optimisation_flags in benchmark_optimisations:
            run_single_benchmark_optimisation(benchmark_file, optimisation_flags, num_compile_runs, v8_flags)

def get_code_sizes_for_benchmarks(benchmark_names: List[str]):
    optimisations = [["None"], ["-DCE"], ["-CSE"], ["-PRE"], ["-CP"], ["-const"], ["-simplify"], ["-DCE", "-simplify"], ["-iterCSE-CP"], ["-iterPRE-CP"], ["-iterCSE-CP", "-DCE"], ["-iterPRE-CP", "-DCE"], ["-iterCSE-CP", "-iterPRE-CP", "-const", "-DCE", "-simplify"]]
    code_sizes = {}  #indexed by 2 strings: benchmark name and optimisation name
    for benchmark_name in benchmark_names:
        code_sizes[benchmark_name] = {}
        for opt_flags in optimisations:
            code_sizes[benchmark_name][" ".join(opt_flags)] = get_code_size(benchmark_name, opt_flags)
    return code_sizes

def get_code_size(benchmark_file: str, opt_flags: List[str]):
    #returns the code size of the benchmark
    output_wat_file = "results/exp" + str(EXPERIMENT_NUMBER)+"/dist/" + get_benchmark_name(benchmark_file) + "".join(opt_flags) + ".wat"
    
    # Create directory if it doesn't exist
    os.makedirs(os.path.dirname(output_wat_file), exist_ok=True)
    
    subprocess.run([BUILD_SCRIPT, 'compile', '-o', output_wat_file, benchmark_file, '-WASM'] + opt_flags, check=True)
    with open(output_wat_file, 'r') as f:
        code_size = len(f.readlines())
    return code_size
    

def main():
    install_the_compiler()
    set_experiment_number()
    # EXPERIMENT_NUMBER = 50

    # UNCOMMENT FOR RESULTS FILE - ALSO USED IN PROGRAM_BENCHMARK.JS
    global results_file
    results_file = "results/exp" + str(EXPERIMENT_NUMBER) + f"/benchmark_results_{str(EXPERIMENT_NUMBER)}.txt"
    prepare_results_file(results_file)


    # UNCOMMENT TO RUN MACROBENCHMARKS
    # benchmark_dir = ROOT_DIR + "examples/benchmark"
    # benchmark_names = [os.path.join(benchmark_dir, f) for f in os.listdir(benchmark_dir)]
    # # benchmark_names = [benchmark_dir + "/taylorSeries.f90"]
    # run_optimisations_for_benchmarks(benchmark_names, 10)
    # # code_sizes = get_code_sizes_for_benchmarks(benchmark_names)

    # # Write code sizes to JSON file
    # json_file = f"results/exp{EXPERIMENT_NUMBER}/code_sizes_{EXPERIMENT_NUMBER}.json"
    # os.makedirs(os.path.dirname(json_file), exist_ok=True)
    # with open(json_file, 'w') as f:
    #     json.dump(code_sizes, f, indent=4)

    # UNCOMMENT TO RUN MICROBENCHMARKS
    micro_benchmark_dir = ROOT_DIR + "examples/microbenchmarks"
    benchmark_optimisations_map = {
        # "dead3.f90" : [["None"], ["-DCE"], ["-DCE", "-simplify"]],
        # "common4.f90" : [["None"], ["-CSE"], ["-iterCSE-CP"]],   #could also add [-iterCSE-CP, -DCE] and [iterPRE-CP] but Want to keep it micro!! - can analyse these other effects in the macrobenchmark taylorSeries results
        "partialredundancy.f90" : [["None"], ["-PRE"], ["-iterPRE-CP"], ["-PRE-2.0"], ["-iterPRE-CP-2.0"]],
        # "matrixMult.f90" : [["None"], ["-tile"]],
        # "copyconst.f90" : [["None"], ["-CP", "-DCE"], ["-const", "-DCE"]]
    }
    benchmark_optimisations_map = {micro_benchmark_dir + "/" + benchmark_file : benchmark_optimisations for benchmark_file, benchmark_optimisations in benchmark_optimisations_map.items()}
    run_micro_benchmarks(benchmark_optimisations_map, 10)


    increment_experiment_number()
if __name__ == "__main__":
    main()