import subprocess
import time
import matplotlib.pyplot as plt
import numpy as np
import os
ROOT_DIR = "../../"
BUILD_SCRIPT = "../build/build.sh"

# Ensure main program is built
subprocess.run([BUILD_SCRIPT, 'install'])

# List of benchmark programs to test
benchmarks = [
    # os.path.join(ROOT_DIR + 'examples/benchmark/', f) for f in os.listdir(ROOT_DIR + 'examples/benchmark/')
    # ROOT_DIR + 'examples/benchmark/taylorSeries.f90',
    # ROOT_DIR + 'examples/benchmark/binomialCoeff.f90',
    # ROOT_DIR + 'examples/benchmark/dijkstra.f90',
    ROOT_DIR + 'examples/benchmark/mandelbrot.f90',
    # Add other benchmark programs here
]

# List of optimization flags to test
optimizations = [
    [],  # No optimizations
    ['-DCE'],  
    ['-CSE'],  
    ['-PRE'],
    ['-CP'],
    ['-const'],
    ['-simplify'], # Loop tiling
]

opt_names = [
    'None',
    'DCE',
    'CSE',
    'PRE',
    'CP',
    'const',
    'simplify',
]

# Run benchmarks
results = {}
times_data = {}  # Store all run times for error bars
for benchmark in benchmarks:
    benchmark_name = benchmark.split('/')[-1].split('.')[0]
    results[benchmark_name] = []
    times_data[benchmark_name] = []
    
    print(f"\nRunning benchmark: {benchmark_name}")
    reference_output = None
    
    for opt, opt_name in zip(optimizations, opt_names):
        run_times = []  # Store times for each run
        runs = 3  # Number of runs to average
        
        print(f"Testing with {opt_name}...")
        
        for run in range(runs):
            
            # Capture output to verify consistency between optimizations
            compile_process = subprocess.run(
                [BUILD_SCRIPT, 'compile', benchmark, '-WASM'] + opt,
                check=True,
            )
            start_time = time.time()
            run_process = subprocess.run(
                [BUILD_SCRIPT, 'run'],
                check=True,
                capture_output=True,
                text=True
            )
            
            # For the first optimization, store the reference output
            if opt_name == opt_names[0] and run == 0 and reference_output is None:
                reference_output = run_process.stdout
            # For other optimizations, verify output matches the reference
            elif reference_output is not None and run_process.stdout != reference_output:
                print(run_process.stdout)
                print(f"WARNING: Output for {opt_name} differs from reference output!")
                
            run_time = time.time() - start_time
            run_times.append(run_time)
            
        avg_time = np.mean(run_times)
        std_dev = np.std(run_times)
        results[benchmark_name].append(avg_time)
        times_data[benchmark_name].append(std_dev)
        print(f"Average time: {avg_time:.3f} seconds (std dev: {std_dev:.3f})")
        
        # Store results in CSV file
        with open('benchmark_results.csv', 'a') as f:
            f.write(f"{benchmark_name},{opt_name},{avg_time:.6f},{std_dev:.6f}\n")

# # Plot results
# fig, axes = plt.subplots(len(benchmarks), 1, figsize=(12, 6*len(benchmarks)))
# if len(benchmarks) == 1:
#     axes = [axes]  # Ensure axes is a list even for a single benchmark
# x = np.arange(len(opt_names))

# for i, (benchmark, times) in enumerate(results.items()):
#     axes[i].bar(x, times, yerr=times_data[benchmark], capsize=5)
#     axes[i].set_xlabel('Optimization Configuration')
#     axes[i].set_ylabel('Execution Time (seconds)')
#     axes[i].set_title(f'Benchmark Results: {benchmark}')
#     axes[i].set_xticks(x)
#     axes[i].set_xticklabels(opt_names, rotation=45)

# plt.tight_layout()
# plt.savefig('benchmark_results.png')
# plt.show()
