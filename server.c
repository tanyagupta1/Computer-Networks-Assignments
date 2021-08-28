#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<errno.h>
#define  PORT 4455

void send_file(FILE *fp, int sockfd){
  int n;
  char data[1024] = {0};

  while(fgets(data, 1024, fp) != NULL) {
    if (send(sockfd, data, sizeof(data), 0) == -1) {
      perror("[-]Error in sending file.");
      exit(1);
    }
    printf("Server sending: %s\n",data);
    fflush(stdout);
    bzero(data, 1024);
  }
}

void main()
{
    int socket_fd,ret;
    struct sockaddr_in server_address;
    int new_socket;
    struct sockaddr_in new_address;
    socklen_t address_size;
    char buffer[1024];

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
                memset(buffer,'\0',1024);
                recv(new_socket,buffer,1024,0);
                if(strcmp(buffer,"exit")==0)
                {
                    printf("Disconnected from %d \n",ntohs(new_address.sin_port));
                    fflush(stdout);
                    close(new_socket);
                    exit(EXIT_SUCCESS);
                }
                printf("Client %d sent : %s\n",ntohs(new_address.sin_port),buffer);
                system("ps -e -o pid,comm,%cpu,%mem --sort=-%cpu | head -n 10 >myfile");

                fflush(stdout);
                // strcpy(buffer,"Received your msg");
                // send(new_socket,buffer,1024,0);
                FILE *fp = fopen("myfile", "r");
                if (fp == NULL) 
                {
                    perror("[-]Error in reading file.");
                    exit(1);
                }
                send_file(fp, new_socket);
                bzero(buffer,1024);
            }
        }

    }

    
}
// ps -e -o pid,comm,%cpu,%mem --sort=-%cpu | head -n 10
//https://unix.stackexchange.com/questions/13968/show-top-five-cpu-consuming-processes-with-ps
//https://stackoverflow.com/questions/1221555/retrieve-cpu-usage-and-memory-usage-of-a-single-process-on-linux