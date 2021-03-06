#!/bin/bash
echo "Test I/O(lr)"
feature=(1 2 5 10 15 20)
tuple_nums=(1789569706 1342177280 766958445 447392426 315806418 244032232)

for i in 0 1 2 3 4 5;do
	./gen_data 1 ${tuple_nums[$i]} ${feature[$i]} 1 1.00 s r 2
	./DB create s 2 ${feature[$i]} ${tuple_nums[$i]} 
	clear s
	echo "io_time"
	time ./DB read T >> io_time
	echo "total_time"
	time ./DB m T 0.01 0 1 lsr 60 >> total_time
	clear T
done

