#include <stdio.h>      //if you don't use scanf/printf change this include
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>


typedef short bool;
#define true 1
#define false 0

#define SHKEY 300
#define REMKEY 400
#define key1 500
#define fin 999
#define PIDKEY 100

int sem1;
struct processData
{
    int arrivaltime;
    int priority;
    int runningtime;
    int id;
};
struct msgbuff
{
    long mtype;
    struct processData temp;
};
typedef enum{Running,Blocked,Finished,Ready}state;

struct PCBElement {

state state;
int arrivaltime;
int priority;
int runningtime;
int id;
int remainingtime;
int starttime;
int finishedtime;
int PID;
double turnarround;
double weightedturnaround;
int turn;
int waitingtime;
};

///==============================
//don't mess with this variable//
int * shmaddr;                 //
struct processData * shmaddrqueue;   
int *remainingshmaddr;   
int *PIDshared;           //

//===============================

int getPID()
{
    return *remainingshmaddr;
}
void setPID(int value)
{
 *PIDshared=value;
}
void initPID()
{
    int shmid = shmget(PIDKEY, 8, 0444);
    while ((int)shmid == -1)
    {
        //Make sure that the clock exists
        printf("Wait! The remainig memory is not initialized yet!\n");
        sleep(1);
        shmid = shmget(PIDKEY, 8, 0444);
    }
    PIDshared = (int *) shmat(shmid, (void *)0, 0);
}
int getremaining()
{
    return *remainingshmaddr;
}
void setremaining(int value)
{
 *remainingshmaddr=value;
}
void initremaining()
{
    int shmid = shmget(REMKEY, 4, 0444);
    while ((int)shmid == -1)
    {
        //Make sure that the clock exists
        printf("Wait! The remainig memory is not initialized yet!\n");
        sleep(1);
        shmid = shmget(REMKEY, 4, 0444);
    }
    remainingshmaddr = (int *) shmat(shmid, (void *)0, 0);
}


int getClk()
{
    return *shmaddr;
}

/*
 * All process call this function at the beginning to establish communication between them and the clock module.
 * Again, remember that the clock is only emulation!
*/
void initClk()
{
    int shmid = shmget(SHKEY, 4, 0444);
    while ((int)shmid == -1)
    {
        //Make sure that the clock exists
        printf("Wait! The clock not initialized yet!\n");
        sleep(1);
        shmid = shmget(SHKEY, 4, 0444);
    }
    shmaddr = (int *) shmat(shmid, (void *)0, 0);
}


struct processData * getqueuehead()
{
    return shmaddrqueue;
}
/*
 * All process call this function at the end to release the communication
 * resources between them and the clock module.
 * Again, Remember that the clock is only emulation!
 * Input: terminateAll: a flag to indicate whether that this is the end of simulation.
 *                      It terminates the whole system and releases resources.
*/

void destroyClk(bool terminateAll)
{
    shmdt(shmaddr);
    if (terminateAll)
    {
        killpg(getpgrp(), SIGINT);
    }
}
void destroyREm(bool terminateAll)
{
    shmdt(remainingshmaddr);
    if (terminateAll)
    {
        killpg(getpgrp(), SIGINT);
    }
}
void destroyshmaddr(bool terminateAll)
{
    shmdt(shmaddrqueue);
    if (terminateAll)
    {
        killpg(getpgrp(), SIGINT);
    }
}

// Node 
typedef struct node { 
    struct processData *data; 
  
    // Lower values indicate higher priority 
    int priority; 
  
    struct node* next; 
  
} Node; 
  
// Function to Create A New Node 
Node* newNode(struct processData *d, int p) 
{ 
    Node* temp = (Node*)malloc(sizeof(Node)); 
    temp->data = d; 
    temp->priority = p; 
    temp->next = NULL; 
  
    return temp; 
} 
  
// Return the value at head 
struct processData  *peek(Node** head) 
{ 
    return (*head)->data; 
} 
  
// Removes the element with the 
// highest priority form the list 
void pop(Node** head) 
{ 
    Node* temp = *head; 
    (*head) = (*head)->next; 
    free(temp);
} 
  
// Function to push according to priority 
void push(Node** head,struct processData  *d, int p) 
{ 
    Node* start = (*head); 
  
    // Create new Node 
    Node* temp = newNode(d, p); 
  
    // Special Case: The head of list has lesser 
    // priority than new node. So insert new 
    // node before head node and change head node. 
    if ((*head)->priority > p) { 
  
        // Insert New Node before head 
        temp->next = *head; 
        (*head) = temp; 
    } 
    else { 
  
        // Traverse the list and find a 
        // position to insert new node 
        while (start->next != NULL && 
               start->next->priority < p) { 
            start = start->next; 
        } 
  
        // Either at the ends of the list 
        // or at required position 
        temp->next = start->next; 
        start->next = temp; 
    } 
} 
void freequeue(Node **head){
    while((*head))
    {
        Node* temp = *head; 
        (*head) = (*head)->next; 
        temp=NULL;
        free(temp);
    }
}
// Function to check is list is empty 
int isEmpty(Node** head) 
{ 
    return (*head) == NULL; 
} 
////////////////////////////////////////////
typedef struct nodePCB { 
    struct PCBElement *data; 
  
    // Lower values indicate higher priority 
    int priority; 
  
    struct nodePCB* next; 
  
} NodePCB; 
  
// Function to Create A New Node 
NodePCB* newNodePCB(struct PCBElement *d, int p) 
{ 
    NodePCB* temp = (NodePCB*)malloc(sizeof(NodePCB)); 
    temp->data = d; 
    temp->priority = p; 
    temp->next = NULL; 
  
    return temp; 
} 
  
// Return the value at head 
struct PCBElement  *peekPCB(NodePCB** head) 
{ 
    return (*head)->data; 
} 
  
// Removes the element with the 
// highest priority form the list 
void popPCB(NodePCB** head) 
{ 
    NodePCB* temp = *head; 
    (*head) = (*head)->next; 
    temp=NULL;
    free(temp);
} 
  
// Function to push according to priority 
void pushPCB(NodePCB** head,struct PCBElement  *d, int p) 
{ 
    if(!(*head)){
               (*head)=newNodePCB(d,p); 
            return;
    }
    NodePCB* start = (*head); 
  
    // Create new Node 
    NodePCB* temp = newNodePCB(d, p); 
  
    // Special Case: The head of list has lesser 
    // priority than new node. So insert new 
    // node before head node and change head node. 
    if ((*head)->priority > p) { 
  
        // Insert New Node before head 
        temp->next = *head; 
        (*head) = temp; 
    } 
    else { 
  
        // Traverse the list and find a 
        // position to insert new node 
        while (start->next != NULL && 
               start->next->priority < p) { 
            start = start->next; 
        } 
  
        // Either at the ends of the list 
        // or at required position 
        temp->next = start->next; 
        start->next = temp; 
    } 
} 
  
// Function to check is list is empty 
int isEmptyPCB(NodePCB** head) 
{ 
    return (*head) == NULL; 
} 
void freePCB(NodePCB **head){
    while((*head))
    {
        NodePCB* temp = *head; 
        (*head) = (*head)->next; 
        temp=NULL;
        free(temp);
    }
}
//////////////////////////
void initqueue()
{
    
int shmid = shmget(key1,sizeof(struct processData), 0666);
    while((int) shmid==-1){
        printf("Wait! The process not initialized yet!\n");
        sleep(1);
        shmid = shmget(key1,sizeof(struct processData), 0666);
    }
   shmaddrqueue = (struct processData *)shmat(shmid, NULL, 0);
    
}
// typedef struct nodelinked { 
//     struct PCBElement *element; 
  
//     // Lower values indicate higher priority 
  
//     struct node* next; 
  
// } NodeLinked; 
// NodeLinked* newNode(struct PCBElement *data) 
// { 
//     NodeLinked* temp = (NodeLinked*)malloc(sizeof(NodeLinked)); 
//     temp->data = data; 
//     temp->next = NULL; 
//     return temp; 
// } 
// void pushLinked(NodeLinked **head,struct PCBElement *data){
//     NodeLinked* temp = (NodeLinked*)malloc(sizeof(NodeLinked)); 
//     Node* temphead = (*head);    
//      while(temphead){
//         if(!(temphead->next)){
//                 temp->data = data; 
//                 temp->next = NULL; 
//                 temphead->next=temp;
//             break;
//         }
//         temphead=temphead->next;
//     }

// }
// void swap(NodeLinked **head,struct processData *ptr)
// {
//     NodeLinked *temphead=(*head);
//     while(temphead){
//         if(temphead->element->data->id==ptr->id){
//             NodeLinked* temp = (NodeLinked*)malloc(sizeof(NodeLinked)); 
//             temp=(*head);
//             temphead=head
//         }
//     }

// }
    /*void initClk()

    int shmid = shmget(SHKEY, 4, 0444);
    while ((int)shmid == -1)
    {
        //Make sure that the clock exists
        printf("Wait! The clock not initialized yet!\n");
        sleep(1);
        shmid = shmget(SHKEY, 4, 0444);
    }
    shmaddr = (int *) shmat(shmid, (void *)0, 0);
}*/
union Semun
{
    int val;                /* value for SETVAL */
    struct semid_ds *buf;   /* buffer for IPC_STAT & IPC_SET */
    ushort *array;          /* array for GETALL & SETALL */
    struct seminfo *__buf;  /* buffer for IPC_INFO */
    void *__pad;
};

int create_sem(int key, int initial_value)
{
    union Semun semun;

    int sem = semget(key, 1, 0666|IPC_CREAT);

    if(sem == -1)
    {
        perror("Error in create sem");
        exit(-1);
    }

    semun.val = initial_value;  /* initial value of the semaphore, Binary semaphore */
    if(semctl(sem, 0, SETVAL, semun) == -1)
    {
        perror("Error in semctl");
        exit(-1);
    }
    
    return sem;
}

void destroy_sem(int sem)
{
    if(semctl(sem, 0, IPC_RMID) == -1)
    {
        perror("Error in semctl");
        exit(-1);
    }
}

void down(int sem)
{
    struct sembuf p_op;

    p_op.sem_num = 0;
    p_op.sem_op = -1;
    p_op.sem_flg = !IPC_NOWAIT;

    if(semop(sem, &p_op, 1) == -1)
    {
        perror("Error in down()");
        exit(-1);
    }
}

void up(int sem)
{
    struct sembuf v_op;

    v_op.sem_num = 0;
    v_op.sem_op = 1;
    v_op.sem_flg = !IPC_NOWAIT;

    if(semop(sem, &v_op, 1) == -1)
    {
        perror("Error in up()");
        exit(-1);
    }
}

void intializesem(){
    sem1=create_sem(IPC_PRIVATE, 0);
}