#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

int g_seconds;
pid_t pid_arb, pid_a, pid_b, pid_x, pid_y, pid_z;

void parse_args(int argc, char *argv[]) {
    if (argc != 2) {
        exit(EXIT_FAILURE);
    }
    g_seconds = atoi(argv[1]);
    if (g_seconds <= 0) {
        exit(EXIT_FAILURE);
    }
}

void handler_X_destroy_leaf(int s) {
    printf("Soy X (%d) y muero\n", getpid());
    exit(0);
}

void run_process_X() {
    pid_x = getpid();
    printf("Soy el proceso X: mi pid es %d. Mi padre es %d. Mi abuelo es %d. "
           "Mi bisabuelo es %d\n", pid_x, pid_b, pid_a, pid_arb);
    signal(SIGUSR2, handler_X_destroy_leaf);
    pause();
}

void create_X_process() {
    switch (pid_x = fork()) {
        case -1:
            perror("fork X");
            exit(EXIT_FAILURE);
        case 0:
            run_process_X();
            exit(0);
        default:
            break;
    }
}

void handler_Y_destroy_leaf(int s) {
    printf("Soy Y (%d) y muero\n", getpid());
    exit(0);
}

void run_process_Y() {
    pid_y = getpid();
    printf("Soy el proceso Y: mi pid es %d. Mi padre es %d. Mi abuelo es %d. "
           "Mi bisabuelo es %d\n", pid_y, pid_b, pid_a, pid_arb);
    signal(SIGUSR2, handler_Y_destroy_leaf);
    pause();
}

void create_Y_process() {
    switch (pid_y = fork()) {
        case -1:
            perror("fork Y");
            exit(EXIT_FAILURE);
        case 0:
            run_process_Y();
            exit(0);
        default:
            break;
    }
}

void handler_Z_alarm(int s) {
    kill(pid_a, SIGUSR1);
}

void handler_Z_destroy_leaf(int s) {
    printf("Soy Z (%d) y muero\n", getpid());
    exit(0);
}

void run_process_Z() {
    pid_z = getpid();
    printf("Soy el proceso Z: mi pid es %d. Mi padre es %d. Mi abuelo es %d. Mi bisabuelo es %d\n", pid_z, pid_b, pid_a, pid_arb);
    signal(SIGALRM, handler_Z_alarm);
    signal(SIGUSR2, handler_Z_destroy_leaf);
    alarm(g_seconds);
    while(1){
        pause();
    }
}

void create_Z_process() {
    switch (pid_z = fork()) {
        case -1:
            perror("fork Z");
            exit(EXIT_FAILURE);
        case 0:
            run_process_Z();
            exit(0);
        default:
            break;
    }
}

void handler_B_destroy_and_propagate(int s) {
    kill(pid_z, SIGUSR2);
    wait(NULL);
    kill(pid_y, SIGUSR2);
    wait(NULL);
    kill(pid_x, SIGUSR2);
    wait(NULL);
}

void run_process_B() {
    pid_b = getpid();
    printf("Soy el proceso B: mi pid es %d. Mi padre es %d. Mi abuelo es %d\n",
           pid_b, pid_a, pid_arb);
    signal(SIGUSR1, SIG_DFL);
    signal(SIGUSR2, handler_B_destroy_and_propagate);

    create_X_process();
    usleep(50000);
    create_Y_process();
    usleep(50000);
    create_Z_process();

    pause();
    printf("Soy B (%d) y muero\n", pid_b);
}

void create_B_process() {
    switch (pid_b = fork()) {
        case -1:
            perror("fork B");
            exit(EXIT_FAILURE);
        case 0:
            run_process_B();
            exit(0);
        default:
            wait(NULL);
    }
}

void handler_A_exec_task(int s) {
	char arb_cad[100];
	sprintf(arb_cad, "%d", pid_arb);
	if(fork() == 0){
		execlp("pstree", "pstree", "-c", arb_cad, NULL);
	}
	else{
		wait(NULL);
		kill(pid_arb, SIGUSR2);
		pause();	
	}
}

void handler_A_destroy_and_propagate(int s) {
    kill(pid_b, SIGUSR2);
}

void run_process_A() {
    pid_a = getpid();
    printf("Soy el proceso A: mi pid es %d. Mi padre es %d\n", pid_a, pid_arb);
    signal(SIGUSR1, handler_A_exec_task);
    signal(SIGUSR2, handler_A_destroy_and_propagate);
    create_B_process();
    printf("Soy A (%d) y muero\n", pid_a);
}

void create_A_process() {
    switch (pid_a = fork()) {
        case -1:
            perror("fork A");
            exit(EXIT_FAILURE);
        case 0:
            run_process_A();
            exit(0);
        default:
            wait(NULL);
    }
}

void handler_start_destruction(int s) {
    kill(pid_a, SIGUSR2);
}

int main(int argc, char *argv[]) {
    parse_args(argc, argv);
    pid_arb = getpid();
    printf("Soy el proceso ejec: mi pid es %d\n", pid_arb);

    signal(SIGUSR2, handler_start_destruction);
    create_A_process();

    printf("Soy ejec (%d) y muero\n", pid_arb);
    return 0;
}
