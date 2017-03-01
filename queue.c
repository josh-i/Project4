#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ucontext.h>
#include "queue.h"

#define ARR_SIZE 100000
#define UTHREAD_STACK_SIZE 32768

struct queue {
    void* arr[100000]; //Array to hold queued data
    int head; //Position of the head of the queue
    int tail; //Position of the tail of the queue
    int size; //Number of elements currently in the queue
};

queue_t queue_create(void) {
    queue_t newQueue = malloc(sizeof(struct queue));
    newQueue->head = 0;
    newQueue->tail = 0;
    newQueue->size = 0;
    for(int i = 0; i < ARR_SIZE; i++) {
        newQueue->arr[i] = malloc(sizeof(void*));
    }
    return newQueue; //malloc returns NULL on failure
}

int queue_destroy(queue_t queue) {
    if(queue != NULL) {
        free(queue);
        queue = NULL; //For queue_destroy error detection
        return 0;
    }
    return -1; //Failure
}

int queue_enqueue(queue_t queue, void *data) {
    if(queue != NULL) {
        //memcpy(queue->arr[queue->head], data, 34084);
        queue->arr[queue->head] = data;
        queue->head++;
        queue->size++;
        return 0;
    }
    return -1; //Failure
}

int queue_dequeue(queue_t queue, void **data) {
    if(queue != NULL && data != NULL) {
        //memcpy(data, queue->arr[queue->tail], 34084);
        *data = queue->arr[queue->tail];
        queue->tail++;
        queue->size--;
        return 0;
    }
    return -1; //Failure 
}

int queue_delete(queue_t queue, void *data) {
    if(queue != NULL && data != NULL) {
        for(int i = queue->tail; i < queue->head; i++) {
            if(&queue->arr[i] == data) {
                for(int j = i; j < ARR_SIZE; j++) {
                    queue->arr[j] = queue->arr[j+1];
                }
                queue->head--; //Move head back to decrease size
                queue->size--;
                return 0;
            }
        }
    }
    return -1; //Failure
}

int queue_iterate(queue_t queue, queue_func_t func) {
    if(queue != NULL && func != NULL) {
        for(int i = queue->tail; i < queue->head; i++) {
            (*func)(queue->arr[i]);
        }
        return 0;
    }
    return -1; //Failure
    //Protection from items being deleted?
}

int queue_length(queue_t queue) {
    if(queue != NULL) {
        return queue->size;
    }
    return -1; //Failure
}
