#ifndef CPUUSAGETRACKER_H
#define CPUUSAGETRACKER_H

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <semaphore.h>
#include <string.h>
sem_t mutex;

FILE *raw_data;

struct data{
    char cpu[5];
    int a;
    int b;
    int c;
    int previdle;

};

void* readerThreadHandler(void);
void* analyzerThreadHandler(void);
struct data parseData(void);
int calculateCpuUsage(void);

#endif