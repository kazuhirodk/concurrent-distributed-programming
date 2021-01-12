#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#define MAX_THREAD 4 // Clients quantity

int request = 0;
int respond = 0;
int sum = 0;

void *client_process(void *id_thread){
  int t_id = *(int*)id_thread;

  while (1) {
    // Begin pre-protocol
    while(respond != t_id){
      request = t_id;
    }
    // Ends pre-protocol
    sum++;
    printf("Client Thread ID = %d, RC access nº = %d\n", t_id, sum);
    respond = 0;
  }
}

void *server_process(void *id_thread){
  long t_id = *(long*)id_thread;
  printf("Server Thread ID = %ld\n", t_id);

  while(1){
    while(request == 0);
      respond = request;
    while(respond != 0);
      request = 0;
  }
}

int main(void){
  long i;
  pthread_t Clients[MAX_THREAD];
  pthread_t Server;

  // Define ids
  long ids_clients[MAX_THREAD] = {1, 2, 3, 4};
  long id_Server = 1;

  // Creating clients
  for(i=0; i<MAX_THREAD;i++)
    pthread_create(&Clients[i], NULL, client_process, (void*)&ids_clients[i]);

  pthread_create(&Server, NULL, server_process, &id_Server);

  for(i=0;i<MAX_THREAD;i++)
    pthread_join(Clients[i], NULL);

  pthread_join(Server, NULL);

  return(0);
}
