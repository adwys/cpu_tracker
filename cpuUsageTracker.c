#include "cpuUsageTracker.h"

struct data parseData(){
    struct data parsedData;
    char cpu_data[100];
    fread(cpu_data, 100, 1,raw_data);
    char temp[5][20];
    int j=0,dataLen = strlen(cpu_data);
    for(int i=0;i<5;i++){
        int index=0;
        temp[i][index] = '\0';
        while(cpu_data[j] != ' ' && dataLen>=j){
            temp[i][index] = cpu_data[j];
            index++;
            j++;
        }
        temp[i][index] = '\0';

        if(dataLen<=j)break;
        j++;

    }

    strcpy(parsedData.cpu,temp[0]);
    parsedData.a = atoi(temp[1]);
    parsedData.b = atoi(temp[2]);
    parsedData.c = atoi(temp[3]);
    parsedData.previdle = atoi(temp[4]);

    return parsedData;
}

int calculateCpuUsage(void){



}


void* readerThreadHandler(void){


    sem_init(&mutex, 0, 1);
    sem_wait(&mutex);
    raw_data = popen("#!/bin/bash\n"
                     "read cpu a b c previdle rest < /proc/stat\n"
                     "echo $cpu $a $b $c $previdle","r");
    sem_post(&mutex);

    return NULL;
}

void* analyzerThreadHandler(void){
    char text[1000];
    sem_wait(&mutex);
    calculateCpuUsage();


    sem_post(&mutex);

}