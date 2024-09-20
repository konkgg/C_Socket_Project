/*
This code implements a TCP client that connects to a server and sends messages.
The process and logic are as follows:

1. Parse command-line arguments for server IP and port.
2. Create a TCP socket.
3. Set up the server address structure.
4. Resolve hostname to IP address if needed.
5. Connect to the server.
6. Enter a loop to:
   a. Read user input.
   b. Send the input to the server.
   c. Receive and display the server's response.
   d. Exit loop if user enters "fin".
7. Close the socket and exit.

The code handles errors using a DieWithError function and supports both
IP addresses and hostnames for the server address.
*/
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>

#define RCVBUFSIZE 32

void DieWithError(char *errorMessage);

int main(int argc, char *argv[])
{
    int sock;
    struct sockaddr_in echoServAddr;
    char *servIP;
    int echoServPort;
    char echoString[RCVBUFSIZE];
    char echoBuffer[RCVBUFSIZE];
    unsigned int echoStringLen;
    int bytesRcvd, totalBytesRcvd;
    int done = 0;

    if (argc != 3)
    {
       fprintf(stderr, "Usage: %s <Server IP> <Port Number>\n", argv[0]);
       exit(1);
    }

    servIP = argv[1];
    echoServPort = atoi(argv[2]);

    if ((sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
        DieWithError("socket() failed");

    memset(&echoServAddr, 0, sizeof(echoServAddr));
    echoServAddr.sin_family = AF_INET;

    if(argv[1][0] >= '0' && argv[1][0] <= '9')
    {
        echoServAddr.sin_addr.s_addr = inet_addr(servIP);
    }
    else
    {
     struct hostent *hname;
 if((hname=gethostbyname(argv[1])) == NULL)
 {
    printf("gethostbyname failed");
    exit(1);
 }
 echoServAddr.sin_addr = *((struct in_addr*)hname->h_addr);
    }
    echoServAddr.sin_port     = htons(echoServPort);

    if (connect(sock, (struct sockaddr*) &echoServAddr, sizeof(echoServAddr)) < 0)
        DieWithError("connect() failed");


    while (done == 0)
    {
     scanf("%s", echoString);
     echoStringLen = strlen(echoString);

        if(strncmp(echoString, "fin", 3) == 0)
                {
                     done = 1;
                }
     if (send(sock, echoString, echoStringLen, 0) != echoStringLen)
         DieWithError("send() sent a different number of bytes than expected");

     totalBytesRcvd = 0;
     printf("Received: ");
     while (totalBytesRcvd < echoStringLen)
     {
         if ((bytesRcvd = recv(sock, echoBuffer, RCVBUFSIZE - 1, 0)) <= 0)
              DieWithError("recv() failed or connection closed prematurely");
         totalBytesRcvd += bytesRcvd;
         echoBuffer[bytesRcvd] = '\0';
         printf(echoBuffer);
     }

     printf("\n");

    }
    close(sock);
    exit(0);
}

void DieWithError(char *errorMessage)
{
    perror(errorMessage);
    exit(1);
}
