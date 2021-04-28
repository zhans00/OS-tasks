#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>

sem_t sem1;
sem_t sem2;
pthread_mutex_t mutex;

/*prev is a variable to check which thread was executed the last time. 
Since there are only 2 threads, it may be eithe 0 or 1 => bool.*/
bool prev = 0;


void * flush(void * ptr) {
    char * s;
    s = (char * ) ptr;
    int l = strlen(s);

    for (int i = 0; i < l; i++) {

    	/*  if the last executed thread was prev=0, and it hasn't changed the val of prev yet,
    		then we put the second thread in the queue since val of sem1 will be negative. because sem_Wait decrements
    		if the last executed thread was prev=1, then we just allow it to run, since sem1.val will be non negative*/
        if (prev == 0) {
            sem_wait( & sem1);
        }

        /*if the last executed prev=1 => the current process is blocked and put into the queue, 
        we later unblock it using post*/
        else {
            sem_wait( & sem2);
        }

        // lock the critical region
        pthread_mutex_lock( & mutex);

        putchar(s[i]);
        fflush(stdout);
        sleep(rand() % 2);

        pthread_mutex_unlock( & mutex);

        /* here we allow processes in the queue to run, after we leave the critical section*/
        if (prev == 0) {
            sem_post( & sem1);
            prev = 1;
        } else {
            sem_post( & sem2);
            prev = 0;
        }

    }
}

int main() {
	// initialize mutex
    pthread_mutex_init( & mutex, NULL);

    // initialize the semaphores.
    // we are synchronizing threads -> the second arg is 0.
    // we use binary semaphores, because each thread cannot run twice (similar to prod-consumer problem with buffer N=1)
    sem_init( & sem1, 0, 1);
    sem_init( & sem2, 0, 0);

    char * s1 = "abcdefghij";
    char * s2 = "ABCDEFGHIJ";
    pthread_t t1, t2;
    int pid1 = pthread_create( & t1, NULL, flush, (void * ) s1);
    int pid2 = pthread_create( & t2, NULL, flush, (void * ) s2);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    printf("\n");
    exit(0);

    //destroying the semaphores and mutexes
    pthread_mutex_destroy( & mutex);
    sem_destroy( & sem1);
    sem_destroy( & sem2);

    return 0;
}