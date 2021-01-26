// //schedular main
// // while(true){
//     // pthread_mutex_lock(&count_mutex);
//     //     temp=getqueuehead();
//     //     //init condition
//     //     if(temp->id!=0){
//     //         processidflag=temp->id;
//     //     }
//     //     // recieve condition 
//     //     if(processidflag!=0){

//     //         if(prevflag!=temp->id){

//     //             printf("Resieved %d\n",(temp)->id);
//     //             prevflag=temp->id;

//     //     if((int)*argv[1]==(int)*"1")
//     //     {

//     //     addtoSJFqueue(temp,&head,flagalgo);
//     //     flagalgo=1;

//     //     }

//     // }



//     //     if((int)*argv[1]==(int)*"1"){
//     //     SJF(&head);
//     //     }
//     // }

//     printf("pair {%d,%d} \n\n",ptr->start,ptr->end);
//     int size =(ptr->end)-ptr->start;
//     int n=ceil(log(size)/log(2));
//     printf("size %d \n",size);
//     if (index==-1){
//     int arraysize=ceil(log(memorycapacity)/log(2));
//     printf("n value %d \n",n);
//     // int Number;
//     // Number = ptr->start / size;  
//     // int nnew =n +Number%n;
//     // int f =0;
//     // if(flag==0)
//     // {
//     // if(nnew<=arraysize)
//     // {
//     //     int value=(pair+nnew)->end-(pair+nnew)->start;
//     //     if(value==0)
//     //     {
//     //             (pair+nnew)->start=ptr->start;
//     //             (pair+nnew)->end=ptr->end+1;
//     //             f=1;
//     //             n=nnew;
//     //     }else{
//     //             (pair+n)->start=ptr->start;
//     //             (pair+n)->end=ptr->end+1;
    
//     //         }
//     //     }
//     // }
    
//     int i;
//     for (i=n;i<arraysize;i++)
//     {
//         int value=(pair+i)->end-(pair+i)->start;
//         if (value!=0)
//             break;
//     }
//     int i2;
//      for (i2=n;i2>=0;i2--)
//     {
//         int value=(pair+i2)->end-(pair+i2)->start;
//         if (value!=0)
//             break;
//     }
//     // n=i-1;  
//     if(i2!=-1)
//     {
//          printf("i2 value %d \n",i2);
//          if(i2==n){
//         if((pair+i2)->end==ptr->start){
        
//         (pair+i2+1)->start=ptr->start;(pair+i2+1)->end=ptr->end;
//         n=i2;
//         size=(pair+i2)->end-(pair+i2)->start;
//         size--;
//             printf("size %d \n",size);
//           for(int i=0;i<11;i++){
//                 printf("pair {%d,%d}",(pair+i)->start,(pair+i)->end);
//             }
//             printf("\n");
//         }
//          }else{
//              n=i-1;
//              if(n<i2){
//                   for (i=n;i<arraysize;i++)
//             {
//                     int value=(pair+i)->end-(pair+i)->start;
//                     if (value!=0)
//                         break;
//             }
//             n=i-1;
//              }
//              (pair+n)->start=ptr->start;
//              (pair+n)->end=ptr->end;
            
//          }
//     }else{
//              n=i-1;
//              (pair+n)->start=ptr->start;
//              (pair+n)->end=ptr->end;
//          }
        
    
        
  
    
//     }else{
//         n =index;
//         (pair+n)->start=ptr->start;
//         (pair+n)->end=ptr->end;
//     }

  
//     printf("n value %d \n",n);

//     if(flag==1){
//         size--;
//     }
//     flag=1;
 
//         if(ptr->end==memorycapacity){
//              ptr->end=1024;
//             return;
//             }   
//         int value=(pair+n+1)->end-(pair+n+1)->start;
//         printf("value %d\n",value);
         
//         if(value!=0&&value==(size+1)){
//             (pair+n+1)->start=(pair+n)->start;
//             (pair+n)->end=0;(pair+n)->start=0;
//                 for(int i=0;i<11;i++){
//                 printf("pair {%d,%d} \n",(pair+i)->start,(pair+i)->end);
//                  }
//                         printf("\n");

//             clearRam((pair+n+1),flag,n+1);
//         }else
//             return;
    
    

     
            
        

       
//     //     //break condition
//     //     if(getClk()==64){
//     //         break;
//     //     }
//     // pthread_mutex_unlock(&count_mutex);
//     // }
//     //////SJF
//     //     struct PCBElement *temp;
// //     int pid;
// //     int rem=*shmaddrrem;
// // if (rem==0){
// //         // tempopPCB(head);
// //     if((*head)){
       
// //     temp=peekPCB(head);
// //     printf("New %d \n",temp->id);
// //     *shmaddrrem=temp->runningtime;
// //     // printf("Finished %d \n",temp->id);
// //     Intializeprocess(head);
// //     printf("Started %d \n",temp->id);
// //     popPCB(head);
// //     // temp->state=Finished;
// //     // pushPCB(head,temp,-1);
// //     }
// // }
// // //  }else if((*head)){
// // //      (*head)->data->remainingtime=(*shmaddrrem);
// // //  }
// /////////////////SendData Shared memory
// // void sendData(Node*ptr){
// //     pthread_mutex_t count_mutex;
// //     struct processData *temp;
// //     shmid2 = shmget(key,sizeof(struct processData), IPC_CREAT | 0666); 
// //     struct processData * shmaddr = (struct processData *)shmat(shmid2,NULL, 0);
// //     int x;
// //     temp=peek(&ptr);
// //     while(ptr!=NULL){
// //     pthread_mutex_lock(&count_mutex);
    
// //         //send int to algo or add it to shared 
// //     /*printf("AT %d\n",temp->arrivaltime);  
// //     printf("Clk %d\n",getClk());*/

// //         x=getClk();
// //         if(temp->arrivaltime==x){
// //                     pop(&ptr);
// //                     sendprocess(shmaddr,temp);
// //          if(ptr!=NULL){
// //                 temp=peek(&ptr);
// //         while(ptr!=NULL&&temp->arrivaltime==x){
// //                     sleep(0.1);
// //                     pop(&ptr);
// //                     sendprocess(shmaddr,temp);
// //                     temp=peek(&ptr);
// //             }
// //         }
// //         }else{
// //              temp=peek(&ptr);
// //         }
// //      pthread_mutex_unlock(&count_mutex);
// //     }
    
// // }