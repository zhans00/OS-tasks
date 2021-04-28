#include <stdio.h>
#include <unistd.h>

int get_num_proc(char name[]) {
   FILE *fp;
   int num_proc;

   if (access(name, F_OK) != 0)
   		return -1;

   fp = fopen(name, "r");
   fscanf(fp, "%d", &(num_proc));
   fclose(fp);
   return num_proc;
}

void read_file(char name[], process procs[]) {
   FILE *fp;
   int vals[3];
   int num_proc;

   fp = fopen(name, "r");
   fscanf(fp, "%d", &(num_proc));

   for (int i = 0; i < num_proc; ++i) {
   		fscanf(fp, "%d", &(vals[0]));
   		fscanf(fp, "%d", &(vals[1]));
   		fscanf(fp, "%d", &(vals[2]));

   		(&procs[i])->priority = vals[0];
   		(&procs[i])->left_time = vals[1];
   		(&procs[i])->start_time = vals[2];
   		(&procs[i])->preempted = 0;
        (&procs[i])->response_time = 0;
        (&procs[i])->waiting_time = 0;
        (&procs[i])->runned = false;
   }
   fclose(fp);
}