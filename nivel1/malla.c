#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

int g_x, g_y;

void parse_args(int argc, char *argv[]) {
    if (argc != 3) {
        exit(EXIT_FAILURE);
    }
    g_x = atoi(argv[1]);
    g_y = atoi(argv[2]);
    if (g_x <= 0 || g_y <= 0) {
        exit(EXIT_FAILURE);
    }
}

void handler_leaf_alarm(int s) {
}

pid_t create_process() {
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    return pid;
}

void close_process() {
    wait(NULL);
    exit(0);
}

void run_leaf_process() {
    signal(SIGALRM, handler_leaf_alarm);
    alarm(10);
    pause();
}

int main(int argc, char *argv[]) {
    pid_t pid;
    int i, j;

    parse_args(argc, argv);

    for (i = 1; i <= g_y; i++) {
        pid = create_process();
        if (pid == 0) {
            for (j = 1; j <= g_x - 1; j++) {
                pid = create_process();
                if (pid != 0) {
                    close_process();
                }
            }
            if (j == g_x) {
                run_leaf_process();
                close_process();
            }
        }
    }
    if (i == g_y + 1) {
        for (i = 1; i <= g_y; i++) {
            wait(NULL);
        }
    }
    return 0;
}
