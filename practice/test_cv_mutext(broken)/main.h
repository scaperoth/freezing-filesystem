#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>

pthread_mutex_t lock;
pthread_cond_t cond;
int buffer;
int count = 0;//initally empty

/*thread variables*/
int MAX_CONCURRENCY = 20;
int start = 0;

/*prototypes*/
static inline void put(int value);
static inline void get();

static inline void *producer(void* args);
static inline void *consumer(void* args);

static inline void put(int value){
	assert(count==0);
	count=1;
	buffer=value;
}

static inline void get(){
	assert(count==1);
	count=0;
	return buffer;
}

static inline void *producer(void* args)
{
	int i;
	for(i = 0; i <loops; i++){
		Pthread_mutex_lock($mutex);
		while(count==1)
			Pthread_cond_wait(&cond,&mutex);
		put(i);
		Pthread_cond_signal(&cond);
		Pthread_mutex_unlock(&mutext);
	}
}

static inline void *consumer(void* args)
{	
	int i;
	for(i = 0; i <loops; i++){
		Pthread_mutex_lock($mutex);
		while(count==0)
			Pthread_cond_wait(&cond,&mutex);
		int tmp = get();
		Pthread_cond_signal(&cond);
		Pthread_mutex_unlock(&mutext);
		printf("%d\n",tmp);
	}
}

