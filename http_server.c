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

    int bytes = read(NewSocket, buffer, sizeof(buffer) - 1);
    if (bytes <= 0)
    {
        if (bytes < 0) perror("read error");
        else printf("client disconnected\n");
        close(NewSocket);
        return NULL;
    }

    // make end in string request of client
    buffer[bytes] = '\0';

    char method[8], path[256];
    sscanf(buffer, "%7s %255s", method, path);

    char file_to_send[512];
    if (strcmp(path, "/") == 0)
    {
        // If the root is requested, serve the main HTML file
        snprintf(file_to_send, sizeof(file_to_send), "server_content/server_content.html");
    }
    else
    {
        // For any other request, construct the path relative to the server_content directory
        snprintf(file_to_send, sizeof(file_to_send), "server_content%s", path);
    }

    // Determine content type from file extension
    char *ext = strrchr(file_to_send, '.');
    char *content_type = "text/plain"; 
    if (ext != NULL)
    {
        if (strcmp(ext, ".html") == 0) content_type = "text/html";
        else if (strcmp(ext, ".css") == 0) content_type = "text/css";
        else if (strcmp(ext, ".js") == 0) content_type = "application/javascript";
    }

    FILE *fp = fopen(file_to_send, "rb");
    if (!fp)
    {
        // File not found, send 404 response
        const char *not_found_response = "HTTP/1.1 404 Not Found\r\nContent-Length: 9\r\n\r\nNot Found";
        send(NewSocket, not_found_response, strlen(not_found_response), 0);
        fprintf(stderr, "File not found: %s\n", file_to_send);
    }
    else
    {
        // read the file size and generate the response
        fseek(fp, 0, SEEK_END);
        long filesize = ftell(fp);
        fseek(fp, 0, SEEK_SET);

        char *file_content = (char *)malloc(filesize);
        if (!file_content)
        {
            perror("Failed to allocate memory for file content");
            fclose(fp);
            close(NewSocket);
            return NULL;
        }

        fread(file_content, 1, filesize, fp);
        fclose(fp);

        char headers[512];
        int header_len = snprintf(headers, sizeof(headers),
                                  "HTTP/1.1 200 OK\r\n"
                                  "Content-Type: %s\r\n"
                                  "Content-Length: %ld\r\n"
                                  "Connection: close\r\n"
                                  "\r\n",
                                  content_type, filesize);

        // Send headers and content
        send(NewSocket, headers, header_len, 0);
        send(NewSocket, file_content, filesize, 0);

        free(file_content);
    }

    // Close the socket for this client
    close(NewSocket);
    return NULL;
}
