import subprocess
import time
import matplotlib.pyplot as plt
import numpy as np
import os
import json

ROOT_DIR = "../../"
BUILD_SCRIPT = "../build/build.sh"
 #run first to make sure the main program is built
subprocess.run([BUILD_SCRIPT, 'install'])   

# matrix_sizes = [100, 200, 500, 1000, 2000, 3000]
# # matrix_sizes = [100, 200, 400, 500, 600, 1000]
# # matrix_sizes = [100, 200, 300, 400, 500, 600]
# tile_sizes = [2, 4, 8, 16, 32, 64, 128, 256, 512]

#matrix files to use
original_untiled_file = ROOT_DIR + 'examples/test/matrix2000.f90'
modified_untiled_file = ROOT_DIR + 'examples/test/matrix-untiled.f90'
original_tiled_file = ROOT_DIR + 'examples/test/matrix2000tiled.f90'
modified_tiled_file = ROOT_DIR + 'examples/test/matrix-tiled.f90'

# Run the main program on tiled and untiled matrix, and time the execution
# runs = 2  # Number of runs to average

#untiled matrix

def run_loop_tiling(matrix_size, tile_sizes, runs, flags):
    execution_map = {}
    for matrix_size in matrix_sizes:
        execution_times = []    #store the execution times for each matrix size. Store every run

        #untiled matrix
        matrix_untiled_time = 0
    
        with open(original_untiled_file, 'r') as file:   #read the original untiled file
            lines = file.readlines()
        with open(modified_untiled_file, 'w') as file:   #write to the modified untiled file with the new matrix size
            for line in lines:
                # Replace '2000' with the current matrix_size
                line = line.replace('2000', str(matrix_size))
                file.write(line)  # Write the modified line to the file
                
        subprocess.run([BUILD_SCRIPT, 'compile', modified_untiled_file, '-WASM'])

        untiled_times = []
        for _ in range(runs):  # Run multiple times 
            start_time = time.time()
            subprocess.run([BUILD_SCRIPT, 'run', *flags])
            untiled_times.append(time.time() - start_time)
            
        # matrix_untiled_time /= runs  # Calculate the average
        execution_times.append(untiled_times)

        #tiled matrices
        for tile_size in tile_sizes:
            matrix_tiled_time = 0  # Reset for each tile size
            
            with open(original_tiled_file, 'r') as file:   #read the original tiled file
                lines = file.readlines()
            with open(modified_tiled_file, 'w') as file:   #write to the modified tiled file with the new tile size
                for line in lines:
                    # Replace '2000' with the current matrix_size
                    line = line.replace('2000', str(matrix_size))
                    # Replace 'tile = ...' with the current tile_size
                    if 'tile = ' in line:
                        file.write(f'tile = {tile_size}\n')
                    else:
                        file.write(line)
            subprocess.run([BUILD_SCRIPT, 'compile', modified_tiled_file, '-WASM'])

            tiled_times = []
            for _ in range(runs):  # Run twice and take the average
                start_time = time.time()
                subprocess.run([BUILD_SCRIPT, 'run', *flags])
                tiled_times.append(time.time() - start_time)

            # matrix_tiled_time /= runs  # Calculate the average
            execution_times.append(tiled_times)
        
        execution_map[matrix_size] = execution_times

    return execution_map


def plot_execution_map_and_save(execution_map, matrix_sizes, tile_sizes, exp_num):
    fig, axs = plt.subplots(2, len(matrix_sizes)//2, figsize=(12, 7))  # Create a figure and a set of subplots
    
    for i, matrix_size in enumerate(matrix_sizes):
        all_runs_execution_times = execution_map[matrix_size]
        mean_times = np.mean(all_runs_execution_times, axis=1)   # take the average of all runs
        min_times = np.min(all_runs_execution_times, axis=1)    # minimum times
        max_times = np.max(all_runs_execution_times, axis=1)    # maximum times
        print(mean_times)
        
        # Calculate error bar heights
        yerr_low = mean_times - min_times
        yerr_high = max_times - mean_times
        yerr = [yerr_low, yerr_high]

        # Plot the execution times for each matrix size as a subplot
        labels = ['1'] + [str(size) for size in tile_sizes]
        colors = plt.cm.magma(np.linspace(0.2, 0.8, len(mean_times)))  # Get colors from viridis
        row = i // (len(matrix_sizes)//2)    # Calculate row index
        col = i % (len(matrix_sizes)//2)     # Calculate column index
        
        # Add error bars to the bar plot
        axs[row, col].bar(labels, mean_times, color=colors)
        axs[row, col].errorbar(labels, mean_times, yerr=yerr, fmt='none', color='black', capsize=3)
        
        axs[row, col].set_title(f'{matrix_size}x{matrix_size}')
        axs[row, col].tick_params(axis='x', rotation=45, labelsize=7)
        axs[row, col].tick_params(axis='y', labelsize=7)

    # fig.suptitle(title)  # Adjust y for more space under the suptitle
    plt.subplots_adjust(wspace=0.3, hspace=0.3)  # Horizontal spacing
    fig.text(0.06, 0.5, 'Execution Time (s)', va='center', rotation='vertical', fontsize=12)  # Common y-axis label
    fig.text(0.5, 0.03, 'Tile Size (no. of elements)', ha='center', va='center', fontsize=12)  # Common x-axis label

    plt.savefig(f'loop_tiling_results/exp{exp_num}/loop_tiling_{exp_num}_another.png')
    plt.show()


def write_execution_map_to_file(execution_map, notes):
    # Find next available file number by scanning directory
    i = 1
    while os.path.exists(f'loop_tiling_results/exp{i}'):
        i += 1
    
    # Create experiment directory and notes file
    exp_dir = f'loop_tiling_results/exp{i}'
    os.makedirs(exp_dir)
    with open(f'{exp_dir}/notes.txt', 'w') as f:
        f.write(f'Experiment {i}\n{notes}')
    
    # Convert execution map to JSON-serializable format
    json_map = {}
    for matrix_size, times in execution_map.items():
        json_map[str(matrix_size)] = times

    with open(f'{exp_dir}/loop_tiling_{i}.json', 'w') as f:
        json.dump(json_map, f)

    return i

def read_execution_map_from_file(file_path: str):
    with open(file_path, 'r') as f:
        json_map = json.load(f)
        # Convert back from JSON format
        execution_map = {}
        for matrix_size, times in json_map.items():
            execution_map[int(matrix_size)] = times
        return execution_map

if __name__ == "__main__":
    matrix_sizes = [100, 200, 500, 1000, 2000, 3000]
    # matrix_sizes = [100, 200, 400, 500, 600, 1000]
    # matrix_sizes = [100, 200, 300, 400, 500, 600]
    # matrix_sizes = [50, 100, 150, 200, 250, 300]
    # tile_sizes = [2, 4, 8]
    tile_sizes = [2, 4, 8, 16, 32, 64, 128, 256, 512]


    # Run the main program on tiled and untiled matrix, and time the execution
    runs = 3  # Number of runs to average
    flags = ['--no-L']

    # execution_map = run_loop_tiling(matrix_sizes, tile_sizes, runs, flags)
    # notes = 'matrix_sizes:'+str(matrix_sizes)+'\n'+'tile_sizes:'+str(tile_sizes)+'\n'+'runs:'+str(runs)+'\n'+'flags:'+str(flags) + '\n' + 'double precision'
    # exp_num = write_execution_map_to_file(execution_map, notes)

    exp_num = 3
    execution_map_from_file = read_execution_map_from_file(f'loop_tiling_results/exp{exp_num}/loop_tiling_{exp_num}.json')
    # title = "Double Precision Matrix Multiplication with Loop Tiling (TurboFan)"
    plot_execution_map_and_save(execution_map_from_file, matrix_sizes, tile_sizes, exp_num)