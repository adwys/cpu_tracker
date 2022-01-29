#ifndef CPUUSAGETRACKER_H
#define CPUUSAGETRACKER_H

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <semaphore.h>
#include <string.h>

typedef struct {
    unsigned int user;
    unsigned int nice;
    unsigned int system;
    unsigned int idle;
    unsigned int iowait;
    unsigned int irq;
    unsigned int softirq;
    unsigned int steal;
    unsigned int guest;
    unsigned int guestnice;
}cpuValues;


_Noreturn void* readerThreadHandler(void);

_Noreturn void* analyzerThreadHandler(void);

_Noreturn void* printerThreadHandler(void);

_Noreturn void* watchdogThreadHandler(void);

_Noreturn void calculateCpuUsage(void);
cpuValues * extractValues(FILE *rData);
#endif