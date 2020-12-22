
#include "headers.h"

/* Modify this file as needed*/
int remainingtime;

int main(int agrc, char * argv[])
{
    initClk();
    initremaining();
    signal (SIGTSTP, SIG_DFL);

    pthread_mutex_t count_mutex;

    //TODO it needs to get the remaining time from somewhere
    remainingtime = getremaining();
    int startremainingtime=remainingtime;
    int quantum=(int)*argv[1];
    int algo=(int)*argv[2];
    while (remainingtime > 0)
    {
        pthread_mutex_lock(&count_mutex);
        if(quantum!=0){
            if(startremainingtime-quantum==remainingtime){
                startremainingtime=remainingtime;
                kill(getppid(),SIGCONT);
                raise(SIGSTOP);
            }
        }
        sleep(1);
        remainingtime = remainingtime-1;
        setremaining(remainingtime);
       // printf("remaining time %d\n",remainingtime);   
        pthread_mutex_unlock(&count_mutex);
    }
    //destroyClk(false);
    if(algo==(int)*"2"||algo==(int)*"3")
    {
        kill(getppid(),SIGCONT);
    }
    exit(2);
    return 0;
}