#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <assert.h>
#include "sdd.h"

static TaskNode* create_node(Task* task){
    TaskNode* node = (TaskNode*)malloc(sizeof(TaskNode));
    if (node == NULL){
        perror("Failed to create a TaskNode");
        exit(EXIT_FAILURE);
        
    }
    node->task = task;
    node->next = NULL;
    node->prev = NULL;
    return node;
}

SortedDispatcherDatabase* create_sdd(){
    SortedDispatcherDatabase* db = (SortedDispatcherDatabase*)malloc(sizeof(SortedDispatcherDatabase));
    if (db == NULL){
        perror("Failed to create an SDD");
        exit(EXIT_FAILURE);
        
    }
    db->head = NULL;
    db->tail = NULL;
    db->size = 0;

    if (pthread_mutex_init(&(db->sdd_lock),NULL) != 0) {
        perror("Failed to start mutex");
        free(db);
        exit(EXIT_FAILURE);
    }

    return db;
}

void destroy_sdd(SortedDispatcherDatabase* db){
    if (db == NULL){
        return;
    }

    pthread_mutex_lock(&(db->sdd_lock));
    TaskNode* current = db->head;
    while ( current != NULL){
        TaskNode* next = current->next;
        if(current->task){
            free(current->task->task_id);
            free(current->task);
        }
        free(current);
        current = next;
    }

    db->head = NULL;
    db->tail = NULL;
    db->size = 0;
    pthread_mutex_unlock(&(db->sdd_lock));
    pthread_mutex_destroy(&(db->sdd_lock));
    free(db);
}

void submitTask(SortedDispatcherDatabase* q, Task* _task) {
	if( q == NULL || _task == NULL){
        return;
    }

    pthread_mutex_lock(&(q->sdd_lock));
    TaskNode* newNode = create_node(_task);

if (q->head == NULL){
    q->head = q->tail = newNode;
    q->size = 1;
    pthread_mutex_unlock(&(q->sdd_lock));
    return;
}

if(_task->task_duration <= q->head->task->task_duration){
    newNode->next = q->head;
    q->head->prev = newNode;
    q->head = newNode;
    q->size++;
    pthread_mutex_unlock(&(q->sdd_lock));
    return;
}

TaskNode* current = q->head;

while(current != NULL && current->task->task_duration <= _task->task_duration){
    if(current->next == NULL){
        break;
    }
    if(current->next->task->task_duration > _task->task_duration){
        break;
    }
    current = current->next;

}

newNode->next = current->next;
newNode->prev = current;
current->next = newNode;
if(newNode->next != NULL){
    newNode->next->prev = newNode;
}
else{
    q->tail = newNode;
}

q->size++;
pthread_mutex_unlock(&(q->sdd_lock));
}


Task* fetchTask(SortedDispatcherDatabase* q) {
	if ( q == NULL){
        return NULL;
    }
    pthread_mutex_lock(&(q->sdd_lock));

    if(q->head == NULL){
        pthread_mutex_unlock(&(q->sdd_lock));
        return NULL;
    }

    TaskNode* node_to_be_removed = q->head;
    Task* task = node_to_be_removed->task;

    q->head = node_to_be_removed->next;
    if(q->head != NULL){
        q->head->prev = NULL;
    }
    else{
        q->tail = NULL;
    }

    q->size--;
    free(node_to_be_removed);
    pthread_mutex_unlock(&(q->sdd_lock));
    return task;
}

Task* fetchTaskFromOthers(SortedDispatcherDatabase* q) {
	if ( q == NULL ){
        return NULL;
    }
    pthread_mutex_lock(&(q->sdd_lock));
    if (q->tail == NULL){
        pthread_mutex_unlock(&(q->sdd_lock));
        return NULL;
    }

    TaskNode* node_to_be_removed = q->tail;
    Task* task = node_to_be_removed->task;
    q->tail = node_to_be_removed->prev;
    if(q->tail != NULL){
        q->tail->next = NULL;
    }
    else{
        q->head = NULL;
    }

    q->size--;
    free(node_to_be_removed);
    pthread_mutex_unlock(&(q->sdd_lock));
    return task;
}

void print_queue(SortedDispatcherDatabase* q, int core_id) {
   if(q == NULL){
    return;
   }
   pthread_mutex_lock(&(print_lock));
   pthread_mutex_lock(&(q->sdd_lock));
   printf("Core %d queue [size=%d]: ", core_id, q->size);

   if(q->head == NULL){
    printf("(empty)\n");
    pthread_mutex_unlock(&(q->sdd_lock));
    pthread_mutex_unlock(&(print_lock));
    return;
   }

    TaskNode* current = q->head;
    while(current != NULL){
        printf("%s(%d)", current->task->task_id, current->task->task_duration);
        if (current->next != NULL){
            printf(" -> ");
        }
        current = current->next;
    }
    printf("\n");
    pthread_mutex_unlock(&(q->sdd_lock));
    pthread_mutex_unlock(&(print_lock));
}
