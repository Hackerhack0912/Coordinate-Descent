//
//  DataManagement.h
//  Coordinate_descent
//
//  Created by Zhiwei Fan on 10/2/15.
//  Copyright (c) 2015 Zhiwei Fan. All rights reserved.
//

#ifndef __Coordinate_descent__DataManagement__
#define __Coordinate_descent__DataManagement__

using namespace std;
#include <vector>
#include <stdio.h>
#include <string>
//Read data from table stored in text file and loaded into the DB
class DataManagement
{
public:
    DataManagement();
    void store(string FileName, int feature_num, int table_type, long row_num);
    vector<string> split(const string &s, char delim);
    void read(string fileName, long row_num);
};

#endif /* defined(__Coordinate_descent__DataManagement__) */
