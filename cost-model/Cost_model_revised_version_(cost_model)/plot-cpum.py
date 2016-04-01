import sys,math,numpy
import matplotlib.pyplot as plt

import costmodel_largem

p = 1024*1024 #pagesize, 1MB
f = 1.4

# RSM: 1GB
# RMM: 50GB
# RLM:  1TB
m = 1*1024

nS = 5e8
nR = 5e7

dS = 8
dR = 40

Iters = 20

def CPU_M(p=p, f=f, m=m, nS=nS, nR=nR, dS=dS, dR=dR, Iters=Iters):
	return costmodel_largem.CPU_M(p, f, m, nS, nR, dS, dR, Iters)

def CPU_S(p=p, f=f, m=m, nS=nS, nR=nR, dS=dS, dR=dR, Iters=Iters):
	return costmodel_largem.CPU_S(p, f, m, nS, nR, dS, dR, Iters)

def CPU_Fact(p=p, f=f, m=m, nS=nS, nR=nR, dS=dS, dR=dR, Iters=Iters):
	return costmodel_largem.CPU_Fact(p, f, m, nS, nR, dS, dR, Iters)

plt.clf()

vals = map(lambda x:1024*x, range(1,900,1)) #mem pages
fig = plt.figure(figsize=(7, 5))
plt.ylabel('CPU Time (min) in Logscale')
plt.xlabel('Buffer Memory (MB) in Logscale')
plt.yscale('log')
plt.xscale('log')
plt.plot(vals, map(lambda v:CPU_M(m = v)/60, vals), linewidth=3.0, linestyle='--', color='blue')
plt.plot(vals, map(lambda v:CPU_S(m = v)/60, vals), linewidth=3.0, linestyle='-.', color='red')
plt.plot(vals, map(lambda v:CPU_Fact(m = v)/60, vals), linewidth=2.0, linestyle='-', color='black')
plt.legend(('Materialize(M)', 'Stream (S)', 'Factorize (F)'), loc='best')
#plt.ylim(ymin=0)
plt.savefig("nS-5e8-nR-5e7-dS-40-dR-60-m-24gb-p-1mb-i-20-CPUm-Vary-m.png")
plt.clf()

vals = map(lambda x:dS*0.5*x, range(1,20))
plt.ylabel('CPU Time (min)')
plt.xlabel('Feature Ratio R:S (dR/dS)')
#plt.yscale('log')
plt.plot(map(lambda v:v/dS, vals), map(lambda v:CPU_M(dR = v)/60, vals), linewidth=3.0, linestyle='--', color='blue')
plt.plot(map(lambda v:v/dS, vals), map(lambda v:CPU_S(dR = v)/60, vals), linewidth=3.0, linestyle='-.', color='red')
plt.plot(map(lambda v:v/dS, vals), map(lambda v:CPU_Fact(dR = v)/60, vals), linewidth=2.0, linestyle='-', color='black')
plt.ylim(ymin=0)
plt.legend(('Materialize(M)', 'Stream (S)','Factorize (F)'), loc='best')
plt.savefig("nS-5e8-nR-5e7-dS-40-dR-60-m-24gb-p-1mb-i-20-CPUm-Vary-dR.png")
plt.clf()

vals = map(lambda x:nR*1.0*x, range(1,50))
plt.ylabel('CPU Time (min)')
plt.xlabel('Tuple Ratio S:R (nS/nR)')
#plt.yscale('log')
plt.plot(map(lambda v:v/nR, vals), map(lambda v:CPU_M(nS = v)/60, vals),linewidth=3.0, linestyle='--', color='blue')
plt.plot(map(lambda v:v/nR, vals), map(lambda v:CPU_S(nS = v)/60, vals),linewidth=3.0, linestyle='-.', color='red')
plt.plot(map(lambda v:v/nR, vals), map(lambda v:CPU_Fact(nS = v)/60, vals),linewidth=2.0, linestyle='-', color='black')
plt.ylim(ymin=0)
plt.legend(('Materialize(M)', 'Stream (S)', 'Factorize (F)'), loc='best')
plt.savefig("nS-5e8-nR-5e7-dS-40-dR-60-m-24gb-p-1mb-i-20-CPUm-Vary-nS.png")
plt.clf()

vals = range(1,41,2) #Iters
plt.ylabel('CPU Time (min)')
plt.xlabel('Number of Iterations')
plt.plot(vals, map(lambda v:CPU_M(Iters = v)/60, vals), linewidth=3.0, linestyle='--', color='blue')
plt.plot(vals, map(lambda v:CPU_S(Iters = v)/60, vals), linewidth=3.0, linestyle='-.', color='red')
plt.plot(vals, map(lambda v:CPU_Fact(Iters = v)/60, vals), linewidth=2.0, linestyle='-', color='black')
plt.ylim(ymin=0)
plt.legend(('Materialize(M)', 'Stream (S)', 'Factorize (F)'), loc='best')
plt.savefig("nS-5e8-nR-5e7-dS-40-dR-60-m-24gb-p-1mb-i-20-CPUm-Vary-i.png")
plt.clf()
