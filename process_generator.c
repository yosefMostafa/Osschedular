#include "headers.h"


void clearResources(int);
Node *ReadData(int *no);
void IntiatClk();
void IntialtSchedular(char algorithm,char *param);
char getalgorithm(char *param);
void sendData(Node *ptr);
void sendprocess(struct processData *shmaddr,struct processData *temp);
void initmsq();
key_t msqid;

int main(int argc, char * argv[])
{
    signal(SIGINT, clearResources);
    // 1. Read the input files.//Done
    int numprocesses;
    Node *ptr= ReadData(&numprocesses);
    // 2. Ask the user for the chosen scheduling algorithm and its parameters, if there are any.Done
    char algorithm,param;
    algorithm=getalgorithm(&param);
    // 3. Initiate and create the scheduler and clock processes.Done
    IntialtSchedular(algorithm,&param);
    IntiatClk();
    initmsq();
    // 4. Use this function after creating the clock process to initialize clock
    initClk();
    // To get time use this
    int x = getClk(); 
    printf("current time is %d\n", getClk());
    // 5. Create a data structure for processes and provide it with its parameters.
    //Done in ReadData
    // 6. Send the information to the scheduler at the appropriate time.Needs To actual send of data
    sendData(ptr);
    freequeue(&ptr);
    // 7. Clear clock resources
    int pid = wait(NULL);
    //destroyClk(true);
}

void initmsq(){
    msqid = msgget(key1,IPC_CREAT|0644);
    if(msqid==-1){
        printf("Faild to create message queue \n");
    }
}



void IntiatClk(){
     int pid =fork();
    if (pid==0){
    char *binaryPath = "./clk.out";
    char *args[] = {binaryPath,NULL} ;
    execvp(binaryPath,args);   
    }
}
void IntialtSchedular(char algorithm,char* param){
     int pid =fork();
    if (pid==0){
    char *binaryPath = "./scheduler.out";
    char *args[] = {binaryPath,&algorithm,param,NULL} ;
    execvp(binaryPath,args);   
    }
}
char getalgorithm(char *param){
    char algo;
    printf("Please enter the name of schedular algotithm: \n");
    printf("0)Round Robin\n");
    printf("1)Shortest Jop first\n");
    printf("2)Premitive\n");
    printf("Enter number of algorithm\n");
    scanf("%s", &algo);
    if(algo=='0'){
        printf("Please enter the Switch parameter\n");
        int temp;
        scanf("%d", &temp);
        *param=(char)temp;
        return algo;
    }
    *param=(char)0;
    return algo;
}

Node *ReadData(int *no){
    FILE *file;
    Node *ptr;
    char filename;
    char dataToBeRead[50]; 
    printf("Please enter the name of processes you want to generate: ");
    scanf("%s", &filename);
    file=fopen(&filename,"r");
     if ( file == NULL ) 
    { 
        printf("file failed to open." ) ; 
    } 
    else
    { 
        printf("The file is now opened.\n") ; 
        //getting the headers
        fgets(dataToBeRead,50,file);
        
        struct processData *temp;
        fscanf ( file, "%d",no ) ;
        temp = (struct processData *)malloc((*no)*sizeof(struct processData));
            for(int i=0;i<(*no);i++){
                fscanf ( file, "%d", &(temp+i)->id);
                fscanf ( file, "%d", &(temp+i)->arrivaltime );
                fscanf ( file, "%d", &(temp+i)->runningtime );
                fscanf ( file, "%d", &(temp+i)->priority );
                if(i==0){
                    ptr=newNode(temp+i,(temp+i)->arrivaltime);
                }else{
                    push(&ptr,temp+i,(temp+i)->arrivaltime);
                    }
            }
            
            fclose(file) ;          
    } 
    printf("Data successfully read from file\n"); 
    return ptr;
}
void sendData(Node *ptr)
{
    pthread_mutex_t count_mutex;
    int clk;
    struct processData *temp;
    struct msgbuff msg;
    int send_val;
    while(true)
    {
    pthread_mutex_lock(&count_mutex);
    temp=peek(&ptr);
    clk=getClk();
    
    while(temp->arrivaltime==clk)
    {
        msg.temp.id=temp->id;msg.temp.arrivaltime=temp->arrivaltime;msg.temp.priority=temp->priority;msg.temp.runningtime=temp->runningtime;
        if(ptr->next){
            msg.mtype=2;
        }
        else{
            msg.mtype=1;
        }
        send_val = msgsnd(msqid, &msg, sizeof(msg),!IPC_NOWAIT);
        if(send_val == -1){
            perror("Errror in send");
        }
        printf("sent\n");
        pop(&ptr);
         if(!ptr){
        break;
    }
        temp=peek(&ptr);
    }
     if(!ptr){
        break;
    }
    pthread_mutex_unlock(&count_mutex);
    }
}

void sendprocess(struct processData *shmaddr,struct processData *temp){
            printf("sent %d\n",temp->id);
            (shmaddr)->id=temp->id;(shmaddr)->arrivaltime=temp->arrivaltime;
            (shmaddr)->priority=temp->priority;(shmaddr)->runningtime=temp->runningtime;
    //         for(int i=0;i<10;i++){
    // printf("process id %d\n",(shmaddr+i)->id);
    // }
}



void clearResources(int signum)
{
    //TODO Clears all resources in case of interruption
    msgctl(msqid, IPC_RMID, (struct msqid_ds *) 0);
    printf("shared memory terminating!\n");
    exit(0);
}