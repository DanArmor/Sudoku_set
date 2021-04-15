#include <stdlib.h>
#include <stdio.h>
#include <math.h>
// #include <time.h>
#include "./src/bitset.h"

typedef struct sudoku_grid{
  int **grid; //grid as matrix of int
  size_t size; //length of one size
  size_t size_sqrt;
  Bit_set rows;
  Bit_set cols;
  Bit_set regs;
  size_t need; //Amount of cells to fill 
}sudoku_grid;

int IS_SOLVED = 0;

//Basic matrix operations
void copy_matrix(int **a, int **b, size_t n){
  for(register size_t i = 0; i < n; i++)
    for(register size_t j = 0; j < n; j++)
      b[i][j] = a[i][j];
}

int **get_memory_matrix(size_t n){
  int **a = (int**)calloc(n, sizeof(int*));
  for(register size_t i = 0; i < n; i++)
    a[i] = (int*)calloc(n, sizeof(int));
  return a;
}

void input_matrix(int **a, size_t n){
  for(register size_t i = 0; i < n; i++)
    for(register size_t j = 0; j < n; j++)
      scanf("%d", &a[i][j]);
}

void output_matrix(int **a, size_t n){
  for(register size_t i = 0; i < n; i++){
    for(register size_t j = 0; j < n; j++)
      printf("%d ", a[i][j]);
    printf("\n");
  }
}

void destroy_matrix(int **a, size_t n){
  for(register size_t i = 0; i < n; i++)
    free(a[i]);
  free(a);
}

//For example with n = 9
//First row indexes are [1 .. 9] and next one - [10 .. 18] and so on
size_t get_row_index(sudoku_grid *grid, size_t value, size_t i){
  return value + i * grid->size;
}

//Similar to rows
size_t get_col_index(sudoku_grid *grid, size_t value, size_t j){
  return value + j * grid->size;
}

//For example with n = 9
//For first area indexes are [1 .. 9]
//first three squares(areas) indexes for numbers from 1 to 9 are [1 .. 27]
size_t get_reg_index(sudoku_grid *grid, size_t value, size_t i, size_t j){
  return value + (i / grid->size_sqrt) * grid->size * grid->size_sqrt + (j / grid->size_sqrt) * grid->size;
}

//If some cells already filled with numbers - we need to cover them before the solution
void cover_advance_grid(sudoku_grid *grid){
  for(register size_t i = 0; i < grid -> size; i++)
    for(register size_t j = 0; j < grid -> size; j++)
      if(grid->grid[i][j]){
        add_set(&grid->rows, get_row_index(grid, grid->grid[i][j], i));
        add_set(&grid->cols, get_col_index(grid, grid->grid[i][j], j));
        add_set(&grid->regs, get_reg_index(grid, grid->grid[i][j], i, j));
      }
      else
        grid->need++;
}

sudoku_grid create_sudoku_grid(int **a, size_t n){
  sudoku_grid grid;
  grid.grid = get_memory_matrix(n);
  copy_matrix(a, grid.grid, n);

  grid.rows = create_set(1, n*n);
  grid.cols = create_set(1, n*n);
  grid.regs = create_set(1, n*n);
  grid.size = n;
  grid.size_sqrt = (size_t)sqrt((double)n);
  grid.need = 0;

  cover_advance_grid(&grid);
  return grid;
}

//If everything is ok - returns 1, otherwise - 0
int check_solution(sudoku_grid *grid, size_t i, size_t j, size_t t){
  char is_good = !(is_in_set(&grid->rows, get_row_index(grid, t, i)) || is_in_set(&grid->cols, get_col_index(grid, t, j)) || is_in_set(&grid->regs, get_reg_index(grid, t, i, j)));
  //We check, is any element already exist in set. If it is - we get 1 and it with ! it becomes 0
  return is_good;
}

/*
  1. While we have cells to go and current cell is filled - we increasing indexes of row and column. Otherwise - skip
  2. If there is no cell to go - turn on flag, print solution and stop processing of sudoku
  3. For each number from 1 to n AND there is no solution yet DO:
    -1. If current combination of row, column and number is NOT valid - try next number. If it IS valid:
      --Add necessary elements to sets of grid.
      --Increase param k.
      --Next step ot the solution
    == If next step have provided no solution at all:
      --Decrease param k.
      --Delete elements from sets
  If you have tried every number - just clear the cell
*/
void solve_inner(sudoku_grid *grid, size_t i, size_t j){
  static size_t k = 1;
  while(grid->grid[i][j] && k <= grid->need){
    i = (j == grid->size - 1) ? i + 1 : i;
    j = (j == grid->size - 1) ? 0 : j + 1;
  }
  if(k > grid->need){
    IS_SOLVED = 1;
    output_matrix(grid->grid, grid->size);
  }
  for(size_t t = 1; t <= grid->size && !IS_SOLVED; t++){
    if(check_solution(grid, i, j, t)){
      grid->grid[i][j] = t;
      add_set(&grid->rows, get_row_index(grid, grid->grid[i][j], i));
      add_set(&grid->cols, get_col_index(grid, grid->grid[i][j], j));
      add_set(&grid->regs, get_reg_index(grid, grid->grid[i][j], i, j));        
      k++;
      solve_inner(grid, i, j);
      k--;
      delete_from_set(&grid->rows, get_row_index(grid, grid->grid[i][j], i));
      delete_from_set(&grid->cols, get_col_index(grid, grid->grid[i][j], j));
      delete_from_set(&grid->regs, get_reg_index(grid, grid->grid[i][j], i, j));
    }
  }
  grid->grid[i][j] = 0;
}

//"Interface" for solver
void solve(sudoku_grid *grid){
  solve_inner(grid, 0, 0);
}

int main(void){
  printf("Enter the length of one edge of sudoku grid: ");
  size_t size;
  scanf("%llu", &size);

  int **sudoku_arr;
  sudoku_arr = get_memory_matrix(size);

  printf("Input grid\n");
  input_matrix(sudoku_arr, size);

  sudoku_grid grid = create_sudoku_grid(sudoku_arr, size);
  printf("\n");
  // clock_t start = clock();
  solve(&grid);
  // clock_t end = clock();
  // printf("\n%lf\n", (double)((end - start) / CLOCKS_PER_SEC));
  destroy_matrix(sudoku_arr, size);
  destroy_matrix(grid.grid, grid.size);
  destroy_set_values(&grid.rows);
  destroy_set_values(&grid.cols);
  destroy_set_values(&grid.regs);
  return 0;
}