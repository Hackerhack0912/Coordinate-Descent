# Coordinate-Descent
Overview:

The whole project simulates a "mini column-oriented database" including functionalities such as:
1. Storing data (from txt from binary format);
2. Read data (output readable format from binary format);
3. Runing stochastic gradient descent (sgd), batch gradient descent (bgd), stochastic coordinate descent (scd), 
   block coordinate descent (bcd) to learn generalized linear models including: 
   linear regression, least square, logistic regression.
   For coordinate descent, there are several "optimized" version available: stream-scd, factorize-scd, factorize-bcd

Generating test data:
First build the data generator by running the script below:
./dataGenerator/build.sh

1. Single table T:
   ./dataGenerator/gen_single <Ration of nS:nr> <nR> <dS> <dR> <Variance> <Tfile>
2. S (entity table) and R (attribute table)
   ./dataGenerator/gen_data <Ratio of nS:nR> <nR> <dS> <dR> <Variance> <Sfile> <Rfile> 
   
Build the instruction menu of DB:
./produceMenu.sh

Display the instruction menu of functionalities of DB:
./menu
