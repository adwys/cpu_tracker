#include "cpuUsageTracker.h"
sem_t mutex;
sem_t empty;
sem_t full;
sem_t print;
int in = 0;
int out = 0;
double percentage =0;
FILE *raw_data[10];
struct timespec ttime = {1,2};

_Noreturn void calculateCpuUsage(void){
    char buffer[1024];
    unsigned int user = 0, nice = 0, system = 0, idle = 0;
    unsigned int iowait = 0, irq = 0, softirq = 0, steal = 0, guest = 0, guestnice = 0;
    unsigned int prevTotal = 0,total = 0,prevIdle = 0;
    while(1){
        sem_wait(&full);
        fread(buffer, sizeof(buffer) - 1, 1,raw_data[out]);
        out = (out+1)%10;

        sscanf(buffer,
               "cpu %16d %16d %16d %16d %16d %16d %16d %16d %16d %16d",
               &user, &nice, &system, &idle, &iowait, &irq, &softirq, &steal, &guest, &guestnice);

        total = user + nice + system + idle + irq + softirq + steal + iowait;

        percentage = 100 - (idle-prevIdle)*100.0/(total-prevTotal);
        prevTotal = total;
        prevIdle = idle;

        sem_post(&print);
        sem_post(&empty);
    }

}


_Noreturn void* readerThreadHandler(void){
    pthread_mutex_init((pthread_mutex_t *) &mutex, NULL);
    sem_init(&empty,0,10);
    sem_init(&full,0,0);


    while(1){
        sem_wait(&empty);
        raw_data[in] = popen("cat /proc/stat","r");
        in = (in+1)%10;
        sem_post(&full);
        nanosleep(&ttime, NULL);
    }

}

 void* analyzerThreadHandler(void){
    calculateCpuUsage();
}

_Noreturn void* printerThreadHandler(void){
    sem_init(&print,0,0);

    while(1){
        sem_wait(&print);
        printf("%%%.0f\n",percentage);
    }

}

_Noreturn void* watchdogThreadHandler(void){

}