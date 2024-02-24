#include <stdio.h>
#include <unistd.h>
#include <signal.h>

volatile sig_atomic_t print_flag = 0;

void sig_handler(int signum) {
    if (signum == SIGINT || signum == SIGTSTP) {
        print_flag = 0;
    } else if (signum == SIGCONT) {
        print_flag = 1;
    }
}

int main(int argc, char* argv[]) {
    signal(SIGINT, sig_handler);
    signal(SIGTSTP, sig_handler);
    signal(SIGCONT, sig_handler);

    int count = 0;
    // pid_t mypid = getpid();
    int mypid = getpid();

    while(1) {
        if (print_flag) {
            printf("Process %d at iteration %d\n", mypid, count++);
            sleep(1);
        }
    }

    return 0;
}