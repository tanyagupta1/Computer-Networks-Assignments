#include <sys/types.h>
#include <dirent.h>
#include <stdio.h>
#include <ctype.h>
#include<unistd.h>
// Helper function to check if a struct dirent from /proc is a PID folder.
int is_pid_folder(const struct dirent *entry) {
    const char *p;

    for (p = entry->d_name; *p; p++) {
        if (!isdigit(*p))
            return 0;
    }

    return 1;
}

int main(void) {
    DIR *procdir;
    FILE *fp;
    struct dirent *entry;
    char path[256 + 5 + 5]; // d_name + /proc + /stat
    int pid;
    unsigned long maj_faults, utime, stime;
    long cutime,cstime;
    unsigned long long starttime;
    long Hertz = sysconf(_SC_CLK_TCK);
    unsigned long long uptime;
    double count=0;
    


    // Open /proc directory.
    procdir = opendir("/proc");
    if (!procdir) {
        perror("opendir failed");
        return 1;
    }

    // Iterate through all files and folders of /proc.
    while ((entry = readdir(procdir))) {
        // Skip anything that is not a PID folder.
        if (!is_pid_folder(entry))
            continue;

        // Try to open /proc/<PID>/stat.
        snprintf(path, sizeof(path), "/proc/%s/stat", entry->d_name);
        fp = fopen(path, "r");

        if (!fp) {
            perror(path);
            continue;
        }

        // Get PID, process name and number of faults.
        fscanf(fp, "%d %s %*c %*d %*d %*d %*d %*d %*u %*lu %*lu %lu %*lu %lu %lu %ld %ld %*ld %*ld %*ld %*ld %llu",
            &pid, &path, &maj_faults,&utime,&stime,&cutime,&cstime,&starttime
        );
        FILE * fp2;
    fp2=fopen("/proc/uptime","r");
    fscanf(fp2,"%llu",&uptime);
        // printf("%lu %lu %ld %ld %llu\n",utime,stime,cutime,cstime,starttime);
        unsigned long total_time = utime+stime;
        total_time+= cutime+cstime;
        unsigned long seconds = uptime -(starttime/Hertz);
        double cpu_usage = (((double)(total_time*1000.0/Hertz)/(double)seconds))/10;
        count+=cpu_usage;
        printf("count: %f\n",count);


        // Pretty print.
        printf("%5d %-20s: %f\n", pid, path, cpu_usage);
        fclose(fp);
    }

    closedir(procdir);
    return 0;
}
