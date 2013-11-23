#include "main.h"

int main(void)
{
    int i = 0;
    int err;
    pthread_t worker_thread[MAX_CONCURRENCY];

    if (pthread_mutex_init(&IO_mux, NULL) != 0)
    {
        printf("\n mutex init failed\n");
        return 1;
    }

    err = pthread_create(&worker_thread[1], NULL, &signal_IO, NULL);
    if (err != 0)
        printf("\ncan't create thread :[%s]", strerror(err));
            
    /*
     err = pthread_create(&worker_thread[0], NULL, &wait_for_IO, NULL);
    if (err != 0)
        printf("\ncan't create thread :[%s]", strerror(err));
      
    */      
   
    
    
    i++;   
    
    while(i < MAX_CONCURRENCY-1)
    {
        pthread_join(worker_thread[i], NULL);
        i++;
    }
    
    pthread_mutex_destroy(&IO_mux);

    return 0;
}
