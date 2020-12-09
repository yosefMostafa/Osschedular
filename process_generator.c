#include "headers.h"


void clearResources(int);
Node *ReadData(int *no);
void IntiatClk();
void IntialtSchedular(char algorithm,char *param,int no);
char getalgorithm(char *param);
void sendData(Node *ptr,int *no);
void sendprocess(struct processData *shmaddr,struct processData *temp,int *counter);

int shmid2;

int main(int argc, char * argv[])
{
    signal(SIGINT, clearResources);
    // 1. Read the input files.//Done
    int numprocesses;
    Node *ptr= ReadData(&numprocesses);
    // 2. Ask the user for the chosen scheduling algorithm and its parameters, if there are any.Done
    char algorithm,param[4];
    algorithm=getalgorithm(param);
    // 3. Initiate and create the scheduler and clock processes.Done
    IntialtSchedular(algorithm,param,numprocesses);
    IntiatClk();
    
    // 4. Use this function after creating the clock process to initialize clock
    initClk();
    // To get time use this
    int x = getClk(); 
    printf("current time is %d\n", getClk());
    // 5. Create a data structure for processes and provide it with its parameters.
    //Done in ReadData
    // 6. Send the information to the scheduler at the appropriate time.Needs To actual send of data
    sendData(ptr,&numprocesses);
    // 7. Clear clock resources
    destroyClk(true);
}





void IntiatClk(){
     int pid =fork();
    if (pid==0){
    char *binaryPath = "./clk.out";
    char *args[] = {binaryPath,NULL} ;
    execvp(binaryPath,args);   
    }
}
void IntialtSchedular(char algorithm,char* param,int no){
     int pid =fork();
    if (pid==0){
    char nupro=(char) no;
    char *binaryPath = "./scheduler.out";
    char *args[] = {binaryPath,&algorithm,param,&nupro,NULL} ;
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
        scanf("%s", param);
        return algo;
    }
    param[0]='0';
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
void sendData(Node*ptr,int *no){
    pthread_mutex_t count_mutex;
    struct processData *temp;
    int counter=0;
    shmid2 = shmget(key, (*no)*sizeof(struct processData), IPC_CREAT | 0666); 
    struct processData * shmaddr = (struct processData *)shmat(shmid2,NULL, 0);
    int x;
    temp=peek(&ptr);
    while(ptr!=NULL){
    pthread_mutex_lock(&count_mutex);
    
        //send int to algo or add it to shared 
    /*printf("AT %d\n",temp->arrivaltime);  
    printf("Clk %d\n",getClk());*/

        x=getClk();
        if(temp->arrivaltime==x){
                    pop(&ptr);
                    sendprocess(shmaddr,temp,&counter);
         if(ptr!=NULL){
                temp=peek(&ptr);
        while(ptr!=NULL&&temp->arrivaltime==x){
                    pop(&ptr);
                    sendprocess(shmaddr,temp,&counter);
                    temp=peek(&ptr);
            }
        }
        }else{
             temp=peek(&ptr);
        }
     pthread_mutex_unlock(&count_mutex);
    }
    
}
void sendprocess(struct processData *shmaddr,struct processData *temp,int *counter){
            printf("sent %d\n",temp->id);
            (shmaddr+*counter)->id=temp->id;(shmaddr+*counter)->arrivaltime=temp->arrivaltime;
            (shmaddr+*counter)->priority=temp->priority;(shmaddr+*counter)->runningtime=temp->runningtime;
    //         for(int i=0;i<10;i++){
    // printf("process id %d\n",(shmaddr+i)->id);
    // }
            *counter=*counter+1;

}



void clearResources(int signum)
{
    //TODO Clears all resources in case of interruption
    shmctl(shmid2, IPC_RMID, NULL);
    printf("queue terminating!\n");
    exit(0);
}