#define _POSIX_C_SOURCE 200112L

#include <stdio.h>                     /* for printf() and fprintf() */
#include <sys/socket.h>                /* for socket(), connect(), send(), and recv() */
#include <arpa/inet.h>                 /* for sockaddr_in and inet_addr() */
#include <stdlib.h>                    /* for atoi() and exit() */
#include <string.h>                    /* for memset() */
#include <unistd.h>                    /* for close() */
#include <time.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/time.h>
#include <netdb.h>

#define RCVBUFSIZE 1024                /* Size of receive buffer */
void DieWithError(char *errorMessage); /* Error handling function */

void httpGetRequest(char* server_url, char* html_path, const char* port, int rttFlag);

int main(int argc, char *argv[])
{

    if (argc < 3 || argc > 4) /* Test for correct number of arguments */
    {
        fprintf(stderr, "Usage: %s [-p] <Server URL or IP> <Port>\n", argv[0]);
        exit(1);
    }
    
    int rttFlag = 0;
    const char* url;
    char* server_url;
    char* html_path;
    const char* port;
   
    

    if(argc == 4){
        rttFlag = 1;
        url = argv[2];
        port = argv[3];
    }
    else if(argc == 3){
        url = argv[1];
        port = argv[2];
    }

    const char* ptr = strchr(url, '/');
    
    if (ptr != NULL){
        //len at / and after
        size_t sub_len = strlen(ptr);
        size_t pref_len = ptr - url;

        //allocate memory
        server_url = malloc(pref_len + 1);
        html_path = malloc(sub_len + 1);

        if (html_path == NULL || server_url == NULL){
            perror("malloc failed");
            exit(EXIT_FAILURE);
        }

        strncpy(server_url, url, pref_len);
        strcpy(html_path, ptr);
    }
    else {
        server_url = strdup(url);
        html_path = "/";
    }

    httpGetRequest(server_url, html_path, port, rttFlag);

    return 0;

}


void httpGetRequest(char* server_url, char* html_path, const char* port, int rttFlag){
    struct addrinfo hints, *res;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    //printf("Server url: %s\nPort: %s\nHTML: %s\n", server_url, port, html_path);
    int status = getaddrinfo(server_url, port, &hints, &res);
    if (status != 0){
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
        DieWithError("getaddrinfo() failed");
    }

    int clientSocket = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if(clientSocket < 0){
        DieWithError("socket() failed");
    }

    

    if(connect(clientSocket, res->ai_addr, res->ai_addrlen) < 0){
        DieWithError("connect() failed");
    }

    
    
    else {char httpRequest[RCVBUFSIZE];
    snprintf(httpRequest, sizeof(httpRequest), "GET %s HTTP/1.1\r\n", html_path);
    snprintf(httpRequest + strlen(httpRequest), sizeof(httpRequest) - strlen(httpRequest), "Host: %s\r\n", server_url);
    snprintf(httpRequest + strlen(httpRequest), sizeof(httpRequest) - strlen(httpRequest), "Connection: close\r\n\r\n");

    struct timeval start_time, end_time;
    if(rttFlag){
        gettimeofday(&start_time, NULL);
    }

    int sentBytes = send(clientSocket, httpRequest, strlen(httpRequest), 0);

    if(sentBytes < 0){
        DieWithError("send() failed");
    }

    
    
    while(1){
        char buffer[RCVBUFSIZE];
        int bytesRead = recv(clientSocket, buffer, RCVBUFSIZE-1, 0);
        if(bytesRead < 0){
            perror("recv() failed");
            exit(1);
        }
         else if(bytesRead == 0){   break; //data finished or erroraw
        }
        buffer[bytesRead] = '\0';

        printf("%s", buffer);
    }

    if(rttFlag){
        gettimeofday(&end_time, NULL);
        double rtt = (end_time.tv_sec - start_time.tv_sec) * 1000.0 + end_time.tv_usec / 1000.0 - start_time.tv_usec / 1000.0;
        printf("Round-Trip Time (RTT): %.6f ms\n", rtt / 100);
    }


    }

    close(clientSocket);
    freeaddrinfo(res);

}

void DieWithError(char *errorMessage)
{
    perror(errorMessage);
    exit(EXIT_FAILURE);
}

