//
//  main.cpp
//  Coordinate_descent
//
//  Created by Zhiwei Fan on 10/2/15.
//  Copyright (c) 2015 Zhiwei Fan. All rights reserved.
//

#include <iostream>
#include "DataManagement.h"

struct setting
{
    int iter_num;
    double error;
};

bool stop(int k, double r_prev, double r_curr, setting &setting);

int main(int argc, const char * argv[]) {
    // insert code here...
    DataManagement dM;
    dM.store("/Users/Hacker/Box Sync/Research/Coordinate_descent/Coordinate_descent/s", 3, 0, 500);
    return 0;
}

bool stop(int k, double r_prev, double r_curr, setting *setting)
{
    double iter_num = k;
    double difference = r_prev - r_curr;
    return true;
}