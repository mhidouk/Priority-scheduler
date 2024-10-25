#include "headers.h"

int main(int argc, char* argv[]) {   //Checks correct number of arguments
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <task_file> <num_threads>\n", argv[0]);
        return 1;
    }

    const char* filename = argv[1];   //Extract filename and number of threads from command-line arguments
    int numThreads = atoi(argv[2]);

    Task tasks[MAX_TASKS]; // array to keep track of tasks
    int taskCount = 0;  // variable to keep track of number of tasks

    readTasksFromFile(filename, tasks, &taskCount);  // function to read tasks from file
    schedule(tasks, taskCount, numThreads);  // calls main scheaduling function

    return 0;
}

void readTasksFromFile(const char* filename, Task* tasks, int* taskCount) {  //Opens input file and checks for error
    FILE* file = fopen(filename, "r");
    if (!file) {
        perror("Error opening file");
        exit(1);
    }

    char line[100];
    while (fgets(line, sizeof(line), file) && *taskCount < MAX_TASKS) {  //Reads each line from the file, parses it into a Task structure, and adds it to the task array
        Task* task = &tasks[*taskCount];
        sscanf(line, "%[^,], %d, %d, %d", 
               task->name, &task->arrivalTime, &task->priority, &task->burstTime);
        task->remainingTime = task->burstTime;
        task->startTime = -1;
        task->endTime = -1;
        (*taskCount)++;
    }

    fclose(file);
}