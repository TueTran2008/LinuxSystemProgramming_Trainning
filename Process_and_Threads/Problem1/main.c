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
        printf("Please enter matrix 1 col - matrix 1 row - matrix 2 col - matrix 2 row\r\n");
        return 0;
    }
    const int row_a = atoi(argv[1]);
    const int col_a = atoi(argv[2]);

    const int row_b = atoi(argv[3]);
    const int col_b = atoi(argv[4]);

    const int row_mul = row_a;
    const int col_mul = col_mul;

    matrix_a = initialize_matrix(row_a, col_a);
    matrix_b = initialize_matrix(row_b, col_b);

    matrix_init_value(matrix_a);
    matrix_init_value(matrix_b);

    matrix_mul = multiply_matrix_mul_thread(matrix_a, matrix_b);
    free(matrix_mul);
    
}