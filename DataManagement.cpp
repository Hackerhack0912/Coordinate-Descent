//
//  DataManagement.cpp
//  Coordinate_descent
//
//  Created by Zhiwei Fan on 10/2/15.
//  Copyright (c) 2015 Zhiwei Fan. All rights reserved.
//


#include "DataManagement.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>


DataManagement::DataManagement(){};

/**
 @param fileName:the name of the file storing the table to be stored
 @param featurenum: number of features in the table
 @param table_type: 0 if the table to be stored is entity table S; 1 if the table to be stored is attribute table R
 @param row_num: the number of rows in the table (number of lines in the input file)
 @corresponding file format for table S: sid,fk,y(label),x_s[](feature vector)
 @corresponding file format for table R: rid,x_r[](feature vector)
 Note: All entries in the tables are assumed to be numeric
 **/
void DataManagement::store(string fileName, int feature_num, int table_type, long row_num)
{
    string table_name;
    vector<string> *table_field;
    int col_num = 0;
    //Decide the number of fields according to the table type and store the corresponding name of each field
    if(table_type == 0)
    {
        cout<<"The table to be stored is entity table S"<<endl;
        //sid + label + fk + x_s[]
        col_num = 1 + 1 + 1 + feature_num;
        table_name = "S";
        table_field = new vector<string>(col_num);
        table_field->at(0) = "sid";
        table_field->at(1) = "label";
        table_field->at(2) = "fk";
        for(int i = 0; i < feature_num; i ++)
        {
            table_field->at(3+i) = "x_s" + to_string(i);
        }
    }
    else if(table_type == 1){
        cout<<"The table to be stores is attribute table R"<<endl;
        //rid + x_r
        col_num = 1 + feature_num;
        table_name = "R";
        table_field = new vector<string>[col_num];
        table_field->at(0) = "rid";
        for(int i = 0; i < feature_num; i ++)
        {
            table_field->at(1+i) = "x_r" + to_string(i);
        }
    }
    else{
        cerr<<"Error Message: "<<"Invalid table type identifier given"<<endl;
        exit(1);
    }
    
    //Create a 2-D array to store the data (in column-store version)
    /**
     Note: This is under the assumption that all columns will perfectly fit in memory for convenience of data reading.
           A more realistic but much more redundant alternative would be provided later
     **/
    double** table = new double*[col_num];
    for(int i = 0; i < col_num; i ++)
    {
        table[i] = new double[row_num];
    }
    
    ifstream infile;
    infile.open(fileName);
    
    
    //Memory free case: all columns can fit in memory the same time
    if(!infile.is_open())
    {
        cerr<<"Error Message: "<<"Cannot open the file: the name of the file might be wrong or the file might be broken"<<endl;
        exit(1);
    }
    
    // All valid files should seperate the columns with ' ' (one space)
    char delim = ' ';
    int row_count = 0;
    while(!infile.eof())
    {
        string s;
        getline(infile, s);
        vector<string> *tuple = new vector<string>;
        *tuple = split(s,delim);
        //Write the value in each filed of current tuple into the corresponding columns
        for(int i = 0; i < col_num; i ++)
        {
            const char *cstr = tuple->at(i).c_str();
            table[i][row_count] = atof(cstr);
        }
        delete tuple;
        row_count ++;
    }
    infile.close();
    
    //Write each column into the corresponding binary file
    double *cur_col = new double[row_num];
    for(int i = 0; i < col_num; i ++)
    {
        ofstream outFile(table_name + "_" + table_field->at(i), ios::out | ios::binary);
        if(!outFile)
        {
            cerr<<"Error Message"<<"Cannot open file."<<endl;
            exit(1);
        }
        for(int j = 0; j < row_num; j ++)
        {
            cur_col[j] = table[i][j];
        }
        outFile.write((char *) cur_col, row_num*(sizeof (double)));
        delete [] table[i];
    }
    
    
    delete [] cur_col;
    delete table_field;
    delete [] table;
}

void DataManagement::read(string fileName, long row_num)
{
    double *col = new double[row_num];
    ifstream inFile(fileName, ios::in | ios::binary);
    inFile.read((char *) col, row_num * (sizeof(double)));
    for(int i = 0; i < row_num; i ++)
    {
        cout<<col[i]<<endl;
    }
}


vector<string> DataManagement::split(const string&s, char delim)
{
    stringstream ss(s);
    string item;
    vector<string> tokens;
    while(getline(ss,item, delim))
    {
        tokens.push_back(item);
    }
    return tokens;
}

