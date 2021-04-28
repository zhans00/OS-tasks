# CFS
A simple simulator for the CFS scheduler.

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
    Next n lines: priority, burst time, and arrival time of each process.
