#!/bin/bash

# tuple ratio RSM
lm=("lr" "lsr")
lm_stepSize=(0.01 0.00001)
tuple_ratio=(1 5 10 15)    # tuple ratio setting
RSM=1  # memory size setting
parameter_RSM=(2000000 15 45 5) # parameters for RSM

echo "tuple ratio"
# RSM 4*2*3*4 = 96
echo "RSM"
# for different tuple ratio
for i in 0 1 2 3;do
    # generate data and create the corresponding table
    ./gen_data ${tuple_ratio[$i]} ${parameter_RSM[0]} ${parameter_RSM[1]} ${parameter_RSM[2]} 1.00 s r >>scd_tuple_ratio_RSM_log
    ./DB create s 0 $((${parameter_RSM[1]}-1)) $((${parameter_RSM[0]}*${tuple_ratio[$i]})) >>scd_tuple_ratio_RSM_log
    ./DB create r 1 ${parameter_RSM[2]} ${parameter_RSM[0]} >>scd_tuple_ratio_RSM_log
    echo "$i"
    # 4 experiments for each method (logistic regression and least square)
    for l in 0 1;do
        echo ${lm[$l]}
        echo "materialize"
        for k in 0 1 2 3; do
            time ./DB join S R T >>scd_tuple_ratio_RSM_log
            time ./DB m T ${lm_stepSize[$l]} 0 ${parameter_RSM[3]} ${lm[$l]} $RSM>>scd_tuple_ratio_RSM_log
            rm T*
        done
        echo "stream"
        for k in 0 1 2 3; do
            time ./DB s S R ${lm_stepSize[$l]} 0 ${parameter_RSM[3]} ${lm[$l]} $RSM>>scd_tuple_ratio_RSM_log
        done
        echo "factorize"
        for k in 0 1 2 3; do
            time ./DB f S R ${lm_stepSize[$l]} 0 ${parameter_RSM[3]} ${lm[$l]} $RSM>>scd_tuple_ratio_RSM_log
        done
    done
    # clear the used data
    rm s
    rm r
    rm S*
    rm R*
done
