#include <stdio.h>
#include <stdlib.h> 
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <limits.h>
#include "sdd.h"
#include "constants.h"

extern int stop_threads;
extern SortedDispatcherDatabase** processor_queues;
pthread_mutex_t print_lock;

void executeJob(Task* task, SortedDispatcherDatabase* my_queue, int my_id);
int all_jobs_finished(int registered_jobs);
static Task* steal_task(int my_id); 
#define LOW_WATERMARK 2
#define HIGH_WATERMARK 6
// Thread function for each core simulator thread
void* processJobs(void* arg) {
    // initalize local variables
    ThreadArguments* my_arg = (ThreadArguments*) arg;
    SortedDispatcherDatabase* my_queue = my_arg -> q;
    int my_id = my_arg -> id;

    // Main loop, each iteration simulates the processor getting The stop_threads flag
    // is set by the main thread when it concludes all jobs are finished. The bookkeeping 
    // of finished jobs is done in executeJob's example implementation. a task from 
    // its or another processor's queue. After getting a task to execute the thread 
    // should call executeJob to simulate its execution. It is okay if the thread
    //  does busy waiting when its queue is empty and no other job is available
    // outside.
    while (!stop_threads) {
        Task* task = NULL;
        task = fetchTask(my_queue);

        if(task == NULL ){
            pthread_mutex_lock(&(my_queue->sdd_lock));
            int my_size = my_queue->size;
            pthread_mutex_unlock(&(my_queue->sdd_lock));

            if (my_size < LOW_WATERMARK) {
                Task* stolen = steal_task(my_id);
                if (stolen != NULL){
                    task = stolen;
                }
            }
        }

        if (task == NULL){
            if(stop_threads){
                break;
            }
            usleep(CYCLE*500);
            continue;
        }
        
        executeJob(task, my_queue, my_id);  

        if (task->task_duration > 0){
            submitTask(my_queue, task);
        }          
        else{
            free(task->task_id);
            free(task);
        }
        
    }
    free(my_arg);
    return NULL;
}

// Do any initialization of your shared variables here.
// For example initialization of your queues, any data structures 
// you will use for synchronization etc.
void initSharedVariables() {
    for (int i = 0; i < NUM_CORES; i++){
        SortedDispatcherDatabase* db = processor_queues[i];

        db->head = NULL;
        db->tail = NULL;
        db->size = 0;

        if(pthread_mutex_init(&(db->sdd_lock),NULL) != 0 ){
            perror("Mutex init failed");
            exit(EXIT_FAILURE);
        }
    }
    if (pthread_mutex_init(&print_lock, NULL) != 0) {
        perror("Mutex init failed");
        exit(EXIT_FAILURE);
    }
}

static Task* steal_task(int my_id){
    for (int i = 0; i<NUM_CORES; i++) {
        if (i == my_id){
            continue;
        }

        SortedDispatcherDatabase* q = processor_queues[i];

        pthread_mutex_lock(&(q->sdd_lock));
        int sz = q->size;
        pthread_mutex_unlock(&(q->sdd_lock));

        if( sz > HIGH_WATERMARK){
            Task* stolen = fetchTaskFromOthers(q);
            if(stolen != NULL){
                return stolen;
            }
        }
    }
    return NULL;
}
