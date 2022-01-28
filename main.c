#include "cpuUsageTracker.h"




int main(void) {


    pthread_t readerThread,analyzerThread,printerThread;
    pthread_create(&readerThread, NULL, (void *(*)(void *)) readerThreadHandler, NULL);
    pthread_create(&analyzerThread, NULL, (void *(*)(void *)) analyzerThreadHandler, NULL);
    pthread_create(&printerThread, NULL, (void *(*)(void *)) printerThreadHandler, NULL);

    pthread_join( readerThread, NULL);
    pthread_join( analyzerThread, NULL);
    pthread_join( printerThread, NULL);

    return 0;
}
