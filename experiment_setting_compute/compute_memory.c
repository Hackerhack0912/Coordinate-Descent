#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#define DIVISOR (1024.00*1024.00*1024.00)

void tuple_ratio_mem(double ratio, double nR, double dS, double dR, char const *cd);
void feature_ratio_mem(double ratio, double nS, double nR, double dS, char const *cd);
void iters_num_mem(double nS, double nR, double dS, double dR, char const *cd);

int main(int argc, const char* argv[])
{
   // 0: tuple_ratio; 1: feature_ratio; 2: iters__num
   // compute_mem option function(...)
    if(argc != 7)
    {
        fprintf(stderr, "Invalid number of paramters\n");
        exit(1);
    }
    int option = atoi(argv[1]);
    int p1, p2, p3, p4;
    char const *p5 = argv[6];
    p1 = atoi(argv[2]);
    p2 = atoi(argv[3]);
    p3 = atoi(argv[4]);
    p4 = atoi(argv[5]);
    if(option == 0)
    {
        tuple_ratio_mem(p1, p2, p3, p4, p5);
    }
    else if(option == 1)
    {
        feature_ratio_mem(p1, p2, p3, p4, p5);
    }
    else if(option == 2)
    {
        iters_num_mem(p1, p2, p3, p4, p5);
    }
    else
    {
        fprintf(stderr, "Invalid argument\n");
        exit(1);
    }
   
    return 0;
}

void tuple_ratio_mem(double ratio, double nR, double dS, double dR, char const *cd)
{
    double s_mem = dS*8*nR*ratio/DIVISOR;
    double r_mem = dR*8*nR/DIVISOR;
    double t_mem = (dS+dR)*8*nR*ratio/DIVISOR;
    
    printf("S_MEM = %lf GB; R_MEM = %lf GB; T_MEM = %lf GB\n", s_mem, r_mem, t_mem);
    
    if(strcmp(cd, "scd") == 0)
    {
        double materialize_mem = 3*8*nR*ratio/DIVISOR;
        double stream_mem = (4*8*nR*ratio + 8*nR)/DIVISOR;
        double factorize_mem = (4*8*nR*ratio + 2*8*nR)/DIVISOR;
        
        printf("Minimum memory required: \n");
        printf("Materilaize: %lf GB; ", materialize_mem);
        printf("Stream: %lf GB; ", stream_mem);
        printf("Factorize: %lf GB \n\n", factorize_mem);
    }
    else if(strcmp(cd, "bcd") == 0)
    {
        double materialize_mem = 5*8*nR*ratio/DIVISOR;
        double factorize_mem = (6*8*nR*ratio + 2*8*nR)/DIVISOR;
        
        printf("Minimum memory required: \n");
        printf("Materilaize: %lf GB; ", materialize_mem);
        printf("Factorize: %lf GB\n\n", factorize_mem);
    }
    else
    {
        fprintf(stderr, "Invalid argument\n");
        exit(1);
    }
}

void feature_ratio_mem(double ratio, double nS, double nR, double dS, char const *cd)
{
    double s_mem = nS*8*dS/DIVISOR;
    double r_mem = nR*8*dS*ratio/DIVISOR;
    double t_mem = (dS+dS*ratio)*8*nS/DIVISOR;
    
    printf("S_MEM = %lf GB; R_MEM = %lf GB; T_MEM = %lf GB\n", s_mem, r_mem, t_mem);
    
    if(strcmp(cd, "scd") == 0)
    {
        double materialize_mem = 3*8*nS/DIVISOR;
        double stream_mem = (4*8*nS + 8*nR)/DIVISOR;
        double factorize_mem = (4*8*nS + 2*8*nR)/DIVISOR;
        
        printf("Minimum memory required: \n");
        printf("Materilaize: %lf GB; ", materialize_mem);
        printf("Stream: %lf GB; ", stream_mem);
        printf("Factorize: %lf GB\n\n", factorize_mem);
    }
    else if(strcmp(cd, "bcd") == 0)
    {
        double materialize_mem = 5*8*nS/DIVISOR;
        double factorize_mem = (6*8*nS + 2*8*nR)/DIVISOR;
        
        printf("Minimum memory required: \n");
        printf("Materilaize: %lf GB; ", materialize_mem);
        printf("Factorize: %lf GB\n\n", factorize_mem);
    }
    else
    {
        fprintf(stderr, "Invalid argument\n");
        exit(1);
    }

}

void iters_num_mem(double nS, double nR, double dS, double dR, char const *cd)
{
    double s_mem = dS*8*nS/DIVISOR;
    double r_mem = dR*8*nR/DIVISOR;
    double t_mem = (dS+dR)*8*nS/DIVISOR;
    
    printf("S_MEM = %lf GB; R_MEM = %lf GB; T_MEM = %lf GB\n", s_mem, r_mem, t_mem);
    
    if(strcmp(cd, "scd") == 0)
    {
        double materialize_mem = 3*8*nS/DIVISOR;
        double stream_mem = (4*8*nS + 8*nR)/DIVISOR;
        double factorize_mem = (4*8*nS + 2*8*nR)/DIVISOR;
        
        printf("Minimum memory required: \n");
        printf("Materilaize: %lf GB;", materialize_mem);
        printf("Stream: %lf GB; ", stream_mem);
        printf("Factorize: %lf GB \n\n", factorize_mem);
    }
    else if(strcmp(cd, "bcd") == 0)
    {
        double materialize_mem = 5*8*nS/DIVISOR;
        double factorize_mem = (6*8*nS + 2*8*nR)/DIVISOR;
        
        printf("Minimum memory required: \n");
        printf("Materilaize: %lf GB; ", materialize_mem);
        printf("Factorize: %lf GB\n\n", factorize_mem);
    }
    else
    {
        fprintf(stderr, "Invalid argument\n");
        exit(1);
    }

}
