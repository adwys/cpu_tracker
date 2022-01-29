#include "cpuUsageTracker.h"




int main(void) {

    initTracker(&data);

    pthread_create(&data->readerThread, NULL, (void *(*)(void *)) readerThreadHandler, NULL);
    pthread_create(&data->analyzerThread, NULL, (void *(*)(void *))  analyzerThreadHandler,NULL);
    pthread_create(&data->printerThread, NULL, (void *(*)(void *)) printerThreadHandler, NULL);
    pthread_create(&data->wathdogThread, NULL, (void *(*)(void *))  watchdogThreadHandler, NULL);

    pthread_join( data->readerThread, NULL);
    pthread_join( data->analyzerThread, NULL);
    pthread_join( data->printerThread, NULL);
    pthread_join( data->wathdogThread, NULL);
    return 0;
}
