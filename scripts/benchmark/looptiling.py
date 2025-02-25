import subprocess
import time
import matplotlib.pyplot as plt
import numpy as np

ROOT_DIR = "../../"
BUILD_SCRIPT = "../build/build.sh"
 #run first to make sure the main program is built
subprocess.run([BUILD_SCRIPT, 'install'])   

matrix_sizes = [100, 200, 500, 1000, 2000, 3000]
# matrix_sizes = [100, 200, 300, 400, 500, 600]
tile_sizes = [2, 4, 8, 16, 32, 64, 128, 256, 512]

#matrix files to use
original_untiled_file = ROOT_DIR + 'examples/matrix2000.f90'
modified_untiled_file = ROOT_DIR + 'examples/matrix-untiled.f90'
original_tiled_file = ROOT_DIR + 'examples/matrix2000tiled.f90'
modified_tiled_file = ROOT_DIR + 'examples/matrix-tiled.f90'

# Run the main program on tiled and untiled matrix, and time the execution

#untiled matrix

fig, axs = plt.subplots(2, len(matrix_sizes)//2, figsize=(12, 7))  # Create a figure and a set of subplots

for i, matrix_size in enumerate(matrix_sizes):
    execution_times = []    #store the execution times for each matrix size

    #untiled matrix
    matrix_untiled_time = 0
    for _ in range(2):  # Run twice and take the average
        start_time = time.time()
        with open(original_untiled_file, 'r') as file:   #read the original untiled file
            lines = file.readlines()
        with open(modified_untiled_file, 'w') as file:   #write to the modified untiled file with the new matrix size
            for line in lines:
                # Replace '2000' with the current matrix_size
                line = line.replace('2000', str(matrix_size))
                file.write(line)  # Write the modified line to the file
             
        subprocess.run([BUILD_SCRIPT, 'run', modified_untiled_file, '-WASM'])
        matrix_untiled_time += time.time() - start_time
    matrix_untiled_time /= 2  # Calculate the average
    execution_times.append(matrix_untiled_time)

    #tiled matrices
    for tile_size in tile_sizes:
        matrix_tiled_time = 0  # Reset for each tile size
        for _ in range(2):  # Run twice and take the average
            start_time = time.time()
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
            subprocess.run([BUILD_SCRIPT, 'run', modified_tiled_file, '-WASM'])
            matrix_tiled_time += time.time() - start_time
        matrix_tiled_time /= 2  # Calculate the average
        execution_times.append(matrix_tiled_time)

    # Plot the execution times for each matrix size as a subplot
    labels = ['1'] + [str(size) for size in tile_sizes]
    colors = plt.cm.magma(np.linspace(0.2, 0.8, len(execution_times)))  # Get colors from viridis
    row = i // (len(matrix_sizes)//2)    # Calculate row index
    col = i % (len(matrix_sizes)//2)     # Calculate column index
    axs[row, col].bar(labels, execution_times, color=colors)
    axs[row, col].set_title(f'{matrix_size}x{matrix_size}')
    axs[row, col].tick_params(axis='x', rotation=45, labelsize=7)  # Rotate x-axis labels for each subplot
    axs[row, col].tick_params(axis='y', labelsize=7)  #make y-axis labels smaller

fig.suptitle('Real Matrix Multiplication with Loop Tiling')  # Adjust y for more space under the suptitle
plt.subplots_adjust(wspace=0.3, hspace=0.3)  # Horizontal spacing
fig.text(0.04, 0.5, 'Execution Time (s)', va='center', rotation='vertical')  # Common y-axis label
fig.text(0.5, 0.04, 'Tile Size (bytes)', ha='center', va='center')  # Common x-axis label


plt.show()
