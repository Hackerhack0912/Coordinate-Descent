//
//  linear_models.hpp
//  Coordinate_descent
//
//  Created by Zhiwei Fan on 10/14/15.
//  Copyright © 2015 Zhiwei Fan. All rights reserved.
//

#ifndef _linear_models_
#define _linear_models_

#include <stdio.h>

class linear_models{
public:
    linear_models();
    double Fe_lr(double a, double b);
    double Fe_lsr(double a, double b);
    double Fe_lsvm(double a, double b);
    double G_lr(double a, double b);
    double G_lsr(double a, double b);
    double G_svm(double a, double b);
    double C_lr(double a); //Confidence Calculation of logistic regression
};


#endif /* _linear_models_ */
