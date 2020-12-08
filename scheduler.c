#include "headers.h"


int main(int argc, char * argv[])
{
    initClk();
    for (int i=1;i<3;i++){
        printf("recieved %s \n",argv[i]);
    }
    //TODO implement the scheduler :)
    //upon termination release the clock resources
    
    destroyClk(true);
}