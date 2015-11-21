//
//  techniques.hpp
//  Coordinate_descent
//
//  Created by Zhiwei Fan on 10/13/15.
//  Copyright © 2015 Zhiwei Fan. All rights reserved.
//

#ifndef _techniques_
#define _techniques_

using namespace std;
#include <stdio.h>
#include <string>

struct setting
{
    int iter_num;
    double error;
    double step_size;
};

class techniques
{
public:
    techniques();
    void materialize(string table_T, setting _setting, double *model);
    void stream(string table_S, string table_R, setting _setting, double *model);
    void factorize(string table_S, string table_R, setting _setting, double *model);
    bool stop(int k, double r_prev, double r_curr, setting &setting);
};


#endif /* defined(_techniques_) */
