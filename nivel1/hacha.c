#include <stdio.h> 
#include <sys/wait.h>
#include <sys/stat.h> 
#include <unistd.h> 
#include <stdlib.h> 
#include <string.h>
#include <fcntl.h>

void slice(char name[], int size) {
    int i, slices, slicesRead, fileIn, fileOut;
    char *sliceRead;      
    int tube[2];
    struct stat fileStats;
    char file[50]; 
        
    sliceRead = (char *) malloc(sizeof(char) * size);
    fileIn = open(name, O_RDONLY);
    
    if(fileIn == 0) {
        stat(name, &fileStats);
        slices = fileStats.st_size / size;
        
        if(fileStats.st_size % size != 0) {
            slices++;
        }
        
        for(i = 0; i < slices; i++) {
            pipe(tube);
            
            if(fork() != 0) {
                slicesRead = read(fileIn, sliceRead, size);
                write(tube[1], sliceRead, slicesRead);
            } else { 
                if(i < 10) {
                    sprintf(file, "%s.h0%d", name, i);
                } else {
                    sprintf(file, "%s.h%d", name, i);
                }
                
                fileOut = creat(file, 0666);   
                slicesRead = read(tube[0], sliceRead, size);         
                write(fileOut, sliceRead, slicesRead);
                
                close(fileOut);
                free(sliceRead);
                break;
            }
        }
        
        if(i == slices) {
            free(sliceRead);
            for(int i = 0; i < slices; i++) {
                wait(NULL);
            }
        }
    }
}

int main(int argc, char *argv[]) {
    if(argc == 3) {  
        slice(argv[1], atoi(argv[2]));
    }
    return 0;
}
