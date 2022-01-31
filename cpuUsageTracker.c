#include "cpuUsageTracker.h"




void destroyTracker(){

    fclose(data->log);
    pthread_kill(data->printerThread,SIGKILL);
    pthread_kill(data->readerThread,SIGKILL);
    pthread_kill(data->analyzerThread,SIGKILL);
    pthread_kill(data->watchdogThread, SIGKILL);
    pthread_kill(data->logThread, SIGKILL);
    free(data);

}

void sigHandler(){
    logMessage("[INFO]: SIGTERM has been caught, ending program");
    printf("\nprogram closed successfully\n");
    destroyTracker();
}

void abrtHandler(){
    logMessage("[ERROR]: one of threads has ben idle for more than 2 sec");
    printf("one of threads has ben idle for more than 2 sec.\n");
    destroyTracker();
}

void initTracker(globalData ** newData){
    signal(SIGTERM, (__sighandler_t) sigHandler);
    signal(SIGINT, (__sighandler_t) sigHandler);
    signal(SIGABRT, (__sighandler_t) abrtHandler);
    *newData = malloc(sizeof(globalData));
    (*newData)->in = 0;
    (*newData)->out = 0;
    sem_init(&(*newData)->empty,0,10);
    sem_init(&(*newData)->full,0,0);
    sem_init(&(*newData)->print,0,0);
    sem_init(&(*newData)->write,0,1);
    sem_init(&(*newData)->read,0,0);
    pipe(data->fd);

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
        logMessage("[INFO]: percentages has been calculated\n");
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
        logMessage("[INFO]: data has been read\n");
        data->in = (data->in+1)%10;
        sem_post(&data->full);
        sleep(1);
        data->readerFlag = true;
    }

}

_Noreturn void analyzerThreadHandler(){
    calculateCpuUsage();
}

_Noreturn void printerThreadHandler(){
    while(1){

        sem_wait(&data->print);
        char buff[1024];
         sprintf(buff,"%-5s %-5s %-5s %-5s %-5s %-5s %-5s %-5s\n%%%-4.0f %%%-4.0f %%%-4.0f %%%-4.0f %%%-4.0f %%%-4.0f %%%-4.0f %%%-4.0f\n","cpu0", "cpu1", "cpu2", "cpu3", "cpu4", "cpu5" ,"cpu6", "cpu7",data->percentage[0],
               data->percentage[1],data->percentage[2],data->percentage[3],data->percentage[4],data->percentage[5],
               data->percentage[6],data->percentage[7]);
        printf("%s",buff);
        logMessage("[INFO]: cpu usage has been printed\n");

        data->printerFlag = true;
    }
}


_Noreturn void watchdogThreadHandler(){
    logMessage("[INFO]: watchdog start\n");
    while (1){
        data->readerFlag = data->analyzerFlag = data->logFlag = data->printerFlag = false;
        sleep(2);
        if(data->readerFlag + data->analyzerFlag + data->logFlag + data->printerFlag != 4 ){
            logMessage("[ERROR]: Watchdog ending program\n");
            pthread_kill(pthread_self(),SIGABRT);
        }
        logMessage("[INFO]: every thread work correctly\n");
    }

}

char * logMessage(char * message){
    unsigned int n = (unsigned int) strlen(message) + 1;
    sem_wait(&data->write);
    write(data->fd[1],&n,sizeof(int));
    write(data->fd[1],message,strlen(message) + 1);
    sem_post(&data->read);
}

_Noreturn void logThreadHandler(void){
    data->log = fopen("log.txt","w");
    if(data->log == NULL){
        pthread_kill(pthread_self(),SIGTERM);
    }
    char buff[1024];
    unsigned int n;
    while(1){
        sem_wait(&data->read);
        read(data->fd[0],&n,sizeof(int));
        read(data->fd[0],buff,n * sizeof(char));
        sem_post(&data->write);
        fwrite(buff,1,n-1,data->log);
        data->logFlag = true;

    }

}
