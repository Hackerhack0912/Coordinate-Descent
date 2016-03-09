/*
 Display the menu 
*/

#include<stdio.h>

int main()
{
    printf("options:\n");
    printf("Type 0: S; Type 1: R\n");
    printf("1. create table: DB create file_name(table_name) type feature_num row_num\n");
    printf("2. join table: DB join table_name1 table_name2 joinTable_name\n");
    printf("3. read table: DB read table_name\n");
    printf("4. materialize: DB m table_name\n");
    printf("5. stream: DB s table_name_S table_name_R\n");
    printf("6. factorize: DB f table_name_S table_name_R\n");
    printf("7. readColumn: DB column_name row_num\n");
    printf("8. Stochastic Gradient Descent: DB SGD train_file_name test_file_name\n");
    printf("9. Batch Gradient Descent:DB BGD train_file_name test_file_name\n");
    printf("10. Block Coordinate Descent: DB BCD table\n");
    return 0;
}
