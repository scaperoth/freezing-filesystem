#include "main.h"

int main(void)
{
    int i = 0;
    int err;
    pthread_t worker_thread[MAX_CONCURRENCY];

    if (pthread_mutex_init(&lock, NULL) != 0)
    {
        printf("\n mutex init failed\n");
        return 1;
    }

    start = 1;
     err = pthread_create(&worker_thread[0], NULL, &consumer, NULL);
    if (err != 0)
        printf("\ncan't create thread :[%s]", strerror(err));
            
   
    err = pthread_create(&worker_thread[1], NULL, &producer, NULL);
    if (err != 0)
        printf("\ncan't create thread :[%s]", strerror(err));
            
    
    i++;   
    
    while(i < MAX_CONCURRENCY)
    {
        pthread_join(worker_thread[i], NULL);
        i++;
    }
    
    pthread_mutex_destroy(&lock);

    printf("Counter is now: %d\n",count);

    return 0;
}
