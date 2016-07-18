#/bin/bash

# iteration number
lm=("lr" "lsr")
lm_stepSize=(0.01 0.00001)
iter_num=5 # iteration number setting
RLM=48 # memory size setting
parameter_RLM=(2000000 15 45) # parameters for RSM

echo "iteration number"
# RLM 4*2*3*4 = 96
echo "RLM"
# generate data and create the corresponding table
./gen_data 10 ${parameter_RLM[0]} ${parameter_RLM[1]} ${parameter_RLM[2]} 1.00 s r >>scd_iter_num_RLM_log
./DB create s 0 $((${parameter_RLM[1]}-1)) $((${parameter_RLM[0]}*10)) >>scd_iter_num_RLM_log
./DB create r 1 ${parameter_RLM[2]} ${parameter_RLM[0]} >>scd_iter_num_RLM_log

# 4 experiments for each method (logistic regression and least square)
for l in 0 1;do
    echo ${lm[$l]}
    echo "materialize"
    for k in 0 1 2 3; do
        time ./DB join S R T >>scd_iter_num_RLM_log
        time ./DB m T ${lm_stepSize[$l]} 0 $iter_num ${lm[$l]} $RLM>>scd_iter_num_RLM_log
        rm T*
    done
    echo "stream"
    for k in 0 1 2 3; do
        time ./DB s S R ${lm_stepSize[$l]} 0 $iter_num ${lm[$l]} $RLM>>scd_iter_num_RLM_log
    done
    echo "factorize"
    for k in 0 1 2 3; do
        time ./DB f S R ${lm_stepSize[$l]} 0 $iter_num ${lm[$l]} $RLM>>scd_iter_num_RLM_log
    done
done
# clear the used data
rm s
rm r
rm S*
rm R*

