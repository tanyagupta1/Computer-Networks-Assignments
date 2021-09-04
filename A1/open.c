#include <sys/types.h>
#include <dirent.h>
#include <stdio.h>
#include <ctype.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<errno.h>
#include<fcntl.h>
#define BUFFER_SIZE 5000
struct process
{
        int pid;
        char path[BUFFER_SIZE];
        unsigned long time;
};
int is_pid_folder(const struct dirent *entry) 
{
    const char *p;

    for (p = entry->d_name; *p; p++) 
    {
        if (!isdigit(*p)) return 0;
    }

    return 1;
}
int compare(const void *p1, const void *p2)
{
    const struct process *proc1 = p1;    
    const struct process *proc2 = p2;

   if (proc1->time < proc2->time) return 1;
   else if (proc1->time > proc2->time) return -1;
   else return 0;
}
void store_n_procs_in_file(int n, char * filename)
{
    DIR *procdir;
    FILE *fp;
    struct dirent *entry;
    char path[266];
    int pid;
    unsigned long utime, stime;
    struct process process_list[1000];
    int cur_proc=0;
    procdir = opendir("/proc");
    if (!procdir) {
        perror("Opendir:");
        return 1;
    }
    while ((entry = readdir(procdir))) 
    {
        
        if (!is_pid_folder(entry))continue;
        snprintf(path, sizeof(path), "/proc/%s/stat", entry->d_name);
        int fd1=open(path,O_RDONLY);
        if(fd1<0)
        {
            perror("Open:");
            exit(EXIT_FAILURE);
        }

        fp = fopen(path, "r");
        //handle fp opening
        fscanf(fp, "%d %s %*c %*d %*d %*d %*d %*d %*u %*lu %*lu %*lu %*lu %lu %lu",&pid, &path,&utime,&stime);
        unsigned long total_time = utime+stime;
        strcpy(process_list[cur_proc].path,path);
        process_list[cur_proc].pid=pid;
        process_list[cur_proc].time=total_time;
        cur_proc++;
        fclose(fp);
        close(fd1);
    }

    closedir(procdir);
    qsort((void*)&process_list,cur_proc,sizeof(struct process),compare);
    FILE * n_procs= fopen(filename,"w");
    for(int i=0;i<n;i++)
    {
        fprintf(n_procs,"PID: %d Name: %-s Time(clock-ticks): %lu\n", process_list[i].pid, process_list[i].path,process_list[i].time);
    }
    fclose(n_procs);

}
int main()
{
    store_n_procs_in_file(5,"open_sys");
}