#include <stdio.h>
#include<stdlib.h>
#include<string.h>
#include <sys/types.h>
#include  <sys/wait.h>
#include <unistd.h>

//defining max len of the command by user
const int LEN = 2000;

//defining const command for each process
const char pidof[] = "pidof ";
const char whereis[] = "whereis ";
const char man[] = "gnome-terminal -- man ";
const char browser[] = "firefox http://www.google.com/search?q=";

//implementing the first process whereis
void where(char name[]) {

    //allocating string for output
    char line[LEN];

    //allocating the whole command (+1 for NULL terminator)
    char * command = malloc(LEN + strlen(whereis) + 1);

    if (command == NULL) {
        printf("Please try again!\n");
        return;
    }

    // merging the const command names with argument
    strcpy(command, whereis);
    strcat(command, name);

    // writing a command to a stream
    FILE * cmd = popen(command, "r");

    // printing output of fgets, fgets writes the data from the stream to string line
    printf("%s\n", fgets(line, LEN, cmd));

    // close the stream
    pclose(cmd);
}

// implementing man page
void manual(char name[]) {
    char line[LEN];
    //allocating the whole command (+1 for NULL terminator)
    char * command = malloc(LEN + strlen(man) + 1);

    if (command == NULL) {
        printf("Please try again!\n");
        return;
    }

    // merging the const command names with argument
    strcpy(command, man);
    strcat(command, name);

    // execute commands in the terminal
    FILE * cmd = popen(command, "r");

    // close the stream
    pclose(cmd);
}

//implementing the search
void firefox(char name[]) {
    char line[LEN];
    //allocating the whole command (+1 for NULL terminator)
    char * command = malloc(LEN + strlen(browser) + 1);

    if (command == NULL) {
        printf("Please try again!\n");
        return;
    }

    // merging the const command names with argument
    strcpy(command, browser);
    strcat(command, name);

    FILE * cmd = popen(command, "r");

    // close the stream
    pclose(cmd);
}

int main(int argc, char * argv[]) {

    if (argc <= 1) {
        printf("Please provide a process name.\n");
        return 0;
    }

    char line[LEN];

    //allocating the whole command (+1 for NULL terminator)
    char * command = malloc(LEN + 7);

    if (command == NULL) {
        printf("Please try again!\n");
        return 0;
    }

    // merging the const command names with argument
    strcpy(command, pidof);
    strcat(command, argv[1]);

    // create a stream to check if the process exists
    FILE * cmd2 = popen(command, "r");

    if (fgets(line, LEN, cmd2) != NULL) {

        // finally run the command

        FILE * cmd = popen(command, "r");
        printf("The process exists in the process table with pids: %s\n", fgets(line, LEN, cmd));

        pclose(cmd);

        // creating the first child
        int p = fork();

        // if it's the first child then execute whereis
        if (p == 0) {
            where(argv[1]);
            exit(0);
        } else {
            // if it's parent, create the second child
            int p2 = fork();

            // if it's the 2nd child, execute man
            if (p2 == 0) {
                manual(argv[1]);
                exit(0);
            } else {

                // finally create the last child
                int p3 = fork();

                if (p3 == 0) {
                    firefox(argv[1]);
                    exit(0);
                }
            }
        }

        //wait for all children to terminate
        while (wait(NULL) > 0);
    } else {
        printf("Process '%s' does not exist.\n", argv[1]);
    }

    pclose(cmd2);
    return 0;
}