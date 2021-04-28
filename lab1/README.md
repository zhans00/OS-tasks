An argument from the user corresponding to a Linux command or process. If the process exists, the parent process creates three children where each of them performs one of the
following tasks:

    1) First child: to execute and display where this command exists in the disk 
    2) Second child: to execute and display the manual page of this command in a new terminal window
    3) Third child: to open a firefox tab displaying the google search page of the word the user entered

If the process does not exist, an error message is displayed.
The parent process waits the children to terminate before it terminates the program.