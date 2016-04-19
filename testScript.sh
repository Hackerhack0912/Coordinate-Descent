#!/bin/bash
echo experiment
for i in 1 2 3 4;do
	time ./DB m T 0.0001 0 5 lr 0.5 >timelog
done 
