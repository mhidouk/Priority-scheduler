#ifndef HEADERS_H  //Header files
#define HEADERS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <signal.h>

#define MAX_TASKS 100  // Maximum number of tasks scheaduler can handle

typedef struct {
    char name[20];
    int arrivalTime;
    int priority;
    int burstTime;
    int remainingTime;
    int startTime;
    int endTime;
} Task;

//Function prototypes
void readTasksFromFile(const char* filename, Task* tasks, int* taskCount);
void schedule(Task* tasks, int taskCount, int numThreads);

#endif