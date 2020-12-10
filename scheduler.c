#include "headers.h"

void clearResources(int signum);
void addtoSJFqueue(struct processData *ptr,NodePCB **head,struct msgbuff msg);
void SJF(NodePCB **head);
void Intializeprocess(NodePCB **head);
bool recieve(struct processData *ptr,struct msgbuff msg);
void initmsq();

key_t msgqid;
int shmid3;
int * shmaddrrem;

int main(int argc, char * argv[])
{
    signal(SIGINT, clearResources);
    initClk();
    initmsq();

    shmid3 = shmget(REMKEY, 4, IPC_CREAT | 0644);
    shmaddrrem = (int *) shmat(shmid3, (void *)0, 0);
    *shmaddrrem=999;
  
    struct processData *temp=(struct processData *)malloc(sizeof(struct processData));;
    pthread_mutex_t count_mutex;
    struct msgbuff msg;
    int processidflag=0,prevflag=0,flagalgo=0;
    NodePCB *head=NULL;

    while(true)
    {
        pthread_mutex_lock(&count_mutex);
        if(recieve(temp,msg)){
            addtoSJFqueue(temp,&head,msg);
        }
        SJF(&head);
        if(getClk()==30){
            break;
        }
        pthread_mutex_unlock(&count_mutex);
    }
    
    printf("out \n");
    //upon termination release the clock resources
    destroyREm(true);
   // destroyshmaddr(true);
    destroyClk(true);
}
void initmsq(){
    msgqid = msgget(key,0);
    if(msgqid==-1){
        printf("Faild to create message queue %d\n",msgqid);
    }
}
bool recieve(struct processData *ptr,struct msgbuff msg)
{
    int rec_val;
    rec_val = msgrcv(msgqid,&msg,sizeof(msg.temp), 0,IPC_NOWAIT);
    if(rec_val == -1){
        ptr->id=0;
        return false;
    }
        ptr->id=msg.temp.id;ptr->arrivaltime=msg.temp.arrivaltime;ptr->priority=msg.temp.priority;ptr->runningtime=msg.temp.runningtime;
        printf(" Recieved %d\n",ptr->id);
        return true;
    
}


void addtoSJFqueue(struct processData *ptr,NodePCB **head,struct msgbuff msg){
    struct PCBElement *temp=(struct PCBElement *)malloc(sizeof(struct PCBElement)); 
    temp->id=ptr->id;temp->arrivaltime=ptr->arrivaltime;temp->priority=ptr->priority;temp->runningtime=ptr->runningtime;
    temp->remainingtime=ptr->runningtime;
        pushPCB(head,temp,temp->runningtime);
        printf("Turn %d \n",(*head)->data->id);
    if(recieve(ptr,msg))
    {
        addtoSJFqueue(ptr,head,msg);
    }    
}
void SJF(NodePCB **head){   
    if(!((*head)==NULL)){
    printf("Running %d\n",(*head)->data->id);
    int status;
    *shmaddrrem=(*head)->data->runningtime;
    Intializeprocess(head);
    printf("Waiting\n");
    popPCB(head);
    int pid = wait(&status);
    }
}
void Intializeprocess(NodePCB **head){
    int pid =fork();
    if(pid==0){
        int PID=getpid();
        (*head)->data->state=Running;
        (*head)->data->PID=PID;
        (*head)->data->starttime=getClk();
        char *binaryPath = "./process.out";
        // char temp=(char)head->data->remainingtime;
        char *args[] = {binaryPath,NULL} ;
        execvp(binaryPath,args);   
    }
}

void clearResources(int signum)
{
    //TODO Clears all resources in case of interruption
    shmctl(shmid3, IPC_RMID, NULL);
    printf("shared memory terminating!\n");
    exit(0);
}

