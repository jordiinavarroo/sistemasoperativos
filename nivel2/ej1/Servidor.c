#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

#define KSIZE 10
#define KPORT 9999
#define KFILE "Google.html"

void start_server(int s){
    int var = 1;
    struct sockaddr_in server;

    bzero((char *) &server, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(KPORT);
    setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &var, sizeof(var));
    if(bind(s, (struct sockaddr *) &server, sizeof(server)) < 0){
        printf("Error no se ha podido vincular el socket\n");
        close(s);
    } else if(listen(s, 5) < 0) {
        printf("Error se ha excedido el numero de personas en la cola\n");
        close(s);
    }
}

void send_file(int s, int news){
    int file;
    char buffer[KSIZE];
    ssize_t in, ready, out;

    close(s);
    file = open(KFILE, O_RDONLY);
    if(file < 0){
        printf("Error no se ha podido abrir el fichero\n");
    } else{
		printf("Se ha abierto el archivo correctamente\n");
        in = read(file, buffer, KSIZE - 1);
        while(in > 0){
            printf("Se ha leido del fichero %zd\n", in);
            out = 0;
            while(out < in){
                ready = write(news, buffer + out, in - out);
                if(ready <= 0){
                    printf("Error al enviar el archivo al cliente\n");
                    break;
                }
                out += ready;
            }
            if(out < in) {
				break;
			}
            in = read(file, buffer, KSIZE - 1);
        }
        if(in < 0) {
			printf("Error al intentar abrir el fichero \n");
		}
		printf("Se ha leido del ultimo bloque %zd\n", in);
		close(file);
    }
    printf("Se ha leido completamente el archivo\n");
    close(news);
    fflush(stdout);
    _exit(0);
}

int main(){
    int s = socket(AF_INET, SOCK_STREAM, 0);
    int news;
    struct sockaddr_in client;
    socklen_t size;
    pid_t pid;

    if(s >= 0){
        start_server(s);
        signal(SIGCHLD, SIG_IGN);
        signal(SIGPIPE, SIG_IGN);
        while(1){
            size = sizeof(client);
            news = accept(s, (struct sockaddr *) &client, &size);
            if(news < 0)
                printf("Error al aceptar la conexion\n");
            else{
                fflush(stdout);
                pid = fork();
                if(pid == 0)
                    send_file(s, news);
                else{
                    if(pid < 0) {
                        printf("Error al crear el proceso hijo\n");
                    }
                    close(news);
                }
            }
        }
    } else {
        printf("Error al intentar abrir el socket\n");
    }
    return 0;
}
