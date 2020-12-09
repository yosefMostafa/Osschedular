#include "headers.h"


int main(int argc, char * argv[])
{
    initClk();
    initqueue();    
    int numofprocess=(int) *argv[3];
    // for (int i=1;i<4;i++){
    //     x=(int) *argv[i];
    //     printf("recieved %d \n",x);
    // }
    struct processData *temp;
    pthread_mutex_t count_mutex;

   // pthread_mutex_lock(&count_mutex);

    
    /*while(true){
    temp=getqueuehead();
    for(int i=0;i<10;i++){
    printf("process id %d\n",(temp+i)->id);
    }
        sleep(1);
    }*/

    sleep(61);
    temp=getqueuehead();
    for(int i=0;i<10;i++){
    printf("process id %d\n",(temp+i)->id);
    }

    //TODO implement the scheduler :)
    //upon termination release the clock resources
    destroyqueue(true,numofprocess);
    //destroyClk(true);
}