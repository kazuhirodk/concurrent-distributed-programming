#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#define MAX_THREAD 4 //Num de clientes

int request = 0;
int respond = 0;
int sum = 0;

void *Client_Process(void *id_thread){
  int t_id = *(int*)id_thread;

  while (1) {
    //Inicio do Pré-Protocolo
    while(respond != t_id){
      request = t_id;
    }
    //Fim do pré-protocolo

    sum++; //seção crítica
    printf("Client Thread ID = %d, RC access nº = %d\n", t_id, sum);

    respond = 0; //Pós-Protocolo
  }
}

void *Server_Process(void *id_thread){
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

  //Definindo IDs
  long id_Server = 1;
  long ids_clients[MAX_THREAD];

  for(i=0; i<MAX_THREAD;i++)
    ids_clients[i] = i+1;

  //Criando os clientes
  for(i=0; i<MAX_THREAD;i++)
    pthread_create(&Clients[i], NULL, Client_Process, (void*)&ids_clients[i]);

  pthread_create(&Server, NULL, Server_Process, &id_Server);

  for(i=0;i<MAX_THREAD;i++)
    pthread_join(Clients[i], NULL);

  pthread_join(Server, NULL);
  return(0);
}
