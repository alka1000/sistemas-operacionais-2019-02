// PingPongOS - PingPong Operating System
// Prof. Carlos A. Maziero, DAINF UTFPR
// Versão 1.0 -- Março de 2015
//
// Interface do núcleo para as aplicações
// Leonardo Reis - Amir Leonardo


#include <stdio.h>
#include "pingpong.h"
#include <stdlib.h>
#include "queue.c"

// Init do código
#define STACKSIZE 32768		/* tamanho de pilha das threads */

task_t *current_task, *main_task; //uma para referenciar como a atual e outra para a main, para conseguir ir e voltar de contextos mais fácil
task_t *dptch_task;
int id_counter = 0; // contador progressivo para dar ids às tasks

// Funcs
struct Queue* readyQ;

void task_yield() {
    dequeue(readyQ);
    enqueue(readyQ, current_task); 
    current_task->state = 0;
    task_switch(dptch_task);
}

task_t *scheduler()
{
  task_t *next_task = dptch_task->next;
  return next_task;
}


void dispatcher_body(void *arg)
{
  int usetTasks = (readyQ->size) - 2;
  while (usetTasks > 0) {
    task_t* next = scheduler();
    if (next) {
      task_switch(next);
      usetTasks = (readyQ->size) - 2;
    }
  }
  task_exit(0);
}



int task_id() {
    //retorna o id da tarefa rodando
    return current_task->id;
}

void task_exit(int exitCode)
{  
  dequeue(readyQ);
  if (current_task == dptch_task) { // se encerando dispatcher
    task_switch(main_task);
    free(dptch_task);
    free(main_task);
  }
  else {
    task_switch(dptch_task);
  }
}

int task_switch (task_t *task) {
    // auxiliar para troca de tarefa
    task_t *aux = current_task;
    current_task = task; // troca da tarefa atual
    task->state = 1;
    swapcontext(&(aux->ctx),&(task->ctx)); // troca o contexto - tem que ser depois da troca da tarefa senão fica em loop.
    return 0;
}


void task_suspend(task_t *task, struct Queue* queue)
{
  if (task == NULL) {
    task = current_task;
  }
  if (queue == NULL) {
    return;
  }
  if (task->state == 1) {
    dequeue(readyQ);
    enqueue(queue, task);
    task->state = 2;
  }
  else
  {
    printf("Tentando suspender tarefa %d, mas nao está executando\n",task->id );
    exit(1);
  }
}


void task_resume (task_t *task)
{
  /**if (task->enqueued) {
    queue_remove((queue_t**)&ready_queue,(queue_t*)task);
  }*/
  queue_append((queue_t**)&ready_queue,(queue_t*)task);
  task->state = 1;
}

int task_create (task_t *task, void (*start_routine)(void *), void *arg) {
    //seguindo contexts.c de p01 criação de stack
    char *stack ;
    getcontext (&task->ctx);
    stack = malloc (STACKSIZE) ;
    if (stack) {
      task->ctx.uc_stack.ss_sp = stack ;
      task->ctx.uc_stack.ss_size = STACKSIZE;
      task->ctx.uc_stack.ss_flags = 0;
      task->ctx.uc_link = 0;
    }
    else
    {
        perror ("Erro na criação da pilha: ");
        exit (1);
    }
    // atribuir o ID
    makecontext(&(task->ctx), (void*)(*start_routine), 1, arg);
    task->id = id_counter;
    task->next = NULL;
    task->prev = NULL;
    id_counter++;
    task->main_ctx = main_task->ctx;
    task->state = 0;
    enqueue(readyQ, task);
    return task->id;
}

void pingpong_init() {
    setvbuf (stdout, 0, _IONBF, 0); /* desativa o buffer da saida padrao (stdout), usado pela função printf */
    readyQ = createQueue(1000);
    main_task = malloc(sizeof(task_t)); // tem q alocar senao dá Segmentation fault
    task_create(main_task,NULL,NULL); // cria a main
    current_task = main_task; //coloca main como atual.
}
