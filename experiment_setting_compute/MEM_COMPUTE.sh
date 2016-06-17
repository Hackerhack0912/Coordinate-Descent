#!/bin/bash

# MEM=("RSM" "RMM" "RLM")

echo "tuple ratio(nS/nR)"
nR=(10000000 10000000 10000000)
dS=(3 3 3)
dR=(9 9 9)

for j in 1 5 10 15;do
    echo "tuple ratio: "$j
    ./compute_memory 0 $j ${nR[$i]} ${dS[$i]} ${dR[$i]} scd
done

echo " "


echo "feature ratio(dR/dS)"
nS=(100000000 100000000 100000000)
nR=(10000000 10000000 10000000)
dS=(3 3 3)


for j in 1 2 3 4 5;do
    echo "feature ratio: "$j
    ./compute_memory 1 $j ${nS[$i]} ${nR[$i]} ${dS[$i]} scd
done

echo " "

echo "iteration number"
nS=(100000000 100000000 100000000)
nR=(10000000 10000000 10000000)
dS=(3 3 3)
dR=(9 9 9)

./compute_memory 2 ${nS[$i]} ${nR[$i]} ${dS[$i]} ${dR[$i]} scd

