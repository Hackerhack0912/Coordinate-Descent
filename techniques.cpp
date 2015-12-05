//
//  techniques.cpp
//  Coordinate_descent
//
//  Created by Zhiwei Fan on 10/13/15.
//  Copyright © 2015 Zhiwei Fan. All rights reserved.
//

#include "techniques.h"
#include "DataManagement.h"
#include "linear_models.h"
#include <iostream>
#include <fstream>
#include <cmath>


techniques::techniques(){};



//Just Logistic Regression by now
void techniques::materialize(string table_T, setting _setting, double *model)
{
    cout<<"Start materialize"<<endl;
    DataManagement DM;
    linear_models lm;
    vector<long> tableInfo(3);
    vector<string> fields = DM.getFieldNames(table_T, tableInfo);
    int feature_num = (int)tableInfo[1];
    long row_num = tableInfo[2];
   
    double *Y;
    double *H;
    double *X;
    
    //setting
    double step_size = _setting.step_size;
    
    //Allocate the memory to the model
    model = new double[feature_num];
    //Allocate the memory to the label Array
    Y = new double[row_num];
    //Allocate the memory to H
    H = new double[row_num];
    //Allocate the memory to X
    X = new double[row_num];
   
    double F = 0.00;
    double F_partial = 0.00;
    double r_curr = 0.00;
    double r_prev = 0.00;
    int k = 0;
    
    
    //cout<<"Model: "<<endl;
    for(int i = 0; i < feature_num; i ++)
    {
        model[i] = 0.00;
        //cout<<model[i]<< " ";
    }
    //cout<<endl;
    

    for(int i = 0; i < row_num; i ++)
    {
        H[i] = 0.00;
    }
   

    
    
    DM.fetchColumn(fields[1], row_num, Y);
    
    //First do Logistic Regression
    do
    {
        //Update one coordinate each time
        for(int j = 0; j < feature_num; j ++)
        {
            F_partial = 0.00;
            //Fetch the each column and store the current column into X
            DM.fetchColumn(fields[2+j], row_num, X);

            //Compute the partial gradient
            for(long i = 0; i < row_num ; i ++)
            {
                F_partial += lm.G_lr(Y[i],H[i])*X[i];
            }
            
            //Store the old W(j)
            double W_j = model[j];
            
            //Update the current coordinate
            model[j] = model[j] - step_size * F_partial;
            
            
            double diff = model[j] - W_j;
    
            //Update the intermediate variable
            //H = H + (Wj - old_Wj)* X(,j)
            for(long m = 0; m < row_num; m ++ )
            {
                H[m] = H[m] + diff*X[m];
            }
        }
        
        r_prev = F;
        //Caculate F
        F = 0.00;
        for(long i = 0; i < row_num ; i ++)
        {
            double tmp = lm.Fe_lr(Y[i],H[i]);
            /**
            if(i < 5){
                cout<<"Yi: "<<Y[i]<<","<<"Hi: "<<H[i]<<","<<"tmp: "<<tmp<<endl;
            }
            **/
            
            F += tmp;
        }
        
        r_curr = F;
        k++;
        //cout<<"Model: "<<endl;
        /**
        for(int i = 0; i < 10; i ++)
        {
            cout<<model[i]<< " ";
        }
        **/
        //cout<<endl;
        //cout<<"Loss value: "<<F<<endl;
        
    }
    while(!stop(k,r_prev,r_curr,_setting));
    
    delete [] Y;
    delete [] X;
    delete [] H;
    
    printf("The final loss: %lf\n",r_curr);
    printf("Number of iteration: %d\n",k);
    printf("Model: ");
    for(int i = 0; i < feature_num; i ++)
    {
        if(i == feature_num - 1)
        {
            printf("%.20f\n",model[i]);
        }
        else
        {
             printf("%.20f, ",model[i]);
        }
       
    }
    
    cout<<"Finish materialize"<<endl;
    
}

/* Should be no oid-oid mapping here */
void techniques::stream(string table_S, string table_R, setting _setting, double *model)
{
    cout<<"Start stream"<<endl;
    DataManagement DM;
    linear_models lm;
    
    //Get the table information and column names
    vector<long> tableInfo_S(3);
    vector<long> tableInfo_R(3);
    vector<string> fields_S = DM.getFieldNames(table_S, tableInfo_S);
    vector<string> fields_R = DM.getFieldNames(table_R, tableInfo_R);
    int feature_num_S = (int)tableInfo_S[1];
    int feature_num_R = (int)tableInfo_R[1];
    int feature_num = feature_num_S + feature_num_R;
    long row_num = tableInfo_S[2];
    long row_num_R = tableInfo_R[2];
    
    double *Y;
    double *H;
    double *X;
    
    //setting
    double step_size = _setting.step_size;
    
    //Allocate the memory to the model
    model = new double[feature_num_S + feature_num_R];
    //Allocate the memory to the label Array
    Y = new double[row_num];
    //Allocate the memory to H
    H = new double[row_num];
    //Allocate the memory to X
    X = new double[row_num];
    
    double F = 0.00;
    double F_partial = 0.00;
    double r_curr = 0.00;
    double r_prev = 0.00;
    int k = 0;
    
    for(int i = 0; i < feature_num; i ++)
    {
        model[i] = 0.00;
        
    }
    
    for(int i = 0; i < row_num; i ++)
    {
        H[i] = 0.00;
    }
    
    DM.fetchColumn(fields_S[1], row_num, Y);
    
    //First do Logistic Regression
    do
    {
        printf("Start fetching KKMR reference\n");
        //OID-OID Mapping (Key Foreign-Key Mapping Reference)
        double *KKMR = new double[row_num];
        //Read the fk column(referred rid in R) in table S, rid column in R
        ifstream fk;
        //Load the fk to KKMR
        fk.open(fields_S[2], ios::in | ios::binary);
        //rid.open(table2_fields[0], ios::in | ios::binary);
        if(!fk.is_open())
        {
            cerr<<"Error Message: "<<"Cannot load the fk column."<<endl;
            exit(1);
        }
        fk.read((char *)KKMR, row_num*(sizeof(double)));
        fk.close();
        printf("Finish fetchig KKMR reference\n");
        
        //Update one coordinate each time
        for(int j = 0; j < feature_num; j ++)
        {
            F_partial = 0.00;
            //Fetch each column in table S and store the current column into X
            if(j < feature_num_S)
            {
                DM.fetchColumn(fields_S[3+j], row_num, X);
            }
            else
            {
                double *tmp_buffer = new double[row_num_R];
                DM.fetchColumn(fields_R[1+j-feature_num_S], row_num_R, tmp_buffer);
                for(long m = 0; m < row_num; m ++)
                {
                    long fk = KKMR[m];
                    X[m]= tmp_buffer[fk-1];
                }
                delete tmp_buffer;
            }
            
            
            //Compute the partial gradient
            for(long i = 0; i < row_num ; i ++)
            {
                F_partial += lm.G_lr(Y[i],H[i])*X[i];
            }
            
            //Store the old W(j)
            double W_j = model[j];
            
            //Update the current coordinate
            model[j] = model[j] - step_size * F_partial;
            
            
            double diff = model[j] - W_j;
            
            //Update the intermediate variable
            //H = H + (Wj - old_Wj)* X(,j)
            for(long m = 0; m < row_num; m ++ )
            {
                H[m] = H[m] + diff*X[m];
            }
        }
        
        r_prev = F;
        //Caculate F
        F = 0.00;
        for(long i = 0; i < row_num ; i ++)
        {
            double tmp = lm.Fe_lr(Y[i],H[i]);
            F += tmp;
        }
        
        r_curr = F;
        k++;
        
        delete [] KKMR;
    }
    while(!stop(k,r_prev,r_curr,_setting));
    
    delete [] Y;
    delete [] X;
    delete [] H;
    
    
    printf("The final loss: %lf\n",r_curr);
    printf("Number of iteration: %d\n",k);
    printf("Model: ");
    for(int i = 0; i < feature_num; i ++)
    {
        if(i == feature_num - 1)
        {
            printf("%.20f\n",model[i]);
        }
        else
        {
            printf("%.20f, ",model[i]);
        }
        
    }
    
    cout<<"Finish stream"<<endl;
}


void techniques::factorize(string table_S, string table_R, setting _setting, double *model)
{
    cout<<"Start factorize"<<endl;
    DataManagement DM;
    linear_models lm;
    
    //Get the table information and column names
    vector<long> tableInfo_S(3);
    vector<long> tableInfo_R(3);
    vector<string> fields_S = DM.getFieldNames(table_S, tableInfo_S);
    vector<string> fields_R = DM.getFieldNames(table_R, tableInfo_R);
    int feature_num_S = (int)tableInfo_S[1];
    int feature_num_R = (int)tableInfo_R[1];
    int feature_num = feature_num_S + feature_num_R;
    long row_num = tableInfo_S[2];
    long row_num_R = tableInfo_R[2];
    
    printf("Start fetching KKMR reference\n");
    //OID-OID Mapping (Key Foreign-Key Mapping Reference)
    double *KKMR = new double[row_num];
    //Read the fk column(referred rid in R) in table S, rid column in R
    ifstream fk;
    //Load the fk to KKMR
    fk.open(fields_S[2], ios::in | ios::binary);
    //rid.open(table2_fields[0], ios::in | ios::binary);
    if(!fk.is_open())
    {
        cerr<<"Error Message: "<<"Cannot load the fk column."<<endl;
        exit(1);
    }
    fk.read((char *)KKMR, row_num*(sizeof(double)));
    fk.close();
    //printf("Finish fetchig KKMR reference\n");
    
    double *Y;
    double *H;
    double *X;
    
    //setting
    double step_size = _setting.step_size;
    
    //Allocate the memory to the model
    model = new double[feature_num_S + feature_num_R];
    //Allocate the memory to the label Array
    Y = new double[row_num];
    //Allocate the memory to H
    H = new double[row_num];
    //Allocate the memory to X
    X = new double[row_num];
    
    double F = 0.00;
    double F_partial = 0.00;
    double r_curr = 0.00;
    double r_prev = 0.00;
    int k = 0;
    
    for(int i = 0; i < feature_num; i ++)
    {
        model[i] = 0.00;
        
    }
    
    for(int i = 0; i < row_num; i ++)
    {
        H[i] = 0.00;
    }
    
    DM.fetchColumn(fields_S[1], row_num, Y);
    
    //First do Logistic Regression
    do
    {
        //Update one coordinate each time
        for(int j = 0; j < feature_num; j ++)
        {
            F_partial = 0.00;
            //Fetch each column in table S and store the current column into X
            if(j < feature_num_S)
            {
                DM.fetchColumn(fields_S[3+j], row_num, X);
                //Compute the partial gradient
                for(int i = 0; i < row_num ; i ++)
                {
                    F_partial += lm.G_lr(Y[i],H[i])*X[i];
                }
                
                //Store the old W(j)
                double W_j = model[j];
                
                //Update the current coordinate
                model[j] = model[j] - step_size * F_partial;
                cout<<"model("<<j<<"): "<<model[j]<<endl;
                
                double diff = model[j] - W_j;
                //Update the intermediate variable
                //H = H + (Wj - old_Wj)* X(,j)
                for(long m = 0; m < row_num; m ++ )
                {
                    H[m] = H[m] + diff*X[m];
                }
            }
            else
            {
                double **tmp_buffer = new double*[2];
                for(int i = 0; i < 2; i ++)
                {
                    tmp_buffer[i] = new double[row_num_R];
                    for(long k = 0; k < row_num_R; k ++)
                    {
                        tmp_buffer[i][k] = 0.00;
                    }
                }
                
                DM.fetchColumn(fields_R[1+j-feature_num_S], row_num_R, tmp_buffer[0]);
                //Compute the partial gradient
                for(long m = 0; m < row_num; m ++)
                {
                    long fk = KKMR[m];
                    tmp_buffer[1][fk-1] += lm.G_lr(Y[m],H[m]);
                }
                for(long j = 0; j < row_num_R; j ++)
                {
                    F_partial += tmp_buffer[0][j]*tmp_buffer[1][j];
                }
                
                
                //Store the old W(j)
                double W_j = model[j];
                
                //Update the current coordinate
                model[j] = model[j] - step_size * F_partial;
                cout<<"model("<<j<<"): "<<model[j]<<endl;
                
                double diff = model[j] - W_j;

                
                //Factorized computation
                for(long k = 0; k < row_num_R; k ++)
                {
                    tmp_buffer[0][k] = diff*tmp_buffer[0][k];
                }
                
                //Update the intermediate variable
                //H = H + (Wj - old_Wj)* X(,j)
                for(long m = 0; m < row_num; m ++ )
                {
                    long fk = KKMR[m];
                    H[m] = H[m] + tmp_buffer[0][fk-1];
                }

                
                delete [] tmp_buffer[0];
                delete [] tmp_buffer[1];
                delete [] tmp_buffer;
            }
            
        }
        
        r_prev = F;
        //Caculate F
        F = 0.00;
        for(int i = 0; i < row_num ; i ++)
        {
            double tmp = lm.Fe_lr(Y[i],H[i]);
            F += tmp;
        }
        
        r_curr = F;
        k++;
    }
    while(!stop(k,r_prev,r_curr,_setting));
    
    delete [] Y;
    delete [] X;
    delete [] H;
    delete [] KKMR;
    
    printf("The final loss: %lf\n",r_curr);
    printf("Number of iteration: %d\n",k);
    printf("Model: ");
    for(int i = 0; i < feature_num; i ++)
    {
        if(i == feature_num - 1)
        {
            printf("%.20f\n",model[i]);
        }
        else
        {
            printf("%.20f, ",model[i]);
        }
        
    }
    
    cout<<"Finish factorize"<<endl;

}

bool techniques::stop(int k, double r_prev, double r_curr, setting &setting)
{
    double iter_num = k;
    double difference = abs(r_prev - r_curr);
    
    if( iter_num == setting.iter_num || difference <= setting.error)
    {
        return true;
    }
    else
    {
        return false;
    }
}

