import glob
import statistics
import readline
import matplotlib.pyplot as plt

START_SEC = 11
SCAN_RANGE = 1000

x_labels = ["Acceptor Number",
            "Acceptor Thread Number",       #Change tohether
            "Leader Thread Number",         #
            "Commander Thread Number",
            "Replica Thread Number",        #
            "Client Thread Number",
            ]

print('\n'.join(x_labels))

x_axis = int(input('Enter x axis:'))-1
line_factor = int(input('Enter line factor:'))-1

thread_num_as_x_axis = True if x_axis == 1 or x_axis == 2 or x_axis == 4 else False

if x_axis < 0 or x_axis > 5 or line_factor < 0 or line_factor > 5:
    raise ValueError("All values must in 1~6")

if x_axis == line_factor:
    raise ValueError("x axis cannot be same as line factor")

filename_keyword = input("Enter log file name:")
filename_list = filename_keyword.replace(".log", "").split("-")
if thread_num_as_x_axis:
    filename_list[1] = '*'
    filename_list[2] = '*'
    filename_list[4] = '*'
else:
    filename_list[x_axis] = '*'


print('\n\nFetch data')
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
        if thread_num_as_x_axis:
            _, acceptor_num, leader_num, _, replica_num, _ = filename.replace(".log", "").split("-")
            if not (acceptor_num == leader_num == replica_num):
                continue
        with open(filename) as file:
            throughputs = []
            for line in file:
                line_list = line.split()
                if int(line_list[0]) == START_SEC:
                    if int(line_list[-1]) > 0:
                        throughputs.append(int(line_list[-1]))
                    else:
                        print(filename)
                        raise ValueError("Throughputs <= 0")
            x_list = filename.replace(".log", "").split("-")
            means[int(x_list[x_axis])] = statistics.mean(throughputs)
    if len(key_set)<2:
        key_set = set(means.keys())
    if key_set != set(means.keys()):
        print("x axis mismatch:")
        print(filenames)
        print(set(means.keys()))
    else:
        lines.append((num,means))
    
print('\n\nPlot graph')
x_list = sorted(list(lines[0][1].keys()))
print(x_list)
#print(lines)
plt.figure(dpi=150)
for num, means in lines:
    y_list = []
    for x in x_list:
        y_list.append(means[x])
    print(num, ':', y_list)
    plt.plot(x_list, y_list, label='workload='+str(num))

plt.legend(loc = "best")
if thread_num_as_x_axis:
    plt.xlabel('Replica/Leader/Acceptor Thread Number')
else:
    plt.xlabel(x_labels[x_axis])
plt.ylabel("Throughput (commands/sec)")
plt.show()