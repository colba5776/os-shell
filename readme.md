# Mini OS / Shell

This project contains two main files: `main.c` and `task.c`.

## main.c

The `main.c` file is a simple shell program that allows you to create, list, schedule, resume, and kill processes. Here's how to use the commands:

- `c <n>`: Creates `n` number of new processes.
- `l`: Lists all the current processes along with their PID, process number, and state.
- `s <algorithm> <q>`: Sets the scheduling algorithm to either `fifo` or `rr`. If `rr` is chosen, `q` is used as the time quantum.
- `k <pid>`: Kills the process with the given PID.
- `r <input>`: Resumes the process with the given process number or resumes all processes if `input` is `all`.
- `x`: Terminates all child processes and exits the parent process.

## task.c

The `task.c` file contains a C program that responds to certain signals and prints a message at regular intervals.

The program sets up signal handlers for `SIGINT`, `SIGTSTP`, and `SIGCONT` signals. The signal handler function `sig_handler` changes the value of the `print_flag` variable based on the signal received:

- If the program receives a `SIGINT` or `SIGTSTP` signal (typically sent by the user pressing Ctrl+C or Ctrl+Z), it sets `print_flag` to 0, which stops the program from printing messages.
- If the program receives a `SIGCONT` signal (which is sent when a stopped process is continued), it sets `print_flag` to 1, which allows the program to start printing messages again.

The main loop of the program checks the value of `print_flag` in each iteration. If `print_flag` is 1, the program prints a message containing the process ID and the current iteration count, then sleeps for 1 second. This loop continues indefinitely until the program is terminated.


## Compilation

To compile the project, use the following commands:

```bash
gcc main.c -o main -pthread
gcc task.c -o task