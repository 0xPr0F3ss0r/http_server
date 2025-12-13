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
        serve_add.sin_port = htons(8080);
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

   //make end in  request of client
    buffer[bytes] = '\0';

    // read server content file
    FILE *file_ptr;
    file_ptr = fopen("server_content.html", "r");

    // check file status
    if (file_ptr == NULL)
    {
        printf("Error Occurred While creating a "
               "file !");
        exit(1);
    }
    // // get the size of file that I want to read

    // Move to end of file
    fseek(file_ptr, 0, SEEK_END);

    // Get current position (number of bytes from start)
    long filesize = ftell(file_ptr);

    // Move back to start to read file
    fseek(file_ptr, 0, SEEK_SET);
     
    //reserve space for the content of file based on the returned length
    //char file_content[filesize];
    
    //response contain the server content and the header so we add extra size which is 512
    char response[filesize+512];
    
    //size of file
    char file_content[filesize];
    
    // generate response and store it
    size_t f_size =  fread(file_content,1,filesize, file_ptr);
    if(f_size<filesize){
        fprintf(stderr, "File read incomplete\n");
        close(socketSP);
    }

   
    //make end to avoid buffer overflow
    file_content[f_size] = '\0';
    int read = snprintf(response, sizeof(response),
             "HTTP/1.1 200 OK\r\n"
             "Content-Type: text/html\r\n"
             "Content-Length: %zu\r\n"
             "\r\n"
             "%s",
             f_size,
            file_content);

  if (read < 0){
    printf("wrong when read\n");
  }
    //send data to client
   int number_sent =  send(NewSocket, response,strlen(response), 0);

     //close the file opened and the socket of client
    fclose(file_ptr);
    close(NewSocket);
    return NULL;
}
