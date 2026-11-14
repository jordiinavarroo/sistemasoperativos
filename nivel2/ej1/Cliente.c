#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <netdb.h> 
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>

#define KSIZE 20
#define KPORT 9999

char g_ip[20];
int g_s;

bool parse_args(int argc, char *argv[]){
    bool result = true;
	int s = socket(AF_INET, SOCK_STREAM, 0);

    if(argc != 2){
        result = false;
        printf("Error al escribir el comando, el uso correcto es %s (IP)\n", argv[0]);
    } else if (s < 0) {
        result = false;
        printf("Error al crear el socket \n");
    }
    strcpy(g_ip, argv[1]);
	g_s = s;

    return result;
}

bool server_connect(struct sockaddr_in address){
    bool result = true;

	bzero((char *) &address, sizeof(address));
	address.sin_port = htons(KPORT);
    address.sin_addr.s_addr = inet_addr(g_ip);
	address.sin_family = AF_INET;
	if (connect(g_s ,(struct sockaddr *) &address,sizeof(address)) < 0) {
	    printf("Error al intentar conectarse al socket \n");
        result = false;
	}

    return result;
}

int main(int argc, char *argv[]) {
    char buffer[KSIZE];
    struct sockaddr_in address;
    ssize_t size;

    if (parse_args(argc, argv) && server_connect(address)) {
		do{
			size = read(g_s, buffer, KSIZE - 1);
			if(size > 0){
				buffer[size] = '\0';
				printf("%s", buffer);
			}
		} while (size > 0);
			
        if(size < 0) {
            perror("Error al intentar leer el socket \n");
        }

        close(g_s);
    }
    
    return 0;
}
