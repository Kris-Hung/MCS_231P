#include <stdio.h> //remove if not using.
#include <stdlib.h>
#include <pthread.h>
#include "util.h"//implementing
#include <math.h>

typedef struct {
    pthread_mutex_t *pmtx;
    int *count;
    int *max;
    int *verb;
} Args;

static int isPrime(int number) 
{ 
    if (number <= 1) 
        return 0; 
    if (number <= 3) 
        return 1;
    long limit = sqrt(number), i;

    // to reduce computations, our for loop runs till the square root of the number
    for(i=2; i<=limit; i++)
    {
        if(number%i == 0) return 0;
    }

    return 1;
}

void* print_primes(void *arg) {
    Args  *a = (Args *) arg;
    int max = (*(int*)a->max);
    while(1) {
        pthread_mutex_lock(a->pmtx);
        int n = (*(int*)a->count);
        *(a->count) = *(a->count) + 1;
        pthread_mutex_unlock(a->pmtx);
        if (n > max)
            break;
        //printf("Thread id[%d] checking [%d]\n", pthread_self(), n);
        if (isPrime(n)) {
            if ((*(int*)a->verb))
                printf("prime: %d\n", n);
        }     
    }
    pthread_exit(NULL);
}

void primes_st(unsigned int max, unsigned int verb){
    for (int i=0; i<max; i++) {
        if (isPrime(i)) {
            if (verb)
                printf("prime: %d\n", i);
        }
    }
    return;
}

void primes_mt(unsigned int max, unsigned int threads, unsigned int verb){
    int const num_thr = threads;
    pthread_t my_threads[num_thr];
    pthread_mutex_t mutex;
    pthread_mutex_init(&mutex, NULL);
    int shrd_count = 1, ret, i;
    Args arg_thr = {&mutex, &shrd_count, &max, &verb};

    for (i=0; i<num_thr; ++i) {
        ret = pthread_create(&my_threads[i], NULL, &print_primes, (void*)&arg_thr);
        if (ret) {
            printf("Error creating thread\n");
            exit(-1);
        }
    }

    for (i=0; i<num_thr; ++i)
        pthread_join(my_threads[i], NULL);

    return;
}
