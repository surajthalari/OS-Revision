#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#define MAX 50
#define PORT 44444

void server(int sockId){
    char buff[MAX];
    while(1){
        bzero(buff, MAX);
        //read message from client
        read(sockId, buff, sizeof(buff));
        printf("Client data : %s\n", buff);
        if (strncmp("end",buff,3) == 0)
        {
            printf("Server Exit.\n");
            break;
        }
        int n = atoi(buff);
        sprintf(buff, "%ld", 2*n);
        write(sockId,buff,sizeof(buff));
    }
}

int main()
{
    int sockId, connId, len;
    //create socket
    sockId = socket(AF_INET, SOCK_STREAM, 0);
    if (sockId == -1)
    {
        printf("Socket creation failed.\n");
        exit(0);
    }
    //initialize address
    struct sockaddr_in server_addr, cli;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port =  htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    //bind socket with the address
    if (bind(sockId, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1)
    {
        printf("Failed to bind with the socket.\n");
        exit(0);
    }
    //listen verification
    if(listen(sockId, 5)!=0){
        printf("Listen verification failed.\n");
        exit(0);
    }
    else{
        printf("Server listening....\n");
    }
    len = sizeof(cli);
    //accept data packets
    connId = accept(sockId, (struct socketaddr*)&cli, &len);
    if (connId < 0)
    {
        printf("Server accept failed.\n");
    }
    server(connId);
    close(sockId);
}
