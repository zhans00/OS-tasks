#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>

#define N 10

/* the number of total operations */
#define TOTAL_OP 20

char arr[N];
int ind = 0;

pthread_mutex_t mtx;
sem_t empty;
sem_t full;

/* variable to keep track of the number of executed operations */
int op = 0;

void *producer()
{	
	/* run until threads TOTAL_OP times executed */
	while (op < TOTAL_OP){

		/*to test speed variations*/
		// usleep(5);

		/* decrement the semaphore to check whether to execute a function or wait until positive number of empty slots (i.e empty > 0) is available */
		sem_wait(&empty);

		/* entering the crititcal region */
		pthread_mutex_lock(&mtx);

		if(op >= TOTAL_OP) {
			break;
		}

		/* increment num of operations and index of the last added element, respectively*/
		op +=1;
		ind += 1;
		arr[ind] = 'A';

		/* printing */
		for (size_t i = 0; i <= ind; i++)
		{
			putchar(arr[i]);
		}
        fflush(stdout);
		printf(" %d\n", ind);

		/* leaving the crititcal region */
		pthread_mutex_unlock(&mtx);

		/* increment full after adding an element to the buffer */
		sem_post(&full);
	} 
}

void *consumer()
{
	/* run until threads TOTAL_OP times executed */
	while (op < TOTAL_OP)	{

		/*to test speed variations*/
		// usleep(5);

		/* decrement the semaphore to check whether to execute a function or wait until positive number of allocated slots (i.e full > 0) is available */
		sem_wait(&full);

		/* entering the critical region */
		pthread_mutex_lock(&mtx);
		if(op >= TOTAL_OP) {
			break;
		}
		op +=1;
		arr[ind] = 0;
		ind -= 1;
		printf("[");

		/* printing */
		for (size_t i = 0; i <= ind; i++)
		{
			putchar(arr[i]);
		}
        fflush(stdout);
		printf("] [%d]\n", ind);

		/* leaving the critical region */
		pthread_mutex_unlock(&mtx);

		/*decrement the number of allocated slots */
		sem_post(&empty);
	} 
}

int main(int argc, char const *argv[])
{
	/* to avoid deadlocks */
	if (N <= 0) {
		return 1;
	}

	/* initialization */
	pthread_t prod, cons;
	pthread_mutex_init(&mtx, NULL);
	sem_init(&empty, 0, N);
	sem_init(&full, 0, 0);

	/*creating the threads */
	pthread_create(&cons, NULL, (void *)consumer, NULL);
	sleep(1);
	pthread_create(&prod, NULL, (void *)producer, NULL);

	/* join, so the main function waits until the threads executed */
	pthread_join(prod, NULL);
	pthread_join(cons, NULL);


	pthread_mutex_destroy(&mtx);
	sem_destroy(&empty);
	sem_destroy(&full);

	return 0;
}