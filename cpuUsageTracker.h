#ifndef CPUUSAGETRACKER_H
#define CPUUSAGETRACKER_H

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <semaphore.h>
#include <string.h>
sem_t mutex;
sem_t empty;
sem_t full;
int in = 0;
int out = 0;
FILE *raw_data[10];

struct timespec sec = {1,2};

_Noreturn void* readerThreadHandler(void);

_Noreturn void* analyzerThreadHandler(void);
unsigned long long parseData(void);
int calculateCpuUsage(void);

#endif