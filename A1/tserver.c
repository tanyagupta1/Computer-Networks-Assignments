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
#include<pthread.h>
#define  PORT 8000

struct socket_info
{
    int socket_fd;
    int port_no;
};
void * connection_hander(void * client)
{
    char buffer [BUFFER_SIZE];
    struct socket_info * info = client;
    int new_socket =info->socket_fd; 
    int sin_port= info->port_no;
    // while(1)
    //         {
                memset(buffer,'\0',BUFFER_SIZE);
                recv(new_socket,buffer,BUFFER_SIZE,0);
                if(strcmp(buffer,"exit")==0)
                {
                    printf("Disconnected from %d \n",sin_port);
                    fflush(stdout);
                    close(new_socket);
                    return EXIT_SUCCESS;
                }
                printf("Client %d sent : %s\n",sin_port,buffer);
                int no= atoi(buffer);
                char for_client[500];
                sprintf(for_client,"for_client_%d",sin_port);
                store_n_procs_in_file(no,for_client);
                fflush(stdout);
                FILE *fp = fopen(for_client, "r");
                if (fp == NULL) 
                {
                    perror("Error in reading file.");
                    return EXIT_FAILURE;
                }
                send_file(fp, new_socket);
                memset(buffer,'\0',BUFFER_SIZE);
                recv(new_socket,buffer,BUFFER_SIZE,0);
                printf("Client %d 's most CPU intensive proc: %s\n",sin_port,buffer);
                memset(buffer,'\0',BUFFER_SIZE);
                printf("Waiting for port %d ...\n",sin_port);
                fflush(stdout);
                sleep(30);
                printf("Wait over for %d.. exiting \n",sin_port);
                fflush(stdout);
                close(new_socket);
            // }

}

void main()
{
    int socket_fd,ret;
    struct sockaddr_in server_address;
    int new_socket;
    struct sockaddr_in new_address;
    socklen_t address_size;
    char buffer[BUFFER_SIZE];
    pid_t child_pid;

    memset(&server_address,'\0',sizeof(server_address));
    server_address.sin_family=AF_INET;
    server_address.sin_port=htons(PORT);
    server_address.sin_addr.s_addr=inet_addr("127.0.0.1");


    if((socket_fd = socket(AF_INET,SOCK_STREAM,0))<0)
    {
        perror("Server Socket:");
        exit(EXIT_FAILURE);
    }
    printf("Created server socket\n");
    fflush(stdout);
    
    if((ret=bind(socket_fd,(struct sockaddr*)&server_address,sizeof(server_address)))<0)
    {
        perror("Binding error:");
        exit(EXIT_FAILURE);
    }
    printf("Binding successful\n");
    fflush(stdout);
    if(listen(socket_fd,10)==0)
    {
        printf("Server listening...\n");
        fflush(stdout);
    }
    else
    {
        perror("Listen error");
        exit(EXIT_FAILURE);
    }
    
    while(1)
    {
        address_size=sizeof(new_address);
        if((new_socket = accept(socket_fd,(struct sockaddr*)&new_address,&address_size))<0)
        {
            perror("Accept failure");
            exit(EXIT_FAILURE);
        }
        printf("Received connection from %s : %d\n",inet_ntoa(new_address.sin_addr),ntohs(new_address.sin_port));
        fflush(stdout);
        pthread_t t;
        struct socket_info * info = malloc(sizeof(struct socket_info));
        info->socket_fd=new_socket; 
        info->port_no=ntohs(new_address.sin_port);
        pthread_create(&t,NULL,connection_hander,info);

    }

}