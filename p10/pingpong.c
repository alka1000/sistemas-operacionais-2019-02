#include <stdio.h>
#include <stdlib.h>
#include "timer.h"
#include "pingpong.h"
#include "queue.h"
#include <limits.h>

#define STACKSIZE 32768
#define DEFAULT_PRIO 0
#define INITIAL_QUANTUM 20

int enable_premp = 1; // Habilita preempção por tempo

int totalID = 0;

task_t *running_task,*main_task,*dispatcher_task;
task_t *ready_queue, *suspended_queue, *sleeping_queue;

int globalTicks = 0;
int awakeTicks = 0;
int closestAwake = INT_MAX;

void list_queue(task_t *queue)
{
  if (!queue)
  {
    return;
  }
  
  task_t* current = queue;
    do
    {
      printf("Tarefa: %d, Awake: %d, Atual: %d\n",current->id,current->awake_tick,systime());
      current = current->next;
    } while (current != queue);
    printf("\n");
}

// tratador do sinal
void handler(int signum)
{
  globalTicks++; //Incrementa o contador de ticks do sistema
  awakeTicks++; //Incrementa o contador de ticks do awake
  if(enable_premp)
  {
    if (running_task != dispatcher_task) {
    running_task->quantum--;
      if (running_task->quantum == 0) {
        running_task->quantum = INITIAL_QUANTUM;
        task_yield();
      }
    }
  }
  
}


void reset_closestAwake()
{
  task_t* current = sleeping_queue;
  closestAwake = INT_MAX;
  if(!current)
    return;
  do
  {
    if (current->awake_tick < closestAwake)
    {
      closestAwake = current->awake_tick;
    }
    current = current->next;
  } while (current != sleeping_queue);
}

void awake()
{
  if(awakeTicks>=0) // A cada 100ms
  {
    awakeTicks = 0;
    unsigned current_time = systime();
    task_t* current= sleeping_queue;
    task_t* prev = current;
    if(!sleeping_queue)
      return;
   
    do
    {
      if(current->awake_tick <= current_time)
      {
        if(queue_size((queue_t*)sleeping_queue)==1)
        {
          queue_remove((queue_t**)&sleeping_queue,(queue_t*)current);
          queue_append((queue_t**)&ready_queue,(queue_t*)current);
          reset_closestAwake();
          return;
        }
        prev = current->prev;
        queue_remove((queue_t**)&sleeping_queue,(queue_t*)current);
        queue_append((queue_t**)&ready_queue,(queue_t*)current);
        reset_closestAwake();
        current = prev;
      }
      current = current->next;
    } while (current->next != sleeping_queue);
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
  else{//Não tem tarefas, procura nas adormecidas
    while(systime() < closestAwake);
    awake();
    if(queue_size((queue_t*)ready_queue)) // Se alguma tarefa foi acordada
    {
      first_task = current_task = ready_queue;
      do{
        if (current_task->dynamic_prio < prio) {
          prio = current_task->dynamic_prio;
          next = current_task;
        }
        current_task = current_task->next;
      }while(current_task != first_task);
    }
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
  while (user_tasks > 0 || queue_size((queue_t*)sleeping_queue)) {
    awake();
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
  task->awake_tick = 0;
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
  running_task->current_state = RUNNING;
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
  task->awake_tick = 0;
  return task->id;
}

void awake_waiting_for(int id)
{
  task_t* current = suspended_queue;
  int size = queue_size((queue_t*)suspended_queue);
  int i = 0; 
  while(queue_size((queue_t*)suspended_queue) > 0)
  {
    if(current->waiting_id == id)
    {
      task_resume(current);
    }
    current = current->next;
    i++;
    if(i == size)
      break;
  }
}

void task_exit(int exitCode)
{
  queue_t* res = queue_remove((queue_t**)&ready_queue,(queue_t*)running_task);
  if(res == NULL)
  {
    printf("Sleep\n");
    list_queue(ready_queue);
    list_queue(suspended_queue);
    list_queue(sleeping_queue);
  }
  unsigned end_time = systime();
  printf("Task %d exit: running time %d ms, cpu time %d ms, %d activations.\n",
        task_id(),end_time-running_task->start_time,running_task->cpu_time, running_task->activations);
  running_task->exit_code = exitCode;
  running_task->current_state = FINISHED;
  if (running_task == dispatcher_task) { // se encerando dispatcher
    task_switch(main_task);
    free(dispatcher_task);
    free(main_task);
  }
  else{ // encerando outra tarefa
    awake_waiting_for(task_id());
    task_switch(dispatcher_task);
  }
}

int task_join (task_t *task) 
{
  if(task == NULL)
  {
    return -1;
  }
  else
  {
    while(task->current_state != FINISHED)
    {
      running_task->waiting_id = task->id;
      task_suspend(running_task, &suspended_queue);
      task_switch(dispatcher_task);
    }
    return task->exit_code;
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
  if(running_task->current_state != FINISHED)
  {
    queue_remove((queue_t**)&ready_queue,(queue_t*)running_task);
    queue_append((queue_t**)&ready_queue,(queue_t*)running_task);
    running_task->current_state = READY;
    task_switch(dispatcher_task); 
  }
}

void task_suspend(task_t *task, task_t **queue)
{
  if (task == NULL) {
    task = running_task;
  }
  if (queue == NULL) {
    printf("Fila passada para task_suspend não existe\n");
    exit(1);
  }
  if (task->enqueued) {
    if (task->current_state == READY || task->current_state == RUNNING)
    {
      queue_remove((queue_t**)&ready_queue,(queue_t*)task);
    }
    if(task->current_state == SUSPENDED)
    {
      queue_remove((queue_t**)&suspended_queue,(queue_t*)task);
    }
    task->enqueued = 0;
  }
  else{
    printf("Tarefa %d nao pertence a nenhuma fila\n",task->id);
    exit(1);
  }
  queue_append((queue_t**)queue,(queue_t*)task);
  task->enqueued = 1;
  task->current_state = SUSPENDED;
}

void task_resume (task_t *task)
{
  if (task->enqueued) {
    if (task->current_state == SUSPENDED)
    {
      queue_remove((queue_t**)&suspended_queue,(queue_t*)task);
      task->enqueued = 0;
    }
  }
  queue_append((queue_t**)&ready_queue,(queue_t*)task);
  task->enqueued = 1;
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

void task_sleep(int t)
{
  if(t)
  {
    int awake_in = systime() + t*1000;
    queue_t* res = queue_remove((queue_t**)&ready_queue,(queue_t*)running_task);  
    if(res == NULL)
    {
      list_queue(ready_queue);
      list_queue(suspended_queue);
      list_queue(sleeping_queue);
    }
    running_task->awake_tick = awake_in;
    if (awake_in < closestAwake && awake_in > systime())
    {
      closestAwake = awake_in;
    }
    queue_append((queue_t**)&sleeping_queue,(queue_t*)running_task);
    running_task->current_state = SLEEPING;
    task_switch(dispatcher_task);
  }
  
}

// ######################################################################

int sem_create (semaphore_t *s, int value)
{
  enable_premp = 0;
  if(!s)
  {
    printf("Semaforo inexistente.\n");
    return -1;
  }
  s->counter = value;
  s->queue = NULL;
  s->destroyed = 0;
  enable_premp = 1;
  return 0;
}

int sem_down (semaphore_t *s)
{
  enable_premp = 0;

  if(!s)
  {
    return -1;
  }
  if(s->destroyed)
  {
    return -1;
  }

  s->counter--;
  if (s->counter < 0)
  {
    task_suspend(running_task,&(s->queue));
    task_switch(dispatcher_task);
  }
  
  enable_premp = 1;
  return 0;
}

int sem_up (semaphore_t *s) 
{
  enable_premp = 0;

  if(!s || s->destroyed)
  {
    return -1;
  }

  s->counter++;
  if (s->counter<=0)
  {
    task_t* first = (task_t*)queue_remove((queue_t**)&(s->queue),(queue_t*)(s->queue));
    queue_append((queue_t**)&ready_queue,(queue_t*)first);
    first->current_state = READY;
  }
  enable_premp =1;
  return 0;
}

int sem_destroy (semaphore_t *s)
{
  enable_premp = 0;
  if (!s || s->destroyed)
  {
    return -1;
  }
  
  task_t* aux = s->queue;
  int size = queue_size((queue_t*)(s->queue));
  int i = 0;
  while(i < size)
  {
    aux = (s->queue);
    queue_remove((queue_t**)&(s->queue),(queue_t*)aux);
    queue_append((queue_t**)&ready_queue,(queue_t*)aux);
    i++;
  }
  s->queue = NULL;
  s->counter = 0;
  s->destroyed = 1;

  enable_premp = 1;
  return 0;
}