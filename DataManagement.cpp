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
#include <stdio.h>
#include <stdlib.h>

/*
 Notes for later work:
 minor revision: key should be kept int, however, use double here first for convenience
 */


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
        table_field = new vector<string>(col_num);
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
    
    /**
    double** table = new double*[col_num];
    for(int i = 0; i < col_num; i ++)
    {
        table[i] = new double[row_num];
    }
    **/
    
    //Reading the file and load the data to the corresponding columns (binary array)
    cout<< "Open the input file"<<endl;
    cout<< "File Name: "<<fileName<<endl;
    ifstream infile;
    infile.open(fileName);
    
    
    //Memory free case: all columns can fit in memory the same time
    if(!infile.is_open())
    {
        cerr<<"Error Message: "<<"Cannot open the file: the name of the file might be wrong or the file might be broken"<<endl;
        exit(1);
    }
    
    cout<<"Open the output File"<<endl;
    // Open the corresponding number of files (representing columns) to store the loading data
    ofstream *outFile = new ofstream[col_num];
    // output buffer
    double *write =  new double[1];
    for(int i = 0; i < col_num; i ++)
    {
        outFile[i].open(table_name + "_" + table_field->at(i), ios::out | ios::binary);
        if(!outFile[i].is_open())
        {
            cerr<<"Error Message: "<<"Cannot open file."<<endl;
            exit(1);
        }
    }
    
    cout<<"Starting to load the data from the file into the database"<<endl;
    // All valid files should seperate the columns with ' ' (one space)
    char delim = ' ';
    while(!infile.eof())
    {
        string s;
        getline(infile, s);
        vector<string> tuple;
        tuple = split(s,delim);
        //Skip the empty line
        if(tuple.size() < col_num)
        {
            continue;
        }
        //Write the value in each filed of current tuple into the corresponding columns
        for(int i = 0; i < col_num; i ++)
        {
            const char *cstr = tuple.at(i).c_str();
            write[0] = atof(cstr);
            outFile[i].write((char *) write, (sizeof(double)));
        }
    }
    infile.close();
    
    // Write the table information to a single file
    ofstream info(table_name + "_" + "info", ios::out | ios::app);
    info<<"table name: "<<table_name<<endl;
    info<<"table type: "<<table_type<<endl;
    info<<"feature num: " <<feature_num<<endl;
    info<<"row number: "<<row_num<<endl;
    info.close();
    
   
    cout<<"Finish loading"<<endl;
    // Close all writing files
    //int result;
    for(int i = 0; i < col_num; i ++)
    {
      
        outFile[i].close();
    }
    
    // Relese all allocated memory
    delete [] write;
    delete [] outFile;
    delete table_field;
    
    //double *cur_col = new double[row_num];
    //Write each column into the corresponding binary file
    /**
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
    **/
    
    /**
    delete [] cur_col;
    delete table_field;
    delete [] table;
    **/
}

void DataManagement::readColumn(string fileName, long row_num)
{
    double *col = new double[row_num];
    ifstream inFile(fileName, ios::in | ios::binary);
    if(!inFile.is_open())
    {
        cerr<<"Error Message: "<<"Cannot open file."<<endl;
        exit(1);
    }
    inFile.read((char *) col, row_num * (sizeof(double)));
    inFile.close();
    for(int i = 0; i < row_num; i ++)
    {
        cout<<col[i]<<endl;
    }
    delete col;
}

void DataManagement::fetchColumn(string fileName, long row_num,double *col )
{
    ifstream inFile(fileName, ios::in | ios::binary);
    if(!inFile.is_open())
    {
        cerr<<"Error Message: "<<"Cannot open file."<<endl;
        exit(1);
    }
    inFile.read((char *) col, row_num * (sizeof(double)));
    inFile.close();
}

/*
 @ fileName: corresponding info file containing the information of the table
 @ fields: reference for the vector used to store the name of corresponding fields
 tableInfo:table_type, table_feature_num, table_row_num
 */
vector<string> DataManagement::getFieldNames(string tableName, vector<long> &tableInfo)
{
    ifstream info;
    vector<string> fields;
    vector<string> table_info;
    int table_type;
    string table_name;
    int table_feature_num;
    long table_row_num;
    
    info.open(tableName+"_info");
    if(!info.is_open())
    {
        cerr<<"Error Message: "<<"Unable to read the given information file"<<endl;
        exit(1);
    }
    string s;
    while(!info.eof())
    {
        getline(info,s);
        char delim = ' ';
        vector<string> tokens = split(s,delim);
        //Skip the white line
        if(tokens.size() == 0)
        {
            continue;
        }
        table_info.push_back(tokens[2]);
    }
   
    cout<<"size of table info: "<<table_info.size()<<endl;
    table_name = table_info[0];
    table_type = atoi(table_info[1].c_str());
    table_feature_num = atoi(table_info[2].c_str());
    table_row_num = atol(table_info[3].c_str());
    
    cout<<"Check the vector size to store the tableInfo: "<<tableInfo.size()<<endl;
    if(tableInfo.size() != 3)
    {
        cerr<<"The size of the vector to store the table info is wrong"<<endl;
        exit(1);
    }
    tableInfo[0] = table_type;
    tableInfo[1] = table_feature_num;
    tableInfo[2] = table_row_num;
    
    cout<<"Fetch the information of fields of the corresponding table"<<endl;
    if(table_type == 0)
    {
        // Entity table S: sid + label + fk + x_s[]
        fields.push_back(table_name+"_"+"sid");
        fields.push_back(table_name+"_"+"label");
        fields.push_back(table_name+"_"+"fk");
        for(int i = 0; i < table_feature_num; i ++)
        {
            fields.push_back(table_name+"_"+"x"+"_s"+to_string(i));
        }
    }
    else if(table_type == 1)
    {
        //Entity table R: rid + x_r[]
        fields.push_back(table_name+"_"+"rid");
        for(int i = 0; i < table_feature_num; i ++)
        {
            fields.push_back(table_name+"_"+"x"+"_r"+to_string(i));
        }
    }
    else if(table_type == 2)
    {
        //Materialized table T: tid(sid) + label + x_s[] + x_r[]
        fields.push_back(table_name + "_" + "tid");
        fields.push_back(table_name + "_" + "label");
        for(int i = 0; i < table_feature_num; i ++)
        {
            fields.push_back(table_name + "_" + "x" + "_t" + to_string(i));
        }
    }
    else
    {
        cerr<<"Error Message: "<<"Invalid table type: the information file is invalid"<<endl;
        exit(1);
    }
    
   
    return fields;
}

/*
 @ table_name1: suppose to be entity table S
 @ table_name2: suppose to be attribute table R (More generalized version will be handled later)
 */
void DataManagement::join(string table_name1, string table_name2, string joinTable)
{
   
    //Get the table information
    vector<string> table1_fields;
    vector<string> table2_fields;
    vector<long> table1_info(3);
    vector<long> table2_info(3);
    //Get the column names of two tables
    table1_fields = getFieldNames(table_name1, table1_info);
    table2_fields = getFieldNames(table_name2, table2_info);
    int table1_feature_num = (int)table1_info[1];
    long table1_row_num = table1_info[2];
    int table2_feature_num = (int)table2_info[1];
    long table2_row_num = table2_info[2];
    
    cout<<"Start fetching KKMR reference"<<endl;
    //OID-OID Mapping (Key Foreign-Key Mapping Reference)
    double *KKMR = new double[table1_row_num];
    //Read the fk column(referred rid in R) in table S, rid column in R
    ifstream fk;
    //Load the fk to KKMR
    fk.open(table1_fields[2], ios::in | ios::binary);
    //rid.open(table2_fields[0], ios::in | ios::binary);
    if(!fk.is_open())
    {
        cerr<<"Error Message: "<<"Cannot load the fk column."<<endl;
        exit(1);
    }
    fk.read((char *)KKMR, table1_row_num*(sizeof(double)));
    fk.close();
    cout<<"Finish fetchig KKMR reference"<<endl;
    
    cout<<"Start fetching the column names for materialized (join) table T"<<endl;
    //Get the features for table T: sid, label, x_s[], x_r[]
    int t_column_num = 1 + 1 + table1_feature_num + table2_feature_num;
    //Store the name of every column of table T
    vector<string> t_columns(t_column_num);
    //Same as sid(tid)
    t_columns[0] = joinTable + "_tid";
    t_columns[1] = joinTable + "_label";
    //Columns corresponding to x_s[]
    for(int i = 2; i < 2+table1_feature_num; i ++)
    {
        //t_columns[i] = joinTable + "_" + "x_t" + to_string(i-2) + "(x_s" + to_string(i-2) + ")";
        t_columns[i] = joinTable + "_" + "x_t" + to_string(i-2);
    }
    //Columns corresponding to x_r[]
    for(int k = 2+table1_feature_num; k < t_column_num; k ++)
    {
        //t_columns[k] = joinTable + "_" + "x_t" + to_string(k-2) + "(x_r" + to_string(k-2-table1_feature_num) + ")";
        t_columns[k] = joinTable + "_" + "x_t" + to_string(k-2);
    }
    cout<<"Finish fetching the column names for materialized (join) table T"<<endl;
    
    cout<<"Open the output file for T"<<endl;
    // Open the corresponding number of files (representing columns) for T
    ofstream *T = new ofstream[t_column_num];
    for(int i = 0; i < t_column_num; i ++)
    {
        T[i].open(t_columns[i], ios::out | ios::binary);
        if(!T[i].is_open())
        {
            cerr<<"Error Message: "<<"Cannot open file."<<endl;
            exit(1);
        }
    }
    
    cout<<"Start writing sid,label to T"<<endl;
    //First write corresponding columns sid,label in S
    ifstream sid;
    double *buffer = new double[table1_row_num];
    sid.open(table1_fields[0], ios::in | ios::binary);
    if(!sid.is_open())
    {
        cerr<<"Error Message: "<<"Cannot open file."<<endl;
        exit(1);
    }
    sid.read((char *)buffer, table1_row_num*(sizeof(double)));
    sid.close();
    T[0].write((char *)buffer, table1_row_num*(sizeof(double)));
    T[0].close();

    ifstream label;
    label.open(table1_fields[1], ios::in | ios::binary);
    if(!label.is_open())
    {
        cerr<<"Error Message: "<<"l open file."<<endl;
        exit(1);
    }
    label.read((char *)buffer, table1_row_num*(sizeof(double)));
    label.close();
    T[1].write((char *)buffer, table1_row_num*(sizeof(double)));
    T[1].close();
    cout<<"Finish writing sid,label to T"<<endl;
    
    cout<<"Start writing x_s[] to T"<<endl;
    //Then write corresponding columns x_s[] in S
    for(int i = 3; i < table1_feature_num+3; i ++)
    {
        ifstream x_s;
        x_s.open(table1_fields[i], ios::in | ios::binary);
        if(!x_s.is_open())
        {
            cerr<<"Error Message: "<<"Cannot open file."<<endl;
            exit(1);
        }
        x_s.read((char *)buffer, table1_row_num*(sizeof(double)));
        x_s.close();
        T[i-1].write((char *)buffer, table1_row_num*(sizeof(double)));
        T[i-1].close();
    }
    cout<<"Finish writing x_s[] to T"<<endl;
    
    cout<<"Start writing x_r[] to T"<<endl;
    //Then write corresponding columns x_r[] in R using KKMR as reference
    //Buffer for reading and probing
    double *read = new double[table2_row_num];
    for(int k = 1; k <= table2_feature_num; k ++ )
    {
        ifstream x_r;
        x_r.open(table2_fields[k], ios::in | ios::binary);
        
        if(!x_r.is_open())
        {
            cerr<<"Error Message: "<<"Cannot open file."<<endl;
            exit(1);
        }
        
        x_r.read((char *) read, table2_row_num*(sizeof(double)));
        x_r.close();
        for(long m = 0; m < table1_row_num; m ++)
        {
            long fk = KKMR[m];
            buffer[m] = read[fk-1];
            
        }
        
        T[1+table1_feature_num+k].write((char *)buffer,table1_row_num*(sizeof(double)));
        T[1+table1_feature_num+k].close();
    }
    cout<<"Finish writing x_r[] to T"<<endl;
    
    cout<<"Start writing the infomation file for table T"<<endl;
    // Write the table information to a single file
    ofstream info(joinTable + "_" + "info", ios::out | ios::app);
    info<<"table name: "<<joinTable<<endl;
    info<<"table type: "<<2<<endl;
    info<<"feature num: "<<table1_feature_num + table2_feature_num<<endl;
    info<<"row number: "<<table1_row_num<<endl;
    info.close();

    
    delete [] KKMR;
    delete [] T;
    delete [] read;
    delete [] buffer;
}



void DataManagement::readTable(string tableName)
{
    cout<<"Start reading the table"<<endl;
    vector<long> tableInfo(3);
    vector<string> fields = getFieldNames(tableName,tableInfo);
    int col_num = (int)fields.size();
    long row_num = tableInfo[2];
    ifstream *inFile = new ifstream[col_num];
    // Input buffer
    double *read =  new double[1];
    
    cout<<"Start loading the table: "<<tableName;
    for(int i = 0; i < col_num; i ++)
    {
       
        string column = fields.at(i);
        cout<< "Current column: "<<column<<endl;
        inFile[i].open(column, ios::in | ios::binary);
        if(!inFile[i].is_open())
        {
            cerr<<"Error Message: "<<"Cannot read the given table, the table may not exist."<<endl;
            exit(1);
        }
    }
    
    //Starting to read the table
    for(long j = 0; j < row_num; j ++)
    {
        for(int k = 0; k < col_num; k ++)
        {
            inFile[k].read((char *)read, (sizeof (double)));
            cout<<read[0]<<" ";
            
        }
        cout<<endl;
    }
    
    // Close all files
    for(int i = 0; i < col_num; i ++)
    {
        
        inFile[i].close();
    }
    
    delete [] inFile;
    delete read;

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

