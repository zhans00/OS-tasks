/*
    How to run:
        1) to generate random values, provide the number of processes as argument. 
        For example, $ gcc main.c -o main.out
                     $ ./main.out 5
        Here, 5 is the number of processes.

        2) to run with specific values, provide two arguments: 
        "-file" and the name of the .txt file with processes.

        For example, $ gcc main.c -o main.out
                     $ ./main.out -file processes.txt

    Structure of the .txt file is the following:
    The first line: the number of processes
    Next n lines: priority, burst time, and arrival time. (they must be valid positive integers: 
        priority in range [1-99] )

    For example:
        3
        12 34 4 
        34 46 7 
        7 23 9

    Here, priorities - 12, 34, 7; burst times - 34, 46, 23; arrival times - 4, 7, 9 for the 1st, 2nd and 3rd processes respectively.
*/

#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include "lib.h"
#include <stdio.h>
#include "read_file.h"

#define MIN_GRAN 1
#define MAX_PRIORITY 99
#define MAX_BURST 300
#define MAX_ARRIVAL 2000

int quantum;

/* start and end define the range of runnables processes*/ 
int start = 0;
int end = 0;
int num_proc;
float total_wait = 0.0;
float total_response = 0.0;

/* nil_leaf = empty RBNode */ 
RBNode *root = &nil_leaf;

/* 1 tick = 1 unit time */
gclock g_clock = {.ticks = 0};

/* get random burst time */
int generate_burst() {
    return 1 + rand() % MAX_BURST; 
}

/* get random priority */
int generate_priority() {
    return 1 + rand() % MAX_PRIORITY; 
}

/* get random arrival times*/
int generate_arrival() {
    return rand() % (MAX_ARRIVAL + 1);
}
    
/* sum of prorities of processes in the tree */
int get_pr(process procs[], int start, int end) {
    int sum = 0;
    for (int i = start; i < end; i++) {
        sum += procs[i].priority;
    }

    return sum;
}

/* get quantum, to ensure minimum granlarity */
int get_q(process procs[]){
    return MIN_GRAN*(get_pr(procs, 0, num_proc));
}

int get_timeslice(int prior, process procs[]) {
    return (quantum * prior)/get_pr(procs, start, end);
}

/* get weight based on priority to compute vruntime. higher priority -> lower weight -> vruntime increases slowly */
double get_weight(int prior) {
    return 45.0/prior;
}

/* compute response time */
void add_response(process procs[]) {
    for (int i = start; i < end; ++i) {
        if(!(&procs[i])->runned){
            (&procs[i])->response_time +=1;
        }
    }
}

/* compute wait time for the waiting processes */
void add_wait(process procs[]) {
    for (int i = start; i < end; ++i) {
        (&procs[i])->waiting_time +=1;
    }
}

/* randomly initialize processes */
void initialize(process procs[]) {
    for (int i = 0; i < num_proc; i++) {
        (&procs[i])->priority = generate_priority();
        (&procs[i])->left_time = generate_burst();
        (&procs[i])->start_time = generate_arrival();
        (&procs[i])->preempted = 0;
        (&procs[i])->response_time = 0;
        (&procs[i])->waiting_time = 0;
        (&procs[i])->runned = false;
    }
}

/* run processes until the last one is finshed */
void run_processes(process procs[]) {
    bool finished = false;

    /* for checking if a process was preempted */
    bool reinsert;

    while(!finished) {
        /* add processes into the tree, if their arrival time == current clock tick */
        while (end < num_proc && g_clock.ticks == procs[end].start_time) {
            RBNode* t = RB_insert(&root, &(procs[end]), 0);
            end +=1;
        }

        /* computer quantum to ensure that every process will get at least MIN_GRAN timeslice */
        quantum = get_q(procs);

        /* if no runnable processes, then just increment clock ticks */
        if(root == &nil_leaf)
            g_clock.ticks +=1;
        else {

            /* delete the leftmost node with the least vruntime */
            RBNode* temp = delete(&root);
            RBNode temp2 = *temp;

            /* calculate timeslice of the chosen process */
            int timeslice = get_timeslice(temp->process->priority, procs);

            /* release describes time when the process will either preempted or finished */
            int release;
            temp->process->runned = true;

            /* if timeslice exceeds the left time -> run until finished then terminate */
            if (temp->process->left_time <= timeslice) {
                release = g_clock.ticks + temp->process->left_time;
                temp->vruntime += (temp->process->left_time*get_weight(temp->process->priority));
                temp->process->left_time = 0;
                start +=1;

                /* if we reached the last process -> all processes have finished, so break the loop */
                if(start == num_proc) {
                    finished = true;
                    break;
                }

                /* since this process is finished, no need to reinsert again */
                reinsert = false;

            } else {
                release = g_clock.ticks + timeslice;
                temp->process->left_time -= timeslice;
                temp->vruntime += (timeslice*get_weight(temp->process->priority));
                reinsert = true;
                temp->process->preempted +=1;
            }

            /* tick until we reach the release time */
            while(g_clock.ticks != release) {
                add_response(procs);
                add_wait(procs);
                while (end < num_proc && g_clock.ticks == procs[end].start_time) {
                        RB_insert(&root, &(procs[end]), 0);
                        end +=1;
                }
                g_clock.ticks +=1;
            }
            if (reinsert){
                RB_insert(&root, (temp2.process), temp2.vruntime);
            }
            quantum = get_q(procs);
        }
    }
}

int main(int argc, char* argv[]) {
    bool file = false;

    /* checking the validity of comman line arguments */
    if(argc == 2) {
        num_proc = (int)strtol(argv[1], NULL, 10);
        if (num_proc == 0){
            printf("Invalid number of processes.\n");
            return 1;
        }
    } else if (argc == 3){
        file = true;
        if (access(argv[2], F_OK) != 0){
            printf("File does not exist.\n");
            return 1;
        }
        num_proc = get_num_proc(argv[2]);
    } else {
    	printf("Invalid number of processes.\n");
    	return 1;
    }

    process procs[num_proc];
    srand ( time(NULL) );

    /* if a user hasn't provided the file, then initialize with random values */ 
    if(!file)
        initialize(procs);
    else
        read_file(argv[2], procs);

    /* sort by arrival time to imitate real world processes */
    qsort(&procs, num_proc, sizeof(process), compare);

    printf("**************************************\n");
    printf("*           Processes list.          *\n");
    printf("**************************************\n\n");

    for (int i = 0; i < num_proc; ++i) {
        printf("Process #%d: %d (arrival), %d (burst), %d (prio)\n", i+1, procs[i].start_time, procs[i].left_time, procs[i].priority);
    }

    run_processes(procs);

    printf("**************************************\n");
    printf("*           Statistics               *\n");
    printf("**************************************\n\n");

    for (int i = 0; i < num_proc; i++) {
        printf("Process #%d: %d (preempted), %d(waiting time) %d (response time)\n", i + 1,  procs[i].preempted, procs[i].waiting_time, procs[i].response_time);
        total_wait += procs[i].waiting_time;
        total_response += procs[i].response_time;
    }

    printf("\nAVERAGE WAITING TIME: %.2f\n", total_wait/num_proc);
    printf("AVERAGE RESPONSE TIME: %.2f\n", total_response/num_proc);

    return 0;
}
