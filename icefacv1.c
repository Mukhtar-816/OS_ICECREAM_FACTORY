#include <stdio.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <stdlib.h>     
#include <pthread.h>    
#include <string.h>     
#include <semaphore.h> 
#include <unistd.h>    
#include <sys/types.h> 
#include <errno.h>      

sem_t mutex;
int counter;
void *handler ( void *ptr) ;
void *handler1 ( void *ptr) ;
void *handler2 ( void *ptr) ;
void *handler3 ( void *ptr) ;
void *handler4 ( void *ptr) ;

typedef struct threadstru {
    char *var1; // Cone
    char *var2; // Cream
    char *var3; // Extra Topping
    char *var4; // Special Flavor
    char *var5; // Packaging
} thread_stru;


thread_stru thrd_arg;

int main ()
{
 
counter = 0 ;

 int i[5];
    pthread_t thread_a;
    pthread_t thread_b;
    pthread_t thread_c;
    pthread_t thread_d;
    pthread_t thread_e;
    
    i[0] = 0; /* argument to threads */
    i[1] = 1;
    i[2] = 2;
    i[3] = 3;
    i[4] = 4;
    
    sem_init(&mutex, 0, 1);      
      
    int n ;               
  int j = 2 ;
  
    thrd_arg.var1 = "Cone";
    thrd_arg.var2 = "Cream";
    thrd_arg.var3 = "Extra Topping";
    thrd_arg.var4 = "Special Flavor";
    thrd_arg.var5 = "Distribution and Packaging";

   while(j != 0)
  {
    j-- ;            
    pthread_create (&thread_a, NULL, (void *) &handler,(void *)&thrd_arg);
    pthread_create (&thread_b, NULL, (void *) &handler1,(void *)&thrd_arg);
    pthread_create (&thread_c, NULL, (void *) &handler2,(void *)&thrd_arg);
    pthread_create (&thread_d, NULL, (void *) &handler3,(void *)&thrd_arg);
    pthread_create (&thread_e, NULL, (void *) &handler4,(void *)&thrd_arg);
    pthread_join(thread_a, NULL);
    pthread_join(thread_b, NULL);
    pthread_join(thread_c, NULL);
    pthread_join(thread_d, NULL);
    pthread_join(thread_e, NULL);
   }
    sem_destroy(&mutex); 
    exit(0);
}


void *handler (void *ptr)
{
    thread_stru *data;
    data=ptr;

    sem_wait(&mutex);   
    
   printf("\norder of icecream arrived at COUNTER1 'CONE COUNTER' with delay of 2 seconds\n");
   printf("COUNTER1: Now in critical region...\n");
   printf("%s counter is processing\n",data->var1) ;
   printf("ice-cream is in manufacturing process, wait please\n") ;
   sleep(2) ;
   printf("\n%s counter of icecream factory is passed Sir\n\n",data->var1) ;
     
 /* END CRITICAL REGION */
    sem_post(&mutex);       
    
    pthread_exit(0); 
}


void *handler1 (void *ptr)
{
  thread_stru *data;
  data=ptr;
    sem_wait(&mutex);       
    
   printf("\nicecream arrived at COUNTER2 'CREAM COUNTER' with delay of 2 seconds\n");
   printf("COUNTER2: Now in critical region...\n");
   printf("%s counter is processing\n",data->var2) ;
   printf("ice-cream is in manufacturing process, wait please\n") ;
   sleep(2) ;
   printf("\n%s counter of icecream factory is passed Sir\n\n",data->var2) ;
     
 /* END CRITICAL REGION */
    sem_post(&mutex);       
    
    pthread_exit(0); 
}


void *handler2 (void *ptr)
{
      thread_stru *data;
      data=ptr;

    sem_wait(&mutex);      
    
   printf("\nicecream arrived at COUNTER3 'EXTRA TOPPING COUNTER' with delay of 2 seconds\n");
   printf("COUNTER3: Now in critical region...\n");
   printf("%s counter is processing\n",data->var3) ;
   printf("ice-cream is in manufacturing process, wait please\n") ;
   sleep(2) ;
   printf("\n%s counter of icecream factory is passed Sir\n\n",data->var3) ;
     
 /* END CRITICAL REGION */
    sem_post(&mutex);      
    
    pthread_exit(0); 
}


void *handler3 (void *ptr)
{
      thread_stru *data;
      data=ptr;

    sem_wait(&mutex);       
    
   printf("\nicecream arrived at COUNTER4 'SPECIAL FLAVOURED CREAM COUNTER' with delay of 2 seconds\n");
   printf("COUNTER4: Now in critical region...\n");
   printf("%s counter is processing\n",data->var4) ;
   printf("ice-cream is in manufacturing process, wait please\n") ;
   sleep(2) ;
   printf("\n%s counter of icecream factory is passed Sir\n\n",data->var4) ;
     
 /* END CRITICAL REGION */
    sem_post(&mutex);       
    
    pthread_exit(0); 
}


void *handler4 (void *ptr)
{
    thread_stru *data;
    data=ptr;    


    sem_wait(&mutex);       
    
   printf("\nicecream arrived at COUNTER5 'PACKAGING AND SERVING COUNTER' with delay of 2 seconds\n");
   printf("COUNTER5: Now in critical region...\n");
   printf("%s counter is processing\n",data->var5) ;
   printf("ice-cream is in PACKAGING process, wait please\n") ;
   sleep(2) ;
   printf("\n%s counter of icecream factory is passed Sir\n\n",data->var5) ;
   printf("\n.....CONGRATULATION Sir, ICE-CREAM is ready.....\n\n") ; 
 /* END CRITICAL REGION */
    sem_post(&mutex);      
    
    pthread_exit(0); 
}






