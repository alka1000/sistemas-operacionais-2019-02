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
#include "timer.h"

// Init do código
#define STACKSIZE 32768     /* tamanho de pilha das threads */

task_t *current_task, *main_task, *disp_task; //uma para referenciar como a atual e outra para a main, para conseguir ir e voltar de contextos mais fácil
int id_counter = 0; // contador progressivo para dar ids às tasks
Queue q;
Queue q_suspended;
Queue q_sleeping;

int tick = 0;
int proc_time_helper = 0;

void debugPrint(Queue* pQueue)
{
    size_t index;
    size_t tmp;

    if (!pQueue)
    {
        printf("null");
        return;
    }

    printf("[");

    if (pQueue->size >= 1)
    {
        printf("%d", (int) ((task_t*)pQueue->data[pQueue->head])->id);
    }

    for (index = 1; index < pQueue->size; ++index)
    {
        tmp = (pQueue->head + index) % QUEUE_CAPACITY;
        printf(", %d", (int) ((task_t*)pQueue->data[tmp])->id);
    }

    printf("]");
}


void bubble_sort (Queue* pQueue) {
    int length = pQueue->size;
    int i, j;
    for (i = pQueue->head; i < length - 1; i++) {
        for (j = pQueue->head; j < length - i - 1; j++) {
          if (((task_t*)pQueue->data[j])->priority > ((task_t*)pQueue->data[j+1])->priority) {
            swap(pQueue->data[j], pQueue->data[j+1]);
          }
        }
    }
}

// Funcs

void task_setprio (task_t *task, int prio)
{
  if (task == NULL) {
    task = current_task;
  }
  task->base_priority = prio;
  task->priority = prio;
}

int task_getprio (task_t *task)
{
  if (task == NULL) {
    task = current_task;
  }
  return task->base_priority;
}

void task_yield () {
    disp_task->activations++;
    if (current_task == disp_task) {
        return;
    }
    else {
        enqueue(&q, (void*) current_task);
        current_task->state = 0;
        current_task->quantum = 20;
        current_task->proc_time+=systime()-proc_time_helper;
        current_task->priority = current_task->base_priority;
        task_switch(disp_task);
    }
}


task_t* scheduler() {
    task_t *aux = NULL;
    // acorda as tarefas
    for (int i = (&q_sleeping)->size-1; i >= 0; i--) {
        if ( ((task_t*)(&q_sleeping)->data[i]) ->sleep_time < systime()) {
            task_t *item = remove_task(&q_sleeping, i);
            item->state = 0;
            
            enqueue(&q, (void*) item);
            
        }
    }
    if (!isEmpty(&q)) {
        bubble_sort(&q);
        aux = dequeue(&q);
        aux->activations++;
        proc_time_helper = systime();
        int i;
        for(i=q.head;i<q.size;i++){
            ((task_t*)q.data[i])->priority--;
        }
    }
    return aux;
    
}

void dispatcher_body () { // dispatcher é uma tarefa
    while ( !isEmpty(&q) || !isEmpty(&q_sleeping) || !isEmpty(&q_suspended) ) {
        
        // printf("%d\n", systime());
        // printf("passou %d \n", systime());
        task_t* next = scheduler(); // scheduler é uma função
        if (next) {
            task_switch (next) ; // transfere controle para a tarefa "next"
        }
    }
    task_exit(0) ; // encerra a tarefa dispatcher
}

int task_join(task_t *task) {
    if (has_task(&q, task) || has_task(&q_suspended, task)) {
        
        current_task->waiting_task = task;
        current_task->state = 2;
        current_task->quantum = 20;
        current_task->proc_time+=systime()-proc_time_helper;
        current_task->priority = current_task->base_priority;

        task_t *c_task = current_task;

        enqueue(&q_suspended, (void*) c_task);

        task_switch(disp_task);
        
        return c_task->waiting_status;
    } else {
        return -1;
    }
}

void task_sleep (int t) {
    current_task->sleep_time = (t*1000)+systime();
    current_task->state = 2;
    current_task->quantum = 20;
    current_task->proc_time+=systime()-proc_time_helper;
    current_task->priority = current_task->base_priority;

    enqueue(&q_sleeping, (void*) current_task);
    
    task_switch(disp_task);

}

// cria um semáforo com valor inicial "value"
int sem_create (semaphore_t *s, int value) {
    initQueue(&s->fila);
    // debugPrint(&s->fila);
    s->count = value;
    // printf("semcreate %d\n", s->count);
    return 0;
}

// requisita o semáforo
int sem_down (semaphore_t *s) {
    if (!s) {
        return -1;
    }
    s->count--;
    // printf("semdown %d\n", s->count);
    if (s->count < 0) {
        enqueue(&s->fila, (void*) current_task);
        enqueue(&q_suspended, (void*) current_task);
        // debugPrint(&s->fila);
        current_task->state = 2;
        current_task->quantum = 20;
        current_task->proc_time+=systime()-proc_time_helper;
        current_task->priority = current_task->base_priority;
        task_t *c_task = current_task;
        task_switch(disp_task);
        return c_task->waiting_status;
    }
    return 0;
}

// libera o semáforo
int sem_up (semaphore_t *s) {
    if (!s) {
        return -1;
    }
    s->count++;
    task_t *item = NULL;
    // printf("semup %d\n", s->count);
    if (s->count <= 0) {
        if (!isEmpty(&s->fila)) {
            item = dequeue(&s->fila);
            for (int i = (&q_suspended)->size-1; i >= 0; i--) {
                if (((task_t*)(&q_suspended)->data[i]) == item) {
                    remove_task(&q_suspended, i);
                }
            }
        }
    }
    if (item){
        item->state = 0;
        item->waiting_status = 0;
        enqueue(&q, (void*) item);
    }
    return 0;
}

// destroi o semáforo, liberando as tarefas bloqueadas
int sem_destroy (semaphore_t *s) {

    if (!s) {
        return -1;
    }
    while (!isEmpty(&s->fila)) {
        task_t *item = dequeue(&s->fila);
        for (int i = (&q_suspended)->size-1; i >= 0; i--) {
            if (((task_t*)(&q_suspended)->data[i]) == item) {
                remove_task(&q_suspended, i);
            }
        }
        if (item) {
            item->state = 0;
            item->waiting_status = -1;
            enqueue(&q, (void*) item);
        }
    }
    s = NULL;
    return 0;
}


int task_id() {
    //retorna o id da tarefa rodando
    return current_task->id;
}

void task_exit (int exit_code) {
    unsigned int end = systime();
    printf("Task %d exit: running time %d ms, cpu time %d ms, %d activations.\n", task_id(), end-current_task->exec_time, current_task->proc_time, current_task->activations);
    
    
    for (int i = (&q_suspended)->size-1; i >= 0; i--) {
        if (((task_t*)(&q_suspended)->data[i])->waiting_task == current_task) {
            
            task_t *item = remove_task(&q_suspended, i);

            item->state = 0;
            item->waiting_status = exit_code;
            enqueue(&q, (void*) item);
        }
    }
    


    if (current_task == disp_task) {
        return;
    }

    task_switch(disp_task);
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
    getcontext (&task->ctx);
    task->activations=0;
    task->proc_time=0;
    task->exec_time=systime();
    char *stack ;
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
    task->main_ctx = main_task->ctx;
    task->id = id_counter;
    task->next = NULL;
    task->prev = NULL;
    id_counter++;
    task->priority = task->base_priority = 0;
    if (task != disp_task)
        enqueue(&q, (void*) task);
    task->state = 0;
    task->quantum=20;
    return task->id;
}

unsigned int systime() {
    return tick/10;
}

void tratador(int signum)
{
    tick++;
  if (current_task != disp_task) {
    current_task->quantum--;
    if (current_task->quantum == 0) {
      task_yield();
    }
  }
}

void pingpong_init() {
    
    setvbuf (stdout, 0, _IONBF, 0); /* desativa o buffer da saida padrao (stdout), usado pela função printf */
    initQueue(&q);
    initQueue(&q_suspended);
    main_task = malloc(sizeof(task_t)); // tem q alocar senao dá Segmentation fault
    task_create(main_task,NULL,NULL); // cria a main
    //dispatcher
    disp_task = malloc(sizeof(task_t));
    task_create(disp_task, dispatcher_body, NULL);

    current_task = main_task; //coloca main como atual.

    start_tim(tratador);

    task_switch(disp_task);


}

