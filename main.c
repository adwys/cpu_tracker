#include "cpuUsageTracker.h"




int main(void) {

    pthread_t readerThread,analyzerThread;
    pthread_create( &readerThread, NULL, readerThreadHandler,NULL);
    pthread_create( &analyzerThread, NULL, analyzerThreadHandler,NULL);
    pthread_join( readerThread, NULL);
    pthread_join( analyzerThread, NULL);

    return 0;
}
