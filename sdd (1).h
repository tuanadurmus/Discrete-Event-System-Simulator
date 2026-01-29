#ifndef SDD_H
#define SDD_H
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
// Structs and methods for SortedDispatcherDatabase, you can use additional structs 
// and data structures ON TOP OF the ones provided here.

// **********************************************************
extern pthread_mutex_t print_lock;
typedef struct Task Task;

typedef struct TaskNode {
    Task* task;
    struct TaskNode* next;
    struct TaskNode* prev;
} TaskNode;

typedef struct SortedDispatcherDatabase SortedDispatcherDatabase;

typedef struct ThreadArguments {
    SortedDispatcherDatabase* q;
    int id;
} ThreadArguments;

typedef struct Task {
    char* task_id;
    int task_duration;
	double cache_warmed_up;
	SortedDispatcherDatabase* owner;
} Task;

// TODO: You can modify this struct and add any 
// fields you may need
typedef struct SortedDispatcherDatabase {
    TaskNode* head;
    TaskNode* tail;
    int size;
    pthread_mutex_t sdd_lock;
}SortedDispatcherDatabase;

// **********************************************************

// SortedDispatcherDatabase API **********************************************************
void submitTask(SortedDispatcherDatabase* q, Task* _task);
Task* fetchTask(SortedDispatcherDatabase* q);
Task* fetchTaskFromOthers(SortedDispatcherDatabase* q);
void print_queue(SortedDispatcherDatabase* q, int core_id);
SortedDispatcherDatabase* create_sdd();
void destroy_sdd(SortedDispatcherDatabase* db);
// You can add more methods to Queue API
// .
// . 
// **********************************************************


// Your simulator threads should call this function to simulate execution. 
// Don't change the function signature, you can use the provided implementation of 
// this function. We will use potentially different implementations while testing.
void executeJob(Task* task, SortedDispatcherDatabase* my_queue, int my_id );
void* processJobs(void* arg);
void initSharedVariables();
#endif