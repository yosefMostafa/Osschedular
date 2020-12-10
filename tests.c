//schedular main
// while(true){
    // pthread_mutex_lock(&count_mutex);
    //     temp=getqueuehead();
    //     //init condition
    //     if(temp->id!=0){
    //         processidflag=temp->id;
    //     }
    //     // recieve condition 
    //     if(processidflag!=0){

    //         if(prevflag!=temp->id){

    //             printf("Resieved %d\n",(temp)->id);
    //             prevflag=temp->id;

    //     if((int)*argv[1]==(int)*"1")
    //     {

    //     addtoSJFqueue(temp,&head,flagalgo);
    //     flagalgo=1;

    //     }

    // }



    //     if((int)*argv[1]==(int)*"1"){
    //     SJF(&head);
    //     }
    // }
            
        

       
    //     //break condition
    //     if(getClk()==64){
    //         break;
    //     }
    // pthread_mutex_unlock(&count_mutex);
    // }
    //////SJF
    //     struct PCBElement *temp;
//     int pid;
//     int rem=*shmaddrrem;
// if (rem==0){
//         // tempopPCB(head);
//     if((*head)){
       
//     temp=peekPCB(head);
//     printf("New %d \n",temp->id);
//     *shmaddrrem=temp->runningtime;
//     // printf("Finished %d \n",temp->id);
//     Intializeprocess(head);
//     printf("Started %d \n",temp->id);
//     popPCB(head);
//     // temp->state=Finished;
//     // pushPCB(head,temp,-1);
//     }
// }
// //  }else if((*head)){
// //      (*head)->data->remainingtime=(*shmaddrrem);
// //  }
/////////////////SendData Shared memory
// void sendData(Node*ptr){
//     pthread_mutex_t count_mutex;
//     struct processData *temp;
//     shmid2 = shmget(key,sizeof(struct processData), IPC_CREAT | 0666); 
//     struct processData * shmaddr = (struct processData *)shmat(shmid2,NULL, 0);
//     int x;
//     temp=peek(&ptr);
//     while(ptr!=NULL){
//     pthread_mutex_lock(&count_mutex);
    
//         //send int to algo or add it to shared 
//     /*printf("AT %d\n",temp->arrivaltime);  
//     printf("Clk %d\n",getClk());*/

//         x=getClk();
//         if(temp->arrivaltime==x){
//                     pop(&ptr);
//                     sendprocess(shmaddr,temp);
//          if(ptr!=NULL){
//                 temp=peek(&ptr);
//         while(ptr!=NULL&&temp->arrivaltime==x){
//                     sleep(0.1);
//                     pop(&ptr);
//                     sendprocess(shmaddr,temp);
//                     temp=peek(&ptr);
//             }
//         }
//         }else{
//              temp=peek(&ptr);
//         }
//      pthread_mutex_unlock(&count_mutex);
//     }
    
// }