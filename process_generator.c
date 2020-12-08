#include "headers.h"


void clearResources(int);
Node *ReadData();
void IntiatClk();
void IntialtSchedular(char algorithm,char *param);
char getalgorithm(char *param);
void sendData(Node *ptr);

int main(int argc, char * argv[])
{
    signal(SIGINT, clearResources);
    // 1. Read the input files.//Done
    Node *ptr= ReadData();
    // 2. Ask the user for the chosen scheduling algorithm and its parameters, if there are any.Done
    char algorithm,param[4];
    algorithm=getalgorithm(param);
    // 3. Initiate and create the scheduler and clock processes.Done
    IntialtSchedular(algorithm,param);
    IntiatClk();
    
    // 4. Use this function after creating the clock process to initialize clock
    initClk();
    // To get time use this
    int x = getClk(); 
    printf("current time is %d\n", getClk());
    // TODO Generation Main Loop
    // 5. Create a data structure for processes and provide it with its parameters.
    //Done in ReadData
    // 6. Send the information to the scheduler at the appropriate time.Needs To actual send of data
    sendData(ptr);
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
        scanf("%s", param);
        return algo;
    }
    param[0]='0';
    return algo;
}

Node *ReadData(){
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
        int no;
        printf("The file is now opened.\n") ; 
        //getting the headers
        fgets(dataToBeRead,50,file);
        
        struct processData *temp;
        fscanf ( file, "%d",&no ) ;
        temp = (struct processData *)malloc(no*sizeof(struct processData));
            for(int i=0;i<no;i++){
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
void sendData(Node*ptr){
    struct processData *temp;
    temp=peek(&ptr);
    while(ptr!=NULL){
        //send int to algo or add it to shared 
    /*printf("AT %d\n",temp->arrivaltime);
    printf("Clk %d\n",getClk());*/

       int x=getClk();
        if(temp->arrivaltime==x){
        pop(&ptr);
        printf("poped %d\n",temp->id);
         if(ptr!=NULL){
        temp=peek(&ptr);
        while(ptr!=NULL&&temp->arrivaltime==x){
            pop(&ptr);
            printf("poped %d\n",temp->id);
            temp=peek(&ptr);
        }
        }
        }else{
             temp=peek(&ptr);
        }
        sleep(1);
       
    }
}


void clearResources(int signum)
{
    //TODO Clears all resources in case of interruption
}