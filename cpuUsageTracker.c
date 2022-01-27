#include "cpuUsageTracker.h"
sem_t mutex;
sem_t empty;
sem_t full;
int in = 0;
int out = 0;
FILE *raw_data[10];
struct timespec ttime = {1,2};

_Noreturn unsigned long long calculateCpuUsage(void){
    char buffer[1024];

    unsigned long long user = 0, nice = 0, system = 0, idle = 0;
    unsigned long long iowait = 0, irq = 0, softirq = 0, steal = 0, guest = 0, guestnice = 0;

    unsigned long long prevTotal = 0,total = 0,prevIdle = 0,idleCurr = 0;
    unsigned int percentage = 0;
    while(1){
        sem_wait(&full);
        pthread_mutex_lock((pthread_mutex_t *) &mutex);

        fread(buffer, sizeof(buffer) - 1, 1,raw_data[out]);
        out = (out+1)%10;
        prevIdle = idleCurr;
        prevTotal = total;
        sscanf(buffer,
               "cpu %16llu %16llu %16llu %16llu %16llu %16llu %16llu %16llu %16llu %16llu",
               &user, &nice, &system, &idle, &iowait, &irq, &softirq, &steal, &guest, &guestnice);

        idleCurr = idle + iowait;
        total = idle + user + nice + system + irq + softirq + steal;

        total-=prevTotal;
        idleCurr-=prevIdle;

        percentage = (100 * (total - idleCurr))/total;
        printf("%d\n",percentage);

        pthread_mutex_unlock((pthread_mutex_t *) &mutex);
        sem_post(&empty);
    }

}


_Noreturn void* readerThreadHandler(void){
    pthread_mutex_init((pthread_mutex_t *) &mutex, NULL);
    sem_init(&empty,0,10);
    sem_init(&full,0,0);


    while(1){
        sem_wait(&empty);
        pthread_mutex_lock((pthread_mutex_t *) &mutex);
        raw_data[in] = popen("cat /proc/stat","r");
        in = (in+1)%10;
        pthread_mutex_unlock((pthread_mutex_t *) &mutex);
        sem_post(&full);
        nanosleep(&ttime, NULL);
    }

}

_Noreturn void* analyzerThreadHandler(void){
    calculateCpuUsage();
}