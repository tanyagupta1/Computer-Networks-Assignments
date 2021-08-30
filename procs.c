#include <sys/types.h>
#include <dirent.h>
#include <stdio.h>
#include <ctype.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>
// Helper function to check if a struct dirent from /proc is a PID folder.
int is_pid_folder(const struct dirent *entry) {
    const char *p;

    for (p = entry->d_name; *p; p++) {
        if (!isdigit(*p))
            return 0;
    }

    return 1;
}
struct process
{
        int pid;
        char path[1000];
        unsigned long time;
};
int compare(const void *p1, const void *p2)
{
    const struct process *elem1 = p1;    
    const struct process *elem2 = p2;

   if (elem1->time < elem2->time) return 1;
   else if (elem1->time > elem2->time) return -1;
   else return 0;
}
void store_n_procs_in_file(int n, char * filename)
{
    DIR *procdir;
    FILE *fp;
    struct dirent *entry;
    char path[256 + 5 + 5]; // d_name + /proc + /stat
    int pid;
    unsigned long utime, stime;
    struct process process_list[1000];
    int cur_proc=0;
    procdir = opendir("/proc");
    if (!procdir) {
        perror("opendir failed");
        return 1;
    }
    while ((entry = readdir(procdir))) 
    {
        
        if (!is_pid_folder(entry))continue;
        snprintf(path, sizeof(path), "/proc/%s/stat", entry->d_name);
        fp = fopen(path, "r");

        if (!fp) 
        {
            perror(path);
            continue;
        }
        fscanf(fp, "%d %s %*c %*d %*d %*d %*d %*d %*u %*lu %*lu %*lu %*lu %lu %lu",&pid, &path,&utime,&stime);
        unsigned long total_time = utime+stime;
        strcpy(process_list[cur_proc].path,path);
        process_list[cur_proc].pid=pid;
        process_list[cur_proc].time=total_time;
        
        cur_proc++;
        fclose(fp);
    }

    closedir(procdir);
    qsort((void*)&process_list,cur_proc,sizeof(struct process),compare);
    FILE * n_procs= fopen(filename,"w");
    for(int i=0;i<n;i++)
    {
        fprintf(n_procs,"%5d %-20s: %lu\n", process_list[i].pid, process_list[i].path,process_list[i].time);
    }
    fclose(n_procs);

}
void write_file(int sockfd,int count,char *filename){
  int n;
  FILE *fp;
   
  char buffer[1024];

  fp = fopen(filename, "w");
  while (count>0) {
    n = recv(sockfd, buffer, 1024, 0);
    printf("Received: %s",buffer);
    count--;
    fflush(stdout);
    if (n <= 0){
      break;
      return;
    }
    if(fprintf(fp, "%s", buffer)<0)
    {
        perror("Write error:");
    }
    // fflush(fp);
    bzero(buffer, 1024);
  }
  fclose(fp);
  return;
}
void send_file(FILE *fp, int sockfd){
  int n;
  char data[1024] = {0};

  while(fgets(data, 1024, fp) != NULL) {
    if (send(sockfd, data, sizeof(data), 0) == -1) {
      perror("[-]Error in sending file.");
      exit(1);
    }
    printf("Sending: %s",data);
    fflush(stdout);
    bzero(data, 1024);
  }
}
// int main(void) 
// {
//     store_n_procs_in_file(10,"tanyafile");
   
//     return 0;
// }
