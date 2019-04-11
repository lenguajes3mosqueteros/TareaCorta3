#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#define MY_ERROR 1
#define PORT 7777
#define MAX_NUM_CLIENTS 1 /* I don't know how to thread right now. */
#define MAX_CLIENT_MSG_LEN 1000
int main()
{
WSADATA wsa;
SOCKET mySocket, acceptSocket;
struct sockaddr_in server, client;
int sockAddrInLength = sizeof(struct sockaddr_in);
char clientMessage[MAX_CLIENT_MSG_LEN];
int clientMessageLength;
char* message;

int running = 1;

if(WSAStartup(MAKEWORD(2,2), &wsa) != 0)
{
    fprintf(stderr, "WSAStartup failed.\n");
    return MY_ERROR;
}
printf("WSAStartup succeded.\n");

mySocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
if (mySocket == INVALID_SOCKET)
{
    fprintf(stderr, "Socket creation failed.\n");
    return MY_ERROR;
}
printf("Socket creation succeeded.\n");

server.sin_addr.s_addr = INADDR_ANY;
server.sin_family = AF_INET;
server.sin_port = htons(PORT);

if (bind(mySocket, (struct sockaddr*) &server, sizeof server) == SOCKET_ERROR)
{
    fprintf(stderr, "Binding socket on port %d failed.\n", PORT);
    return MY_ERROR;
}
printf("Binding socket on port %d successfully.\n", PORT);

while (running)
{
        listen(mySocket, MAX_NUM_CLIENTS);
        printf("Waiting for a connection...\n");

        acceptSocket = accept(mySocket, (struct sockaddr*) &client, &sockAddrInLength);
        if (acceptSocket == INVALID_SOCKET)
        {
            fprintf(stderr, "Accept failed.\n");
            return MY_ERROR;
        }
        printf("Accept succeeded.\n");

        if ((clientMessageLength = recv(acceptSocket, clientMessage, sizeof clientMessage, 0)) == SOCKET_ERROR)
        {
            fprintf(stderr, "Recv failed.\n");
            return MY_ERROR;
        }
        printf("Recv succeeded.\n");

        printf("Data:\n");
        clientMessage[clientMessageLength] = NULL; /* Null terminator */
        printf("Client: %s\n", clientMessage);

        message = "Hello client, I'm the server. Bye bye. :-)\n";
        if (send(acceptSocket, message, strlen(message), 0) < 0)
        {
            fprintf(stderr, "Send failed.\n");
            return MY_ERROR;
        }
        printf("Send succeded.\n");
}
closesocket(mySocket);
WSACleanup();
getchar();
return 0;

}