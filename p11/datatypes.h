// PingPongOS - PingPong Operating System
// Prof. Carlos A. Maziero, DAINF UTFPR
// Versão 1.0 -- Março de 2015
//
// Estruturas de dados internas do sistema operacional

#include <ucontext.h>
#include "queue.h"

#ifndef __DATATYPES__
#define __DATATYPES__


// Estrutura que define uma tarefa
typedef struct task_t {
  struct task_t *prev, *next ;  // para usar com a biblioteca de filas (cast)
  int id;                   //id de uma tarefa
  ucontext_t ctx;      //contexto de uma tarefa
  ucontext_t main_ctx;      //contexto da main
  int state;              // 0: Waiting, 1: Running, 2: Suspended
  int base_priority;      //prioridade base a que volta quando sai quando vai da enqueue
  int priority;           //prioridade que muda com envelhecimento
  int quantum;  
  unsigned int proc_time; //guarda o tempo de processamento (quanto tempo ficou com o processador)
  unsigned int exec_time; //guarda o tempo de execução desde sua criação
  int activations;  //quantas vezes foi chamada a função
  struct task_t *waiting_task;  //tarefa que está esperando para sair do modo suspenso
  int waiting_status;  //status de saída da tarefa que estava esperando para sair do modo suspenso
  unsigned int sleep_time;  //dormir até o tempo sleep_time
} task_t ;
  

// estrutura que define um semáforo
typedef struct {
  Queue fila; // fila de tarefas
  int count; //contador
} semaphore_t ;

// estrutura que define um mutex
typedef struct {
  // preencher quando necessário
} mutex_t ;

// estrutura que define uma barreira
typedef struct {
  Queue fila; // fila de tarefas
  int count; //contador
} barrier_t ;

// estrutura que define uma fila de mensagens
typedef struct {
  // preencher quando necessário
} mqueue_t ;

#endif
