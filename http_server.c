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

// define socket
int socketSP, newSocket;

//define some functions
void MakeNewConnectionThread(int newSocket);
void* HandleData(void* newSocket);
// define structure for our socket connection
struct sockaddr_in serve_add;
struct sockaddr_in client_add;

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
            perror("Error in Binding\n");
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

        // create new thread to handle new connetion client
        MakeNewConnectionThread(newSocket);
    }
    else
    {
        printf("erro , socket not setup ");
        return 1;
    }

   
    return 0;
}

void MakeNewConnectionThread(int newSocket)
{
    printf("start make new connection thread \n");
    pthread_t thread1;

    int thread = pthread_create(&thread1, NULL, HandleData, &newSocket);
    if(thread ==0){
        printf("thread joined ..\n");
    }
    pthread_join(thread1, NULL);
    // close the socket
    close(newSocket);
}

void* HandleData(void* newSocket)
{ 
    int NewSocket = *(int*)(newSocket);
    // data buffer
    char buffer[1024] = {0};
    
    int bytes = read(NewSocket, buffer, sizeof(buffer));
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

     
    return NULL;
}