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
struct mempair * Intializemem(int mem);
struct mempair * Buddymemory(int size);
struct mempair* copymem(struct mempair *ptr);
void clearRam(struct mempair *ptr);
void searchandaddonhold(int size,int start,int position,int arraysize);
void pushOnHold(struct mempair *ptr);




key_t msgqid;
int shmid3;
int memorycapacity=1024;
int * shmaddrrem;
char param;
char algorithm;
FILE *fptr;
FILE *fptrMemory;
long flag=0;
struct PCBElement *Runningprocess=NULL;
struct PCBElement *tempSRTN=NULL;
struct mempair *pair=NULL;
struct mempair *OnHold=NULL;

int main(int argc, char * argv[])
{
    signal(SIGINT, clearResources);
    signal (SIGTSTP, SIG_DFL);
    signal (SIGCONT, SIG_DFL);

    initClk();
    initmsq();
    pair=Intializemem(memorycapacity);

    fptr=fopen("Schedular.log","w");
    fptrMemory=fopen("Memory.log","w");
    fprintf(fptr, "At\tTime\tx\tprocess\ty\tstate\tArr\tw\ttotal\tz\tremain\ty\twait\tk\n"); 
    fprintf(fptrMemory, "At\tTime\tx\tallocated\ty\tbytes\tfor\tprocess\tw\tfrom\tz\tto\ty\n"); 
    
  
    param=*argv[2];
    struct processData *temp=(struct processData *)malloc(sizeof(struct processData));
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
        ptr->id=msg.temp.id;
        ptr->arrivaltime=msg.temp.arrivaltime;
        ptr->priority=msg.temp.priority;
        ptr->runningtime=msg.temp.runningtime;
        ptr->memorysize=msg.temp.memorysize;
        printf("Recieved %d\n",ptr->id);
       
        flag=msg.mtype;
        return true;
    
}


void addtoSJFqueue(struct processData *ptr,NodePCB **head,struct msgbuff msg){
    struct PCBElement *tempcopied=copystruct(ptr,NULL);
    pushPCB(head,tempcopied,tempcopied->runningtime);
    printf("Turn %d \n",(*head)->data->id);
    NodePCB *temp=(*head);
    if(recieve(ptr,msg))
    {
        addtoSJFqueue(ptr,&temp,msg);
    }
}

void addtoPHPFqueue(struct processData *ptr,NodePCB **head,struct msgbuff msg){
    struct PCBElement *tempcopied=copystruct(ptr,NULL);
    pushPCB(head,tempcopied,-tempcopied->priority);
    printf("Turn %d \n",(*head)->data->id);
     NodePCB *temp=(*head);
    if(recieve(ptr,msg))
    {

        addtoPHPFqueue(ptr,&temp,msg);
    }    
}
void addtoRRobinQueue(struct processData *ptr,NodePCB **head,struct msgbuff msg)
{
       struct PCBElement *tempcopied=copystruct(ptr,NULL);
       tempcopied->turn=0;
       pushPCB(head,tempcopied,tempcopied->turn);
       printf("Turn %d \n",(*head)->data->id);
       NodePCB *temp=(*head);
    if(recieve(ptr,msg))
    {
        addtoRRobinQueue(ptr,&temp,msg);
    }    
}
void addtoSRTNQueue(struct processData *ptr,NodePCB **head,struct msgbuff msg)
{
 struct PCBElement *tempcopied=copystruct(ptr,NULL);
       tempcopied->remainingtime=tempcopied->runningtime;
       pushPCB(head,tempcopied,tempcopied->remainingtime);
       printf("Turn %d \n",(*head)->data->id);
       NodePCB *temp=(*head);
    if(recieve(ptr,msg))
    {
        addtoSRTNQueue(ptr,&temp,msg);
    }    
}

struct PCBElement * copystruct(struct processData *ptr,NodePCB **head)
{
    if(!head){
    struct PCBElement *temp=(struct PCBElement *)malloc(sizeof(struct PCBElement)); 
    temp->id=ptr->id;
    temp->arrivaltime=ptr->arrivaltime;
    temp->priority=ptr->priority;
    temp->runningtime=ptr->runningtime;
    temp->remainingtime=ptr->runningtime;
    temp->memorysize=ptr->memorysize;
    return temp;
    }else{
    struct PCBElement *temp=(struct PCBElement *)malloc(sizeof(struct PCBElement)); 
    temp->id=(*head)->data->id;
    temp->arrivaltime=(*head)->data->arrivaltime;
    temp->finishedtime=(*head)->data->finishedtime;
    temp->PID=(*head)->data->PID;
    temp->priority=(*head)->data->priority;
    temp->remainingtime=(*head)->data->remainingtime;
    temp->runningtime=(*head)->data->runningtime;
    temp->starttime=(*head)->data->starttime;
    temp->state=(*head)->data->state;
    temp->turnarround=(*head)->data->turnarround;
    temp->weightedturnaround=(*head)->data->weightedturnaround;
    temp->waitingtime=(*head)->data->starttime-(*head)->data->arrivaltime;
    temp->memorypair.start=(*head)->data->memorypair.start;
    temp->memorypair.end=(*head)->data->memorypair.end;
    temp->memorysize=(*head)->data->memorysize;
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
    fprintf(fptr,"At\tTime\t%d\tprocess\t%d\t%s\tArr\t%d\ttotal\t%d\tremain\t%d\twait\t%d\n",temp->starttime,temp->id,getstate(temp->state),temp->arrivaltime,temp->runningtime,temp->remainingtime,temp->waitingtime); 
    popPCB(head);
    wait(&status);
    printf("Finished %d \n ",temp->id);
    clearRam(&temp->memorypair);
    fprintf(fptrMemory,  "At\tTime\t%d\tfreed\t%d\tbytes\tfor\tprocess\t%d\tfrom\t%d\tto\t%d\n",getClk(),temp->memorysize,temp->id,temp->memorypair.start,temp->memorypair.end); 
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
            *shmaddrrem=(*head)->data->remainingtime;
            printf("remaning time %d\n",*shmaddrrem);
            if((*head)->data->turn==0)
            {
                (*head)->data->starttime=getClk();
                Intializeprocess(head);
            }else{
                kill((*head)->data->PID,SIGCONT);
                printf("here \n");
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
                printf("value of turn %d \n",(*head)->data->turn);
            
            }
            else
            {
            //finalize process adding to finixhed queue
                printf("pair {%d,%d} ",(*head)->data->memorypair.start,(*head)->data->memorypair.end);
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
                wait(NULL);
                temp=copystruct(NULL,head);
               
                temp->finishedtime=getClk();
                temp->turnarround=temp->finishedtime-temp->arrivaltime;
                temp->weightedturnaround=temp->turnarround/temp->runningtime;
                temp->state=Finished;
                temp->remainingtime=0;
                
                clearRam(&(*head)->data->memorypair);
                fprintf(fptrMemory,  "At\tTime\t%d\tfreed\t%d\tbytes\tfor\tprocess\t%d\tfrom\t%d\tto\t%d\n",getClk(),(*head)->data->memorysize,(*head)->data->id,(*head)->data->memorypair.start,(*head)->data->memorypair.end); 
                 for(int i=0;i<11;i++){
                printf("pair {%d,%d} ",(pair+i)->start,(pair+i)->end);
            }
            printf("\n\n");
            popPCB(head);
            
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
            clearRam(&(*head)->data->memorypair);
            
            fprintf(fptrMemory,  "At\tTime\t%d\tfreed   \t%d\tbytes\tfor\tprocess\t%d\tfrom\t%d\tto\t%d\n",getClk(),(*head)->data->memorysize,(*head)->data->id,(*head)->data->memorypair.start,(*head)->data->memorypair.end); 
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
            clearRam(&Runningprocess->memorypair);
            fprintf(fptrMemory,  "At\tTime\t%d\tfreed\t%d\tbytes\tfor\tprocess\t%d\tfrom\t%d\tto\t%d\n",getClk(),Runningprocess->memorysize,Runningprocess->id,Runningprocess->memorypair.start,Runningprocess->memorypair.end); 
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
    struct mempair *temp=Buddymemory((*head)->data->memorysize);
    (*head)->data->memorypair.start=temp->start;(*head)->data->memorypair.end=temp->end;
    fprintf(fptrMemory,  "At\tTime\t%d\tallocated\t%d\tbytes\tfor\tprocess\t%d\tfrom\t%d\tto\t%d\n",getClk(),(*head)->data->memorysize,(*head)->data->id,(*head)->data->memorypair.start,(*head)->data->memorypair.end); 
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
    return NULL;
}
void printFinished(NodePCB **Fin){
    FILE *tempptr;
    tempptr=fopen("scedular.perf","w");
    fclose(fptrMemory);
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
struct mempair *Buddymemory(int size)
{
int n =ceil(log(size)/log(2));
int actualsize=pow(2,n);
printf("The memory allocated %d\n",actualsize);

if(((pair+n)->end-(pair+n)->start)==size)
{
        struct mempair *temp=copymem((pair+n));
        (pair+n)->start=-1;
        (pair+n)->end=-1;
        printf("from %d to %d\n",temp->start,temp->end-1);
        return temp;
}
else
{
    int index;int size=ceil(log(memorycapacity)/log(2))+1;
    for(index=n+1;index<size;index++)
    {
        if((pair+index)->end!=-1)
            break;
    }
    if (index == size)  
        { 
            printf("failed to allocate memory \n"); 
            return NULL;
        } 
        else
        {
            struct mempair *temp=copymem((pair+index));
            (pair+index)->start=-1;(pair+index)->end=-1;
            //index--;
            for(;index>n;index--)
            {
                if(ceil(log(temp->end-temp->start)/log(2))==n)
                    break;
                (pair+index)->start=temp->start+(temp->end-temp->start)/2;
                (pair+index)->end=temp->end;
                (pair+index-1)->start=temp->start;
                (pair+index-1)->end=temp->start+(temp->end-temp->start)/2;
                temp=copymem((pair+index-1));
                (pair+index-1)->start=-1;(pair+index-1)->end=-1;
                if(ceil(log(temp->end-temp->start)/log(2))==n)
                    break;
            }
            temp->end=temp->end-1;
            printf("from %d to %d\n",temp->start,temp->end);
            for(int i=0;i<10;i++){
                if(((OnHold+i)->end-(OnHold+i)->start)==actualsize)
                {
                    (pair+n)->start=(OnHold+i)->start;(pair+n)->end=(OnHold+i)->end;
                    (OnHold+i)->start=0;(OnHold+i)->end=0;
                    break;
                }
            }
            int place,position;
            for(int i=0;i<11;i++){
                if((pair+i)->start!=-1){
                    place=(pair+i)->end-(pair+i)->start;
                    position=ceil(log(place)/log(2))+1;
                    if(i!=position){
                        (pair+position)->start=(pair+i)->start;
                        (pair+position)->end=(pair+i)->end;
                        (pair+i)->start=-1;(pair+i)->end=-1;
                    }   
                }
            }
             for(int i=0;i<11;i++){
                printf("pair {%d,%d} \n",(pair+i)->start,(pair+i)->end);
                 }
                        printf("\n");
            return temp;
        }
    

}
 

}
struct mempair * Intializemem(int mem)
{

    int n=ceil(log(mem)/log(2));
    struct mempair *pair=(struct mempair *)malloc((n+1)*sizeof(struct mempair));
    OnHold=(struct mempair *)malloc((n+1)*sizeof(struct mempair));
    (pair+n)->start=0;
    (pair+n)->end=mem;
    (OnHold+n)->end=-1;(OnHold+n)->start=-1;
    for(int i=0;i<n;i++){
        (pair+i)->start=-1;
        (pair+i)->end=-1;
        (OnHold+i)->start=-1;
        (OnHold+i)->end=-1;

    }
    return pair;
    
        
        

}
struct mempair* copymem(struct mempair *ptr){
    struct mempair *temp=(struct mempair *)malloc(sizeof(struct mempair));
    temp->end=ptr->end;
    temp->start=ptr->start;
    return temp;
}
void clearRam(struct mempair *ptr)
{
    int size=(ptr->end)-ptr->start+1;
    int position=ceil(log(size)/log(2));
    size=pow(2,position);
    position++;
    int arraysize=ceil(log(memorycapacity)/log(2));
    if((pair+position)->start==-1)
    {
        (pair+position)->start=ptr->start;
        (pair+position)->end=ptr->end+1;
        searchandaddonhold(size,ptr->start,position,arraysize);
    }else
    {
        struct mempair *temp=ptr;
        pushOnHold(temp);
        // for(int i=0;i<11;i++){
        //         printf("pair {%d,%d} \n",(OnHold+i)->start,(OnHold+i)->end);
        //          }
        //                 printf("\n");
        searchandaddonhold(size,ptr->start,position,arraysize);
    }
    int place,position2;
     for(int i=0;i<11;i++){
                if((pair+i)->start!=-1){
                    place=(pair+i)->end-(pair+i)->start;
                    position2=ceil(log(place)/log(2))+1;
                    if(i!=position2){
                        if((pair+position2)->start==-1){
                        (pair+position2)->start=(pair+i)->start;
                        (pair+position2)->end=(pair+i)->end;
                        (pair+i)->start=-1;(pair+i)->end=-1;
                        }
                    }   
                }
            }
    int size1,size2;
    for(int i=0;i<arraysize;i++)
    {
        size1=(pair+i)->end-(pair+i)->start;
        size2=(pair+i+1)->end-(pair+i+1)->start;
        if(size1==size2&&size1!=0)
        {
            if((pair+i)->end!=(pair+i+1)->start){
                 struct mempair *temp=copymem((pair+i));
                (pair+i)->start=-1;(pair+i)->end=-1;
                temp->end=temp->end-1; 
                pushOnHold(temp);
                break;
            }
            (pair+i+1)->start=(pair+i)->start;
            (pair+i)->start=-1;(pair+i)->end=-1;
        }
    }
 
                    for(int i=0;i<11;i++){
                printf("pair {%d,%d} ",(pair+i)->start,(pair+i)->end);
                 }
                        printf("\n");
                                        

   
    
}
void searchandaddonhold(int size,int start,int position,int arraysize)
{   int trial;
    int sizeonhold;
    for(int i=0;i<arraysize;i++)
    {
    if((OnHold+i)->start!=-1){
        sizeonhold=(OnHold+i)->end-(OnHold+i)->start+1;
        trial=(OnHold+i)->start/sizeonhold;
        if(sizeonhold==size){
            if(trial%2==0){
                 if((pair+position)->start==(OnHold+i)->end+1){
                    (pair+position)->start=(OnHold+i)->start;
                    (OnHold+i)->start=-1;(OnHold+i)->end=-1;
                    return;
                }

            }else{
                if((pair+position)->end==(OnHold+i)->start){
                    (pair+position)->end=(OnHold+i)->end+1;
                     (OnHold+i)->start=-1;(OnHold+i)->end=-1;
                    return;
                }

            }
        }
    }
    }
// int trial=start/size;
// int address;
//         if(trial%2==0)          
//             address=start+size;
//         else
//             address=start;    
//         if(start==0){
//             address=start+size;
//         }    
//         printf("address %d %d %d\n",    address,start,size);
//         for(int i=0;i<arraysize;i++)
//         {
//             printf("on hold %d \n",(OnHold+i)->start);
//             if((OnHold+i)->end==address)
//             {
//                 if(trial%2==0){
//                     (pair+position)->start=(OnHold+i)->start;
//                     (pair+position)->end=(pair+position)->end;
//                     (OnHold+i)->start=-1;(OnHold+i)->end=-1;
//                     break;
//                 }else{
//                      (pair+position)->start=(pair+position)->start;
//                     (pair+position)->end=(OnHold+i)->end+1;
//                     (OnHold+i)->start=-1;(OnHold+i)->end=-1;
//                     break;
//                 }
//             }
//         }
}
void pushOnHold(struct mempair *ptr)
{
     int size =ptr->end-ptr->start;
        for(int i=0;i<10;i++){
            if (((OnHold+i)->end-(OnHold+i)->start)==size){
                if((ptr->end+1)==(OnHold+i)->start){
                    (OnHold+i)->start=ptr->start;
                }else{
                    (OnHold+i)->end=ptr->end;
                }
                int position=ceil(log((size+1)*2)/log(2))+1;
                (pair+position)->start=(OnHold+i)->start;
                (pair+position)->end=(OnHold+i)->end+1;
                (OnHold+i)->start=-1;(OnHold+i)->end=-1;
                // int arraysize=ceil(log(memorycapacity)/log(2));
                // searchandaddonhold(size+1,(OnHold+i)->start,position,arraysize);
                return;
            }
        }
    

    for(int i=0;i<10;i++)
    {
        if((OnHold+i)->start==-1)
        {
            if(ptr->start==0)
            (OnHold+i)->end=ptr->end;
            else
            (OnHold+i)->end=ptr->end;
            
            (OnHold+i)->start=ptr->start;
            break;
        }
    }  
}



// int size=(ptr->end)-ptr->start;
//    int position=ceil(log(size)/log(2))+1;
//    int arraysize=ceil(log(memorycapacity)/log(2));
//    if((pair+position)->end==-1)
//    {
//     (pair+position)->end=ptr->end+1;
//     (pair+position)->start=ptr->start;
//    }
//     int address;
//     int trial=ptr->start/(size+1);
//    if(trial%2==0){
//        address=ptr->start+size+1;
//    }else{
//        address=ptr->start-size-1;
//    }
//    if(ptr->start==0){
//        address=ptr->end+1;
//    }

//    int i;
   
//     for (i =0;i<=arraysize;i++)
//     {
//         if((pair+i)->start==address&&address!=-1)
//         {

//             if(trial%2==0)
//             {
//             (pair+i)->start=ptr->start;
//             (pair+i)->end=(pair+i)->end;
//             (pair+i-1)->start=-1;(pair+i-1)->end=-1;
//                         //sreturn;
//             }else
//             {
//                 if((pair+position+1)->start==ptr->end+1){
//                      (pair+position+1)->start=(pair+i)->start;
//                     (pair+i)->start=-1;(pair+i)->end=-1;
//                      address= (pair+position+1)->end;

//                 }else{
//             (pair+position+1)->end=ptr->end+1;
//             (pair+position+1)->start=(pair+i)->start;
//             address=(pair+position+1)->end;
//             trial=(pair+position+1)->start/((pair+position+1)->end-(pair+position+1)->start);
//                 }
//             (pair+i)->start=-1;(pair+i)->end=-1;
//                         return;
//             }
       
//         }
//     }
    

// if(ptr->start==0&&address==512)
//     {
//         (pair+i)->start=0;
//         (pair+i-1)->start=-1;(pair+i-1)->end=-1;
//         return;
//     }
//         for(int i=0;i<11;i++){
//                 printf("pair {%d,%d} \n",(pair+i)->start,(pair+i)->end);
//                  }
//                         printf("\n");
//         if(trial%2==0)
//         {
//             temp->start=ptr->start;
//             temp->end=(pair+i)->end-1;
//             (pair+i)->start=-1;(pair+i)->end=-1;
//             clearRam(temp,1,-1);
//         }
//         else
//         {
//             temp->end=ptr->end-1;
//             temp->start=(pair+i)->start;
//             (pair+i)->start=-1;(pair+i)->end=-1;
//             clearRam(temp,1,-1);   
//         }
//  if(trial%2==0)
//             {
//                 (pair+i)->start=ptr->start;
//                 size= (pair+i)->end- (pair+i)->start;
//                 (pair+position)->start=-1;(pair+position)->end=-1;
//                 if(ptr->start==0){
//                 address= (pair+i)->start+size+2;
//                 }else{
//  address= (pair+i)->start+size;
//                 }
               
//                 printf("here \n");
//             }
//             else
//             {
//              (pair+position+1)->start=(pair+i)->start; 
//              (pair+position+1)->end=ptr->end;
//              (pair+i)->start=-1;(pair+i)->end=-1;
             
//              size=(pair+position)->end-(pair+position)->start;
//             address=(pair+position)->start+size;
//             }
//                printf("address %d\n",address);
//                printf("i %d\n",i);

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
