#include "cpuUsageTracker.h"

unsigned long long parseData(){
    char buffer[1024];
    fread(buffer, sizeof(buffer) - 1, 1,raw_data[out]);
    out = (out+1)%10;
    unsigned long long user = 0, nice = 0, system = 0, idle = 0;
    unsigned long long iowait = 0, irq = 0, softirq = 0, steal = 0, guest = 0, guestnice = 0;
    sscanf(buffer,
           "cpu  %16llu %16llu %16llu %16llu %16llu %16llu %16llu %16llu %16llu %16llu",
           &user, &nice, &system, &idle, &iowait, &irq, &softirq, &steal, &guest, &guestnice);

    return user + nice + system + idle + iowait + irq + softirq + steal;
}

int calculateCpuUsage(void){
   unsigned long long wyn = parseData();
    printf("wyn = %llu\n",wyn);
    return NULL;
}


_Noreturn void* readerThreadHandler(void){
    pthread_mutex_init(&mutex, NULL);
    sem_init(&empty,0,10);
    sem_init(&full,0,0);


    while(1){
        sem_wait(&empty);
        pthread_mutex_lock(&mutex);
        raw_data[in] = popen("cat /proc/stat","r");
        in = (in+1)%10;
        pthread_mutex_unlock(&mutex);
        sem_post(&full);
        nanosleep(&sec, NULL);
    }

}

_Noreturn void* analyzerThreadHandler(void){

    while(1){
        sem_wait(&full);
        pthread_mutex_lock(&mutex);
        calculateCpuUsage();
        pthread_mutex_unlock(&mutex);
        sem_post(&empty);
    }

}