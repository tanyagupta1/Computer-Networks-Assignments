#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<errno.h>
#include "procs.c"
#define  PORT 8000
#define BUFFER_SIZE 5000

void main()
{
    int client_socket;
    struct sockaddr_in server_address;
    char buffer[BUFFER_SIZE];

    if((client_socket = socket(AF_INET,SOCK_STREAM,0))<0)
    {
        perror("Socket error:");
        exit(EXIT_FAILURE);
    }
    printf("Socket Created\n");
    fflush(stdout);
    memset(&server_address,'\0',sizeof(server_address));
    server_address.sin_family=AF_INET;
    server_address.sin_port=htons(PORT);
    server_address.sin_addr.s_addr=inet_addr("127.0.0.1");

    if(connect(client_socket,(struct sockaddr*)&server_address,sizeof(server_address))<0)
    {
        perror("Connect:");
        exit(EXIT_FAILURE);
    }
    printf("Connected Successfully\n");
    fflush(stdout);
    while(1)
    {
        memset(buffer,'\0',BUFFER_SIZE);
        printf("Message to send:");
        scanf("%s",&buffer[0]);
        send(client_socket,buffer,strlen(buffer),0);
        if(strcmp(buffer,"exit")==0)
        {
            printf("Closing connection with server!\n");
            fflush(stdout);
            close(client_socket);
            exit(EXIT_SUCCESS);
        }
        write_file(client_socket,atoi(buffer),"client_received");
        store_n_procs_in_file(1,"for_server");
        FILE* fp5=fopen("for_server","r");
        send_file(fp5,client_socket);
    }
}