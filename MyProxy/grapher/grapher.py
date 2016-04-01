import matplotlib.pyplot as plt
import numpy as np
from pylab import *
import os
import csv
import re

"""
Plotting access time results.
TODO: Replace code with actual implementation.
"""
for path in os.listdir("../results/access_time"):
    access_time = []
    file = open("../results/access_time/"+path, "rb")

    for line in file:
        access_time.append(float(line)*1000)
        
    results = np.array(access_time)
    
    x = np.arange(1.0, len(access_time)+1)
    plt.plot(x, access_time, "-", label="Access Time (ms)")
    plt.xlabel("Accesses")
    plt.ylabel("Access Time (ms)")
    plt.savefig("graphs/"+path.replace(".csv", ".png"))
    plt.cla()    