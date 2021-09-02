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



void main()
{
    int socket_fd,ret;
    struct sockaddr_in server_address;
    int new_socket;
    struct sockaddr_in new_address;
    socklen_t address_size;
    char buffer[BUFFER_SIZE];

    pid_t child_pid;

    if((socket_fd = socket(AF_INET,SOCK_STREAM,0))<0)
    {
        perror("Server Socket:");
        exit(EXIT_FAILURE);
    }
    printf("Created server socket\n");
    fflush(stdout);
    memset(&server_address,'\0',sizeof(server_address));

    server_address.sin_family=AF_INET;
    server_address.sin_port=htons(PORT);
    server_address.sin_addr.s_addr=inet_addr("127.0.0.1");

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
        if((child_pid=fork())==0)
        {
            close(socket_fd);

            while(1)
            {
                memset(buffer,'\0',BUFFER_SIZE);
                recv(new_socket,buffer,BUFFER_SIZE,0);
                if(strcmp(buffer,"q")==0)
                {
                    printf("Disconnected from %d \n",ntohs(new_address.sin_port));
                    fflush(stdout);
                    close(new_socket);
                    exit(EXIT_SUCCESS);
                }
                printf("Client %d sent : %s\n",ntohs(new_address.sin_port),buffer);
                int no= atoi(buffer);
                // system("ps -e -o pid,comm,%cpu,%mem --sort=-%cpu | head -n 10 >myfile");
                store_n_procs_in_file(no,"for_client");
                fflush(stdout);
                // strcpy(buffer,"Received your msg");
                // send(new_socket,buffer,BUFFER_SIZE,0);
                FILE *fp = fopen("for_client", "r");
                if (fp == NULL) 
                {
                    perror("[-]Error in reading file.");
                    exit(1);
                }
                send_file(fp, new_socket);
                // write_file(new_socket,10,"server_received_this");
                memset(buffer,'\0',BUFFER_SIZE);
                recv(new_socket,buffer,BUFFER_SIZE,0);
                printf("Client %d 's most CPU intensive proc: %s\n",ntohs(new_address.sin_port),buffer);
                bzero(buffer,BUFFER_SIZE);
            }
        }

    }

    
}
// ps -e -o pid,comm,%cpu,%mem --sort=-%cpu | head -n 10
//https://unix.stackexchange.com/questions/13968/show-top-five-cpu-consuming-processes-with-ps
//https://stackoverflow.com/questions/1221555/retrieve-cpu-usage-and-memory-usage-of-a-single-process-on-linux
//https://stackoverflow.com/questions/63372288/getting-list-of-pids-from-proc-in-linux
//https://stackoverflow.com/questions/16726779/how-do-i-get-the-total-cpu-usage-of-an-application-from-proc-pid-stat