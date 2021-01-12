#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <sys/time.h>
#include <omp.h>

#define SRAND_VALUE 1985
#define MAX_THREAD 8
#define GENERATIONS 2000
#define dimension 2048

int **grid;
int **newGrid;
int sum_alive;

void generate_random_matrix() {
  int i, j;

  srand(SRAND_VALUE);
  for(i = 1; i <= dimension; i++) {
    for(j = 1; j <= dimension; j++) {
      grid[i][j] = rand() % 2;
    }
  }
}

void create_border() {
  int i, j;

  for(i=1; i < dimension+1; i++){
    grid[0][i] = grid[dimension][i];
    grid[dimension+1][i] = grid[1][i];
  }

  for(i=1; i<dimension+1; i++){
    grid[i][dimension+1] = grid[i][1];
    grid[i][0] = grid[i][dimension];
  }

  grid[0][0] = grid[dimension][dimension];
  grid[0][dimension+1] = grid[dimension][1];
  grid[dimension+1][0] = grid[1][dimension];
  grid[dimension+1][dimension+1] = grid[1][1];
}

void copy_matrix() {
  int **tmpGrid = grid;
  grid = newGrid;
  newGrid = tmpGrid;
}

void deallocate_matrix() {
  int i, j;

  for(i=0; i < dimension+2; i++){
    free(grid[i]);
    free(newGrid[i]);
  }

  free(grid);
  free(newGrid);

  return;
}

int** alocate_matrix() {
  int i, j;
  grid = (int**)malloc(sizeof(int*) * (dimension+2));

  for (i = 0; i < dimension+2; i++){
    grid[i] = (int*) malloc(sizeof(int) * (dimension+2));
  }

  newGrid = (int**)malloc(sizeof(int*) * (dimension+2));

  for(i=0;i<dimension+2;i++){
    newGrid[i] = (int*)malloc(sizeof(int) * (dimension+2));
  }

  return(grid);
}

int get_neighbors_sum(int i, int j) {
  int sum;

  sum = grid[i+1][j] + grid[i-1][j]
        + grid[i][j+1] + grid[i][j-1]
        + grid[i+1][j+1] + grid[i-1][j-1]
        + grid[i-1][j+1] + grid[i+1][j-1];

  return sum;
}

void *iterate_neighbors(){
  int i, j, count = 0, neighbor;

  #pragma omp parallel private(i, j, neighbor) num_threads(MAX_THREAD)
  #pragma omp for
  for(i=1; i<dimension+1; i++){
    for(j=1; j<dimension+1; j++){
      int numNeighbors = get_neighbors_sum(i, j);

      if (grid[i][j] == 1 && numNeighbors < 2)
        newGrid[i][j] = 0;
      else if (grid[i][j] == 1 && (numNeighbors == 2 || numNeighbors == 3))
        newGrid[i][j] = 1;
      else if (grid[i][j] == 1 && numNeighbors > 3)
        newGrid[i][j] = 0;
      else if (grid[i][j] == 0 && numNeighbors == 3)
        newGrid[i][j] = 1;
      else
        newGrid[i][j] = grid[i][j];
    }
  }
}

int get_alive(int **grid){
  int i, j, count;

  sum_alive=0;
  for(i = 1; i < dimension+1; i++){
    #pragma omp parallel for reduction(+:sum_alive)
    for(j = 1; j < dimension+1; j++){
      sum_alive = sum_alive + grid[i][j];
    }
  }

  count = sum_alive;

  return(count);
}

int main() {
  int i, j, k, time_for_threads, alive_count=0;
  int start, end;

  start = omp_get_wtime();
  alocate_matrix();
  generate_random_matrix();
  create_border();
  alive_count = get_alive(grid);
  printf("Initial generation: %d\n", alive_count);

  for(k = 1; k <= GENERATIONS; k++){
    iterate_neighbors();
    copy_matrix();
    alive_count = get_alive(grid);
  }

  alive_count = get_alive(grid);
  printf("Generation %d: %d\n", k - 1, alive_count);
  end = omp_get_wtime();
  time_for_threads = (end - start)*1000;
  printf("Time with %d threads: %d ms \n", MAX_THREAD, time_for_threads);
  deallocate_matrix();

  return 0;
}
