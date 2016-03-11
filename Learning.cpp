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
 1. create table: DB create file_name(table_name) type feature_num row_num
 2. join table: DB join table_name1 table_name2 joinTable_name
 3. read table: DB read table_name
 4. materialize: DB m table_name
 5. stream: DB s table_name_S table_name_R
 6. factorize: DB f table_name_S table_name_R
 7. readColumn: DB readColumn column_name row_num
 8. Stochastic Gradient Descent: DB SGD train_file_name test_file_name
 9. Batch Gradient Descent:DB BGD train_file_name test_file_name
 10. Block Coordinate Descent(materialize): DB BCD table_name
 11. Block Coordinate Descent(factorize): DB BCD table_name_S table_name_R
 */
int main(int argc, const char * argv[])
{
    
    
    DataManagement dM;
    techniques t;
    string option1 = "create";
    string option2 = "join";
    string option3 = "read";
    string option4 = "m";
    string option5 = "s";
    string option6 = "f";
    string option7 = "readColumn";
    string option8 = "SGD";
    string option9 = "BGD";
    string option10 = "BCD";
    string option11 = "fBCD";
    
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
    else if(argc == 3)
    {
        
        string option = argv[1];
        string tableName = argv[2];
        if(option == option3)
        {
            dM.readTable(tableName);
        }
        else
        {
            long avail_mem;
            string tableName = argv[2];
            double *model;
            setting _setting;
            printf("Setting the available memory\n");
            scanf("%lu",&avail_mem);
            printf("Setting stepSize: \n");
            scanf("%lf",&_setting.step_size);
            //_setting.step_size = 0.01;
            printf("Setting error tolearence: \n");
            //_setting.error = 0.00005;
            scanf("%lf",&_setting.error);
            printf("Setting number of iterations: \n");
            _setting.iter_num = 10;
            scanf("%d",&_setting.iter_num);
            
            if(option == option4)
            {
                //Feature Number to be dealt with it later
                t.materialize(tableName, _setting, model, avail_mem);
            }
            else if(option == option10)
            {
                int block_size;
                printf("Setting number of block size: \n");
                scanf("%d", &block_size);
                t.materializeBCD(tableName, _setting, model, block_size, avail_mem);
            }
            delete [] model;
        }
        
        
    }
    else if(argc == 4 && (argv[1] == option8 || argv[1] == option9) )
    {
       
        string tableName = argv[2];
        string testTable = argv[3];
        string option = argv[1];
        
        double *model;
        int feature_num = 0;
        vector<double> model_vector;
        setting _setting;
        printf("Setting stepSize: \n");
        scanf("%lf",&_setting.step_size);
        //_setting.step_size = 0.01;
        printf("Setting error tolearence: \n");
        //_setting.error = 0.00005;
        scanf("%lf",&_setting.error);
        printf("Setting number of iterations: \n");
        //_setting.iter_num = 10;
        scanf("%d",&_setting.iter_num);
        
    
        if(option == option8)
        {
            vector< vector<double> > data = dM.rowStore(tableName);
            int featureNum = (int)(data.at(0).size()-2);
            feature_num = featureNum;
            t.SGD(data, _setting, model, featureNum);
        }
        else if(option == option9)
        {
            vector< vector<double> > data = dM.rowStore(tableName);
            int featureNum = (int)(data.at(0).size()-2);
            feature_num = featureNum;
            t.BGD(data, _setting, model, featureNum);
        }
        else
        {
            DataManagement::message("Invalid option");
        }
        
        
        
        vector< vector<double> > testData = dM.rowStore(testTable);
        for(int i = 0; i < feature_num; i ++)
        {
            model_vector.push_back(model[i]);
        }
        t.classify(testData, model_vector);
        

        delete [] model;
    }
    else if(argc == 4)
    {
        long avail_mem;
        string tableName_S = argv[2];
        string tableName_R = argv[3];
        double *model;
        setting _setting;
        printf("Setting the available memory\n");
        scanf("%lu",&avail_mem);
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
            t.stream(tableName_S, tableName_R, _setting, model, avail_mem);
        }
        else if(argv[1] == option6)
        {
            t.factorize(tableName_S, tableName_R, _setting, model, avail_mem);
        }
        else if(argv[1] == option11)
        {
            int block_size;
            printf("Setting number of block size: \n");
            scanf("%d", &block_size);
            t.factorizeBCD(tableName_S, tableName_R, _setting, model, block_size, avail_mem);
        }
        else
        {
            DataManagement::message("Invalid option");
        }
        
        //Print the model
        delete [] model;
    }
    else
    {
        cerr<<"Invalid command: wrong number of arguments or invalid option"<<endl;
        exit(1);
    }
  
    return 0;
}

