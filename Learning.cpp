//
//  main.cpp
//  Coordinate_descent
//
//  Created by Zhiwei Fan on 10/2/15.
//  Copyright (c) 2015 Zhiwei Fan. All rights reserved.
//

#include <iostream>
#include "DataManagement.h"
#include "techniques.h"


/*
 options:
 1. create table: coordinate_descent create file_name(table_name) type feature_num row_num
 2. join table: coordinate_descent join table_name1 table_name2 joinTable_name
 3. read table: coordinate_descent read table_name
 4. materialize: coordinate_descent m talbe_name
 5. stream: coordinate_descent s table_name_S table_name_R
 6. factorize: coordinate_descent f table_name_S table_name_R
 7. readColumn: coordinate_descent column_name row_num
 */
int main(int argc, const char * argv[]) {
    
    
    DataManagement dM;
    techniques t;
    string option1 = "create";
    string option2 = "join";
    string option3 = "read";
    string option4 = "m";
    string option5 = "s";
    string option6 = "f";
    
    if(argc == 6 && argv[1] == option1)
    {
        //option 1
        string fileName = argv[2];
        int tableType = atoi(argv[3]);
        int featureNum = atoi(argv[4]);
        long rowNum = atol(argv[5]);
        dM.store(fileName, featureNum, tableType, rowNum);
    }
    else if(argc == 5 && argv[1] == option2)
    {
        //option 2
        string tableName1 = argv[2];
        string tableName2 = argv[3];
        string joinTableName = argv[4];
        dM.join(tableName1, tableName2, joinTableName);
    }
    else if(argc == 3 && argv[1] == option3)
    {
        //option3
        string tableName = argv[2];
        dM.readTable(tableName);
    }
    else if(argc == 3 && argv[1] == option4)
    {
        //option4
        string tableName = argv[2];
        double *model;
        setting _setting;
        printf("Setting stepSize: \n");
        scanf("%lf",&_setting.step_size);
        //_setting.step_size = 0.01;
        printf("Setting error tolearence: \n");
        //_setting.error = 0.00005;
        scanf("%lf",&_setting.error);
        printf("Setting number of iterations: \n");
        _setting.iter_num = 10;
        scanf("%d",&_setting.iter_num);
        t.materialize(tableName, _setting, model);
        //Print the model
        delete model;
    }
    else if(argc == 4)
    {
        string tableName_S = argv[2];
        string tableName_R = argv[3];
        double *model;
        setting _setting;
        printf("Setting stepSize: \n");
        scanf("%lf",&_setting.step_size);
        //_setting.step_size = 0.01;
        printf("Setting error tolearence: \n");
        //_setting.error = 0.00005;
        scanf("%lf",&_setting.error);
        printf("Setting number of iterations: \n");
        scanf("%d",&_setting.iter_num);

        if(argv[1] == option5)
        {
            t.stream(tableName_S, tableName_R, _setting, model);
        }
        
        if(argv[1] == option6)
        {
            t.factorize(tableName_S, tableName_R, _setting, model);
        }
        
        //Print the model
        delete model;
    }
    else
    {
        cerr<<"Invalid command: wrong number of arguments or invalid option"<<endl;
        exit(1);
    }
    
    //dM.readTable("/Users/Hacker/Box Sync/Research/Coordinate_descent/Coordinate_descent/S");
    //dM.readTable("/Users/Hacker/Box Sync/Research/Coordinate_descent/Coordinate_descent/R");
  
    return 0;
}

