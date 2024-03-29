#include "headers.h"


void clearResources(int);
Node *ReadData(int *no);
void IntiatClk();
void IntialtSchedular(char algorithm,char *param);
char getalgorithm(char *param);
void sendData(Node *ptr);
void initmsq();
key_t msqid;


int PIDsh;
int valueofalgo;

int main(int argc, char * argv[])
{
    signal(SIGINT, clearResources);
    signal (SIGCONT, SIG_DFL);
    // 1. Read the input files.//Done
    int numprocesses;
    Node *ptr= ReadData(&numprocesses);
    // 2. Ask the user for the chosen scheduling algorithm and its parameters, if there are any.Done
    char algorithm,param;
    algorithm=getalgorithm(&param);
    // 3. Initiate and create the scheduler and clock processes.Done
    initmsq();
    IntialtSchedular(algorithm,&param);
    IntiatClk();
    // 4. Use this function after creating the clock process to initialize clock
    initClk();
    // To get time use this
    int x = getClk(); 
    printf("current time is %d\n", x);
    // 5. Create a data structure for processes and provide it with its parameters.
    //Done in ReadData
    // 6. Send the information to the scheduler at the appropriate time.Needs To actual send of data
    sendData(ptr);
    freequeue(&ptr);


    // 7. Clear clock resources
    wait(NULL);


    destroyClk(true);
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
    PIDsh=pid;
}
char getalgorithm(char *param){
    char algo;
    printf("Please enter the name of schedular algotithm: \n");
    printf("0)Round Robin\n");
    printf("1)Shortest Jop first\n");
    printf("2)Shortest Remaining Time First\n");
    printf("3)Premitive\n");
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
    valueofalgo=(int)algo;
    return algo;
}

Node *ReadData(int *no){
    FILE *file;
    Node *ptr;
    char *filename=NULL;
    char dataToBeRead[50]; 
    int way;
    printf("Enter way to open file ");
    scanf("%d", &way);
    if(way==1){
        filename="processes.txt";
    }else{
    printf("Please enter the name of processes you want to generate: ");
    scanf("%s", filename);
    }
    file=fopen(filename,"r");
     if ( file == NULL ) 
    { 
        printf("file failed to open." ) ; 
    } 
    else
    { 
        printf("The file is now opened.\n") ; 
        //getting the headers
        fgets(dataToBeRead,50,file);
        printf("%s",dataToBeRead);
        struct processData *temp;
        fscanf ( file, "%d",no ) ;
        printf("%d\n",*no);
        temp = (struct processData *)malloc((*no)*sizeof(struct processData));
            for(int i=0;i<(*no);i++){
                fscanf ( file, "%d", &(temp+i)->id);
                fscanf ( file, "%d", &(temp+i)->arrivaltime );
                fscanf ( file, "%d", &(temp+i)->runningtime );
                fscanf ( file, "%d", &(temp+i)->priority );
                fscanf ( file, "%d", &(temp+i)->memorysize );
                if(i==0){
                    ptr=newNode(temp+i,(temp+i)->arrivaltime);
                }else{
                    push(&ptr,temp+i,(temp+i)->arrivaltime);
                    }
            }
            
            fclose(file) ; 
             printf("Data successfully read from file\n");          
    } 
   
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
        msg.temp.id=temp->id;
        msg.temp.arrivaltime=temp->arrivaltime;
        msg.temp.priority=temp->priority;
        msg.temp.runningtime=temp->runningtime;
        msg.temp.memorysize=temp->memorysize;
        if(ptr->next){
            msg.mtype=2;
        }
        else{
            msg.mtype=1;
        }
        send_val = msgsnd(msqid, &msg, sizeof(msg),IPC_NOWAIT);
        // printf("data %d ,%d,%d,%d,%d \n",msg.temp.id,msg.temp.arrivaltime,msg.temp.runningtime,msg.temp.priority,msg.temp.memorysize);
        if(send_val == -1){
            perror("Errror in send");
        }
        printf("sent\n");
         if(valueofalgo==(int)*"2"||valueofalgo==(int)*"3"){
            kill(PIDsh,SIGCONT);
        }
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

void clearResources(int signum)
{
    //TODO Clears all resources in case of interruption
    msgctl(msqid, IPC_RMID, (struct msqid_ds *) 0);
    printf("shared memory terminating!\n");
    printf("Inturpted %d\n",getpid());

   exit(0);
}