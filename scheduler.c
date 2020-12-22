#include "headers.h"
#include <math.h>

void clearResources(int signum);
void addtoSJFqueue(struct processData *ptr,NodePCB **head,struct msgbuff msg);
void addtoRRobinQueue(struct processData *ptr,NodePCB **head,struct msgbuff msg);
void addtoPHPFqueue(struct processData *ptr,NodePCB **head,struct msgbuff msg);
void addtoSRTNQueue(struct processData *ptr,NodePCB **head,struct msgbuff msg);
struct PCBElement * copystruct(struct processData *ptr,NodePCB **head);
void SJF(NodePCB **head, NodePCB **Fin);
void SRTN(NodePCB **head, NodePCB **Fin);
void RRalgo(NodePCB **head,NodePCB **Fin);
void PHPF(NodePCB **head, NodePCB **Fin);
void Intializeprocess(NodePCB **head);
bool recieve(struct processData *ptr,struct msgbuff msg);
void initmsq();
void printFinished(NodePCB **FinNod);
char *getstate(int x);
void updatequeue(NodePCB **head,struct PCBElement *Runningprocess);


key_t msgqid;
int shmid3;

int * shmaddrrem;
char param;
char algorithm;
FILE *fptr;
long flag=0;
struct PCBElement *Runningprocess=NULL;
struct PCBElement *tempSRTN=NULL;

int main(int argc, char * argv[])
{
    signal(SIGINT, clearResources);
    signal (SIGTSTP, SIG_DFL);
    signal (SIGCONT, SIG_DFL);

    initClk();
    initmsq();
    fptr=fopen("Schedular.log","w");
    fprintf(fptr, "At\tTime\tx\tprocess\ty\tstate\tArr\tw\ttotal\tz\tremain\ty\twait\tk\n"); 

  
    param=*argv[2];
    struct processData *temp=(struct processData *)malloc(sizeof(struct processData));;
    pthread_mutex_t count_mutex;
    struct msgbuff msg;
    // int processidflag=0,prevflag=0,flagalgo=0;
    NodePCB *head=NULL;
    NodePCB *Fin=NULL;
    algorithm=*argv[1];
    int algo=(int)*argv[1];
    while(true)
    {
        pthread_mutex_lock(&count_mutex);

        //recieve and add to queue
        if(recieve(temp,msg)){
            if(algo==(int)*"0")
            {
            addtoRRobinQueue(temp,&head,msg);
            }
            else if(algo==(int)*"1")
            {
            addtoSJFqueue(temp,&head,msg);
            }else if(algo==(int)*"2"){
            addtoSRTNQueue(temp,&head,msg);
            }
            else
            {
            addtoPHPFqueue(temp,&head,msg);
            }
            

        }

        ///calling algorithm
        if(algo==(int)*"0")
            {
            RRalgo(&head,&Fin);
            }
        else if(algo==(int)*"1")
            {
            SJF(&head,&Fin);
            }
        else if(algo==(int)*"2")
        {
            SRTN(&head,&Fin);
        }else
            {
            PHPF(&head,&Fin);
            }
        ///break while 
        if(flag==1){
            if(!head){
            break;
            }
        }

        pthread_mutex_unlock(&count_mutex);
    }
    
    printf("out \n");
    fclose(fptr);
    printFinished(&Fin);
    //upon termination release the clock resources
    freePCB(&Fin);
    destroyREm(true);
    destroyClk(true);
}
void initmsq(){
    msgqid = msgget(key1,0);
    if(msgqid==-1){
        printf("Faild to create message queue %d\n",msgqid);
    }
      shmid3 = shmget(REMKEY, 4, IPC_CREAT | 0644);
    shmaddrrem = (int *) shmat(shmid3, (void *)0, 0);
}
bool recieve(struct processData *ptr,struct msgbuff msg)
{
    int rec_val;
    rec_val = msgrcv(msgqid,&msg,sizeof(msg), 0,IPC_NOWAIT);
    if(rec_val == -1){
        return false;
    }
        ptr->id=msg.temp.id;ptr->arrivaltime=msg.temp.arrivaltime;ptr->priority=msg.temp.priority;ptr->runningtime=msg.temp.runningtime;
        printf("Recieved %d\n",ptr->id);
        flag=msg.mtype;
        return true;
    
}


void addtoSJFqueue(struct processData *ptr,NodePCB **head,struct msgbuff msg){
    struct PCBElement *temp=copystruct(ptr,NULL);
    pushPCB(head,temp,temp->runningtime);
    printf("Turn %d \n",(*head)->data->id);
    if(recieve(ptr,msg))
    {
        addtoSJFqueue(ptr,head,msg);
    }    
}

void addtoPHPFqueue(struct processData *ptr,NodePCB **head,struct msgbuff msg){
    struct PCBElement *temp=copystruct(ptr,NULL);
    pushPCB(head,temp,-temp->priority);
    printf("Turn %d \n",(*head)->data->id);
    if(recieve(ptr,msg))
    {
        addtoSJFqueue(ptr,head,msg);
    }    
}
void addtoRRobinQueue(struct processData *ptr,NodePCB **head,struct msgbuff msg)
{
       struct PCBElement *temp=copystruct(ptr,NULL);
       temp->turn=0;
       pushPCB(head,temp,temp->turn);
       printf("Turn %d \n",(*head)->data->id);
    if(recieve(ptr,msg))
    {
        addtoSJFqueue(ptr,head,msg);
    }    
}
void addtoSRTNQueue(struct processData *ptr,NodePCB **head,struct msgbuff msg)
{
 struct PCBElement *temp=copystruct(ptr,NULL);
       temp->remainingtime=temp->runningtime;
       pushPCB(head,temp,temp->remainingtime);
       printf("Turn %d \n",(*head)->data->id);
    if(recieve(ptr,msg))
    {
        addtoSJFqueue(ptr,head,msg);
    }    
}

struct PCBElement * copystruct(struct processData *ptr,NodePCB **head)
{
    if(!head){
 struct PCBElement *temp=(struct PCBElement *)malloc(sizeof(struct PCBElement)); 
    temp->id=ptr->id;temp->arrivaltime=ptr->arrivaltime;
    temp->priority=ptr->priority;temp->runningtime=ptr->runningtime;
    temp->remainingtime=ptr->runningtime;
    return temp;
    }else{
    struct PCBElement *temp=(struct PCBElement *)malloc(sizeof(struct PCBElement)); 
    temp->id=(*head)->data->id;temp->arrivaltime=(*head)->data->arrivaltime;temp->finishedtime=(*head)->data->finishedtime;
    temp->PID=(*head)->data->PID;temp->priority=(*head)->data->priority;temp->remainingtime=(*head)->data->remainingtime;
    temp->runningtime=(*head)->data->runningtime;temp->starttime=(*head)->data->starttime;
    temp->state=(*head)->data->state;temp->turnarround=(*head)->data->turnarround;temp->weightedturnaround=(*head)->data->weightedturnaround;
    temp->waitingtime=(*head)->data->starttime-(*head)->data->arrivaltime;
    return temp;
    }
}

void SJF(NodePCB **head, NodePCB **Fin){   
    if(!((*head)==NULL)){
        //
    printf("Running %d\n",(*head)->data->id);
    int status;
    *shmaddrrem=(*head)->data->runningtime;
    (*head)->data->starttime=getClk();
    Intializeprocess(head);
    printf("Waiting\n");
    struct PCBElement *temp=copystruct(NULL,head);
    fprintf(fptr, "At\tTime\t%d\tprocess\t%d\t%s\tArr\t%d\ttotal\t%d\tremain\t%d\twait\t%d\n",temp->starttime,temp->id,getstate(temp->state),temp->arrivaltime,temp->runningtime,temp->remainingtime,temp->waitingtime); 
    popPCB(head);
    int pid = wait(&status);
    temp->finishedtime=getClk();
    temp->turnarround=temp->finishedtime-temp->arrivaltime;
    temp->weightedturnaround=temp->turnarround/temp->runningtime;
    temp->state=Finished;
    temp->remainingtime=0;
    fprintf(fptr, "At\tTime\t%d\tprocess\t%d\t%s\tArr\t%d\ttotal\t%d\tremain\t%d\twait\t%d\tTA\t%.2f\tWTA\t%.2f\n",getClk(),temp->id,getstate(temp->state),temp->arrivaltime,temp->runningtime,temp->remainingtime,temp->waitingtime,temp->turnarround,temp->weightedturnaround); 
    pushPCB(Fin,temp,temp->arrivaltime);
}
    
}
void RRalgo(NodePCB **head, NodePCB **Fin)
{
    if(!((*head)==NULL))
    {
            printf("Running %d\n",(*head)->data->id);
            int status,finished;
            *shmaddrrem=(*head)->data->remainingtime;
            printf("remaning time %d\n",*shmaddrrem);
            if((*head)->data->turn==0)
            {
                (*head)->data->starttime=getClk();
                Intializeprocess(head);
            }else{
                kill((*head)->data->PID,SIGCONT);
            }
            if((*head)->data->remainingtime>param)
            {
            printf("Stopped \n");
            (*head)->data->state=Running;
            struct PCBElement *temp;
            if((*head)->data->turn==0){
                temp=copystruct(NULL,head);
                fprintf(fptr, "At\tTime\t%d\tprocess\t%d\t%s\tArr\t%d\ttotal\t%d\tremain\t%d\twait\t%d\n",temp->starttime,temp->id,getstate(temp->state),temp->arrivaltime,temp->runningtime,temp->remainingtime,temp->waitingtime); 
            }else{
                (*head)->data->state=Ready;
                temp=copystruct(NULL,head);
                 fprintf(fptr,"At\tTime\t%d\tprocess\t%d\t%s\tArr\t%d\ttotal\t%d\tremain\t%d\twait\t%d\n",getClk(),temp->id,getstate(temp->state),temp->arrivaltime,temp->runningtime,temp->remainingtime,temp->waitingtime); 
            }
            raise(SIGSTOP);
            //Adding to queue again with changing parameters

                (*head)->data->remainingtime=(*head)->data->remainingtime-param;
                (*head)->data->state=Blocked;
                temp=copystruct(NULL,head);
                fprintf(fptr, "At\tTime\t%d\tprocess\t%d\t%s\tArr\t%d\ttotal\t%d\tremain\t%d\twait\t%d\n",getClk(),temp->id,getstate(temp->state),temp->arrivaltime,temp->runningtime,temp->remainingtime,temp->waitingtime); 
                printf("process %d Blocked \n",(*head)->data->id);
                popPCB(head);
                temp->turn=temp->turn+1;
                pushPCB(head,temp,temp->turn);
            
            }
            else
            {
            //finalize process adding to finixhed queue
                printf("Waiting\n");
                (*head)->data->state=Running;
                struct PCBElement *temp;
                if((*head)->data->turn==0){
                    temp=copystruct(NULL,head);
                    fprintf(fptr, "At\tTime\t%d\tprocess\t%d\t%s\tArr\t%d\ttotal\t%d\tremain\t%d\twait\t%d\n",temp->starttime,temp->id,getstate(temp->state),temp->arrivaltime,temp->runningtime,temp->remainingtime,temp->waitingtime); 
                 }else{
                      (*head)->data->state=Ready;
                     temp=copystruct(NULL,head);
                 fprintf(fptr, "At\tTime\t%d\tprocess\t%d\t%s\tArr\t%d\ttotal\t%d\tremain\t%d\twait\t%d\n",getClk(),temp->id,getstate(temp->state),temp->arrivaltime,temp->runningtime,temp->remainingtime,temp->waitingtime); 
                 }
                /// pop head was here
                int pid = wait(NULL);
                temp=copystruct(NULL,head);
                popPCB(head);
                temp->finishedtime=getClk();
                temp->turnarround=temp->finishedtime-temp->arrivaltime;
                temp->weightedturnaround=temp->turnarround/temp->runningtime;
                temp->state=Finished;
                temp->remainingtime=0;
                fprintf(fptr, "At\tTime\t%d\tprocess\t%d\t%s\tArr\t%d\ttotal\t%d\tremain\t%d\twait\t%d\tTA\t%.2f\tWTA\t%.2f\n",getClk(),temp->id,getstate(temp->state),temp->arrivaltime,temp->runningtime,temp->remainingtime,temp->waitingtime,temp->turnarround,temp->weightedturnaround); 
                pushPCB(Fin,temp,temp->arrivaltime);
            }     
    }
}
void PHPF(NodePCB **head, NodePCB **Fin)
{
    if((*head))
    {
        if(!Runningprocess){
            printf("Running %d\n",(*head)->data->id);
            *shmaddrrem=(*head)->data->remainingtime;
            if((*head)->data->state==Blocked){
                (*head)->data->state=Ready;
                kill((*head)->data->PID,SIGCONT);
                printf("Continue PID %d\n",(*head)->data->PID);
            }else{
            (*head)->data->state=Running;
            (*head)->data->starttime=getClk();
            Intializeprocess(head);
            }
            Runningprocess=copystruct(NULL,head);
            printf("Stopped \n");
            fprintf(fptr, "At\tTime\t%d\tprocess\t%d\t%s\tArr\t%d\ttotal\t%d\tremain\t%d\twait\t%d\n",getClk(),Runningprocess->id,getstate(Runningprocess->state),Runningprocess->arrivaltime,Runningprocess->runningtime,Runningprocess->remainingtime,Runningprocess->waitingtime); 
            raise(SIGSTOP);
        }
        if((*head)->data->id!=Runningprocess->id)
        {
            kill(Runningprocess->PID,SIGSTOP);
            Runningprocess->state=Blocked;
            updatequeue(head,Runningprocess);
            fprintf(fptr, "At\tTime\t%d\tprocess\t%d\t%s\tArr\t%d\ttotal\t%d\tremain\t%d\twait\t%d\n",getClk(),Runningprocess->id,getstate(Runningprocess->state),Runningprocess->arrivaltime,Runningprocess->runningtime,Runningprocess->remainingtime,Runningprocess->waitingtime); 
            printf("Blocked %d\n",Runningprocess->id);
            printf("Running %d\n",(*head)->data->id);
            *shmaddrrem=(*head)->data->remainingtime;
            (*head)->data->state=Running;
            (*head)->data->starttime=getClk();
            Intializeprocess(head);
            Runningprocess=copystruct(NULL,head);
            fprintf(fptr, "At\tTime\t%d\tprocess\t%d\t%s\tArr\t%d\ttotal\t%d\tremain\t%d\twait\t%d\n",getClk(),Runningprocess->id,getstate(Runningprocess->state),Runningprocess->arrivaltime,Runningprocess->runningtime,Runningprocess->remainingtime,Runningprocess->waitingtime); 
            printf("Stopped \n");         
            raise(SIGSTOP);

        }
        if(*shmaddrrem==0){
            //finished data
            printf("Finished %d\n",(*head)->data->id);
            (*head)->data->finishedtime=getClk();
            (*head)->data->turnarround=(*head)->data->finishedtime-(*head)->data->arrivaltime;
            (*head)->data->weightedturnaround=(*head)->data->turnarround/(*head)->data->runningtime;  
            (*head)->data->state=Finished;(*head)->data->remainingtime=0;
            pushPCB(Fin,(*head)->data,(*head)->data->arrivaltime);
            fprintf(fptr, "At\tTime\t%d\tprocess\t%d\t%s\tArr\t%d\ttotal\t%d\tremain\t%d\twait\t%d\tTA\t%.2f\tWTA\t%.2f\n",getClk(),(*head)->data->id,getstate((*head)->data->state),(*head)->data->arrivaltime,(*head)->data->runningtime,(*head)->data->remainingtime,(*head)->data->waitingtime,(*head)->data->turnarround,(*head)->data->weightedturnaround); 
            popPCB(head);
            Runningprocess=NULL;
        }   
    }
}
void SRTN(NodePCB **head, NodePCB **Fin)
{
        if(!((*head)==NULL))
        {
            if(!Runningprocess)
            {
                printf("Running %d\n",(*head)->data->id);
                *shmaddrrem=(*head)->data->remainingtime;
                if((*head)->data->state==Blocked)
                    {
                        (*head)->data->state=Ready;
                        kill((*head)->data->PID,SIGCONT);
                    }
                    else
                    {
                printf("Running %d\n",(*head)->data->id);
                *shmaddrrem=(*head)->data->remainingtime;
                (*head)->data->starttime=getClk();
                (*head)->data->state=Running;
                Intializeprocess(head);
                    }
                Runningprocess=copystruct(NULL,head);
                fprintf(fptr, "At\tTime\t%d\tprocess\t%d\t%s\tArr\t%d\ttotal\t%d\tremain\t%d\twait\t%d\n",getClk(),Runningprocess->id,getstate(Runningprocess->state),Runningprocess->arrivaltime,Runningprocess->runningtime,Runningprocess->remainingtime,Runningprocess->waitingtime); 
                // tempSRTN=copystruct(NULL,head);
                popPCB(head);
                raise(SIGSTOP);
            }
            else if((*head)->data->id!=Runningprocess->id)
            {
                Runningprocess->remainingtime=*shmaddrrem;
                // tempSRTN->remainingtime=*shmaddrrem;
                if((*head)->data->remainingtime<Runningprocess->remainingtime)
                {
                    kill(Runningprocess->PID,SIGSTOP);
                    printf("Blocked %d\n",Runningprocess->id);
                    Runningprocess->state=Blocked;
                    pushPCB(head,Runningprocess,Runningprocess->remainingtime);
                    if((*head)->data->state==Blocked)
                    {
                        (*head)->data->state=Ready;
                        kill((*head)->data->PID,SIGCONT);
                    }
                    else
                    {
                printf("Running %d\n",(*head)->data->id);
                *shmaddrrem=(*head)->data->remainingtime;
                (*head)->data->starttime=getClk();
                (*head)->data->state=Running;
                Intializeprocess(head);
                    }
                    Runningprocess=copystruct(NULL,head);
                    // tempSRTN=copystruct(NULL,head);
fprintf(fptr, "At\tTime\t%d\tprocess\t%d\t%s\tArr\t%d\ttotal\t%d\tremain\t%d\twait\t%d\n",getClk(),Runningprocess->id,getstate(Runningprocess->state),Runningprocess->arrivaltime,Runningprocess->runningtime,Runningprocess->remainingtime,Runningprocess->waitingtime); 
                    popPCB(head);
                }
                    raise(SIGSTOP);
            }
             if(*shmaddrrem==0){
            //finished data
            printf("Finished %d\n",Runningprocess->id);
            Runningprocess->finishedtime=getClk();
            Runningprocess->turnarround=Runningprocess->finishedtime-Runningprocess->arrivaltime;
            Runningprocess->weightedturnaround=Runningprocess->turnarround/Runningprocess->runningtime;  
            Runningprocess->state=Finished;Runningprocess->remainingtime=0;
            pushPCB(Fin,Runningprocess,Runningprocess->arrivaltime);
            fprintf(fptr, "At\tTime\t%d\tprocess\t%d\t%s\tArr\t%d\ttotal\t%d\tremain\t%d\twait\t%d\tTA\t%.2f\tWTA\t%.2f\n",getClk(),Runningprocess->id,getstate(Runningprocess->state),Runningprocess->arrivaltime,Runningprocess->runningtime,Runningprocess->remainingtime,Runningprocess->waitingtime,Runningprocess->turnarround,Runningprocess->weightedturnaround); 
            Runningprocess=NULL;
        }   
           

        }
}
void Intializeprocess(NodePCB **head){
    int pid =fork();
    if(pid==0){
        char temp=(char)param;
        char *binaryPath = "./process.out";
        // char temp=(char)head->data->remainingtime;
        char *args[] = {binaryPath,&temp,&algorithm,NULL} ;
        execvp(binaryPath,args);   
    }
    (*head)->data->PID=pid;
    printf("PID %d\n",(*head)->data->PID);
}

void clearResources(int signum)
{

    //TODO Clears all resources in case of interruption
    shmctl(shmid3, IPC_RMID, NULL);
    printf("shared memory terminating!\n");
    exit(0);
}
char * getstate(int x){
    switch(x){
        case 0:return "Started";
        case 1:return "Stopped";
        case 2:return "Finishd";
        case 3:return "Resumed";
    }
}
void printFinished(NodePCB **Fin){
    FILE *tempptr;
    tempptr=fopen("scedular.perf","w");
    NodePCB* startfin = (*Fin); 
    double avgWTA,AVGWAting,std,sum1=0,sum2=0,sum3=0;
    struct PCBElement *temp;
    int counter=0;
    while(startfin){
        temp=copystruct(NULL,&startfin);
        sum1=temp->weightedturnaround+sum1;
        sum2=temp->waitingtime+sum2;
        counter++;
        startfin=startfin->next;
    }
    avgWTA=sum1/counter;
    startfin = (*Fin);
    while(startfin){
        temp=copystruct(NULL,&startfin);
        sum3=(temp->weightedturnaround-avgWTA)*(temp->weightedturnaround-avgWTA)+sum3;
      startfin=startfin->next;   
    }
    std=sqrt(sum3/counter);
    AVGWAting=sum2/counter;
    fprintf(tempptr,"CPU Itilization\t100%%\n");
    fprintf(tempptr,"avgWTA\t%.2f\n",avgWTA);
    fprintf(tempptr,"AVGWating\t%.2f\n",AVGWAting);
    fprintf(tempptr,"STD WTA\t%.2f\n",std);

fclose(tempptr);
}
void updatequeue(NodePCB **head,struct PCBElement *temp)
{
    NodePCB* start = (*head); 
    while(start){
        if(temp->id==start->data->id){
            start->data->state=temp->state;
            start->data->remainingtime=*shmaddrrem;
            temp->remainingtime=*shmaddrrem;
            break;
        }
        start=start->next;
    }
}
// {
    
//     Node* starthead = (*head); 
//     if(starthead){
//         while(starthead)
//         {
//             struct PCBElement *temp=copystruct(NULL,head);
//             pushPCB(Fin,temp,temp->arrivaltime);
//             starthead=starthead->next;
//         }
//     }
//     freePCB(head);
//     Node* startfin = (*Fin); 
   
//     while(startfin|st)
//     {
//        fprintf(fptr, "At\tTime\t%d\tprocess\ty\tstate\tArr\tw\ttotal\tz\tremain\ty\twait\tk\n",); 
//         startfin=startfin->next;
//     }
// }
