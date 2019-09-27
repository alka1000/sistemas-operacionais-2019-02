#include <stdio.h>
#include <stdlib.h>
#include "timer.h"
#include "pingpong.h"
#include "queue.h"

#define STACKSIZE 32768
#define DEFAULT_PRIO 0
#define INITIAL_QUANTUM 20

int totalID = 0;

task_t *running_task,*main_task,*dispatcher_task;
task_t *ready_queue, *suspended_queue;

int globalTicks = 0;


// tratador do sinal
void handler(int signum)
{
  globalTicks++; //Incrementa o contador de ticks do sistema
  if (running_task != dispatcher_task) {
    running_task->quantum--;
    if (running_task->quantum == 0) {
      running_task->quantum = INITIAL_QUANTUM;
      task_yield();
    }
  }
}

task_t* get_next_task() // Retorna a proxima a tarefa a ser executada de acordo com prioridade
{
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

  //Adiciona devolta dispatcher
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
    if (tmp_task != dispatcher_task)
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
  const int system_tasks = 1;
  int user_tasks = queue_size((queue_t*)ready_queue) - system_tasks;
  while (user_tasks > 0) {
    task_t* next = scheduler();
    if (next) {
      unsigned cpu_start = systime();
      task_switch(next);
      unsigned cpu_end = systime();
      next->cpu_time += cpu_end - cpu_start;
      user_tasks = queue_size((queue_t*)ready_queue) - system_tasks;
    }
  }
  task_exit(0);
}

int main_create(task_t* task)
{
  task->next = task->prev = NULL;
  task->start_time = systime();
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
  task->main_context = task->context;
  task->id = totalID;
  totalID ++;
  task->static_prio = DEFAULT_PRIO;
  task->dynamic_prio = task->static_prio;
  queue_append((queue_t**)&ready_queue,(queue_t*)task);
  task->current_state = READY;
  task->enqueued = 1;
  task->quantum = INITIAL_QUANTUM;
  return task->id;
}

void pingpong_init()
{
  setvbuf(stdout, 0, _IONBF, 0);
  main_task = malloc(sizeof(task_t));
  dispatcher_task = malloc(sizeof(task_t));
  ready_queue = NULL;
  suspended_queue = NULL;
  main_create(main_task);
  task_create(dispatcher_task,dispatcher_body,NULL);
  init_timer(handler);
  running_task = main_task;
}

int task_create(task_t *task, void (*start_func)(void*), void *arg)
{
  task->next = task->prev = NULL;
  task->start_time = systime();
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
  totalID ++;
  task->static_prio = DEFAULT_PRIO;
  task->dynamic_prio = task->static_prio;
  queue_append((queue_t**)&ready_queue,(queue_t*)task);
  task->current_state = READY;
  task->enqueued = 1;
  task->quantum = INITIAL_QUANTUM;
  return task->id;
}

void task_exit(int exitCode)
{
  queue_remove((queue_t**)&ready_queue,(queue_t*)running_task);
  unsigned end_time = systime();
  printf("Task %d exit: running time %d ms, cpu time %d ms, %d activations.\n",
        task_id(),end_time-running_task->start_time,running_task->cpu_time, running_task->activations);
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
  running_task->activations++;
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
    if (task->current_state == SUSPENDED)
    {
      queue_remove((queue_t**)&suspended_queue,(queue_t*)task);
    }
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
  task->dynamic_prio = prio;
}

int task_getprio (task_t *task)
{
  if (task == NULL) {
    task = running_task;
  }
  return task->static_prio;
}

unsigned int systime ()
{
  return globalTicks;
}
