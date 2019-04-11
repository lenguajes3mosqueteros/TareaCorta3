/********************
 * Exemplo básico de uso de sockets no Linux.
 * Por Luiz Felipe - https://github.com/Silva97
 *
 * Link do vídeo: https://youtu.be/GaxjJvMnz-I
 ********************/

#include <stdio.h>
#include <winsock2.h>

int main(){
    char buff[129];
    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 0), &wsa);

    struct sockaddr_in caddr;
    struct sockaddr_in saddr = {
        .sin_family      = AF_INET,
        .sin_addr.s_addr = htonl(INADDR_ANY),
        .sin_port        = htons(5000)
    };

    int server = socket(AF_INET, SOCK_STREAM, 0);
    int client, x;
    int csize  = sizeof caddr;

    bind(server, (struct sockaddr *) &saddr, sizeof saddr);
    listen(server, 5);

    while(1){
        client = accept(server, (struct sockaddr *) &caddr, &csize);
        x = recv(client, buff, sizeof buff, 0);

        send(client, buff, x, 0);

        puts(buff);
        fflush(stdout);

        closesocket(client);
    }

    return 0;
}