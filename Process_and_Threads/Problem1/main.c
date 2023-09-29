#include <stdio.h>
#include <stdlib.h>

#include "matrix.h"
#include "utilities.h"
#include "time_count.h"


static void matrix_init_value(matrix_t *matrix)
{
    for(int i = 0; i < matrix->m_row; i++)
    {
        for (int j = 0; j < matrix->m_column; j++)
        {
            matrix->m_data[i][j] = j * i;
        }
    }
}
int main(int argc, char *argv[])
{
    matrix_t *matrix_a = NULL;
    matrix_t *matrix_b = NULL;
    matrix_t *matrix_mul = NULL;
    if(argc < 5 || strcmp(argv[1], "--help") == 0)
    {
        printf("Compare mulpiple matrix in c\r\n");
    }
    const int row_a = atoi(argv[1]);
    const int col_a = atoi(argv[2]);

    const int row_b = atoi(argv[3]);
    const int col_b = atoi(argv[4]);

    const int row_mul = row_a;
    const int col_mul = col_mul;

    matrix_a = initialize_matrix(row_a, col_a);
    matrix_b = initialize_matrix(row_b, col_b);
    //matrix_mul = initialize_matrix(row_mul, col_mul);

    matrix_init_value(matrix_a);
    matrix_init_value(matrix_b);

    //print_matrix(matrix_a);
   // print_matrix(matrix_b);

    time_count_begin();
    matrix_mul = multiply_matrix(matrix_a, matrix_b);
    time_count_stop();
    time_count_print();

    //print_matrix(matrix_mul);
}