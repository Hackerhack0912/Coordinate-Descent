#!/bin/bash

MEM=("RSM" "RMM" "RLM")

echo "tuple ratio(nS/nR)"
nR=(5000000 1000000 500000)
dS=(40 40 6)
dR=(60 60 9)

for i in 0 1 2;do
	echo ${MEM[$i]}
	for j in 1 5 10 15;do
		./compute_memory 0 $j ${nR[$i]} ${dS[$i]} ${dR[$i]} scd  
	done
done
echo " "


echo "feature ratio(dR/dS)"
nS=(50000000 10000000 5000000)
nR=(5000000 1000000 500000)
dS=(40 40 6)

for i in 0 1 2; do 
	echo ${MEM[$i]}
	for j in 0.5 1.5 3 4.5;do
		./compute_memory 1 $j ${nS[$i]} ${nR[$i]} ${dS[$i]} scd
	done 
done 
echo " "

echo "iteration number"
nS=(50000000 10000000 5000000)
nR=(5000000 1000000 500000)
dS=(40 40 6)
dR=(60 60 9)

for i in 0 1 2; do
	echo ${MEM[$i]}
	./compute_memory 2 ${nS[$i]} ${nR[$i]} ${dS[$i]} ${dR[$i]} scd
done
