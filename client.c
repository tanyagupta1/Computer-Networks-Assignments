#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<errno.h>
#define  PORT 4455
void main()
{
    int client_socket,ret;
    struct sockaddr_in server_address;
    char buffer[1024];

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

    if((ret = connect(client_socket,(struct sockaddr*)&server_address,sizeof(server_address)))<0)
    {
        perror("Connect:");
        exit(EXIT_FAILURE);
    }
    printf("Connected Successfully\n");
    fflush(stdout);
    while(1)
    {
        memset(buffer,'\0',1024);
        printf("Message to send:");
        scanf("%s",&buffer[0]);
        send(client_socket,buffer,strlen(buffer),0);
        if(strcmp(buffer,"exit")==0)
        {
            printf("Adios!\n");
            fflush(stdout);
            exit(EXIT_SUCCESS);
        }
        if(recv(client_socket,buffer,1024,0)<0)
        {
            perror("Receive error:");
        }
        
        else printf("Server sent: %s\n",buffer);
    }
}