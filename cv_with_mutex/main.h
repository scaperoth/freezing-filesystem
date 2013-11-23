#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

typedef int bool;
#define true 1
#define false 0

bool IO_ready = false;
int nblked = 0;
pthread_mutex_t IO_mux;
pthread_cond_t IO_blklist;

int count = 100000;
int MAX_CONCURRENCY = 2;
int MAX_COUNT = 200000;

/**
 * prototypes
 */
static inline void* wait_for_IO(void* args);
static inline void* signal_IO(void* args);

static inline void* wait_for_IO(void* args)
{	
	pthread_mutex_lock(&IO_mux);
    if(!IO_ready){
        nblked++;
        printf("wait_for_IO: %d\n",nblked);
        pthread_cond_wait(&IO_blklist, &IO_mux);
    }
    pthread_mutex_unlock(&IO_mux);
    return NULL;
}

static inline void* signal_IO(void* args)
{	
	pthread_mutex_lock(&IO_mux);
    if(nblked){
        nblked--;
        printf("signal_IO: %d\n",nblked);
        pthread_cond_signal(&IO_blklist);
    }   
    pthread_mutex_unlock(&IO_mux);
    return NULL;
}
