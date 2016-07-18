#!/bin/bash

# MEM=("RSM" "RMM" "RLM")

echo "tuple ratio(nS/nR)"
nR=(2000000 2000000 2000000)
dS=(15 15 15)
dR=(45 45 45)

for j in 1 5 10 15;do
    echo "tuple ratio: "$j
    ./compute_memory 0 $j ${nR[$i]} ${dS[$i]} ${dR[$i]} scd
done

echo " "


echo "feature ratio(dR/dS)"
nS=(20000000 20000000 20000000)
nR=(2000000 2000000 2000000)
dS=(15 15 15)


for j in 1 2 3 4 5;do
    echo "feature ratio: "$j
    ./compute_memory 1 $j ${nS[$i]} ${nR[$i]} ${dS[$i]} scd
done

echo " "

echo "iteration number"
nS=(20000000 20000000 20000000)
nR=(2000000 2000000 2000000)
dS=(15 15 15)
dR=(45 45 45)

./compute_memory 2 ${nS[$i]} ${nR[$i]} ${dS[$i]} ${dR[$i]} scd

