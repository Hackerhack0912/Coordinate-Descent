#!/bin/bash

# iteration number
lm=("lr" "lsr")
lm_stepSize=(0.01 0.00001)
iter_num=(5 10 20 40)    # iteration number setting
RSM=(1.80 1.80 1.80 1.80)  # memory size setting
RLM=(1.00 1.00 1.00 1.00)  # memory size setting
parameter_RSM=(5000000 40 60) # parameters for RSM
parameter_RLM=(500000 6 9) # parameters for RLM

echo "iteration number"
# RSM 4*2*3*4 = 96
echo "RSM"
# for different iteration number
for i in 0 1 2 3;do
    # generate data and create the corresponding table
    ./gen_data 10 ${parameter_RSM[0]} ${parameter_RSM[1]} ${parameter_RSM[2]} 1.00 s r >>scd_iter_num_RSM_log
    ./DB create s 0 $((${parameter_RSM[1]}-1)) $((${parameter_RSM[0]}*10)) >>scd_iter_num_RSM_log
    ./DB create r 1 ${parameter_RSM[2]} ${parameter_RSM[0]} >>scd_iter_num_RSM_log
    echo "$i"
    # 4 experiments for each method (logistic regression and least square)
    for l in 0 1;do
        echo ${lm[$l]}
        echo "materialize"
        for k in 0 1 2 3; do
            time ./DB join S R T >>scd_iter_num_RSM_log
            time ./DB m T ${lm_stepSize[$l]} 0 ${iter_num[$i]} ${lm[$l]} ${RSM[$i]} >>scd_iter_num_RSM_log
            rm T*
        done
        echo "stream"
        for k in 0 1 2 3; do
            time ./DB s S R ${lm_stepSize[$l]} 0 ${iter_num[$i]} ${lm[$l]} ${RSM[$i]} >>scd_iter_num_RSM_log
        done
        echo "factorize"
        for k in 0 1 2 3; do
            time ./DB f S R ${lm_stepSize[$l]} 0 ${iter_num[$i]} ${lm[$l]} ${RSM[$i]} >>scd_iter_num_RSM_log
        done
    done
    # clear the used data
    rm s
    rm r
    rm S*
    rm R*
done

# RLM 4*2*3*4 = 96
echo "RLM"
# for different iteration number
for i in 0 1 2 3;do
    # generate data and create the corresponding table
    ./gen_data 10 ${parameter_RLM[0]} ${parameter_RLM[1]} ${parameter_RLM[2]} 1.00 s r >>scd_iter_num_RLM_log
    ./DB create s 0 $((${parameter_RLM[1]}-1)) $((${parameter_RLM[0]}*10)) >>scd_iter_num_RLM_log
    ./DB create r 1 ${parameter_RLM[2]} ${parameter_RLM[0]} >>scd_iter_num_RLM_log
    echo "$i"
    # 4 experiments for each method (logistic regression and least square)
    for l in 0 1;do
        echo ${lm[$l]}
        echo "materialize"
        for k in 0 1 2 3; do
            time ./DB join S R T >>scd_iter_num_RLM_log
            time ./DB m T ${lm_stepSize[$l]} 0 ${iter_num[$i]} ${lm[$l]} ${RLM[$i]} >>scd_iter_num_RLM_log
            rm T*
        done
        echo "stream"
        for k in 0 1 2 3; do
            time ./DB s S R ${lm_stepSize[$l]} 0 ${iter_num[$i]} ${lm[$l]} ${RLM[$i]} >>scd_iter_num_RLM_log
        done
        echo "factorize"
        for k in 0 1 2 3; do
            time ./DB f S R ${lm_stepSize[$l]} 0 ${iter_num[$i]} ${lm[$l]} ${RLM[$i]} >>scd_iter_num_RLM_log
        done
    done
    # clear the used data
    rm s
    rm r
    rm S*
    rm R*
done