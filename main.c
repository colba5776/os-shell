/* In this assignment, you are asked to build a mini OS (shell + scheduler).

We talked about a user process could be characterized by alternative CPU and IO operations, i.e., any process consists of c1, i1, c2, i2, c3, i3, c4,...., cm, im, ... where the values of cm, im are determined by the user process behavior (implementation of the algorithm in solving different problems)

In evaluating the OS (scheduler) impact on the use processes, we only need to consider the CPU cycles of the user processes. Hence, the first task is to create (simulate) user processes with only CPU cycle, the user process could use an infinite loop to implement. When the process is scheduled to run, it prints a line of output in each iteration with the format of “Process #### at iteration @@@”, where #### is the process ID assigned by the OS, and @@@ is a counter starting from 0 to mimic the execution progress. of the process. 

A sample implementation of such a process could be as follows:

 

 int main(int argc, char* argv[]) {

      //initialization and other necessary operations

      // signal handler registration: for example, you could define my_sig_handler(), then call from here   

     signal(SIGINT, my_sig_handler); 

      while(1) {

            printf("Process %d at iteration %d\n", mypid, count++);

            sleep(1); // you could also use usleep() with specified high resolution time length

      }

 }

The second task is to create a Shell, which is a command interpreter that is used for users to interact with the system. When Shell starts, it will first print out a prompt (e.g., "shell 5500>>>"), then wait for command inputs from the user. 

For the shell implementation, you can envision it as an ongoing process that patiently awaits user input, interprets the provided commands, and takes appropriate actions accordingly. A sample code could be as follows:

                          while(1) {

                                 read_size = getline(&buffer, &size, stdin);

                                 //handling on read_size;

                                // interprets the buffer to get the command and its parameters...

                                switch(command) {

                                           case 'x':

                                                       //handling 'x' command 

                                                       break;

                                           case 'l':

                                                      // handling 'l' command

                                                       break;

                                           etc... for other commands

                            }

 

The following commands should be implemented:

OK ?? - 1. x: when a user inputs x (X), your Shell "system" should exit including the shell and all the processes created through/by the Shell. 

OK ?? - 2. c n: create n (a natural number) processes with the fork() system call, the user processes use exec() system call to load the program defined in first task above. The number of processes n should be at least 5 and no more than 10 for debugging purpose. The shell creates users processes P1, P2, P3, ..., Pn, a smaller process number (the subscript of Pi ,where 1<=i<=n) indicates it arrives at the system earlier. Also the process number can be used in the Shell to manage the processes. In order to mimic the ready state, you could suspend them immediately when it starts (SIGSTOP) so the process' state is ready.

OK ?? - 3. Ctrl-C handler should be implemented to suspend the currently running process and transfer the control to the scheduler (shell prompt, after this command, the user can use the commands to check the status of processes, configure scheduling, after that the 'scheduler' can schedule another process to run).

GOOD - 4. l: to list the current user processes in the system including it PID (underlying process ID from the host OS - Ubuntu), process number, and state.

MAYBE - 5. s rr q: to set the scheduler to be round robin with time quantum of q time units (seconds).

MAYBE - 6. s fcfs: to set the scheduler to be first come first serve, the process number determines its arrival order.

OK ?? - 7. k #: to kill a process with process number #, then state of the process # is "terminated".

MAYBE ? - 8. r #: resume process with process number #, which was suspended by Ctrl-C, the process # is put back into ready queue and enables the scheduler

MAYBE ??? - 9. r all: run all the processes after configurations (e.g., specifying the scheduling algorithms), which essentially enables your scheduler.

 

Programming Tips:

1. for round robin scheduling, please use alarm() system call to implement the time quantum.

2. Robust and/or secure programming is very important for a system, so always to check the return values of system calls, handle the errors and provide informative error messages (imaging the function of a syslog). For example in this task, when you are using fork(), please make sure to handle the possible error: 

                         pid_t process_pid = fork();

                         if (process_pid == -1) {

                                    //error handling

                         }

3. signal handlers: understand the meaning of each signal including SIGKILL, SIGTERM, SIGTSTP, SIGSTOP, SIGCONT, SIGUSR1, SIGUSR2, make sure to consider the context in which the handler is executed and keep it concise. 

                       #include <signal.h>

                       

                       void my_sig_handler(int signum) {

                       //signal handling...

                       }

                      //make sure to register the signal handler (Ctrl-C) in the program

                       signal(SIGINT, my_sig_handler);

4. [this part you may not need, but I keep it here.] parent-child process communication can be implemented by shared memory, pipes, and other IPCs. when there are communications between two processes, you need to be aware of synchronization and avoid race conditions (the connent in synchronization lecture); Here is an example of pipe based implementation.


                       int pipe_fd[2];
                       pipe(pipe_fd);

                       pid_t process_pid = fork();
                       if (process_pid == 0) {  // Child process
                           // Close unused read end

                           close(pipe_fd[0]);  
                           // Write data to the parent
                           write(pipe_fd[1], "message from child!", strlen("message from child!"));
                           //close write end

                           close(pipe_fd[1]); 
                           exit(EXIT_SUCCESS);
                       } else {  // Parent process

                           //close unused write end
                           close(pipe_fd[1]);  
                           char buffer[BUFFERLENGTH];
                           // Read data from the child
                           read(pipe_fd[0], buffer, sizeof(buffer));
                           printf("Parent received: %s\n", buffer);

                           //close read end
                           close(pipe_fd[0]);  
                       }*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <signal.h>
#include <pthread.h>
#include <sched.h>

#include <ctype.h>

#define MAX_CMD_LEN 256
#define MAX_NUM_PROCESSES 10

pid_t processes[MAX_NUM_PROCESSES];
int num_processes = 0;

// void create_processes(int n) {
//     // Create n processes
//     for (int i = 0; i < n; i++) {
//         pid_t pid = fork();
//         if (pid == 0) {
//             printf("Child process %d\n", getpid());
//             while (1) {
//                 sleep(1);
//             }
//         } else {
//             processes[num_processes++] = pid;
//         }
//     }
// }

void create_processes(int n) {
    // Create n processes
    for (int i = 0; i < n; i++) {
        pid_t pid = fork();
        if (pid == 0) {
            // Child process
            execl("./task", "task", (char *)NULL);
            perror("execl failed");  // execl only returns on error
            exit(1);
        } else if (pid < 0) {
            perror("fork failed");
        } else {
            // Parent process
            processes[num_processes++] = pid;
        }
    }
}


// helper function to get state of a process
char get_process_state(pid_t pid) {
    char state;
    char path[40], line[100], *p;
    FILE* statusf;

    sprintf(path, "/proc/%d/stat", pid);

    statusf = fopen(path, "r");
    if (!statusf)
        return 'X';  // return 'X' if the process doesn't exist.

    if (fgets(line, 100, statusf) == NULL) {
        fclose(statusf);
        return 'X';  // return 'X' if the process doesn't exist.
    }

    // The state is the third field in the file.
    sscanf(line, "%*d %*s %c", &state);

    fclose(statusf);
    return state;
}


void list_processes() {
    // List current processes
    for (int i = 0; i < num_processes; i++) {
        // list the current user processes in the system including it PID (underlying process ID from the 
        // host OS - Ubuntu), process number, and state.

        // printf("Process %d\n", processes[i]);
        // printf("PID: %d, Process number: %d, State: %s\n", processes[i], i, get_process_state(processes[i]) == 'T' ? "Stopped" : "Running");
        printf("PID: %d, Process number: %d, State: %c\n", processes[i], i, get_process_state(processes[i]));
    }
}

// void set_scheduler(char* algorithm) {
//     // Set scheduler algorithm
//     printf("Set scheduler to %s\n", algorithm);

//     int policy;
//     if (strcmp(algorithm, "SCHED_OTHER") == 0) {
//         policy = SCHED_OTHER;
//     } else if (strcmp(algorithm, "SCHED_FIFO") == 0) {
//         policy = SCHED_FIFO;
//     } else if (strcmp(algorithm, "SCHED_RR") == 0) {
//         policy = SCHED_RR;
//     } else {
//         fprintf(stderr, "Unknown scheduler algorithm: %s\n", algorithm);
//         return;
//     }

//     for (int i = 0; i < num_processes; i++) {
//         struct sched_param param;
//         param.sched_priority = 0;
//         if (sched_setscheduler(processes[i], policy, &param) < 0) {
//             perror("sched_setscheduler failed");
//         }
//     }

//     printf("Scheduler set to %s\n", algorithm);
// }


void set_scheduler(char* input) {
    char algorithm[10];
    int q;

    // Parse the input string
    if (sscanf(input, "%s %d", algorithm, &q) < 1) {
        printf("Error: Invalid command: %s\n", input);
        return;
    }

    int policy;
    if (strcmp(algorithm, "fifo") == 0) {
        policy = SCHED_FIFO;
    } else if (strcmp(algorithm, "rr") == 0) {
        policy = SCHED_RR;
    } else {
        fprintf(stderr, "Unknown scheduler algorithm: %s\n", algorithm);
        return;
    }

    for (int i = 0; i < num_processes; i++) {
        // Check if the process is still alive
        if (kill(processes[i], 0) == 0) {
            struct sched_param param;
            param.sched_priority = 1;
            if (policy == SCHED_RR) {
                param.sched_priority = q; // Set time quantum for round-robin scheduling
            }
            if (sched_setscheduler(processes[i], policy, &param) < 0) {
                perror("sched_setscheduler failed");
            }
        }
    }

    printf("Scheduler set to %s\n", algorithm);
}


void kill_process(int pid) {
    // Kill process with pid
    kill(pid, SIGKILL);

    // Wait for the process to terminate
    int status;
    waitpid(pid, &status, 0);
    
    if (WIFEXITED(status)) {
        printf("Process %d exited with status %d\n", pid, WEXITSTATUS(status));
    } else if (WIFSIGNALED(status)) {
        printf("Process %d killed by signal %d\n", pid, WTERMSIG(status));
    }

    // // Remove the process from the list
    // for (int i = 0; i < num_processes; i++) {
    //     if (processes[i] == pid) {
    //         for (int j = i; j < num_processes - 1; j++) {
    //             processes[j] = processes[j + 1];
    //         }
    //         num_processes--;
    //         break;
    //     }
    // }

    printf("Process %d killed\n", pid);
}

// void resume_process(int pid) {
//     // Resume process with pid
//     kill(pid, SIGCONT);
//     printf("Process %d resumed\n", pid);
// }

void resume_process(char* input) {
    if (isdigit(input[0])) {
        // input is a process number
        int process_number = atoi(input);
        if (process_number < num_processes) {
            // process exists
            kill(processes[process_number], SIGCONT);
        } else {
            // process does not exist
            printf("Error: No such process: %d\n", process_number);
        }
    } else if (strcmp(input, "all") == 0) {
        // input is "all"
        for (int i = 0; i < num_processes; i++) {
            kill(processes[i], SIGCONT);
        }
    } else {
        // invalid input
        printf("Error: Invalid command: r %s\n", input);
    }
}

int main() {
    // Make the shell ignore SIGINT (Ctrl-C will be ignored)
    signal(SIGINT, SIG_IGN);

    char cmd[MAX_CMD_LEN];
    while (1) {
        printf("shell 5500>>> ");
        fgets(cmd, MAX_CMD_LEN, stdin);
        cmd[strcspn(cmd, "\n")] = 0;  // Remove trailing newline

        switch (cmd[0]) {
            case 'x':
            {
                for (int i = 0; i < num_processes; i++)
                {
                    kill(processes[i], SIGTERM);
                    printf("Child process %d terminated\n", processes[i]);
                }
                printf("Parent process exiting\n");
                exit(0);
                break;
            }
            case 'c': {
                int n = atoi(cmd + 2);
                create_processes(n);
                break;
            }
            case 'l':
                list_processes();
                break;
            case 's':
                set_scheduler(cmd + 2);
                break;
            case 'k': {
                int pid = atoi(cmd + 2);
                kill_process(pid);
                break;
            }
            case 'r': {
                // int pid = atoi(cmd + 2);
                // resume_process(pid);
                resume_process(cmd + 2);
                break;
            }
            default:
                printf("Unknown command: %s\n", cmd);
                break;
        }
    }
    return 0;
}