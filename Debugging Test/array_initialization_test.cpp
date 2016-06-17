#include<iostream>
#include<stdio.h>
#include<cstring>
#include<stdlib.h>
#include<time.h>

#define TUPLE_NUMBER 2000000

int main()
{
	clock_t start_t, end_t;
	double *r = new double[2000000];
	int total = 0;
	
	printf("Initialize the column array using loop:\n");
	start_t = clock();
	for(int i = 0; i < 2000000; i ++){
		r[i] = 1.00;
	}
	end_t = clock();
	printf("Time: %f\n\n", (double)(end_t-start_t)/(double)CLOCKS_PER_SEC);

	printf("Initialize the column array using memset:\n");
	start_t = clock();
	memset(r, 0, 2000000);
	end_t = clock();
	printf("Time: %f\n", (double)(end_t-start_t)/(double)CLOCKS_PER_SEC);

	printf("Addition Saving Time:\n");
	start_t = clock();
	for(int i = 0; i < 2000000; i ++){
		total += r[i];
	}
	end_t = clock();
	printf("Time: %f\n", (double)(end_t-start_t)/(double)CLOCKS_PER_SEC);

	printf("Multiplication Saving Time:\n");
	start_t = clock();
	for(int i = 0; i < 2000000; i ++){
		total *= r[i];
	}
	printf("Time: %f\n", (double)(end_t-start_t)/(double)CLOCKS_PER_SEC);

}	
