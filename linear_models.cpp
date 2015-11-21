//
//  linear_models.cpp
//  Coordinate_descent
//
//  Created by Zhiwei Fan on 10/14/15.
//  Copyright © 2015 Zhiwei Fan. All rights reserved.
//

#include "linear_models.h"
#include <cmath>

#define MAX(x,y) (x > y ? x : y)

linear_models::linear_models(){};

double linear_models::Fe_lr(double a, double b)
{
    double power = -(a*b);
    return log(1+pow(exp(1.0), power));
}

double linear_models::Fe_lsr(double a, double b)
{
    double base = a - b;
    return pow(base, 2);
}

double linear_models::Fe_lsvm(double a, double b)
{
    return MAX(0, 1 - a*b);
}

double linear_models::G_lr(double a, double b)
{
    double power = a*b;
    return -(a/(1+pow(exp(1.0),power)));
}

double linear_models::G_lsr(double a, double b)
{
    return 2*(b-a);
}

//Unfinished
double linear_models::G_svm(double a, double b)
{
    return 0;
}
