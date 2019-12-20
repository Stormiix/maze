import timeit
import subprocess
import matplotlib
import matplotlib.pyplot as plt
import numpy as np
import re

sizes = [(201,401)]
percentages = range(0, 60, 10)
solvers = ["cell", "line", "corridor"]

def run(command):
    bashCommand = "./{}".format(command)
    process = subprocess.Popen(bashCommand.split(), stdout=subprocess.PIPE)
    out = process.communicate()[0]
    output = out.decode("utf-8")
    if "No solutions" in output:
        print("Run crashed or failed to find solution.")
        return False
    else:
        time = re.search(r'Time: \b([\d]+)', output).group(1)
        steps = re.search(r'solved in \b([\d]+) ', output).group(1)
        return (time, steps)

def generateMaze(size, percentage):
    import subprocess
    bashCommand = "./generator {} {} {}".format(size[0], size[1], percentage)
    process = subprocess.Popen(bashCommand.split(), stdout=subprocess.PIPE)
    print("Generated maze: {} {} {}".format(size[0], size[1], percentage))

results = dict((solver, [] ) for solver in solvers)
avg = 10 #Number of runs per size-complexity combo

for size in sizes:
    print("Size: {} x {}".format(*size))
    for percent in percentages:
        print("> Difficulty: {}".format(percent))
        generateMaze(size, percent)
        for solver in solvers:
            s = 0
            for i in range(avg):
                print("Benchmark #{}: ./{}".format(i+1, solver), end=" | Result: ")
                output = run(solver)
                if(output):
                    elapsed_time, steps = output
                    print(elapsed_time, " ms")
                else: 
                    elapsed_time = -100000
                s += int(elapsed_time)
            print("-------------- Average time: {} ms".format(s/avg))
            results[solver].append(s/avg)
def generateLabels(sizes, percentages):
    labels = []
    for size in sizes:
        for percentage in percentages:
            labels.append("{} x {} - {}".format(size[0], size[1], percentage))
    return labels
labels = generateLabels(sizes, percentages)
x = np.arange(len(labels))  # the label locations
width = 0.35  # the width of the bars

fig, ax = plt.subplots()

rects1 = ax.bar(x - width/3 - width/6, results['cell'], width/3, label='Cell')
rects2 = ax.bar(x - width/6, results['line'], width/3, label='Line')
rects3 = ax.bar(x + width/6, results['corridor'], width/3, label='Corridor')

# Add some text for labels, title and custom x-axis tick labels, etc.
ax.set_ylabel('Elapsed Times')
ax.set_title('Maze Solvers benchmark (Lower is better)')
ax.set_xticks(x)
ax.set_xticklabels(labels)
ax.legend()
fig.tight_layout()
plt.savefig('../benchmark_results_latest.png')
plt.show()