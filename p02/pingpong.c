#include <stdio.h>
#include <stdlib.h>
#include "pingpong.h"

#define STACKSIZE 32768

int totalID = 0;

task_t *running_task,*main_task;

void pingpong_init()
{
  setvbuf(stdout, 0, _IONBF, 0);
  main_task = malloc(sizeof(task_t));
  task_create(main_task,NULL,NULL);
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
  totalID ++;
  return task->id;
}

void task_exit(int exitCode)
{
  task_switch(main_task);
}

int task_switch(task_t *task)
{
  task_t *old_task = running_task;
  running_task = task;
  swapcontext(&(old_task->context),&(task->context));
  return 0;
}

int task_id()
{
  return running_task->id;
}
