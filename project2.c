#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <pthread.h>
#include <semaphore.h>

#define DEBUG 1     // switch to 0 before submitting
#define BUFFER 1024 // max line length
int counter;
int totalWordCount;

// Definition of queue
typedef struct
{
    int fill;         // where to write
    int use;          // where to read
    int q_len;        // length of queue
    char *buffer;     // actual queue where contents of line will be stored
    sem_t queue_lock; // control variable(s) lock
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

// Should we get the current line number from here too? 
// Pop an entry from the queue 
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

// TOTO 
// Thread function
void *wordCount(void *)
{
}

int main(int argc, char **argv[])
{
    int numTasks = 0;   // number of tasks from user input
    FILE *fp;           // file pointer
    char *line = NULL;  // line pointer
    ssize_t n = 0;      // size of allocated buffer
    ssize_t length = 0; // length of line
    int lineCount = 0;  // number of lines
    totalWordCount = 0; // total number of words in file

    assert(argc < 3);

    numTasks = argv[1];

    fp = stdin;

    // Get number of lines to determine size of queue
    while ((length = getline(&line, &n, fp)) != -1)
    {
        if (DEBUG)
            printf("line = %s\n", line);
        lineCount++;
    }

    if (DEBUG)
        printf("%d\n", lineCount);

    // Create the queue
    char *buffer1[lineCount];
    QUEUE q1 = {0, 0, lineCount, buffer1};
    sem_init(&q1.queue_lock, 0, 1);
    assert(sem_init(&q1.empty, 0, lineCount) == 0);
    assert(sem_init(&q1.full, 0, 0) == 0);

    // Set up the data to be passed to the threads
    thread_data thread1 = {lineCount, &q1};

    line = NULL; // reset line pointer
    n = 0;       // reset buffer sie
    length = 0;  // reset length of line

    // Pushes each line from the file to the queue
    while ((length = getline(&line, &n, fp)) != -1)
    {
        line[length - 1] = '\0'; // strip newline char
        if (DEBUG)
            printf("line q = %s\n", line);
        put(&q1, line);
    }
    free(line); // avoid memory leak

    // pthread_t p1;
    // pthread_t p2;
    return 0;
}
