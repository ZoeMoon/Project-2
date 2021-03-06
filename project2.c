#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <pthread.h>
#include <semaphore.h>

#define DEBUG 1     // switch to 0 before submitting
int totalWordCount;

// Definition of queue
typedef struct
{
    int fill;         // where to write
    int use;          // where to read
    int q_len;        // length of queue
    char *buffer;     // actual queue where contents of line will be stored
    sem_t queue_lock; // control variable(s)lock
    sem_t empty;      // semaphore empty condition
    sem_t full;       // semaphore full condition

} QUEUE;

// Thread data
typedef struct
{
    int taskNum;
    QUEUE *queue;
} thread_data;

// Push an entry onto the queue
void put(QUEUE *q, char *line)
{
    assert(sem_wait(&q->empty) == 0);
    assert(sem_wait(&q->queue_lock) == 0);

    q->buffer[q->fill] = (char *)line;
    q->fill = (q->fill + 1) % q->q_len;

    assert(sem_post(&q->queue_lock) == 0);
    assert(sem_post(&q->full) == 0);
}

// Pop a line from the queue
char *get(QUEUE *q)
{
    assert(sem_wait(&q->full) == 0);
    assert(sem_wait(&q->queue_lock) == 0);

    char *tmp = q->buffer[q->use];
    q->use = (q->use + 1) % q->q_len;

    assert(sem_post(&q->queue_lock) == 0);
    assert(sem_post(&q->empty) == 0);

    return tmp;
}

// TODO
// Consumer Thread function
// Pops line from queue
// Counts words on line
// Accumulates sum of words
void *wordCount(void *thread)
{
    thread_data *data = (thread_data *)thread;
    // int taskNumber = thread->numTasks;
    // int len = thread->queue->q1;
    // int i = 1;
    // while(1){
    //    int val = get(data->queue);
    //     if (val == '\0'){
    //         break;
    //     }
    //     printf("Inside wordcount: %d\n", i);
    //     i++;
    // }
    printf("Terminating\n");
    return NULL;
}

int main(int argc, char *argv[])
{
    int numTasks = 0;   // number of tasks from user input
    FILE *fp;           // file pointer
    char *line = NULL;  // line pointer
    ssize_t n = 0;      // size of allocated buffer
    ssize_t length = 0; // length of line
    int lineCount = 0;  // number of lines

    assert(argc < 3);

    numTasks = atoi(argv[1]);

    if (DEBUG){
        printf("Num tasks: %d\n", numTasks);
    }

    // Read in txt file
    fp = stdin;

    // Get number of lines to determine size of queue
    while ((length = getline(&line, &n, fp)) != -1)
    {
        if (DEBUG)
            printf("line = %s\n", line);
        lineCount++;
    }

    // Create the queue
    char *buffer1[lineCount];
    QUEUE q1 = {0, 0, lineCount, buffer1};
    sem_init(&q1.queue_lock, 0, 1);
    assert(sem_init(&q1.empty, 0, lineCount) == 0);
    assert(sem_init(&q1.full, 0, 0) == 0);

    line = NULL;   // reset line pointer
    n = 0;         // reset buffer size
    length = 0;    // reset length of line
    rewind(stdin); // points stream to beginning of file

    // Pushes each line from the file to the queue
    while ((length = getline(&line, &n, fp)) != -1)
    {
        if (DEBUG)
            printf("line q = %s\n", line);
        put(&q1, line);
    }

    // Set up the data to be passed to the threads
    thread_data threadD[numTasks];  // initialize array of structs 

    // Start the threads
    pthread_t threads[numTasks];

    // Pass data to threads
    for(int i = 1; i <= numTasks; i++){
        threadD[i].taskNum = i;
        threadD[i].queue = &q1;
    }

    // Create threads
    for (int i = 1; i <= numTasks; i++)
    {
        assert(pthread_create(&threads[i], NULL, wordCount, (void *)&threadD) == 0);
    }

    printf("Waiting for threads\n");

    // Join threads 
    for (int i = 1; i <= numTasks; i++)
    {
        assert(pthread_join(threads[i], NULL) == 0);
    }
    printf("Threads finished\n");

    free(line);

    return 0;
}
