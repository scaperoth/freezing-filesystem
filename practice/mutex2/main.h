#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

int count = 200000;
pthread_mutex_t lock;
int MAX_CONCURRENCY = 2;
int MAX_COUNT = 400000;
int start = 0;

/**
 * prototypes
 */
static inline void* producer(void* args);
static inline void* consumer(void* args);

static inline void* producer(void* args)
{	
	pthread_mutex_lock(&lock);
	while(count!=MAX_COUNT){
    	count++;
    	printf("Increment count: %d\n", count);
	}
    pthread_mutex_unlock(&lock);
    return NULL;
}

static inline void* consumer(void* args)
{	
	pthread_mutex_lock(&lock);
	while(count!=0){
    	count--;
    	printf("Decrement count: %d\n", count);
	}
    pthread_mutex_unlock(&lock);
    return NULL;
}
