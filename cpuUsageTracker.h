#ifndef CPUUSAGETRACKER_H
#define CPUUSAGETRACKER_H

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <semaphore.h>
#include <string.h>



_Noreturn void* readerThreadHandler(void);

_Noreturn void* analyzerThreadHandler(void);
unsigned long long parseData(void);

_Noreturn unsigned long long calculateCpuUsage(void);

#endif