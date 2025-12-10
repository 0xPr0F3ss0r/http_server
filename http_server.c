#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>

// define socket
int socketSP, newSocket;
// define structure for our socket connection
struct sockaddr_in serve_add;
struct sockaddr_in client_add;

// data buffer
char buffer[1024] = {0};


socklen_t addr_size = sizeof(client_add);

int main(int argc, char *argv[])
{
    socketSP = socket(AF_INET, SOCK_STREAM, 0);
    if (socketSP != -1)
    {
        // zero structure out ss
        bzero(&serve_add, sizeof(serve_add));
        bzero(&client_add, sizeof(client_add));

        // match the socket() call with IPV4
        serve_add.sin_family = AF_INET;
        // specifique port to listen on
        serve_add.sin_port = htons(5100);
        // allow the server to accept connetion of client in any interface
        serve_add.sin_addr.s_addr = htonl(INADDR_ANY);

        if (bind(socketSP, (struct sockaddr *)&serve_add, sizeof(serve_add)) < 0)
        {
            printf("Error in Binding</spane>n");
            exit(1);
        };
        // listen to incoming connection
        int Listen = listen(socketSP, 5);

        if (Listen == -1)
        {
            printf("Error on listning incoming connection.. !");
        }

        // accept to incomming connections from clients
        newSocket = accept(socketSP, (struct sockaddr *)&client_add, &addr_size);
        if (newSocket < 0)
        {
            printf("error in accept ..!");
            return 1;
        };

        // start sending and receive data

        int bytes = read(newSocket, buffer, sizeof(buffer));
        if (bytes < 0)
        {
            perror("read error");
        }
        else if (bytes == 0)
        {
            printf("client disconnected\n");
        }

        // print our data
        printf("data is: %s\n", buffer);
    }
    else
    {
        printf("erro , socket not setup ");
        return 1;
    }

    // close the socket
    close(socketSP);
    return 0;
}