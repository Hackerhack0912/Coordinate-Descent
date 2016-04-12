#!/bin/bash
echo experiment
for i in 1 2 3 4;do
	time ./DB m T 0.01 0 100000 lr 0.0005 >timelog
done 
