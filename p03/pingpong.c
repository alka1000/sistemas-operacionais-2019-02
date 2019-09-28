// PingPongOS - PingPong Operating System
// Prof. Carlos A. Maziero, DAINF UTFPR
// Versão 1.0 -- Março de 2015
//
// Interface do núcleo para as aplicações
// Leonardo Reis - Amir Leonardo


#include <stdio.h>
#include "pingpong.h"
#include <stdlib.h>
#include "queue.h"


// Init do código
#define STACKSIZE 32768		/* tamanho de pilha das threads */

task_t *current_task, *main_task, *dptch_task; //uma para referenciar como a atual e outra para a main, para conseguir ir e voltar de contextos mais fácil, agora outra pro dispatcher
task_t *queue_rdy = NULL;
task_t *queue_sus = NULL;

int id_counter = 0; // contador progressivo para dar ids às tasks

// Funcs

void task_yield() {
  queue_append((queue_t**)&queue_rdy,(queue_t*)current_task);
  // dequeue(readyQ);
  // enqueue(readyQ, current_task); 
  current_task->state = 0;
  task_switch(dptch_task);
}

//coloca a main sempre no final da fila
void reset_main() {
  queue_remove((queue_t**)&queue_rdy,(queue_t*)main_task);
  queue_append((queue_t**)&queue_rdy,(queue_t*)main_task);
}


task_t *scheduler() {
    reset_main();
    task_t* next_task = dptch_task->next;
    return next_task;
}


void dispatcher_body() {
  int userTasks = queue_size((queue_t*)queue_rdy) - 2;
  while (userTasks > 0) {
    task_t* next = scheduler();
    if (next) {
      userTasks = queue_size((queue_t*)queue_rdy) - 2; //atualiza user tasks
      task_switch(next); // muda de tarefa
    }
  }
  task_exit(0);
}


void task_suspend(task_t *task, task_t **queue)   //usar queue_sus aqui
{
  if (task == NULL) {
    task = current_task;
  }
  if (queue == NULL) {
    return;
  }
  if (task->state == 1) {
    queue_remove((queue_t**)&queue_rdy,(queue_t*)task);
    queue_append((queue_t**)&queue,(queue_t*)task);
    task->state = 2;
  }
}

void task_resume (task_t *task)
{
    queue_remove((queue_t**)&queue_sus,(queue_t*)task);
    queue_append((queue_t**)&queue_rdy,(queue_t*)task);
    task->state = 0;
}


int task_id() {
    //retorna o id da tarefa rodando
    return current_task->id;
}

void task_exit (int exit_code) {
    //sair ou voltar pra main.
    queue_remove((queue_t**)&queue_rdy,(queue_t*)current_task);
    if (current_task == dptch_task) { // se encerando dispatcher
        task_switch(main_task);
        free(dptch_task);
        free(main_task);
    }
    else { // encerando outra tarefa
        task_switch(dptch_task);
    }

}

int task_switch (task_t *task) {
    task->state = 1;                    //roda a tarefa
    // auxiliar para troca de tarefa
    task_t *aux = current_task;
    current_task = task; // troca da tarefa atual
    swapcontext(&(aux->ctx),&(task->ctx)); // troca o contexto - tem que ser depois da troca da tarefa senão fica em loop.
    return 0;
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
    return task->id;
}

void pingpong_init() {
    setvbuf (stdout, 0, _IONBF, 0); /* desativa o buffer da saida padrao (stdout), usado pela função printf */
    main_task = malloc(sizeof(task_t)); // tem q alocar senao dá Segmentation fault
    task_create(main_task,NULL,NULL); // cria a main
    current_task = main_task; //coloca main como atual.
    //cria o dispatcher
    dptch_task = malloc(sizeof(task_t));
    task_create(dptch_task,dispatcher_body,NULL);
}
