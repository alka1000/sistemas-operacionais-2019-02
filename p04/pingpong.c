#include <stdio.h>
#include <stdlib.h>
#include "pingpong.h"
#include "queue.h"

#define STACKSIZE 32768
#define DEFAULT_PRIO 0

int totalID = 0;

task_t *running_task,*main_task,*dispatcher_task;
task_t *ready_queue;

task_t* get_next_task() // Retorna a proxima a tarefa a ser executada de acordo com prioridade
{
  //Remove temporariamente main
  queue_remove((queue_t**)&ready_queue,(queue_t*)main_task);
  int prio = 21; // Nao existe prioridade maior que 21
  task_t *next = NULL;
  task_t *first_task = dispatcher_task->next; // A primeira tarefa de user sempre esta dps do dispatcher
  queue_remove((queue_t**)&ready_queue,(queue_t*)dispatcher_task); // Remove temporariamente para facilitar
  task_t *current_task = first_task;

  if (queue_size((queue_t*)ready_queue)) {
    //Como apos remocao queue_size nao é zero(por isso esta nesse if) first_task definitivamente e de usuario
    do{
      if (current_task->dynamic_prio < prio) {
        prio = current_task->dynamic_prio;
        next = current_task;
      }
      current_task = current_task->next;
    }while(current_task != first_task);
  }
  else{//Scheduler deve ser sempre chamado com ao menos uma tarefa de usuario
    printf("Aqui deu ruim\n");
  }
  //Adiciona devolta main e dispatcher
  queue_append((queue_t**)&ready_queue,(queue_t*)main_task);
  queue_append((queue_t**)&ready_queue,(queue_t*)dispatcher_task);
  return next;
}

task_t *scheduler()
{
  task_t *next_task = get_next_task();
  task_t *tmp_task = next_task->next;
  int aging_factor = -1;

  while(tmp_task != next_task)
  {
    //Atualiza prioridade dinamica para todas tarefas de usuario
    if (tmp_task != dispatcher_task && tmp_task != main_task)
    {
      tmp_task->dynamic_prio += aging_factor;
    }
    tmp_task = tmp_task->next;
  }
  next_task->dynamic_prio = task_getprio(next_task); //Reseta dinamica para estatica
  return next_task;
}

void dispatcher_body(void *arg)
{
  int user_tasks = queue_size((queue_t*)ready_queue) - 2; // -main e -dispatcher

  while (user_tasks > 0) {
    task_t* next = scheduler();
    if (next) {
      task_switch(next);
      user_tasks = queue_size((queue_t*)ready_queue) - 2;
    }
  }

  task_exit(0);
}


void pingpong_init()
{
  setvbuf(stdout, 0, _IONBF, 0);
  main_task = malloc(sizeof(task_t));
  dispatcher_task = malloc(sizeof(task_t));
  ready_queue = NULL;
  task_create(main_task,NULL,NULL);
  task_create(dispatcher_task,dispatcher_body,NULL);
  running_task = main_task;
}

int task_create(task_t *task, void (*start_func)(void*), void *arg)
{
  getcontext(&(task->context));

  char *stack;
  stack = malloc(STACKSIZE);

  if (stack) {
    task->context.uc_stack.ss_sp = stack;
    task->context.uc_stack.ss_size = STACKSIZE;
    task->context.uc_stack.ss_flags = 0;
    task->context.uc_link = 0;
  }
  else{
    printf("Erro na criação da pilha.");
    return -1;
  }
  makecontext (&(task->context), (void*)(*start_func), 1, arg);
  task->main_context = main_task->context;
  task->id = totalID;
  task->next = task->prev = NULL;
  totalID ++;
  task->static_prio = DEFAULT_PRIO;
  task->dynamic_prio = task->static_prio;
  queue_append((queue_t**)&ready_queue,(queue_t*)task);
  task->current_state = READY;
  task->enqueued = 1;
  return task->id;
}

void task_exit(int exitCode)
{

  queue_remove((queue_t**)&ready_queue,(queue_t*)running_task);
  if (running_task == dispatcher_task) { // se encerando dispatcher
    task_switch(main_task);
    free(dispatcher_task);
    free(main_task);
  }
  else{ // encerando outra tarefa
    task_switch(dispatcher_task);
  }
}

int task_switch(task_t *task)
{
  task_t *old_task = running_task;
  running_task = task;
  task->current_state = RUNNING;
  swapcontext(&(old_task->context),&(task->context));
  return 0;
}

int task_id()
{
  return running_task->id;
}

void task_yield()
{
  queue_remove((queue_t**)&ready_queue,(queue_t*)running_task);
  queue_append((queue_t**)&ready_queue,(queue_t*)running_task);
  running_task->current_state = READY;
  task_switch(dispatcher_task);
}

void task_suspend(task_t *task, task_t **queue)
{
  if (task == NULL) {
    task = running_task;
  }
  if (queue == NULL) {
    printf("Fila de tarefa suspensa não existe\n");
    exit(1);
  }
  if (task->current_state == RUNNING) {
    if (task->enqueued) {
      queue_remove((queue_t**)&ready_queue,(queue_t*)task);
    }
    else{
      printf("Tarefa %d nao pertence a nenhuma fila\n",task->id);
      exit(1);
    }
    queue_append((queue_t**)&queue,(queue_t*)task);
    task->current_state = SUSPENDED;
  }
  else
  {
    printf("Tentando suspender tarefa %d, mas nao está executando\n",task->id );
    exit(1);
  }
}

void task_resume (task_t *task)
{
  if (task->enqueued) {
    queue_remove((queue_t**)&ready_queue,(queue_t*)task);
  }
  queue_append((queue_t**)&ready_queue,(queue_t*)task);
  task->current_state = READY;
}

void task_setprio (task_t *task, int prio)
{
  if (task == NULL) {
    task = running_task;
  }
  int min_prio = -20;
  int max_prio = 20;
  if (prio<min_prio || prio>max_prio) {
    printf("Valor de prioridade %d inválido\n",prio);
    exit(1);
  }
  task->static_prio = prio;
}

int task_getprio (task_t *task)
{
  if (task == NULL) {
    task = running_task;
  }
  return task->static_prio;
}
