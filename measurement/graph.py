import glob
import statistics
import readline
import matplotlib.pyplot as plt

START_SEC = 11
SCAN_RANGE = 1000

print('''    1. Acc Cnt
    2. Acc thread Cnt
    3. Lea thread Cnt
    4. Comm thread Cnt
    5. Replica thread Cnt
    6. Client thread Cnt''')

x_axis = int(input('Enter x axis:'))-1
line_factor = int(input('Enter line factor:'))-1

x_labels = ["Acceptor Number",
            "Acceptor Thread Number",
            "Leader Thread Number",
            "Commander Thread Number",
            "Replica Thread Number",
            "Client Thread Number",
            ]

if x_axis == line_factor or x_axis < 0 or x_axis > 5 or line_factor < 0 or line_factor > 5:
    raise ValueError("x axis cannot be same as line factor")

filename_keyword = input("Enter log file name:")
filename_list = filename_keyword.replace(".log", "").split("-")
filename_list[x_axis] = '*'


lines = []
key_set = {}
for num in range(1, SCAN_RANGE+1):
    filename_list[line_factor] = str(num)
    filenames = glob.glob('-'.join(filename_list)+'.log')
    if len(filenames) == 0:
        continue
    #print(filenames)
    means = {}
    for filename in filenames:
        with open(filename) as file:
            throughputs = []
            for line in file:
                line_list = line.split()
                if int(line_list[0]) == START_SEC:
                    throughputs.append(int(line_list[-1]))
            x_list = filename.replace(".log", "").split("-")
            means[int(x_list[x_axis])] = statistics.mean(throughputs)
    if len(key_set)==0:
        key_set = set(means.keys())
    elif key_set != set(means.keys()):
        raise ValueError("x axis mismatch")
    lines.append((num,means))
    
#print(lines)
x_list = sorted(list(means.keys()))
print(x_list)
for num, means in lines:
    y_list = []
    for x in x_list:
        y_list.append(means[x])
    print(num, ':', y_list)
    plt.plot(x_list, y_list, label=str(num))

plt.legend(loc = "best")
plt.xlabel(x_labels[x_axis])
plt.ylabel("Throughput (commands/sec)")
plt.show()