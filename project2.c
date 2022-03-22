#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <pthread.h>
#include <semaphore.h>

#define BUFFER 1024
int counter;
int totalWords;

typedef struct node_t {
    int value;
    struct node_t * next;
} node_t;

typedef struct queue_t{
    node_t nextIn;   
    node_t nextOut;    
    int queue_size; 
    char qbuff[BUFFER];
    pthread_mutex_t lock;
    pthread_cond_t cond;
} queue_t;

int main(int argc, char* argv[]) {
    int numTasks;
    FILE *fp;   // file pointer
    char* line = NULL; // line pointer
    ssize_t n = 0;  // size of allocated buffer
    ssize_t length = 0; // length of line 

    assert(argc < 2);
    numTasks = argv[1];

    fp = stdin;
   
    while((length = getline(&line, &n, fp))!= -1) {
       line[length-1] = '\0';   // strip newline char
       printf("line = %s\n", line);
    }

    free(line);

    pthread_t p1;
    pthread_t p2;
    return 0;

}
