import numpy as np
import matplotlib
import matplotlib.pyplot as plt
import parse_result as pr
# usage: python3 parse_result_example.py

# sudo apt-get install python3-tk
# sudo yum install python3-tk
matplotlib.use('TkAgg')

# create the dat file to load with:
# qucsator < rc_ac_sweep.net > rc_ac_sweep.dat

data = pr.parse_file('rc_ac_sweep.dat')

x = data['acfrequency']
y = np.abs(data['out.v'])
c = data['Cx']

plt.loglog(x,y[0,:],'-rx')
plt.loglog(x,y[4,:],'-go')

plt.legend(['Cx=' + str(c[0]), 'Cx=' + str(c[4])])

plt.xlabel('acfrequency')
plt.ylabel('abs(out.v)')
plt.grid()
plt.show()
