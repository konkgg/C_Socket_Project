#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAXPENDING 5
#define RCVBUFSIZE 32
#define PORT 8650

void DieWithError(char *errorMessage);
void HandleTCPClient(int clntSocket);

int main()
{

    int servSock;
    int clntSock;
    int port;
    struct sockaddr_in echoServAddr;
    struct sockaddr_in echoClntAddr;
    unsigned int clntLen;

    if ((servSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
        DieWithError("socket() failed");

    memset(&echoServAddr, 0, sizeof(echoServAddr));
    echoServAddr.sin_family = AF_INET;
    echoServAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    echoServAddr.sin_port = htons(PORT);

    if (bind(servSock, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) < 0)
        DieWithError("bind() failed1");

    if (listen(servSock, MAXPENDING) < 0)
        DieWithError("listen() failed");

    for (;;)
    {
     printf("Waiting for Client to connect....\n");
        clntLen = sizeof(echoClntAddr);

        if ((clntSock = accept(servSock, (struct sockaddr *) &echoClntAddr, &clntLen)) < 0)
            DieWithError("accept() failed");

        printf("Handling client %s\n", inet_ntoa(echoClntAddr.sin_addr));

        HandleTCPClient(clntSock);
 exit(1);
    }
}


void DieWithError(char * errorMessage)
{
  perror(errorMessage);
  exit(1);
}


void HandleTCPClient(int clntSocket)
{
    int done = 0;
    char echoBuffer[RCVBUFSIZE];
    int recvMsgSize;

    if ((recvMsgSize = recv(clntSocket, echoBuffer, RCVBUFSIZE, 0)) < 0)
        DieWithError("recv() failed1");

    while (recvMsgSize > 0 && done==0)
    {
        char returnBuffer[recvMsgSize];
        int j=recvMsgSize;
        int i=0;

        for(i = 0; i < recvMsgSize; i++, j--)
                {

                        returnBuffer[i] = echoBuffer[j-1];

                }

        if(strncmp(echoBuffer, "fin", 3) == 0)
                {
                    send(clntSocket, "nif", 3, 0);
                    printf("Closing Connection \n");
                    done = 1;
                }
        else
     {
         if (send(clntSocket, returnBuffer, recvMsgSize, 0) != recvMsgSize)
                DieWithError("send() failed");

         if ((recvMsgSize = recv(clntSocket, echoBuffer, RCVBUFSIZE, 0)) < 0)
                DieWithError("recv() failed2");
     }
    }

    close(clntSocket);
    exit(1);
}
