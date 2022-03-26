#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <pthread.h>
#include <semaphore.h>

#define DEBUG 1     // switch to 0 before submitting
#define BUFFER 1028 // max line length
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

int main(int argc, char **argv[])
{
    int numTasks;
    FILE *fp;              // file pointer
    char *line = NULL;     // line pointer
    ssize_t n = 0;         // size of allocated buffer
    ssize_t length = 0;    // length of line
   int lineCount = 0;     // number of lines
    // assert(argc < 2);
    // numTasks = argv[1];

    fp = stdin;

    while ((length = getline(&line, &n, fp)) != -1)
    {
        // line[length-1] = '\0';   // strip newline char
        printf("line = %s\n", line);
        lineCount++;
    
    }
    line = NULL;
    n = 0;      // reset buffer size
    length = 0; // reset length of line

    rewind(stdin);
    // Pushes each line from the file to the queue
    while ((length = getline(&line, &n, fp)) != -1)
    {
       // lineCopy[length - 1] = '\0'; // strip newline char
        if (DEBUG)
            printf("line q = %s\n", line);
    }

    free(line);

    return 0;
}
