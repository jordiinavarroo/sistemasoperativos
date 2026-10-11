#include <stdio.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>

void end(){};

int main(int argc, char *argv[]){
    if(argc == 3){
        int i;
        pid_t pid;
        pid_t pid_desc = getpid();
        int x = atoi(argv[1]);
        int y = atoi(argv[2]);
        int shmx = shmget(IPC_PRIVATE, sizeof(int) * x, IPC_CREAT | 0666);
        int shmy = shmget(IPC_PRIVATE, sizeof(int) * y, IPC_CREAT | 0666);
        int *px = (int *) shmat (shmx, 0, 0);
        int *py = (int *) shmat (shmy, 0, 0);

        for(i = 1; i <= x; i++){
            pid = fork();
            if(pid != 0){
                wait(NULL);
                break;
            }
            else{
                px[i - 1] = getpid();
                printf("Soy el proceso %d. Mis padres son: ", getpid());
                printf("%d", pid_desc);
                for(int j = 0; j < i - 1; j++){
                    printf(", %d", px[j]);
                }
                printf("\n");   
            }
        }
        if(i == 1){
            printf("Soy el super padre %d, mis hijos finales son: ", getpid());
            for(i = 0; i < y; i++){
                printf("%d" ,py[i]);
                if(i != y - 1){
                    printf(", ");
                }
            }
            printf("\n");
            shmdt((char *) px);
            shmdt((char *) py);
            shmctl(shmx, IPC_RMID, 0);
            shmctl(shmy, IPC_RMID, 0);
        }
        else{
            if(i == x + 1){
                for(i = 1; i <= y; i++){
                    pid = fork();
                    if(pid == 0){
                        py[i-1] = getpid();
                        signal(SIGALRM, end);
                        alarm(10);
                        pause();
                        break;
                    }
                }
                if(i == y + 1){
                    for(i = 1; i <= y; i++){
                        wait(NULL);
                    }
                }
            }
        }
    };
    return 0;
}
