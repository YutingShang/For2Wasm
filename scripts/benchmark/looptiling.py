import subprocess
import time
import matplotlib.pyplot as plt

ROOT_DIR = "../../"
BUILD_SCRIPT = "../build/build.sh"
 #run first to make sure the main program is built
subprocess.run([BUILD_SCRIPT, 'install'])   

execution_times = []    #store the execution times for each matrix size
tile_sizes = [2, 4, 8]# 16, 32, 64, 128, 256, 512]

#matrix files to use
original_untiled_file = ROOT_DIR + 'examples/matrix2000.f90'
modified_untiled_file = ROOT_DIR + 'examples/matrix-untiled.f90'
original_tiled_file = ROOT_DIR + 'examples/matrix2000tiled.f90'
modified_tiled_file = ROOT_DIR + 'examples/matrix-tiled.f90'

# Run the main program on tiled and untiled matrix, and time the execution

#untiled matrix
matrix_untiled_time = time.time()
subprocess.run([BUILD_SCRIPT, 'run', original_untiled_file, '-WASM'])
matrix_untiled_time = time.time() - matrix_untiled_time
execution_times.append(matrix_untiled_time)

#tiled matrices
matrix_tiled_time = time.time()
for tile_size in tile_sizes:
    with open(original_tiled_file, 'r') as file:   #read the original tiled file
        lines = file.readlines()
    with open(modified_tiled_file, 'w') as file:   #write to the modified tiled file with the new tile size
        for line in lines:
            if 'tile = ' in line:
                file.write(f'tile = {tile_size}\n')
            else:
                file.write(line)
    start_time = time.time()
    subprocess.run([BUILD_SCRIPT, 'run', modified_tiled_file, '-WASM'])
    execution_time = time.time() - start_time
    execution_times.append(execution_time)
matrix_tiled_time = time.time() - matrix_tiled_time

# Plot the execution times
labels = ['1'] + [str(size) for size in tile_sizes]
plt.bar(labels, execution_times)
plt.xlabel('Tile Size')
plt.ylabel('Execution Time (s)')
plt.title('Matrix Multiplication with Loop Tiling')
plt.show()

