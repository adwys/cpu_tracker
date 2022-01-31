#ifndef CPUUSAGETRACKER_H
#define CPUUSAGETRACKER_H

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <semaphore.h>
#include <string.h>
#include <signal.h>
#include <zconf.h>
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

typedef enum {
    false = 0,
    true = 1
} Bool;

typedef struct{
    sem_t empty;
    sem_t full;
    sem_t print;
    sem_t write;
    sem_t read;
    int in;
    int out;
    double percentage[8];
    FILE *raw_data[10];
    pthread_t readerThread,analyzerThread,printerThread,watchdogThread,logThread;
    Bool readerFlag,analyzerFlag,printerFlag,logFlag;
    int fd[2];
    FILE * log;
}globalData;

extern globalData * data;

void sigHandler(void);

void abrtHandler(void);

void destroyTracker(void);

void logMessage(char *);

void initTracker(void);

_Noreturn void readerThreadHandler(void);

_Noreturn void analyzerThreadHandler(void);

_Noreturn void printerThreadHandler(void);

_Noreturn void watchdogThreadHandler(void);

_Noreturn void logThreadHandler(void);


cpuValues * extractValues(FILE *rData);

#endif
