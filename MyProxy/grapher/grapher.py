import matplotlib.pyplot as plt
import numpy as np
from pylab import *
import os
import re

"""
Plotting MPI MCS and Tournament Barriers.
TODO: Replace code with actual implementation.
"""
mcs_vals = []
file = open("aggregate_results/mpi_mcs_final.txt", "r")
for line in file:
    p = re.compile("([0-9]+): ([0-9]+.[0-9]+)")
    m = p.search(line)
    if m:
        mcs_vals.append(float(m.groups()[1]))
mcs_results = np.array(mcs_vals)

tournament_vals = []
file = open("aggregate_results/mpi_tournament_final.txt", "r")
for line in file:
    p = re.compile("([0-9]+): ([0-9]+.[0-9]+)")
    m = p.search(line)
    if m:
        tournament_vals.append(float(m.groups()[1]))
tournament_results = np.array(tournament_vals)

x = np.arange(2.0, 13.0)
plt.plot(x, mcs_results, "o-", label="MCS")
plt.plot(x, tournament_results, "s-", label="Tournament")
plt.xlabel("Number of Processors")
plt.ylabel(r"Time ($\mu$s)")
plt.legend(("MCS","Tournament"), loc="upper left")
plt.savefig("graphs/MCS_Tourney.png")