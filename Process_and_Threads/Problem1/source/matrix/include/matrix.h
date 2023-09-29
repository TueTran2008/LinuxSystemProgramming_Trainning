#ifndef _MATRIX_H_
#define _MATRIX_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>
#include <ctype.h> 
#include <string.h>

typedef struct {
    int m_row;
    int m_column;
	float **m_data;
} matrix_t;

/*!
* @brief <Allocate memory for matrix>
*
* @return *matrix_t <pointer point to struct matrix_t of the matrix>
*/
matrix_t *initialize_matrix(int row, int collum);

/*!
* @brief <Take input matrix from the keyboard>
*
* @param *p_matrix [In/Out] <pointer point to struct matrix_t of the matrix>
*
* @return None
*/
void get_matrix_data(matrix_t *p_matrix);

/*!
* @brief <Print the matrix to the terminal>
*
* @param *p_matrix [In] <pointer point to struct matrix_t of the matrix>
*
* @return None
*/
void print_matrix(matrix_t *p_matrix);

/*!
* @brief <Calculate the sum of two matrices>
*
* @param *p_matrix_a [In] <pointer point to struct matrix_t of the first matrix>
*
* @param *p_matrix_b [In] <pointer point to struct matrix_t of the seccond matrix>
*
* @return *sum_matrix <pointer point to struct matrix_t of the sum of 2 matrix>
*/
matrix_t *sum_matrix(matrix_t *p_matrix_a, matrix_t *p_matrix_b);

/*!
* @brief <Calculate the sum of two matrices>
*
* @param *p_matrix_a [In] <pointer point to struct matrix_t of the first matrix>
*
* @param *p_matrix_b [In] <pointer point to struct matrix_t of the seccond matrix>
*
* @return *product_matrix <pointer point to struct matrix_t of the product of 2 matrix>
*/
matrix_t *multiply_matrix(matrix_t *p_matrix_a, matrix_t *p_matrix_b);

/*!
* @brief <Free matrix allocated memory>
*
* @param count [In] <numbers of matrix to free>
*
* @param matrix_t *, ... [In] <matrix to free>
*
* @return None
*/
void free_matrix(int count, ...);

/*!
* @brief <Get matrix size of the user>
*
* @param *row [In/Out] <pointer point to the row of matrix>
*
* @param *collum [In/Out] <pointer point to the collum of matrix>
*
* @return None
*/
void get_matrix_size(int *row, int *collum);

#endif