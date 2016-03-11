//
//  techniques.cpp
//  Coordinate_descent
//
//  Created by Zhiwei Fan on 10/13/15.
//  Copyright © 2015 Zhiwei Fan. All rights reserved.
//

#include <iostream>
#include <fstream>
#include <cmath>
#include <stdlib.h>
#include <time.h>
#include "techniques.h"
#include "DataManagement.h"
#include "linear_models.h"

techniques::techniques(){};

/**
 Coordinate Descent/Block Coordinate Descent:
 (CD/BCD)
 Materialize, Stream, Factorize
 
 Stochastic Gradient Descent/Batch Gradient Descent:
 (SGD/BGD)
 Materialize only
*/
//Just Logistic Regression by now
#pragma mark - Stochastic Coordiante Descent
void techniques::materialize(string table_T, setting _setting, double *&model, long avail_mem)
{
    DataManagement DM;
    DM.message("Start materialize");
    linear_models lm;
    vector<long> tableInfo(3);
    vector<string> fields = DM.getFieldNames(table_T, tableInfo);
    int feature_num = (int)tableInfo[1];
    long row_num = tableInfo[2];
    
    //For cache
    int avail_col = 0;
    int avail_cache = 0;
    double **cache;
   
    //Primary main memory space: three columns
    
    //Label array
    double *Y;
    //Residual vector
    double *H;
    //Buffer for column reading
    double *X;
    
    //setting
    double step_size = _setting.step_size;
    
    //Calculate the available memory measured by size of each column
    avail_col = avail_mem/(sizeof(double)*row_num);
    
    //Calculate the available remaining space for cache
    avail_cache = avail_col - 3;
    if(avail_cache < 0)
    {
        DM.errorMessage("Insufficient memory space");
        exit(1);
    }
    else if (avail_cache == 0)
    {
        DM.message("No space for caching");
    }
    else
    {
        if( avail_cache >= feature_num - 1 )
        {
            cache = new double*[feature_num];
            for(int i = 0; i < feature_num; i ++)
            {
                cache[i] = new double[row_num];
            }
            //No need to reserve the X buffer to read single column
            avail_cache = feature_num;
        }
        else
        {
            //Allocate the memory to X
            X = new double[row_num];
            cache = new double*[avail_cache];
            for(int i = 0; i < avail_cache; i ++)
            {
                cache[i] = new double[row_num];
            }
        }
    }
    
    //Dynamic memory allocation
    model = new double[feature_num];
    Y = new double[row_num];
    H = new double[row_num];
    
    double F = 0.00;
    double F_partial = 0.00;
    double r_curr = 0.00;
    double r_prev = 0.00;
    int k = 0;
    
    for(int i = 0; i < feature_num; i ++)
    {
        model[i] = 0.00;
        H[i] = 0.00;
    }
    
    DM.fetchColumn(fields[1], row_num, Y);
    
    vector<int> original_index_set;
    vector<int> shuffling_index;
    //Initialize the original_index_set
    for(long i = 0; i < feature_num; i ++)
    {
        original_index_set.push_back(i);
    }
    
    //Shuffling
    shuffling_index = shuffle(original_index_set, (unsigned)time(NULL));
    
    //Caching
    for(int i = 0; i < avail_cache; i ++)
    {
        DM.fetchColumn(fields[i+2],row_num, cache[i]);
    }
    
    //First do Logistic Regression
    do
    {
        //Update one coordinate each time
        for(int j = 0; j < feature_num; j ++)
        {
            int cur_index = shuffling_index.at(j);
            F_partial = 0.00;
            
            //If the column corresponding to the current updating coordinate is in the cache, no extra I/O is needed
            if( cur_index < avail_cache)
            {
                for(long i = 0; i < row_num ; i ++)
                {
                    F_partial += lm.G_lr(Y[i],H[i])*cache[cur_index][i];
                }
            }
            else
            {
                //Fetch the column and store the current column into X
                DM.fetchColumn(fields[cur_index+2], row_num, X);
                //Compute the partial gradient
                for(long i = 0; i < row_num ; i ++)
                {
                    F_partial += lm.G_lr(Y[i],H[i])*X[i];
                }
            }

            //Store the old W(j)
            double W_j = model[cur_index];
            
            //Update the current coordinate
            model[cur_index] = model[cur_index] - step_size * F_partial;
            
            double diff = model[cur_index] - W_j;
    
            
            //Update the intermediate variable
            //H = H + (Wj - old_Wj)* X(,j)
            if( cur_index < avail_cache)
            {
                for(long m = 0; m < row_num; m ++ )
                {
                    H[m] = H[m] + diff*cache[cur_index][m];
                }
            }
            else
            {
                for(long m = 0; m < row_num; m ++ )
                {
                    H[m] = H[m] + diff*X[m];
                }
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
        
    }
    while(!stop(k,r_prev,r_curr,_setting));
    
    delete [] Y;
    delete [] H;
    
    if( avail_cache < feature_num - 1 ){
        delete [] X;
    }
    
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
    
    DM.message("Finish materialize");
    
}

/* Should not store oid-oid mapping here */
void techniques::stream(string table_S, string table_R, setting _setting, double *&model, long avail_mem)
{
    DataManagement DM;
    DM.message("Start stream");
    linear_models lm;
    
    //Get the table information and column names
    vector<long> tableInfo_S(3);
    vector<long> tableInfo_R(3);
    vector<string> fields_S = DM.getFieldNames(table_S, tableInfo_S);
    vector<string> fields_R = DM.getFieldNames(table_R, tableInfo_R);
    int feature_num_S = (int)tableInfo_S[1];
    int feature_num_R = (int)tableInfo_R[1];
    int feature_num = feature_num_S + feature_num_R;
    long row_num_S = tableInfo_S[2];
    long row_num_R = tableInfo_R[2];
    
    //for Cache
    long avail_mem_total = avail_mem;
    long avail_cache;
    int avail_col_S = 0;
    int avail_col_R = 0;
    double **cache_R;
    double **cache_S;
    
    //Label array
    double *Y;
    //Residual vector
    double *H;
    //Buffer for column reading in S
    double *X_S;
    //Buffer for column reading in R
    double *X_R;
    
    //OID-OID Mapping (Key Foreign-Key Mapping Reference)
    double *KKMR;
    
    //setting
    double step_size = _setting.step_size;

    //Calculate the available memory measured by size of each column in R and S
    avail_cache = avail_mem_total - sizeof(double)*(4*row_num_S + row_num_R);

    if(avail_cache < 0)
    {
    	DM.errorMessage("Insufficient memory space");
	exit(1);
    }
    else if(avail_cache == 0)
    {
    	DM.message("No space for caching");
    }
    else
    {
    	//first consider caching columns in S
        avail_col_S = avail_cache/(sizeof(double)*row_num_S);
        if(avail_col_S == 0)
        {
            DM.message("No space for caching S");
            //Then consider caching columns in R
            avail_col_R = avail_cache/(sizeof(double)*row_num_R);
            if(avail_col_R == 0)
            {
                DM.message("No space for caching R");
            }
            else
            {
                if(avail_col_R >= feature_num_R - 1)
                {
                    cache_R = new double*[feature_num_R];
                    for(int i = 0; i < feature_num_R; i ++)
                    {
                        cache_R[i] = new double[row_num_R];
                    }
                    //No need to reserve the X_R buffer to read a single column in R
                    avail_col_R = feature_num_R;
                }
                else
                {
                    //Allocate the memory to X_R
                    X_R = new double[row_num_R];
                    cache_R = new double*[avail_col_R];
                    for(int i = 0; i < avail_col_R; i ++)
                    {
                        cache_R[i] = new double[row_num_R];
                    }
                }
            }
        }
        else
        {
	    if(avail_col_S >= feature_num_S)
            {
                cache_S = new double*[feature_num_S];
                for(int i = 0; i < feature_num_S; i ++)
                {
                    cache_S[i] = new double[row_num_S];
                }
                //X_S is still needed to "reconstruct" the complete column from single column fetched from R
                avail_col_S = feature_num_S;
		
            }
            else
            {
                cache_S = new double*[avail_col_S];
                for(int i = 0; i < avail_col_S; i ++)
                {
                    cache_S[i] = new double[row_num_S];
                }
            }
		
            //Then consider the caching for R using the remaining caching space
            if(avail_col_S == feature_num_S)
            {
                avail_cache = avail_cache - (avail_col_S - 1)*sizeof(double)*row_num_S;
            }
            else
            {
                avail_cache = avail_cache - avail_col_S*sizeof(double)*row_num_S;
            }
            avail_col_R = avail_cache/(sizeof(double)*row_num_R);
            if(avail_col_R == 0)
            {
                DM.message("No space for caching R");
            }
            else
            {
                if(avail_col_R >= feature_num_R - 1)
                {
                    cache_R = new double*[feature_num_R];
                    for(int i = 0; i < feature_num_R; i ++)
                    {
                        cache_R[i] = new double[row_num_R];
                    }
                    //No need to reserve the X_R buffer to read a single column in R
                    avail_col_R = feature_num_R;
                }
                else
                {
                    //Allocate the memory to X_R
                    X_R = new double[row_num_R];
                    cache_R = new double*[avail_col_R];
                    for(int i = 0; i < avail_col_R; i ++)
                    {
                        cache_R[i] = new double[row_num_R];
                    }
                }
            }
        }
        
    }
    
    //Dynamic memory allocation
    model = new double[feature_num];
    Y = new double[row_num_S];
    H = new double[row_num_S];
    X_S = new double[row_num_S];
    KKMR = new double[row_num_S];
   
    double F = 0.00;
    double F_partial = 0.00;
    double r_curr = 0.00;
    double r_prev = 0.00;
    int k = 0;
    
    for(int i = 0; i < feature_num; i ++)
    {
        model[i] = 0.00;
        
    }
    
    for(int i = 0; i < row_num_S; i ++)
    {
        H[i] = 0.00;
    }
    
    DM.fetchColumn(fields_S[1], row_num_S, Y);
    
    vector<int> original_index_set;
    vector<int> shuffling_index;
    //Initialize the original_index_set
    for(int i = 0; i < feature_num; i ++)
    {
        original_index_set.push_back(i);
    }
    
    //Shuffling
    shuffling_index = shuffle(original_index_set, (unsigned)time(NULL));
    
    //Caching S
    cout<<"Avail_col_S: "<<avail_col_S<<endl;
    for(int i = 0; i < avail_col_S; i ++)
    {
        cout<<"Cache "<<i<<" th column in S"<<endl;
        DM.fetchColumn(fields_S[3+i], row_num_S, cache_S[i]);
    }
    
    //Caching R
    cout<<"Avail_col_R: "<<avail_col_R<<endl;
    for(int k = 0; k < avail_col_R; k ++)
    {
        cout<<"Cache "<<k<<" th column in R"<<endl;
        DM.fetchColumn(fields_R[1+k],row_num_R, cache_R[k]);
    }

    //First do Logistic Regression
    do
    {
        printf("Start fetching KKMR reference\n");
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
        fk.read((char *)KKMR, row_num_S*(sizeof(double)));
        fk.close();
        printf("Finish fetchig KKMR reference\n");
        
        //Update one coordinate each time
        for(int j = 0; j < feature_num; j ++)
        {
            int cur_index = shuffling_index.at(j);
            
            F_partial = 0.00;
         
            if(cur_index < feature_num_S)
            {
                //Check cache for S
                if(cur_index < avail_col_S)
                {
                    //Compute the partial gradient
                    for(long i = 0; i < row_num_S; i ++)
                    {
                        F_partial += lm.G_lr(Y[i],H[i])*cache_S[cur_index][i];
                    }
                }
                else
                {
                    //Fetch the corresponding column in S and store in X_S
                    DM.fetchColumn(fields_S[3+cur_index], row_num_S, X_S);
                    //Compute the partial gradient
                    for(long i = 0; i < row_num_S; i ++)
                    {
                        F_partial += lm.G_lr(Y[i],H[i])*X_S[i];
                    }
                }
            }
            else
            {
                //Check cache for R
                int col_index_R = cur_index - feature_num_S;
                cout<<"col_index_R: "<<col_index_R<<endl;
                if(col_index_R < avail_col_R)
                {
                    for(long m = 0; m < row_num_S; m ++)
                    {
                        long fk = KKMR[m];
                        X_S[m]= cache_R[col_index_R][fk-1];
                    }
                }
                else
                {
                    DM.fetchColumn(fields_R[1+col_index_R], row_num_R, X_R);
                    for(long m = 0; m < row_num_S; m ++)
                    {
                        long fk = KKMR[m];
                        X_S[m]= X_R[fk-1];
                    }
                }
                
                //Compute the partial gradient
                for(long i = 0; i < row_num_S; i ++)
                {
                    F_partial += lm.G_lr(Y[i],H[i])*X_S[i];
                }

                
            }
            
            
            //Store the old W(j)
            double W_j = model[cur_index];
            
            //Update the current coordinate
            model[cur_index] = model[cur_index] - step_size * F_partial;
            
            double diff = model[cur_index] - W_j;
            
            //Update the intermediate variable
            //H = H + (Wj - old_Wj)* X(,j)
            if( cur_index < avail_col_S)
            {
                for(long m = 0; m < row_num_S; m ++ )
                {
                    H[m] = H[m] + diff*cache_S[cur_index][m];
                }
            }
            else
            {
                for(long m = 0; m < row_num_S; m ++ )
                {
                    H[m] = H[m] + diff*X_S[m];
                }
            }

        }
        
        r_prev = F;
        //Caculate F
        F = 0.00;
        for(long i = 0; i < row_num_S; i ++)
        {
            double tmp = lm.Fe_lr(Y[i],H[i]);
            F += tmp;
        }
        
        r_curr = F;
        k++;
        
    }
    while(!stop(k,r_prev,r_curr,_setting));
    
    delete [] Y;
    delete [] H;
    delete [] X_S;
    delete [] KKMR;
    if(avail_col_R < feature_num_R)
    {
        delete [] X_R;
    }
    
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
    
    DM.message("Finish stream");
}


void techniques::factorize(string table_S, string table_R, setting _setting, double *&model, long avail_mem)
{
    DataManagement DM;
    DM.message("Start factorize");
    linear_models lm;
    
    //Get the table information and column names
    vector<long> tableInfo_S(3);
    vector<long> tableInfo_R(3);
    vector<string> fields_S = DM.getFieldNames(table_S, tableInfo_S);
    vector<string> fields_R = DM.getFieldNames(table_R, tableInfo_R);
    int feature_num_S = (int)tableInfo_S[1];
    int feature_num_R = (int)tableInfo_R[1];
    int feature_num = feature_num_S + feature_num_R;
    long row_num_S = tableInfo_S[2];
    long row_num_R = tableInfo_R[2];
    
    //for Cache
    long avail_mem_total = avail_mem;
    long avail_cache;
    int avail_col_S = 0;
    int avail_col_R = 0;
    double **cache_R;
    double **cache_S;
    
    //Label array
    double *Y;
    //Residual vector
    double *H;
    //Buffer for column reading in S
    double *X_S;
    //Buffer for column reading in R
    double *X_R;
    //Buffer to store factorized factor when considering column R
    double *X_R_f;
    
    //OID-OID Mapping (Key Foreign-Key Mapping Reference, to be kept in memory)
    double *KKMR;
    
    //setting
    double step_size = _setting.step_size;
    
    //Calculate the available memory measured by size of each column in R and S
    avail_cache = avail_mem_total - sizeof(double)*(4*row_num_S + 2*row_num_R);
    
    if(avail_cache < 0)
    {
        DM.errorMessage("Insufficient memory space");
        exit(1);
    }
    else if(avail_cache == 0)
    {
        DM.message("No space for caching");
    }
    else
    {
        //first consider caching columns in S
        avail_col_S = avail_cache/(sizeof(double)*row_num_S);
        if(avail_col_S == 0)
        {
            DM.message("No space for caching S");
            X_S = new double[row_num_S];
            //Then consider caching columns in R
            avail_col_R = avail_cache/(sizeof(double)*row_num_R);
            if(avail_col_R == 0)
            {
                DM.message("No space for caching R");
            }
            else
            {
                if(avail_col_R >= feature_num_R - 1)
                {
                    cache_R = new double*[feature_num_R];
                    for(int i = 0; i < feature_num_R; i ++)
                    {
                        cache_R[i] = new double[row_num_R];
                    }
                    //No need to reserve the X_R buffer to read a single column in R
                    avail_col_R = feature_num_R;
                }
                else
                {
                    //Allocate the memory to X_R
                    X_R = new double[row_num_R];
                    cache_R = new double*[avail_col_R];
                    for(int i = 0; i < avail_col_R; i ++)
                    {
                        cache_R[i] = new double[row_num_R];
                    }
                }
            }
        }
        else
        {
            if(avail_col_S >= feature_num_S)
            {
                cache_S = new double*[feature_num_S];
                for(int i = 0; i < feature_num_S; i ++)
                {
                    cache_S[i] = new double[row_num_S];
                }
                //No need to reserve X_S for single column reading
                avail_col_S = feature_num_S;
                
            }
            else
            {
                X_S = new double[row_num_S];
                cache_S = new double*[avail_col_S];
                for(int i = 0; i < avail_col_S; i ++)
                {
                    cache_S[i] = new double[row_num_S];
                }
            }
            
            //Then consider the caching for R using the remaining caching space
            if(avail_col_S == feature_num_S)
            {
                avail_cache = avail_cache - (avail_col_S - 1)*sizeof(double)*row_num_S;
            }
            else
            {
                avail_cache = avail_cache - avail_col_S*sizeof(double)*row_num_S;
            }
            avail_col_R = avail_cache/(sizeof(double)*row_num_R);
            if(avail_col_R == 0)
            {
                DM.message("No space for caching R");
            }
            else
            {
                if(avail_col_R >= feature_num_R - 1)
                {
                    cache_R = new double*[feature_num_R];
                    for(int i = 0; i < feature_num_R; i ++)
                    {
                        cache_R[i] = new double[row_num_R];
                    }
                    //No need to reserve the X_R buffer to read a single column in R
                    avail_col_R = feature_num_R;
                }
                else
                {
                    //Allocate the memory to X_R
                    X_R = new double[row_num_R];
                    cache_R = new double*[avail_col_R];
                    for(int i = 0; i < avail_col_R; i ++)
                    {
                        cache_R[i] = new double[row_num_R];
                    }
                }
            }
        }
        
    }
    
    
    //Dynamic memory alloaction
    model = new double[feature_num];
    Y = new double[row_num_S];
    H = new double[row_num_S];
    X_R_f = new double[row_num_R];
    KKMR = new double[row_num_S];
    
    double F = 0.00;
    double F_partial = 0.00;
    double r_curr = 0.00;
    double r_prev = 0.00;
    int k = 0;
    
    for(int i = 0; i < feature_num_S; i ++)
    {
        model[i] = 0.00;
        
    }
    
    for(int i = 0; i < row_num_S; i ++)
    {
        H[i] = 0.00;
    }
    
    DM.fetchColumn(fields_S[1], row_num_S, Y);
    
    vector<int> original_index_set;
    vector<int> shuffling_index;
    //Initialize the original_index_set
    for(int i = 0; i < feature_num; i ++)
    {
        original_index_set.push_back(i);
    }
    
    //Shuffling
    shuffling_index = shuffle(original_index_set, (unsigned)time(NULL));
    
    //Caching S
    cout<<"Avail_col_S: "<<avail_col_S<<endl;
    for(int i = 0; i < avail_col_S; i ++)
    {
        cout<<"Cache "<<i<<" th column in S"<<endl;
        DM.fetchColumn(fields_S[3+i], row_num_S, cache_S[i]);
    }
    
    //Caching R
    cout<<"Avail_col_R: "<<avail_col_R<<endl;
    for(int k = 0; k < avail_col_R; k ++)
    {
        cout<<"Cache "<<k<<" th column in R"<<endl;
        DM.fetchColumn(fields_R[1+k],row_num_R, cache_R[k]);
    }
    
    DM.message("Start fetching KKMR reference\n");
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
    fk.read((char *)KKMR, row_num_S*(sizeof(double)));
    fk.close();
    DM.message("Finished fetching KKMR reference\n");
    
    //First do Logistic Regression
    do
    {
        //Update one coordinate each time
        for(int j = 0; j < feature_num; j ++)
        {
            
            int cur_index = shuffling_index.at(j);
            
            F_partial = 0.00;
            //Fetch each column in table S and store the current column into X
            if(cur_index < feature_num_S)
            {
                //Check cache for S
                if(cur_index < avail_col_S)
                {
                    //Compute the partial gradient
                    for(long i = 0; i < row_num_S; i ++)
                    {
                        F_partial += lm.G_lr(Y[i],H[i])*cache_S[cur_index][i];
                    }
                }
                else
                {
                    //Fetch the corresponding column in S and store in X_S
                    DM.fetchColumn(fields_S[3+cur_index], row_num_S, X_S);
                    //Compute the partial gradient
                    for(int i = 0; i < row_num_S; i ++)
                    {
                        F_partial += lm.G_lr(Y[i],H[i])*X_S[i];
                    }
                    
                }
                
                //Store the old W(j)
                double W_j = model[cur_index];
                
                //Update the current coordinate
                model[cur_index] = model[cur_index] - step_size * F_partial;
                
                double diff = model[cur_index] - W_j;
                
		//Update the intermediate variable
                //H = H + (Wj - old_Wj)* X(,j)
                if(cur_index < avail_col_S)
                {
                    for(long m = 0; m < row_num_S; m ++ )
                    {
                        H[m] = H[m] + diff*cache_S[cur_index][m];
                    }
                }
                else{
                    for(long m = 0; m < row_num_S; m ++ )
                    {
                        H[m] = H[m] + diff*X_S[m];
                    }
                }
                
            }
            else
            {
                
                //Check cache for R
                int col_index_R = cur_index - feature_num_S;
                cout<<"col_index_R: "<<col_index_R<<endl;
                
                //Compute the factorized factor
		for(long k = 0; k < row_num_R; k ++){
			X_R_f[k] = 0.00;
		}
                for(long m = 0; m < row_num_S; m ++)
                {
                    long fk = KKMR[m];
                    X_R_f[fk-1] += lm.G_lr(Y[m],H[m]);
                }
                
                if(col_index_R < avail_col_R)
                {
                    //Compute the partial gradient
                    for(long k = 0; k < row_num_R; k ++)
                    {
                        F_partial += cache_R[col_index_R][k]*X_R_f[k];
                    }
                }
                else
                {
                    DM.fetchColumn(fields_R[1+col_index_R], row_num_R, X_R);
                    for(long k = 0; k < row_num_R; k ++)
                    {
                        F_partial += X_R[k]*X_R_f[k];
                    }
                }
                
                //Store the old W(j)
                double W_j = model[cur_index];
                
                //Update the current coordinate
                model[cur_index] = model[cur_index] - step_size * F_partial;
                
                double diff = model[cur_index] - W_j;
                
                //Factorized computation
                if(col_index_R < avail_col_R)
                {
                    for(long k = 0; k < row_num_R; k ++)
                    {
                        X_R_f[k] = diff*cache_R[col_index_R][k];
                    }
                    
                    //Update the intermediate variable
                    //H = H + (Wj - old_Wj)* X(,j)
                    for(long m = 0; m < row_num_S; m ++ )
                    {
                        long fk = KKMR[m];
                        H[m] = H[m] + X_R_f[fk-1];
                    }
                    
                }
                else
                {
                    for(long k = 0; k < row_num_R; k ++)
                    {
                        X_R_f[k] = diff*X_R[k];
                    }
                    
                    //Update the intermediate variable
                    //H = H + (Wj - old_Wj)* X(,j)
                    for(long m = 0; m < row_num_S; m ++ )
                    {
                        long fk = KKMR[m];
                        H[m] = H[m] + X_R_f[fk-1];
                    }
                }
                
            }
            
        }
        
        r_prev = F;
        //Caculate F
        F = 0.00;
        for(int i = 0; i < row_num_S; i ++)
        {
            double tmp = lm.Fe_lr(Y[i],H[i]);
            F += tmp;
        }
        
        r_curr = F;
        k++;
    }
    while(!stop(k,r_prev,r_curr,_setting));
    
    delete [] Y;
    delete [] H;
    delete [] X_R_f;
    delete [] KKMR;
    
    if(avail_col_S < feature_num_S)
    {
        delete [] X_S;
    }
    if(avail_col_R < feature_num_R)
    {
        delete [] X_R;
    }
    
    
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
    
    DM.message("Finish factorize");
    
}

#pragma mark - Block Coordinate Descent
void techniques::materializeBCD(string table_T, setting _setting, double *&model, int block_size)
{
    DataManagement DM;
    DM.message("Start materializeBCD");
    linear_models lm;
    vector<long> tableInfo(3);
    vector<string> fields = DM.getFieldNames(table_T, tableInfo);
    int feature_num = (int)tableInfo[1];
    long row_num = tableInfo[2];
    int block_num = feature_num/block_size;
    int block_residual = feature_num%block_size;
    block_num = block_residual > 0 ? (block_num + 1) : block_num;
    
    double *Y;
    double *H;
    double *X;
    //Additional column space reserved for gradient computation
    double *G;
    double *difference;
    
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
    //Allocate the memory of G
    G = new double[row_num];
    //Allocate the memory of difference
    difference = new double[row_num];
    
    double F = 0.00;
    double F_partial[block_size];
    //Initialize the partial graident for every block
    for(int i = 0; i < block_size; i ++)
    {
        F_partial[i] = 0.00;
    }
    double r_curr = 0.00;
    double r_prev = 0.00;
    int k = 0;
    
    for(int i = 0; i < feature_num; i ++)
    {
        model[i] = 0.00;
        H[i] = 0.00;
        G[i] = 0.00;
        difference[i] = 0.00;
    }
    
    DM.fetchColumn(fields[1], row_num, Y);
    
    //Two level shuffling: first shuffling all columns, then all blocks
    vector<int> original_index;
    vector<int> shuffling_index;
    vector<int> original_block_index;
    vector<int> shuffling_block_index;
    
    //Initialize the original_index_set
    for(int i = 0; i < feature_num; i ++)
    {
        original_index.push_back(i);
    }
    
    for(int i = 0; i < block_num; i ++)
    {
        original_block_index.push_back(i);
    }
    
    //Shuffling
    shuffling_index = shuffle(original_index, (unsigned)time(NULL));
    shuffling_block_index = shuffle(original_block_index, (unsigned)time(NULL));
    
    //Print the shuffling_index and shuffling_block_index
    cout<<"After shuffling, the feature indexes: "<<endl;
    for(int i = 0; i < feature_num; i ++)
    {
        cout<<"["<<shuffling_index.at(i)<<"] "<<endl;
    }
    
    cout<<"After shuffling, the block indexes: "<<endl;
    for(int i = 0; i < block_num; i ++)
    {
        cout<<"["<<shuffling_block_index.at(i)<<"] "<<endl;
    }
    
    //First do Logistic Regression
    do
    {
        //Update one "block" each time
        //"Cumulative" difference in H caused by block
        for(int j = 0; j < block_num; j ++)
        {
            int cur_block_index = shuffling_block_index.at(j);
            cout<<"Current_block_index: "<<cur_block_index<<endl;
            
            int cur_block_size = 0;
            
            //Check whether the current block is the "residual"
            if( (cur_block_index == block_num - 1) && block_residual > 0 )
            {
                cur_block_size = block_residual;
            }
            else
            {
                cur_block_size = block_size;
            }
            
            for(long d = 0; d < row_num; d ++)
            {
                difference[d] = 0.00;
            }
            
            //Start with "first level" block index
            int block_start_index= 0;
            
            //Double indexing: here, the index is the "index" of the "real index"
            //Update each 'block' by starting with getting the block index
            block_start_index = cur_block_index*block_size;
            
            cout<<"Block_start_index: "<<shuffling_index.at(block_start_index)<<endl;
            
            //First calculate the statistics used for gradient
            for(long g = 0; g < row_num; g ++)
            {
                G[g] = lm.G_lr(Y[g],H[g]);
            }
            
            for(int b = 0; b < cur_block_size; b ++)
            {
                int cur_index = shuffling_index.at(block_start_index+b);
                cout<<"Current feature index: "<<cur_index<<endl;
                
                F_partial[b] = 0.00;
                //Fetch the each column and store the current column into X
                DM.fetchColumn(fields[cur_index+2], row_num, X);
                
                //Compute the partial gradient
                for(long i = 0; i < row_num ; i ++)
                {
                    F_partial[b] += G[i]*X[i];
                }
                
                
                //Store the old W(j)
                int cur_model_index = cur_index;
                double diff = model[cur_model_index];
                //Update the current coordinate
                model[cur_model_index] = model[cur_model_index] - step_size * F_partial[b];
                
                //Compute the difference on current coordinate
                diff = model[cur_model_index] - diff;
                //Update the cumulative difference
                for(long m = 0; m < row_num; m ++)
                {
                    difference[m] += diff*X[m];
                }
            }
            
            for(long m = 0; m < row_num; m ++ )
            {
                H[m] = H[m] + difference[m];
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
        
    }
    while(!stop(k,r_prev,r_curr,_setting));
    
    delete [] Y;
    delete [] X;
    delete [] H;
    delete [] G;
    delete [] difference;
    
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
   
    DM.message("Finish materializeBCD");
    
}

void techniques::factorizeBCD(string table_S, string table_R, setting _setting, double *&model, int block_size)
{
    DataManagement DM;
    DM.message("Start factorizeBCD");
    linear_models lm;
    
    //Get the table information and column names
    vector<long> tableInfo_S(3);
    vector<long> tableInfo_R(3);
    vector<string> fields_S = DM.getFieldNames(table_S, tableInfo_S);
    vector<string> fields_R = DM.getFieldNames(table_R, tableInfo_R);
    int feature_num_S = (int)tableInfo_S[1];
    int feature_num_R = (int)tableInfo_R[1];
    int feature_num = feature_num_S + feature_num_R;
    int row_num = tableInfo_S[2];
    int row_num_R = tableInfo_R[2];
    
    //Block Info
    int block_num = feature_num/block_size;
    int block_residual = feature_num%block_size;
    block_num = block_residual > 0 ? (block_num + 1) : block_num;
    
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
    //Additional column space reserved for gradient computation
    double *G;
    double *difference;
    
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
    //Allocate the memory of G
    G = new double[row_num];
    //Allocate the memory of difference
    difference = new double[row_num];
    
    double F = 0.00;
    double F_partial[block_size];
    //Initialize the partial graident for every block
    for(int i = 0; i < block_size; i ++)
    {
        F_partial[i] = 0.00;
    }
    double r_curr = 0.00;
    double r_prev = 0.00;
    int k = 0;
    
    
    //cout<<"Model: "<<endl;
    for(int i = 0; i < feature_num; i ++)
    {
        model[i] = 0.00;
        H[i] = 0.00;
        G[i] = 0.00;
        difference[i] = 0.00;
    }
    //cout<<endl;
    
    DM.fetchColumn(fields_S[1], row_num, Y);
    
    //Two level shuffling: first shuffling all columns, then all blocks
    vector<int> original_index;
    vector<int> shuffling_index;
    vector<int> original_block_index;
    vector<int> shuffling_block_index;
    
    //Initialize the original_index_set
    for(int i = 0; i < feature_num; i ++)
    {
        original_index.push_back(i);
    }
    
    for(int i = 0; i < block_num; i ++)
    {
        original_block_index.push_back(i);
    }
    
    //Shuffling
    shuffling_index = shuffle(original_index, (unsigned)time(NULL));
    shuffling_block_index = shuffle(original_block_index, (unsigned)time(NULL));
    
    //Print the shuffling_index and shuffling_block_index
    cout<<"After shuffling, the feature indexes: "<<endl;
    for(int i = 0; i < feature_num; i ++)
    {
        cout<<"["<<shuffling_index.at(i)<<"] "<<endl;
    }
    
    cout<<"After shuffling, the block indexes: "<<endl;
    for(int i = 0; i < block_num; i ++)
    {
        cout<<"["<<shuffling_block_index.at(i)<<"] "<<endl;
    }
    
    //First do Logistic Regression
    do
    {
        //Update one "block" each time
        //"Cumulative" difference in H caused by block
        
        for(int j = 0; j < block_num; j ++)
        {
            int cur_block_index = shuffling_block_index.at(j);
            cout<<"Current_block_index: "<<cur_block_index<<endl;
            
            int cur_block_size = 0;
            
            //Check whether the current block is the "residual"
            if( (cur_block_index == block_num - 1) && block_residual > 0 )
            {
                cur_block_size = block_residual;
            }
            else
            {
                cur_block_size = block_size;
            }
            
            for(long d = 0; d < row_num; d ++)
            {
                difference[d] = 0.00;
            }
            
            //Start with "first level" block index
            int block_start_index= 0;
            
            //Double indexing: here, the index is the "index" of the "real index"
            //Update each 'block' by starting with getting the block index
            block_start_index = cur_block_index*block_size;
            
            cout<<"Block_start_index: "<<shuffling_index.at(block_start_index)<<endl;
            
            //First calculate the statistics used for gradient
            for(long g = 0; g < row_num; g ++)
            {
                G[g] = lm.G_lr(Y[g],H[g]);
            }
            
            
            for(int b = 0; b < cur_block_size; b ++)
            {
                int cur_index = shuffling_index.at(block_start_index + b);
                cout<<"Current feature index: "<<cur_index<<endl;
                F_partial[b] = 0.00;
                
                //Check whether the column is in table R. If it is, applied factorized learning
                if(cur_index < feature_num_S)
                {
                    
                    //Fetch each column and store the column into X
                    DM.fetchColumn(fields_S[cur_index+3], row_num, X);
                    
                    //Compute the partial gradient
                    for(long i = 0; i < row_num; i ++)
                    {
                        F_partial[b] += G[i]*X[i];
                    }
                    
                    //Store the old Wj
                    int cur_model_index = cur_index;
                    double diff = model[cur_model_index];
                    //Update the current coordinate
                    model[cur_model_index] = model[cur_model_index] - step_size * F_partial[b];
                    //Compute the difference
                    diff = model[cur_model_index] - diff;
                    //Update the cumulative difference
                    for(long m = 0; m < row_num; m ++)
                    {
                        difference[m] += diff*X[m];
                    }
                    
                }
                else
                {
                    //Dynamically allocate two columns of size of column in R
                    //One to store statistic for computing gradient;
                    //one to store corresponding column in R;
                    //one to store statistics for updating coordinate (difference)
                    double **tmp_buffer = new double*[3];
                    for(int i = 0; i < 3; i ++)
                    {
                        tmp_buffer[i] = new double[row_num_R];
                        for(long j = 0; j < row_num_R; j ++)
                        {
                            tmp_buffer[i][j] = 0.00;
                        }
                    }
                    
                    //Fetch the corresponding column in R
                    DM.fetchColumn(fields_R[1+cur_index-feature_num_S],row_num_R, tmp_buffer[0]);
                    
                    //Apply factorized learning to gradient computation
                    for(long m = 0; m < row_num; m ++)
                    {
                        long fk = KKMR[m];
                        tmp_buffer[1][fk-1] += G[m];
                    }
                    for(long j = 0; j < row_num_R; j ++)
                    {
                        F_partial[b] += tmp_buffer[0][j]*tmp_buffer[1][j];
                    }
                    
                    int cur_model_index = cur_index;
                    double diff = model[cur_model_index];
                    model[cur_model_index] = model[cur_model_index] - step_size * F_partial[b];
                    diff = model[cur_model_index] - diff;
                    //Apply factorized learning to difference (of model/coordinate) computation
                    for(int i = 0; i < row_num_R; i ++ )
                    {
                        tmp_buffer[2][i] = diff*tmp_buffer[0][i];
                    }
                    for(long m = 0; m < row_num; m ++)
                    {
                        long fk = KKMR[m];
                        difference[m] += tmp_buffer[2][fk-1];
                    }
                    
                    delete [] tmp_buffer[0];
                    delete [] tmp_buffer[1];
                    delete [] tmp_buffer[2];
                    delete [] tmp_buffer;
                }
                
            }
            
            for(long m = 0; m < row_num; m ++)
            {
                H[m] = H[m] + difference[m];
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
    
    }
    while(!stop(k,r_prev,r_curr,_setting));
    
    delete [] Y;
    delete [] X;
    delete [] H;
    delete [] G;
    delete [] difference;
    
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
    
    DM.message("Finish factorizeBCD");
}


#pragma mark - Gradient descent
/*
 Read a single file the columns of which are in format like: id, label, feature
 The offset entry for W0 is not considered for now
 Logistic Regression for now
 */

//Specific techniques selection: flag (for generalization purpose)
//Stochastic Gradient Descent
void techniques::SGD(vector< vector<double> > data, setting _setting, double *&model, int feature_num)
{
    DataManagement::message("Start SGD");
    long data_size = data.size();
    vector<long> original_index_set;
    vector<long> shuffling_index;
    //Initialize the original_index_set
    for(long i = 0; i < data_size; i ++)
    {
        original_index_set.push_back(i);
    }
    
    //Shuffling
    shuffling_index = shuffle(original_index_set, (unsigned)time(NULL));
    
    linear_models lm;
    //setting
    double step_size = _setting.step_size;
    
    //Allocate the memory to model
    model = new double[feature_num];
    
    for(int i = 0; i < feature_num; i ++)
    {
        model[i] = 0.00;
        
    }
    
    //Loss Function
    double F = 0.00;
    double r_curr = 0.00;
    double r_prev = 0.00;
    int k = 0;
    
    do
    {
        r_prev = F;
        F = 0.00;
        vector<double> gradient(feature_num,0.00);
        
        for(long j = 0; j < data_size; j ++)
        {
            long cur_index = shuffling_index[j];
            
            //Update the model
            double output = 0.00;
            for(int k = 0; k < feature_num; k ++)
            {
                output += model[k]*data[cur_index][k+2];
            }
            
            for(int k = 0; k < feature_num; k ++)
            {
                gradient[k] = lm.G_lr(data[cur_index][1],output)*data[cur_index][2+k];
                model[k] = model[k]-step_size*gradient[k];
            }
            
        }
        
        //Calculate F
        for(long j = 0; j < data_size; j ++)
        {
            double output = 0.00;
            for(int k = 0; k < feature_num; k ++)
            {
                output += model[k]*data[j][k+2];
            }
            double tmp = lm.Fe_lr(data[j][1], output);
            F += tmp;
        }
        
        r_curr = F;
        k ++;
    }
    while(!stop(k,r_prev,r_curr,_setting));
    
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
    
    DataManagement::message("Finish SGD");

}

#pragma mark - Batch Gradient Descent
//Batch Gradient Descent
void techniques::BGD(vector< vector<double> > data, setting _setting, double *&model, int feature_num)
{
    DataManagement::message("Start BGD");
    long data_size = data.size();
    
    linear_models lm;
    //setting
    double step_size = _setting.step_size;
    
    //Allocate the memory to the model
    model = new double[feature_num];
    
    for(int i = 0; i < feature_num; i ++)
    {
        model[i] = 0.00;
        
    }
    
    //Loss Function
    double F = 0.00;
    double r_curr = 0.00;
    double r_prev = 0.00;
    int k = 0;
    
    do
    {
        r_prev = F;
        F = 0.00;
        vector<double> gradient(feature_num,0.00);
        
        for(long j = 0; j < data_size; j ++)
        {
            
            //Update the model
            double output = 0.00;
            for(int k = 0; k < feature_num; k ++)
            {
                output += model[k]*data[j][2+k];
            }
            
            for(int k = 0; k < feature_num; k ++)
            {
                gradient[k] += lm.G_lr(data[j][1],output)*data[j][2+k];
            }
            
        }
        
        
        for(int k = 0; k < feature_num; k ++)
        {
            model[k] = model[k]-step_size*gradient[k];
        }
        
        for(long j = 0; j < data_size; j ++)
        {
            double output = 0.00;
            for(int k = 0; k < feature_num; k ++)
            {
                output += model[k]*data[j][2+k];
            }
            double tmp = lm.Fe_lr(data[j][1], output);
            cout<<"tmp loss: "<<tmp<<endl;
            F += tmp;
        }
        
        
        r_curr = F;
        printf("The loss: %lf\n",F);
        k ++;
    }
    while(!stop(k,r_prev,r_curr,_setting));
    
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
    
    DataManagement::message("Finish BGD");

}

void techniques::classify(vector< vector<double> > data, vector<double> model)
{
    linear_models lm;
    // Count the number of correct classifcation
    long count = 0;
    long data_size =  data.size();
    if(data.at(0).size() != model.size()+2)
    {
        DataManagement::errorMessage("Inconsistent file provided");
    }
    
    int featureNum = (int)model.size();
    for(long i = 0; i < data_size; i ++)
    {
        double actual_label = data[i][1];
        double predicted_label = 0.00;
        double confidence = 0.00;
        double output = 0.00;
        for(int j = 0; j < featureNum; j ++)
        {
            output += model[j]*data[i][2+j];
        }
        cout<<"W^TX: "<<output<<endl;
        confidence = lm.C_lr(output);
        if(confidence > 0.5)
        {
            predicted_label = 1.00;
        }
        else
        {
            predicted_label = -1.00;
        }
        if(actual_label == predicted_label)
        {
            cout<<"Prediction Correct"<<endl;
            count++;
        }
        else
        {
            cout<<"Prediction Wrong"<<endl;
        }
        cout<<"Confidence: "<<confidence<<endl;
        cout<<"Actual Label: "<<actual_label<<","<<"Predicted Label: "<<predicted_label<<endl;
    }
    cout<<"Correcteness: "<<(double)count/(double)data_size<<endl;
    
}

#pragma mark - shuffling
vector<int> techniques::shuffle(vector<int> &index_set, unsigned seed)
{
    vector<int> original_set = index_set;
    int size = (int)index_set.size();
    vector<int> new_index_set;
    srand (seed);
    for(int i = 0; i < size; i ++)
    {
        int cur_size = (int)original_set.size();
        int rand_index = random()%cur_size;
        new_index_set.push_back(original_set.at(rand_index));
        original_set.erase(original_set.begin()+rand_index);
    }
    
    return new_index_set;
}

vector<long> techniques::shuffle(vector<long> &index_set, unsigned seed)
{
    vector<long> original_set = index_set;
    long size = (long)index_set.size();
    vector<long> new_index_set;
    srand(seed);
    for(long i = 0; i < size; i ++)
    {
        long cur_size = original_set.size();
        long rand_index = random()%cur_size;
        new_index_set.push_back(original_set.at(rand_index));
        original_set.erase(original_set.begin()+rand_index);
    }
    
    return new_index_set;
}

#pragma mark - stop creteria
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




