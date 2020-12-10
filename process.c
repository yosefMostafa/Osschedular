
#include "headers.h"

/* Modify this file as needed*/
int remainingtime;

int main(int agrc, char * argv[])
{
    initClk();
    initremaining();
    pthread_mutex_t count_mutex;

    //TODO it needs to get the remaining time from somewhere
    remainingtime = getremaining();
    while (remainingtime > 0)
    {
        pthread_mutex_lock(&count_mutex);
        sleep(1);
        remainingtime = remainingtime-1;
        setremaining(remainingtime);
        pthread_mutex_unlock(&count_mutex);
    }
    exit(1);
    //destroyClk(false);
    
    return 0;
}