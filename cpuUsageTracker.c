#include "cpuUsageTracker.h"
sem_t empty;
sem_t full;
sem_t print;
int in = 0;
int out = 0;
double percentage[8];
FILE *raw_data[10];


struct timespec ttime = {1,2};

cpuValues * extractValues(FILE *rData){
    cpuValues * values = malloc(8 * sizeof(cpuValues));
    char  buffer[1024];
    int index;
    char *pbuff;
    fread(buffer, sizeof(buffer) - 1, 1,rData);
    pbuff=buffer;
    while (1) {
        if (*pbuff == '\n'){pbuff++; break;}
        pbuff++;
    }
    for(int i=0;i<8;i++) {
        sscanf(pbuff,
               "cpu%d %16d %16d %16d %16d %16d %16d %16d %16d %16d %16d",&index,
               &(values + i)->user, &values[i].nice, &values[i].system, &values[i].idle, &values[i].iowait, &values[i].irq, &values[i].softirq, &values[i].steal, &values[i].guest, &values[i].guestnice);
        while (1) {
            if (*pbuff == '\n'){pbuff++; break;}
            pbuff++;
        }
    }
    return values;
}

_Noreturn void calculateCpuUsage(void){

    unsigned int prevTotal[8],total[8],prevIdle[8];
    cpuValues * values;
    while(1){
        sem_wait(&full);
        values = extractValues(raw_data[out]);
        out = (out+1)%10;
        for(int i=0;i<8;i++){
            total[i] = (values + i)->user + (values + i)->nice + (values + i)->system +
                    (values + i)->idle + (values + i)->irq +
                    (values + i)->softirq + (values + i)->steal + (values + i)->iowait;

            percentage[i] = 100 - ((values + i)->idle-prevIdle[i])*100.0/(total[i]-prevTotal[i]);
            prevTotal[i] = total[i];
            prevIdle[i] = (values + i)->idle;
        }
        free(values);
        sem_post(&print);
        sem_post(&empty);
    }

}


_Noreturn void* readerThreadHandler(void){
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
        printf("%-5s %-5s %-5s %-5s %-5s %-5s %-5s %-5s\n", "cpu0", "cpu1", "cpu2", "cpu3", "cpu4", "cpu5" ,"cpu6", "cpu7");
        printf("%%%-4.0f %%%-4.0f %%%-4.0f %%%-4.0f %%%-4.0f %%%-4.0f %%%-4.0f %%%-4.0f\n",percentage[0],percentage[1],percentage[2],percentage[3],percentage[4],percentage[5],
               percentage[6],percentage[7]);

    }

}

_Noreturn void* watchdogThreadHandler(void){

}