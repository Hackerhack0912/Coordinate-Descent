#!/bin/bash

# iteration number
lm=("lr" "lsr")
lm_stepSize=(0.01 0.00001)
iter_num=20  # iteration number setting
RSM=3.5  # memory size setting
parameter_RSM=(10000000 3 9) # parameters for RSM

echo "iteration number"
# RSM 4*2*3*4 = 96
echo "RSM"
# generate data and create the corresponding table
./gen_data 10 ${parameter_RSM[0]} ${parameter_RSM[1]} ${parameter_RSM[2]} 1.00 s r >>scd_iter_num_RSM_log
./DB create s 0 $((${parameter_RSM[1]}-1)) $((${parameter_RSM[0]}*10)) >>scd_iter_num_RSM_log
./DB create r 1 ${parameter_RSM[2]} ${parameter_RSM[0]} >>scd_iter_num_RSM_log

# 4 experiments for each method (logistic regression and least square)
for l in 0 1;do
    echo ${lm[$l]}
    echo "materialize"
    for k in 0 1 2 3; do
        time ./DB join S R T >>scd_iter_num_RSM_log
        time ./DB m T ${lm_stepSize[$l]} 0 $iter_num ${lm[$l]} $RSM>>scd_iter_num_RSM_log
        rm T*
    done
    echo "stream"
    for k in 0 1 2 3; do
        time ./DB s S R ${lm_stepSize[$l]} 0 $iter_num ${lm[$l]} $RSM>>scd_iter_num_RSM_log
    done
    echo "factorize"
    for k in 0 1 2 3; do
        time ./DB f S R ${lm_stepSize[$l]} 0 $iter_num ${lm[$l]} $RSM>>scd_iter_num_RSM_log
    done
done
# clear the used data
rm s
rm r
rm S*
rm R*
