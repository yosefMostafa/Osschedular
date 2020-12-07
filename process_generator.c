#include "headers.h"

void clearResources(int);
struct processData *ReadData();

struct processData
{
    int arrivaltime;
    int priority;
    int runningtime;
    int id;
};

int main(int argc, char * argv[])
{
    signal(SIGINT, clearResources);
    // TODO Initialization
    // 1. Read the input files.
    struct processData *ptr= ReadData();
   
    // 2. Ask the user for the chosen scheduling algorithm and its parameters, if there are any.
    // 3. Initiate and create the scheduler and clock processes.
    int pid =fork();
    if (pid==0){
    char *binaryPath = "./clk.out";
    char *args[] = {binaryPath,NULL} ;
    execvp(binaryPath,args);   
    }
    
    // 4. Use this function after creating the clock process to initialize clock
    initClk();
    // To get time use this
    int x = getClk(); 

    printf("current time is %d\n", getClk());
    // TODO Generation Main Loop
    // 5. Create a data structure for processes and provide it with its parameters.
    // 6. Send the information to the scheduler at the appropriate time.
    // 7. Clear clock resources
    destroyClk(true);
}
struct processData *ReadData(){
    FILE *file;
    struct processData *ptr;
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
        int no;
        int temp;
        fscanf ( file, "%d",&no ) ;
        ptr = (struct processData *)malloc(no * sizeof(struct processData));
            for(int i=0;i<no;i++){
                fscanf ( file, "%d", &temp );
                (ptr+i)->id=temp;
                fscanf ( file, "%d", &temp );
                (ptr+i)->arrivaltime=temp;
                fscanf ( file, "%d", &temp );
                (ptr+i)->runningtime=temp;
                fscanf ( file, "%d", &temp );
                (ptr+i)->priority=temp;

            }
            
            fclose(file) ; 
          
        printf("Data successfully read from file\n"); 
    } 
    return ptr;
}
void clearResources(int signum)
{
    //TODO Clears all resources in case of interruption
}