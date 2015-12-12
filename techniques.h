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
#include <vector>

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
    void SGD(vector< vector<double> > data, setting _setting, double *&model, int feature_num);
    void BGD(vector< vector<double> > data, setting _setting, double *&model, int feature_num);
    void classify(vector< vector<double> > data, vector<double> model);
private:
    vector<int> shuffle(vector<int> &index_set, unsigned seed);
    vector<long> shuffle(vector<long> &index_set, unsigned seed);
};


#endif /* defined(_techniques_) */
