#include "queue.h"

void queue_append (queue_t **queue, queue_t *elem){
    if (queue == NULL) {
        printf("Fila nao existe\n");
        return;
    }
    if (elem == NULL) {
        printf("Elemento nao existe\n");
        return;
    }
    if (elem->next != NULL || elem->prev != NULL) {
        printf("Elemento pertence a outra fila\n");
        return;
    }
    if (*queue == NULL) {
        *queue = elem;
        elem->next = elem;
        elem->prev = elem;
        return;
    }
    queue_t *last = (*queue)->prev;
    elem->next = *queue;
    elem->prev = last;
    (*queue)->prev = elem;
    last->next = elem;

}

queue_t *queue_remove (queue_t **queue, queue_t *elem){
    if (queue == NULL) {
        printf("Fila nao existe\n");
        return NULL;
    }
    if ((*queue)->next == NULL && (*queue)->prev == NULL) {
        printf("Fila esta vazia\n");
        return NULL;
    }
    if (elem == NULL) {
        printf("Elemento nao existe\n");
        return NULL;
    }
    if (elem != *queue) {
        queue_t *current = *queue;
        int found = 0;
        while (current->next != *queue) {
            if (current->next == elem) {
                found = 1;
            }
            current = current->next;
        }
        if (!found) {
            printf("Elemento nao esta nessa fila.\n");
            return NULL;
        }
    }
    if (queue_size(*queue) == 1) {

        (*queue)->next = NULL;
        (*queue)->prev = NULL;
        *queue = NULL;
        return elem;
    }
    queue_t *prev;

    if (*queue == elem) { // removendo primeiro
        prev = (*queue)->prev;
        (*queue) = (*queue)->next;
        prev->next = (*queue);
        (*queue)->prev = prev;
    }
    else if ((*queue)->prev == elem) { // removendo ultimo
        prev = elem->prev;
        (*queue)->prev = prev;
        prev->next = *queue;
    }
    else {
        prev = elem->prev;
        queue_t *next = elem->next;

        prev->next = elem->next;
        next->prev = elem->prev;
    }
    elem->next = NULL;
    elem->prev = NULL;
    return elem;
}

int queue_size (queue_t *queue){
    if (queue == NULL) {
        return 0;
    }
    if (queue->next == NULL && queue->prev == NULL) {
        return 0;
    }
    else{
        int tamanho = 1;
        queue_t *current = queue;
        while (current->next != queue) {
            tamanho++;
            current = current->next;
        }
        return tamanho;
    }
}

void queue_print (char *name, queue_t *queue, void print_elem (void*) ){
    printf("%s", name);
    queue_t *current = queue;
    printf("[");
    if (current == NULL) {
        printf("]\n");
        return;
    }
    while (current->next != queue) {
        print_elem(current);
        current = current->next;
    }
    printf("]\n");
}
