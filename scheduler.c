#include "headers.h"
#include <pthread.h>

typedef struct {  // Structure to pass arguments to worker thread
    Task* tasks;
    int taskCount;
    int* currentTime;
    int* completedTasks;
    pthread_mutex_t* mutex;
    pthread_cond_t* cond;
} ThreadArgs;

void* workerThread(void* arg);   //Function prototypes for helper functions
int compareTaskArrival(const void* a, const void* b);                 
Task* findReadyHighestPriorityTask(Task* tasks, int taskCount, int currentTime);

void schedule(Task* tasks, int taskCount, int numThreads) {
    qsort(tasks, taskCount, sizeof(Task), compareTaskArrival);      //Sort tasks by arrival time

    int currentTime = 0;
    int completedTasks = 0;
    pthread_t threads[numThreads];
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond, NULL);

    ThreadArgs args = {tasks, taskCount, &currentTime, &completedTasks, &mutex, &cond}; // Worker thread created
                                              
    for (int i = 0; i < numThreads; i++) {
        pthread_create(&threads[i], NULL, workerThread, &args);    //Waits for all threads to complete
    }

    for (int i = 0; i < numThreads; i++) {
        pthread_join(threads[i], NULL);
    }

    pthread_mutex_destroy(&mutex);  //Cleans up sync primitives
    pthread_cond_destroy(&cond);

    // Print Gantt chart
    for (int i = 0; i < taskCount; i++) {
        printf("Task: %s, Start: %d, End: %d\n", tasks[i].name, tasks[i].startTime, tasks[i].endTime);
    }
}

void* workerThread(void* arg) { //This function implements the core scheaduling logic ensuring safe operations on data
    ThreadArgs* args = (ThreadArgs*)arg;

    while (1) {
        //lock mutex for exclusive access to shared data
        pthread_mutex_lock(args->mutex);

        // Wait if there are no tasks ready to execute
        while (*args->completedTasks < args->taskCount && 
               !findReadyHighestPriorityTask(args->tasks, args->taskCount, *args->currentTime)) {
                
                //release the mutex and wait for signal
            pthread_cond_wait(args->cond, args->mutex);
        }
            // if all tasks completed exit thread
        if (*args->completedTasks == args->taskCount) {
            pthread_mutex_unlock(args->mutex);
            break;
        }

        // find highest priority task
        Task* currentTask = findReadyHighestPriorityTask(args->tasks, args->taskCount, *args->currentTime);
        if (currentTask) {
            if (currentTask->startTime == -1) {
                currentTask->startTime = *args->currentTime;
            }

            // execute the task for 1 time unit or until completion
            int executionTime = (currentTask->remainingTime < 1) ? currentTask->remainingTime : 1;
            currentTask->remainingTime -= executionTime;
            *args->currentTime += executionTime;

            //if task is completed, update end time and increment completed task count
            if (currentTask->remainingTime == 0) {
                currentTask->endTime = *args->currentTime;
                (*args->completedTasks)++;
            }
        } else {
            (*args->currentTime)++;
        }

        pthread_cond_broadcast(args->cond);
        pthread_mutex_unlock(args->mutex);
    }

    return NULL;
}

int compareTaskArrival(const void* a, const void* b) {
    return ((Task*)a)->arrivalTime - ((Task*)b)->arrivalTime;
}

Task* findReadyHighestPriorityTask(Task* tasks, int taskCount, int currentTime) {
    Task* highestPriorityTask = NULL;
    for (int i = 0; i < taskCount; i++) {
        if (tasks[i].arrivalTime <= currentTime && tasks[i].remainingTime > 0) {
            if (!highestPriorityTask || tasks[i].priority > highestPriorityTask->priority) {
                highestPriorityTask = &tasks[i];
            }
        }
    }
    return highestPriorityTask;
}