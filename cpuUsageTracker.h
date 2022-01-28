#ifndef CPUUSAGETRACKER_H
#define CPUUSAGETRACKER_H

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <semaphore.h>
#include <string.h>



_Noreturn void* readerThreadHandler(void);

_Noreturn void* analyzerThreadHandler(void);

_Noreturn void* printerThreadHandler(void);

_Noreturn void* watchdogThreadHandler(void);

_Noreturn void calculateCpuUsage(void);

#endif