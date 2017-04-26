import csv
import sys
if len(sys.argv) != 2:
	print "Usage: python chart_gen.py <file_name.csv>"
	sys.exit(1)
if " " in sys.argv[1]: 
	print "file name should not contain space"
	sys.exit(1)
file_name = sys.argv[1]
# print file_name
csvfile = open(file_name,"r")
spamreader = csv.reader(csvfile, delimiter=',')

success = {}
for row in spamreader:
	if len(row) < 4:
		print len(row)
		print "wrong format"
		sys.exit(1)
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
	data_stack.append(success[i+1])


yTicks = [ x for x in range(91) if x%10 ==0]
# print yTicks
d = np.array(data_stack)


d_widths = [1]*N
d_labels = [ str(round(sum(success[x+1])*100/90,2))+"%\n " + str(x+1)for x in range(N)]
legends = ['ip','user name','file path']
d_colors = ['#2166ac', '#fee090', '#fdbb84']

fig = plt.figure()

ax = fig.add_subplot(111)
SBG.stackedBarPlot(ax,
                   d,
                   d_colors,
                   xLabels=d_labels,
                   gap=0.2,
                   yTicks =[yTicks,yTicks],
                   legends=legends, 
                   scale=False,
                   endGaps=True
                  )
plt.title("Volume Change Effect")
plt.xlabel("Volume Key")
plt.yticks(np.arange(0, 91, 10))
plt.ylabel("Success Counts")
plt.savefig(file_name[:-4]+".png",bbox_inches='tight')
plt.close()
import commands
commands.getstatusoutput("open "+file_name[:-4]+".png")
