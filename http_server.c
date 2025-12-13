#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdint.h>
#include <ctype.h>
#include <ctype.h>
// define socket
int socketSP, newSocket;

// define some functions
void *MakeNewConnectionThread(void *newSocket);
// define structure for our socket connection
struct sockaddr_in serve_add;
struct sockaddr_in client_add;

socklen_t addr_size = sizeof(client_add);

int main(int argc, char *argv[])
{
    socketSP = socket(AF_INET, SOCK_STREAM, 0);
    if (socketSP != -1)
    {
        // zero structure out
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
            perror("Error in Binding\n");
            exit(1);
        };
        // listen to incoming connection
        int Listen = listen(socketSP, 5);

        if (Listen == -1)
        {
            printf("Error on listning incoming connection.. !");
        }

        while (1)
        {

            //  accept to incomming connections from clients
            int newSocket = accept(socketSP, (struct sockaddr *)&client_add, &addr_size);

            if (newSocket < 0)
            {
                printf("error in accept ..!");
                return 1;
            };

            // create new thread and reserve memory for it to handle new connetion client
            pthread_t thread1;
            int *client = (int *)malloc(sizeof(int));
            if (!client)
            {
                perror("address not reserved correctly");

                close(newSocket);
                continue;
            }
            *client = newSocket;
            int thread = pthread_create(&thread1, NULL, MakeNewConnectionThread, client);
        }
    }
    else
    {
        printf("erro , socket not setup ");
        return 1;
    }

    return 0;
}

void *MakeNewConnectionThread(void *newSocket)
{
    int NewSocket = *(int *)(newSocket);
    free(newSocket);
    printf("start make new connection thread \n");

    // data buffer
    char buffer[1024];

    int bytes = read(NewSocket, buffer, sizeof(buffer));
    if (bytes < 0)
    {
        perror("read error");
    }
    else if (bytes == 0)
    {
        printf("client disconnected\n");
    }
    char html[] =
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/html\r\n"
        "Content-Length: 98\r\n"
        "\r\n"
        "<!DOCTYPE html>"
        "<html lang=\"en\">"
        "<head><meta charset=\"UTF-8\"><title>Simple Server Page</title></head>"
        "<body><h1>Welcome to my server!</h1><p>This is a very simple HTML page.</p></body>"
        "</html>";

    buffer[bytes] = '\0';
    char text[] = "hello";
    printf("this the buffer %s", buffer);
    strtok(buffer, "\r\n");
    if (strcmp(buffer, "website") != NULL)
    {
        send(NewSocket, html, sizeof(html), 0);
    }
    else
    {
        printf("not \n");
    }
    printf("data %s\n", buffer);
    close(NewSocket);
    return NULL;
}
