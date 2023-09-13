/* 
Lab 3: NxM Matrix Multiplication
CSCI-2500 Fall 2023
*/

/******************************************************************************/
/* Usual suspects to include  */
/******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

/******************************************************************************/
/* Function Calls */
/******************************************************************************/

typedef struct{
  double** data;
  int rows;
  int cols;
} matrix; 

int mm_alloc(matrix* mat);
int mm_free (matrix* mat);
int mm_print(matrix* mat);
int mm_read (char* filename, matrix* mat);
matrix* mm_matrix_mult(matrix* mat1, matrix* mat2);


int mm_alloc(matrix* mat)
{
/*
Notes:
  Allocate an array of pointers to a double
  Then, allocate an row/array of doubles and assign each double pointer that row
  Make sure you're allocating using mat->data in the matrix struct
  You can use calloc for allocation and zeroing out all the bytes
  - otherwise, you'd want to manually set all arrays to 0 (good practice)
  If your malloc/calloc returns null, the allocation failed
*/
  mat->data = calloc(mat->rows, sizeof(double*));
  if (mat->data == NULL) {
    printf("mm_alloc: allocation failed.\n");
    exit(-1);
  }
  
  for (int i = 0; i < mat->rows; ++i) {
    mat->data[i] = (double *)calloc(mat->cols, sizeof(double));
    if(mat->data[i] == NULL) {
      printf("mm_alloc: allocation failed.\n");
      exit(-1);
    }
  }
  
  return 0;
}

int mm_free(matrix* mat)
{
/*
Notes:
  Reverse the allocation process, but you'll be calling free() instead
*/
  for (int i = 0; i < mat->rows; ++i) {
    free(mat->data[i]);
  }
  free(mat->data);
  /* note: free does not return a value */
  
  return 0;
}

int mm_print(matrix* mat)
{
/*
Notes:
  Print all the data values in your matrix
  Go row by row
  At the end of each row, print a newline
  Between each data value, insert a tab
  Use 2 digits of precision after the decimal (use "%10.2lf\t" as format string)
  See output.txt for expected output formatting
*/
  printf("\n/******** START of 2-D Matrix **********************************/\n");
  for (int i = 0; i < mat->rows; ++i) {
    for (int j = 0; j < mat->cols; ++j) {
      printf( "%10.2lf\t", mat->data[i][j] );
    }
    printf("\n");
  }
  printf("/******** END of 2-D Matrix ************************************/\n");
  
  return 0;
}

int mm_read(char* filename, matrix* mat) 
{
/*
Notes:
  Use fopen to open the matrix file, then use fgets or getline to read lines
  The first line of the matrix contains integers for "#rows #columns"
  - Use these to allocate your arrays for matrix data
  Each subsequent line contains a single data value
  - You can use sscanf to parse each matrix entry
  - First value is data[0][0], next is data[0][1] ... data[#rows-1][#columns-1]
*/
  FILE *matrix_file = NULL;
  mat->data         = NULL;
  char buffer[256];
  
  if (NULL == (matrix_file = fopen(filename, "r"))) {
    // file doesn't exist, or error occurred
    printf("mm_read: failed to open file.");
    exit(-1);
  }
  
  if (NULL == fgets(buffer, 256, matrix_file)) {
    // file is empty
    printf("mm_read: failed to read from file.");
    exit(-1);
  }
  
  if (2 != sscanf(buffer, "%d %d", &mat->rows, &mat->cols)) {
    // number of integers on first row is not equal to 2
    // so there is a formatting issue with the matrix file
    printf("mm_read: failed to read matrix dimensions.\n");
    exit(-1);
  }
  
  mm_alloc(mat);
  
  // Read matrix row by row, if we encounter eof or an empty line,
  // we know there's some formatting issues with the file
  for (int i = 0; i < mat->rows; ++i) {
    for (int j = 0; j < mat->cols; ++j) {
      if (NULL == fgets(buffer, 256, matrix_file)) {
        // we prematurely hit the end of the file
        printf("mm_read: failed to read matrix values.\n");
        exit(-1);
      }
      if (1 != sscanf(buffer, "%lf", &mat->data[i][j])) {
        // we've hit a line without any float value
        printf("mm_read: failed to read matrix values.\n");
        exit(-1);
      }
    }
  }
  
  if(0 != fclose(matrix_file)) {
    // Error closing file - you didn't need to check for this
    printf("mm_read: fclose failed: %s\n", strerror(errno));
  }
  
  return 0;
}


matrix* mm_matrix_mult(matrix* mat1, matrix* mat2)
{
/*
Notes:
  First check that it is actually possible to multiply the two inputs
  If so, construct a result matrix
  Then perform the multiplication
  Then return a pointer to your result matrix
*/
  if (mat1->cols != mat2->rows) {
    printf("mm_matrix_mult: dimension mismatch between matrices.\n");
    exit(-1);
  }
  
  matrix* result = (matrix*)malloc(sizeof(matrix));
  result->rows = mat1->rows;
  result->cols = mat2->cols;
  mm_alloc(result);
  
  for(int i = 0; i < mat1->rows; ++i) {
    for(int j = 0; j < mat2->cols; ++j) {
      for(int k = 0; k < mat1->cols; ++k) {
        // Note: because I called calloc() to initialize, I can
        // directly place results into and update data[i][j]
        result->data[i][j] +=  mat1->data[i][k] * mat2->data[k][j];
      }
    }
  }
  
  return result;
}

int main()
{
  char filename_A[256];
  char filename_B[256];
  matrix* A = (matrix*)malloc(sizeof(matrix));
  matrix* B = (matrix*)malloc(sizeof(matrix));
  matrix* C = NULL;

  printf("Please enter the matrix A data file name: ");
  if (1 != scanf("%s", filename_A)) {
    printf("main: scanf failed to read filename\n");
    exit(-1);
  }
  mm_read(filename_A, A);
  mm_print(A);
  
  printf("Please enter the matrix B data file name: ");
  if (1 != scanf("%s", filename_B)) {
    printf("main: scanf failed to read filename\n");
    exit(-1);
  }
  mm_read(filename_B, B);
  mm_print(B);
  
  C = mm_matrix_mult(A, B);
  mm_print(C);

  mm_free(A);
  mm_free(B);
  mm_free(C);
  free(A);
  free(B);
  free(C);
  
  return 0;
}