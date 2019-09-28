// PingPongOS - PingPong Operating System
// Prof. Carlos A. Maziero, DAINF UTFPR
// Versão 1.0 -- Março de 2015
//
// Estruturas de dados internas do sistema operacional

#ifndef __DATATYPES__
#define __DATATYPES__

#include <ucontext.h>

typedef enum states
{
    READY,
    RUNNING,
    SUSPENDED
}STATES;

// Estrutura que define uma tarefa
typedef struct task_t
{
    struct task_t *prev, *next; //para biblioteca de filas (necessário cast)
    int id;                     //ID da tarefa
    ucontext_t context;         //contexto da tarefa
    ucontext_t main_context;    //contexto do programa principal
    STATES current_state;       //estado da tarefa (pronta, ativa, suspensa)
    int enqueued;               //flag indicando se tarefa esta em alguma fila.
  // preencher quando necessário
} task_t ;

// estrutura que define um semáforo
typedef struct
{
  // preencher quando necessário
} semaphore_t ;

// estrutura que define um mutex
typedef struct
{
  // preencher quando necessário
} mutex_t ;

// estrutura que define uma barreira
typedef struct
{
  // preencher quando necessário
} barrier_t ;

// estrutura que define uma fila de mensagens
typedef struct
{
  // preencher quando necessário
} mqueue_t ;

#endif