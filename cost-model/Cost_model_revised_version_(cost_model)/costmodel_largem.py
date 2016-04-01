import sys,math,numpy
import matplotlib.pyplot as plt

#IO Costs:
#---------

IORATE = 100*1024*1024 #100 MB/sec modern SATA HDD read/write throughput
#I also include F_e with G in this cpu costs

def RSTp(p, nS, nR, dS, dR):
	Scol = math.ceil((nS * 8) / p)
	Rcol = math.ceil((nR * 8) / p)
	#Sp = (2 + dS) * Scol
	#Rp = (1 + dR) * Rcol
	#Tp = (1 + dS + dR) * Scol
	return (Scol, Rcol)

# Memory space required: Y, H, X, (Extra output for first iteration)
def IOm_M(p, f, m, nS, nR, dS, dR, Iters):
	(Scol, Rcol) = RSTp(p, nS, nR, dS, dR)
	Tpp = (dS + dR - 1) * Scol
	io_m = (1 + dS) * Scol + (1 + dR) * Rcol + \
		(1 + Iters) * Tpp + \
		- min(Tpp, m - 4 * Scol - f * Rcol) \
		- (Iters - 1) * min(Tpp, m - 3 * Scol)
	if (Tpp < m - 4 * Scol - f * Rcol) :
		io_m -= Tpp
	return io_m

def IOm_S(p, f, m, nS, nR, dS, dR, Iters):
	(Scol, Rcol) = RSTp(p, nS, nR, dS, dR)
	io_s =  Scol + \
		Iters * (dS * Scol + (1 + dR) * Rcol) \
		 - (Iters - 1) * min(dS * Scol + (1 + dR) * Rcol, m - 4 * Scol - f * Rcol)
	return io_s

def IOm_Fact(p, f, m, nS, nR, dS, dR, Iters):
	(Scol, Rcol) = RSTp(p, nS, nR, dS, dR)
	io_fact = 2 * Scol + Rcol + \
		Iters * ((dS - 1) * Scol + dR * Rcol) \
		- (Iters - 1) * min((dS - 1) * Scol + dR * Rcol, m - 4 * Scol)
	return io_fact

#----------------------------------------

#CPU Costs:
#---------

if(0) : #prev cost model
	HASH = 200e-9
	COMP = 4e-9
	COPY = 4e-9
	ADD = 10e-9
	MULT =  20e-9
	#FUNC = 10e-9 #LSR
	FUNC = 300e-9 #LR

HASH = 100e-9 #40e-9
COPY = 4e-10
COMP = 4e-9
ADD = 4e-9
MULT =  4e-9

#FUNC = 60e-9 #LR
#FUNC2 = 80e-9 #LR

#FUNC = ADD + MULT #LSR
#FUNC2 = FUNC #LSR

FUNC = ADD + MULT + COMP #LSVM
FUNC2 = MULT + COMP #LSVM


def CPU_M(p, f, m, nS, nR, dS, dR, Iters):
	cpu_m = (nR + nS) * HASH \
		+ (nS + nR) * COPY \
		+ (nS * (1 + dS) + nR * dR) * COPY \ #Why nR*dR and Why nS*(1+dS)
        + f*nS**COMP\
		+ Iters * ( \
			+ (dS + dR - 1) * ( \
				nS * FUNC \
				+ nS * (ADD + MULT) \
				+ 2 * ADD + MULT \
				+ nS * (ADD + MULT) \
			) \
			+ nS * (FUNC2 + ADD) \
		)
	return cpu_m

def CPU_S(p, f, m, nS, nR, dS, dR, Iters):
	cpu_s = Iters * ( \
			(nR + nS) * HASH \
			+ (nR + nS) * COPY \
            + f*nS*COMP\
			+ dR * (\
				nS * COPY \
				+ nS * FUNC \
				+ nS * (ADD + MULT) \
				+ 2 * ADD + MULT \
				+ nS * (ADD + MULT) \
			) \
			+ (dS - 1) * ( \
				nS * FUNC \
				+ nS * (ADD + MULT) \
				+ 2 * ADD + MULT \
				+ nS * (ADD + MULT) \
			) \
			+ nS * (FUNC2 + ADD) \
		)
	return cpu_s

def CPU_Fact(p, f, m, nS, nR, dS, dR, Iters):
	cpu_fact = (nR + nS) * HASH \
		+ (nR + nS) * COPY \
        + f*nS**COMP\    #Why ** 
		+ Iters * ( \
			dR * ( \
				nS * FUNC \
				+ nR * MULT \
				+ nS * ADD \
				+ 2 * ADD + MULT \
				+ nR * MULT \
				+ nS * ADD
			) \
			+ (dS - 1) * ( \
				nS * FUNC \
				+ nS * (ADD + MULT) \
				+ 2 * ADD + MULT \
				+ nS * (ADD + MULT) \
			) \
			+ nS * (FUNC2 + ADD) \
		)
	return cpu_fact
