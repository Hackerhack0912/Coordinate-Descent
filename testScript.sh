#!/bin/bash
echo experiment
for i in 1 2 3 4;do
        ./DB m T 0.01 0 10000 lr 0.005 >>log
	time ./DB m T 0.01 0 100 lr 0.0005 >timelog
done 
