// PingPongOS - PingPong Operating System
// Prof. Carlos A. Maziero, DAINF UTFPR
// Versão 1.0 -- Março de 2015
//
// Estruturas de dados internas do sistema operacional

#include <ucontext.h>

#ifndef __DATATYPES__
#define __DATATYPES__

// Estrutura que define uma tarefa
typedef struct task_t {

  int id;                   //id de uma tarefa
  ucontext_t task_ctx;      //contexto de uma tarefa
  ucontext_t main_ctx;      //contexto da main

} task_t ;

// estrutura que define um semáforo
typedef struct {
  // preencher quando necessário
} semaphore_t ;

// estrutura que define um mutex
typedef struct {
  // preencher quando necessário
} mutex_t ;

// estrutura que define uma barreira
typedef struct {
  // preencher quando necessário
} barrier_t ;

// estrutura que define uma fila de mensagens
typedef struct {
  // preencher quando necessário
} mqueue_t ;

#endif
