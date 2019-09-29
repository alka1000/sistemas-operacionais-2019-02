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

task_t *current_task, *main_task, *disp_task; //uma para referenciar como a atual e outra para a main, para conseguir ir e voltar de contextos mais fácil
int id_counter = 0; // contador progressivo para dar ids às tasks
//queue_t **queue_rdy = NULL;
task_t *rq;

// Funcs

void task_yield () {
    queue_remove((queue_t**)&rq,(queue_t*)current_task);
    queue_append((queue_t**)&rq,(queue_t*)current_task);
    current_task->state = 0;
    task_switch(disp_task);
}               //aqui ta pronto


task_t* scheduler() {
  return disp_task->next;
}

dispatcher_body () { // dispatcher é uma tarefa
    int userTasks = queue_size((queue_t**)&rq);
    while ( userTasks > 0 ) {
        task_t* next = scheduler(); // scheduler é uma função
        if (next) {
            task_switch (next) ; // transfere controle para a tarefa "next"
            int userTasks = queue_size((queue_t**)&rq);
        }
    }
    task_exit(0) ; // encerra a tarefa dispatcher
}



int task_id() {
    //retorna o id da tarefa rodando
    return current_task->id;
}

void task_exit (int exit_code) {
    //sair ou voltar pra main.
    queue_remove((queue_t**)&rq, (queue_t*)current_task);
    task_switch(main_task); 
}

int task_switch (task_t *task) {
    // auxiliar para troca de tarefa
    task_t *aux = current_task;
    current_task = task; // troca da tarefa atual
    task->state = 1;
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
    queue_append((queue_t**)&rq,(queue_t*)task);
    task->state = 0;
    task->main_ctx = main_task->ctx;
    return task->id;
}

void pingpong_init() {
    setvbuf (stdout, 0, _IONBF, 0); /* desativa o buffer da saida padrao (stdout), usado pela função printf */
    main_task = malloc(sizeof(task_t)); // tem q alocar senao dá Segmentation fault
    task_create(main_task,NULL,NULL); // cria a main
    queue_remove((queue_t**)&rq, (queue_t*)main_task);
    rq = NULL;
    //dispatcher.
    disp_task = malloc(sizeof(task_t));
    task_create(disp_task, dispatcher_body, NULL);
    queue_remove((queue_t**)&rq, (queue_t*)disp_task);
    current_task = main_task; //coloca main como atual.
}

