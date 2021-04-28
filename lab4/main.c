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

typedef struct node
{
    char val;
    struct node *next;
} node_t;


pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t full = PTHREAD_COND_INITIALIZER;
pthread_cond_t empty = PTHREAD_COND_INITIALIZER;

node_t *nodes;
int op = 0;

/* the number of exsiting items in the buffer */
int counter = 0;


void *producer() {

	/* run until threads TOTAL_OP times executed */
	while (op < TOTAL_OP)
	{

        /*to test speed variations*/
		// usleep(5);


        /*entering the crtitical region */
		pthread_mutex_lock(&mtx);


		if(op >= TOTAL_OP) {
            
			break;
		}

        /* increment num of operations */
		op +=1;

        /* check condition of a condition variable 'full' 
        since we cannot produce more than N items */
        while (counter == N){
            /* lock the thread */
            pthread_cond_wait(&full, &mtx);
        }

        /* allocate memory for a new node */
        struct node *newNode = malloc(sizeof(struct node));
        newNode->val = 'A';
        newNode->next = NULL;

        /* if we have an empty buffer */
        if(nodes == NULL){
            nodes = newNode;
            printf("A ");
        }

        /* if not empty -> find the last node and set the newNode as a 'next' of the last node */
        else
        {
            struct node *lastNode = nodes;

            while(lastNode->next != NULL)
            {
                /* printing values */
                printf("%c", lastNode->val);
                lastNode = lastNode->next;
            }

            /* since we quit before printing the lastNode val and plus the added node's value -> 2 A's */
            printf("AA ");
            //add the newNode at the end of the linked list
            lastNode->next = newNode;
        }

        /* increment since we added an item */
        counter +=1;
        printf("%d\n", counter);

        /*if the buffer was empty, after producing we signal that consumer now can take items; if it was not empty, the signal is ignored */
        pthread_cond_signal(&empty);

        /* leaving the critical region */
		pthread_mutex_unlock(&mtx);
	} 
}

void *consumer() {

    /* run until threads TOTAL_OP times executed */
    while (op < TOTAL_OP)
	{
		/*to test speed variations*/
		// usleep(5);

		pthread_mutex_lock(&mtx);

		if(op >= TOTAL_OP) {
			break;
		}


        op +=1;

        /* check condition of a condition variable 'full' 
        since we cannot consume if there are no items in the buffer*/
        while(nodes == NULL) {
            /* lock the thread */
            pthread_cond_wait(&empty, &mtx);
        }

 
            /* lastnode's 'next' points to temp and temp is the last node found so far */
            node_t *temp = nodes;
            node_t *lastNode;
            printf("[");

            /* case when the list has only one node */
            if (nodes->next == NULL)
            {
                nodes = NULL;
                free(nodes);
                printf("] ");
            }
            
            else if(nodes != NULL) {
                while(temp->next != NULL)
                {
                    lastNode = temp;
                    temp = temp->next;
                    printf("A");
                }
                lastNode->next = NULL;
                free(temp);
                printf("] ");
            }

        counter-=1;
        printf("[%d]\n", counter);

        /*if the buffer was full, after consuming, we signal that producer now can add items; if it was not full, the signal is ignored */
        pthread_cond_signal(&full);

        /* leaving the critical region */
		pthread_mutex_unlock(&mtx);
	} 
}

int main(int argc, char const *argv[])
{

    /* avoid deadlocks */
    if(N <= 0) {
        return 1;
    } 

        /* init*/
    pthread_t prod, cons;
	pthread_mutex_init(&mtx, NULL);


	pthread_create(&cons, NULL, (void *)consumer, NULL);
	sleep(1);
	pthread_create(&prod, NULL, (void *)producer, NULL);

	pthread_join(prod, NULL);
	pthread_join(cons, NULL);

	pthread_mutex_destroy(&mtx);
    pthread_cond_destroy(&full);
    pthread_cond_destroy(&empty);
	return 0;
}

