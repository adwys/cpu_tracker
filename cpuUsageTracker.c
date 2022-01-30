#include "cpuUsageTracker.h"

void destroyTracker(){
    pthread_kill(data->printerThread,SIGKILL);
    pthread_kill(data->readerThread,SIGKILL);
    pthread_kill(data->analyzerThread,SIGKILL);
    pthread_kill(data->watchdogThread, SIGKILL);
    pthread_kill(data->logThread, SIGKILL);
    free(data);
}

void sigHandler(){
    printf("\nprogram closed\n");
    destroyTracker();
}

void abrtHandler(){
    printf("program has ben idle for more than 2 sec.\n");
    destroyTracker();
}

void initTracker(globalData ** newData){
    signal(SIGTERM,sigHandler);
    signal(SIGINT,sigHandler);
    signal(SIGABRT,abrtHandler);
    *newData = malloc(sizeof(globalData));
    (*newData)->in = 0;
    (*newData)->out = 0;
    sem_init(&(*newData)->empty,0,10);
    sem_init(&(*newData)->full,0,0);
    sem_init(&(*newData)->print,0,0);
}

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

_Noreturn void calculateCpuUsage(){

    unsigned int prevTotal[8],total[8],prevIdle[8];
    cpuValues * values;
    while(1){
        sem_wait(&data->full);
        values = extractValues(data->raw_data[data->out]);
        data->out = (data->out+1)%10;
        for(int i=0;i<8;i++){
            total[i] = (values + i)->user + (values + i)->nice + (values + i)->system +
                    (values + i)->idle + (values + i)->irq +
                    (values + i)->softirq + (values + i)->steal + (values + i)->iowait;

            data->percentage[i] = 100 - ((values + i)->idle-prevIdle[i])*100.0/(total[i]-prevTotal[i]);
            prevTotal[i] = total[i];
            prevIdle[i] = (values + i)->idle;
        }
        free(values);
        sem_post(&data->print);
        sem_post(&data->empty);
        data->analyzerFlag = true;
    }

}


_Noreturn void readerThreadHandler(){


    while(1){
        sem_wait(&data->empty);
        data->raw_data[data->in] = popen("cat /proc/stat","r");
        data->in = (data->in+1)%10;
        sem_post(&data->full);
        sleep(1);
//        nanosleep(&ttime, NULL);
        data->readerFlag = true;
    }

}

void analyzerThreadHandler(){
    calculateCpuUsage();
}

_Noreturn void printerThreadHandler(){
     while(1){
        sem_wait(&data->print);
        printf("%-5s %-5s %-5s %-5s %-5s %-5s %-5s %-5s\n", "cpu0", "cpu1", "cpu2", "cpu3", "cpu4", "cpu5" ,"cpu6", "cpu7");
        printf("%%%-4.0f %%%-4.0f %%%-4.0f %%%-4.0f %%%-4.0f %%%-4.0f %%%-4.0f %%%-4.0f\n",data->percentage[0],
               data->percentage[1],data->percentage[2],data->percentage[3],data->percentage[4],data->percentage[5],
               data->percentage[6],data->percentage[7]);
        data->printerFlag = true;
    }
}


_Noreturn void watchdogThreadHandler(){

    while (1){
        data->readerFlag = data->analyzerFlag = data->logFlag = data->printerFlag = false;

        sleep(2);
        if(data->readerFlag + data->analyzerFlag + data->logFlag + data->printerFlag != 4 ){
            pthread_kill(data->printerThread,SIGABRT);
        }
    }

}

_Noreturn void logThreadHandler(void){


    while(1){

        data->logFlag = true;
    }


}