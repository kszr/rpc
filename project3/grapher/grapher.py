import matplotlib.pyplot as plt
import numpy as np
from pylab import *
import os
import re

"""
LRU Policy
"""
hit_rate_lru = []
cache_usage_lru = []
access_time_lru = []
file = open("../results/lru/lru_lru-good.txt");
for line in file:
    text = line.split(",")
    hit_rate_lru.append((float(text[2])-float(text[1]))/float(text[2]))
    cache_usage_lru.append(float(text[3])/1024.0/float(text[0])*100)
    access_time_lru.append(float(text[4])*1000)
    
hit_rate_ran = []
cache_usage_ran = []
access_time_ran = []
file = open("../results/lru/lru_random.txt");
for line in file:
    text = line.split(",")
    hit_rate_ran.append((float(text[2])-float(text[1]))/float(text[2]))
    cache_usage_ran.append(float(text[3])/1024.0/float(text[0])*100)
    access_time_ran.append(float(text[4])*1000)

x = np.arange(6.0, 11.0)
plt.plot(x, hit_rate_lru, "-", label="Hit Rate")
plt.plot(x, hit_rate_ran, "-", label="Hit Rate")
plt.xlabel("log(cache size)")
plt.ylabel("Hit Rate")
plt.legend(("lru_good", "random"), loc="best", title="Workloads")
plt.title('LRU Hit Rate')
plt.savefig("graphs/lru_hit_rate.png")
plt.cla()

x = np.arange(6.0, 11.0)
plt.plot(x, cache_usage_lru, "-", label="Avg. Cache Usage %")
plt.plot(x, cache_usage_ran, "-", label="Avg. Cache Usage %")
plt.xlabel("log(cache size)")
plt.ylabel("Avg. Cache Usage %")
plt.legend(("lru_good", "random"), loc="best", title="Workloads")
plt.title('LRU Avg. Cache Usage')
plt.savefig("graphs/lru_cache_usage.png")
plt.cla()

x = np.arange(6.0, 11.0)
plt.plot(x, access_time_lru, "-", label="Avg. Access Time (ms)")
plt.plot(x, access_time_ran, "-", label="Avg. Access Time (ms)")
plt.xlabel("log(cache size)")
plt.ylabel("Avg. Access Time (ms)")
plt.legend(("lru_good", "random"), loc="best", title="Workloads")
plt.title('LRU Avg. Access Time')
plt.savefig("graphs/lru_access_time.png")
plt.cla()

"""
RND Policy
"""
hit_rate_lru = []
cache_usage_lru = []
access_time_lru = []
file = open("../results/random/rnd_lru-good.txt");
for line in file:
    text = line.split(",")
    hit_rate_lru.append((float(text[2])-float(text[1]))/float(text[2]))
    cache_usage_lru.append(float(text[3])/1024.0/float(text[0])*100)
    access_time_lru.append(float(text[4])*1000)
    
hit_rate_ran = []
cache_usage_ran = []
access_time_ran = []
file = open("../results/random/rnd_random.txt");
for line in file:
    text = line.split(",")
    hit_rate_ran.append((float(text[2])-float(text[1]))/float(text[2]))
    cache_usage_ran.append(float(text[3])/1024.0/float(text[0])*100)
    access_time_ran.append(float(text[4])*1000)

x = np.arange(6.0, 11.0)
plt.plot(x, hit_rate_lru, "-", label="Hit Rate")
plt.plot(x, hit_rate_ran, "-", label="Hit Rate")
plt.xlabel("log(cache size)")
plt.ylabel("Hit Rate")
plt.legend(("lru_good", "random"), loc="best", title="Workloads")
plt.title('Random Hit Rate')
plt.savefig("graphs/rnd_hit_rate.png")
plt.cla()

x = np.arange(6.0, 11.0)
plt.plot(x, cache_usage_lru, "-", label="Avg. Cache Usage %")
plt.plot(x, cache_usage_ran, "-", label="Avg. Cache Usage %")
plt.xlabel("log(cache size)")
plt.ylabel("Avg. Cache Usage %")
plt.legend(("lru_good", "random"), loc="best", title="Workloads")
plt.title('Random Avg. Cache Usage')
plt.savefig("graphs/rnd_cache_usage.png")
plt.cla()

x = np.arange(6.0, 11.0)
plt.plot(x, access_time_lru, "-", label="Avg. Access Time (ms)")
plt.plot(x, access_time_ran, "-", label="Avg. Access Time (ms)")
plt.xlabel("log(cache size)")
plt.ylabel("Avg. Access Time (ms)")
plt.legend(("lru_good", "random"), loc="best", title="Workloads")
plt.title('Random Avg. Access Time')
plt.savefig("graphs/rnd_access_time.png")
plt.cla()

"""
GDS Policy
"""
hit_rate_lru = []
cache_usage_lru = []
access_time_lru = []
file = open("../results/gds/gds_lru-good.txt");
for line in file:
    text = line.split(",")
    hit_rate_lru.append((float(text[2])-float(text[1]))/float(text[2]))
    cache_usage_lru.append(float(text[3])/1024.0/float(text[0])*100)
    access_time_lru.append(float(text[4])*1000)
    
hit_rate_ran = []
cache_usage_ran = []
access_time_ran = []
file = open("../results/gds/gds_random.txt");
for line in file:
    text = line.split(",")
    hit_rate_ran.append((float(text[2])-float(text[1]))/float(text[2]))
    cache_usage_ran.append(float(text[3])/1024.0/float(text[0])*100)
    access_time_ran.append(float(text[4])*1000)

x = np.arange(6.0, 11.0)
plt.plot(x, hit_rate_lru, "-", label="Hit Rate")
plt.plot(x, hit_rate_ran, "-", label="Hit Rate")
plt.xlabel("log(cache size)")
plt.ylabel("Hit Rate")
plt.legend(("lru_good", "random"), loc="best", title="Workloads")
plt.title('GDS Hit Rate')
plt.savefig("graphs/gds_hit_rate.png")
plt.cla()

x = np.arange(6.0, 11.0)
plt.plot(x, cache_usage_lru, "-", label="Avg. Cache Usage %")
plt.plot(x, cache_usage_ran, "-", label="Avg. Cache Usage %")
plt.xlabel("log(cache size)")
plt.ylabel("Avg. Cache Usage %")
plt.legend(("lru_good", "random"), loc="best", title="Workloads")
plt.title('GDS Avg. Cache Usage')
plt.savefig("graphs/gds_cache_usage.png")
plt.cla()

x = np.arange(6.0, 11.0)
plt.plot(x, access_time_lru, "-", label="Avg. Access Time (ms)")
plt.plot(x, access_time_ran, "-", label="Avg. Access Time (ms)")
plt.xlabel("log(cache size)")
plt.ylabel("Avg. Access Time (ms)")
plt.legend(("lru_good", "random"), loc="best", title="Workloads")
plt.title('GDS Avg. Access Time')
plt.savefig("graphs/gds_access_time.png")
plt.cla()