#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include <time.h>
#include <sys/time.h>

#define SRAND_VALUE 1985
#define MAX_THREAD 2
#define GENERATIONS 2000
#define dimension 2048

int **grid;
int **newGrid;

typedef struct thread_data{
	int id;
	int start;
	int end;
} ThreadData;

ThreadData thread[MAX_THREAD];

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

  create_border();
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

void *iterate_neighbors(void *thread_arg){
  int i, j, count = 0, neighbor, dim_start, dim_end;

  ThreadData *data;
  data = (ThreadData*) thread_arg;

	dim_start = data->start;
	dim_end = data->end;

  for(i = dim_start; i < dim_end; i++){
    for(j = 1; j < dimension+1; j++){
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

  pthread_exit(NULL);
}

int get_alive(int **grid){
  int i, j, count = 0;

  for(i = 1; i < dimension+1; i++){
    for(j = 1; j < dimension+1; j++){
      count = count + grid[i][j];
    }
  }

  return(count);
}

int main() {
  int i, j, k, neighbor=0, count=0, tmili, time_for_threads, alive_count=0;

  struct timeval start, end, thread_start, thread_end;
  pthread_t t[MAX_THREAD];
  int result = dimension/MAX_THREAD;

  alocate_matrix();
  generate_random_matrix();
	create_border();

	alive_count = get_alive(grid);

  printf("Initial generation: %d\n", alive_count);

	gettimeofday(&thread_start, NULL);

  for(k = 1; k <= GENERATIONS; k++){
    for(i = 0; i < MAX_THREAD; i++){
      thread[i].id = i;
      thread[i].start = result * i+1;

      if(i != (MAX_THREAD-1)){
        thread[i].end = (result * (i+1) + 1);
      }
      else{
        thread[i].end = dimension+1;
      }
      pthread_create(&t[i], NULL, &iterate_neighbors, (void*)&thread[i]);
    }

    for(i=0; i < MAX_THREAD; i++){
      pthread_join(t[i], NULL);
    }

    copy_matrix();
	}

  gettimeofday(&thread_end, NULL);

  time_for_threads = (int)(
    1000 * (thread_end.tv_sec - thread_start.tv_sec) + (thread_end.tv_usec - thread_start.tv_usec)/1000
  );

  alive_count = get_alive(grid);
  printf("Generation %d: %d\n", k - 1, alive_count);

  printf("Time with %d threads: %d ms \n", MAX_THREAD, time_for_threads);

	deallocate_matrix();
	pthread_exit(NULL);

  return(0);
}
