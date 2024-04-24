#define _GNU_SOURCE
#include <stdio.h>                     /* for printf() and fprintf() */
#include <sys/socket.h>                /* for socket(), connect(), send(), and recv() */
#include <arpa/inet.h>                 /* for sockaddr_in and inet_addr() */
#include <stdlib.h>                    /* for atoi() and exit() */
#include <string.h>                    /* for memset() */
#include <unistd.h>                    /* for close() */
#include <time.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/time.h>
#include <signal.h>

#define MAXPENDING 5 /* Maximum outstanding connection requests */
#define BUFFER_SIZE 1024

void DieWithError(char *errorMessage); /* Error handling function */
void HandleTCPClient(int clntSocket);  /* TCP client handling function */

int serverSock; /*Socket descriptor for server */

void terminationSignal(int signum)
{
    printf("Termination signal received. Server shutting down...\n");
    close(serverSock);
    exit(signum);
}

int main(int argc, char *argv[])
{
    if (argc != 2) /* Test for correct number of arguments */
    {
        fprintf(stderr, "Usage: %s <Server Port>\n", argv[0]);
        exit(1);
    }

    signal(SIGTERM, terminationSignal);

    int servSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (servSock < 0)
    {
        DieWithError("socket() failed");
    }

    serverSock = servSock;

    struct sockaddr_in echoServAddr;             /* Local address */
    unsigned short echoServPort = atoi(argv[1]); /* Server port */

    /* Create socket for incoming connections */
    if ((serverSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
        DieWithError("socket() failed");

    /* Construct local address structure */
    memset(&echoServAddr, 0, sizeof(echoServAddr));   /* Zero out structure */
    echoServAddr.sin_family = AF_INET;                /* Internet address family */
    echoServAddr.sin_addr.s_addr = htonl(INADDR_ANY); /* Any incoming interface */
    echoServAddr.sin_port = htons(echoServPort);      /* Local port */

    /* Bind to the local address */
    if (bind(servSock, (struct sockaddr *)&echoServAddr, sizeof(echoServAddr)) < 0)
    {
        DieWithError("bind() failed");
    }

    /* Mark the socket so it will listen for incoming connections */
    if (listen(servSock, MAXPENDING) < 0)
    {
        DieWithError("listen() failed");
    }

    for (;;) /* Run forever */
    {
        struct sockaddr_in echoClntAddr;
        unsigned int clntLen = sizeof(echoClntAddr);
        int clntSock = accept(servSock, (struct sockaddr *)&echoClntAddr, &clntLen);
        if (clntSock < 0)
        {
            perror("accept() failed");
            continue;
        }

        printf("Handling client: %s\n", inet_ntoa(echoClntAddr.sin_addr));
        HandleTCPClient(clntSock);
        close(clntSock);
    }
}
void DieWithError(char *errorMessage)
{
    perror(errorMessage);
    exit(1);
}
void HandleTCPClient(int clntSocket) /* TCP client handling function */
{
    char buf[BUFFER_SIZE];
    int bytesRead = recv(clntSocket, buf, BUFFER_SIZE - 1, 0);
    if (bytesRead <= 0)
    {
        if (bytesRead < 0)
        {
            perror("Client read failed");
        }
        close(clntSocket);
        return;
    }

    buf[bytesRead] = '\0';

    char *resourcePath = strstr(buf, "GET ");
    if (resourcePath != NULL)
    {
        resourcePath += 4;                        
        char *pathEnd = strchr(resourcePath, ' '); 
        if (pathEnd != NULL)
        {
            *pathEnd = '\0';
        }
    }
    else
    {
        resourcePath = "/";
    }

    // Check if it's an HTTP GET request
    if (strstr(buf, "GET ") != NULL)
    {
        
        char fullPath[BUFFER_SIZE];
        snprintf(fullPath, sizeof(fullPath), ".%s", resourcePath);

        // Open file
        FILE *file = fopen(fullPath, "r");
        if (file != NULL)
        {
            char* fileContent = NULL;
            size_t fileSize;
            ssize_t bytes_read = getdelim( &fileContent, &fileSize, '\0', file);
            if (bytes_read == -1) {
                //error reading file
                DieWithError("Error reading file");
            } else if (fileContent == NULL) {
                // memory allocation failed
                DieWithError("Memory allocation failed");
            } else {
                // data read successfully

                const char *contentType = "text/plain";
                const char *fileExtension = strrchr(resourcePath, '.');
                if (fileExtension != NULL)
                {
                    if (strcmp(fileExtension, ".html") == 0)
                    {
                        contentType = "text/html";
                    }
                    else if (strcmp(fileExtension, ".txt") == 0)
                    {
                        contentType = "text/plain";
                    }
                }
                

                char response[fileSize];
                snprintf(response, sizeof(response), "HTTP/1.1 200 OK\r\nContent-Type: %s\r\n\r\n%s\r\n", contentType, fileContent);

                // Send the response to the client
                int responseLength = strlen(response);
                //printf("Response length: %i\nFile Size: %li\nResponse: %s\n", responseLength, fileSize, response);
                int sentBytes = send(clntSocket, response, responseLength, 0);
                if (sentBytes < 0)
                {
                    perror("Client write failed");
                }

                // Clean up
                fclose(file);
                free(fileContent);
            }
        }
        else
        {
            // If the file is not found, send a 404 Not Found response
            const char *notFoundResponse = "HTTP/1.1 404 Not Found\r\nContent-Type: text/html\r\n\r\n<h1>404 Not Found</h1>\r\n";
            int notFoundResponseLength = strlen(notFoundResponse);
            int sentBytes = send(clntSocket, notFoundResponse, notFoundResponseLength, 0);
            if (sentBytes < 0)
            {
                perror("Client write failed");
            }
        }
    }
    else
    {
        const char *errorResponse = "HTTP/1.1 400 Bad Request\r\nContent-Type: text/html\r\n\r\n<h1> 400 Bad Request</h1>\r\n";
        int errorResponseLength = strlen(errorResponse);
        int sentBytes = send(clntSocket, errorResponse, errorResponseLength, 0);
        if (sentBytes < 0)
        {
            perror("Client write failed");
        }
    }
}
/* NOT REACHED */
