import csv
csvfile = open("Volume_change_effect_on Transfer.csv","r")
spamreader = csv.reader(csvfile, delimiter=',')

success = {}
for row in spamreader:
	if not row[0].isdigit(): continue
	key = int(row[0])
	if key not in success: success[key] = [0.0 for x in range(3)]
	success[key][0]+=int(row[1])
	success[key][1]+=int(row[2])
	success[key][2]+=int(row[3])

# a stacked bar plot with errorbars
import numpy as np
import matplotlib.pyplot as plt
from stackedBarGraph import StackedBarGrapher
SBG = StackedBarGrapher()



N = 8
ind = np.arange(N)    
data_stack = [] 
for i in ind:
	data_stack.append([success[i+1][0],success[i+1][1],success[i+1][2]])



d = np.array(data_stack)


d_widths = [1]*N
d_labels = [ str(x+1)+" : "+str(float("{0:.2f}".format(float(sum([l for l in success[x+1]]))/90)))+"%"  for x in range(N)]
legends = ['ip','user name','file path']
d_colors = ['#2166ac', '#fee090', '#fdbb84']

fig = plt.figure()

ax = fig.add_subplot(111)
SBG.stackedBarPlot(ax,
                   d,
                   d_colors,
                   xLabels=d_labels,
                   gap=0.2,
                   yTicks=3,
                   legends=legends, 
                   scale=False
                  )
plt.title("Volume Change Effect")
plt.xlabel("Volume Key")
plt.ylabel("Success Counts out of 90")
plt.savefig("measurement.png",bbox_inches='tight')
plt.close()

