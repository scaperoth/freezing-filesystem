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

    while(i < MAX_CONCURRENCY)
    {
        if(i%2 == 0){
            err = pthread_create(&worker_thread[i], NULL, &consumer, NULL);
            if (err != 0)
                printf("\ncan't create thread :[%s]", strerror(err));
            i++;
        }else{
            err = pthread_create(&worker_thread[i], NULL, &producer, NULL);
            if (err != 0)
                printf("\ncan't create thread :[%s]", strerror(err));
            i++;
        }
        
    }

    start = 1;
    sleep(1);
    while(i < MAX_CONCURRENCY)
    {
        pthread_join(worker_thread[i], NULL);
        i++;
    }
    
    pthread_mutex_destroy(&lock);

    return 0;
}
