#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include<fcntl.h>

#include<unistd.h>
#include<termios.h> // using the termios.h library
#include <wiringPi.h>
#define NUM_THREADS 2
/* create thread argument struct for thr_func() */
typedef struct _thread_data_t {
     int tid;
     double stuff;
    } thread_data_t;

/* thread function */
     void *thr_func(void *arg) {
     int t;
      printf ("Raspberry Pi wiringPi test program\n");
      wiringPiSetupGpio();
      pinMode (18, PWM_OUTPUT) ;
      pwmSetMode (PWM_MODE_MS);
      pwmSetRange (2000);
      pwmSetClock (192);
      for(t=0;t<=5;t++){

      pwmWrite(18,1);  //setting duty cycle to 1%
      usleep(100000);
      pwmWrite(18,210); //setting duty cycle to 25%

      usleep(100000);

     }
     pwmWrite(18,0);
 pthread_exit(NULL);
}
int main(int argc, char **argv) {
   int policy = SCHED_FIFO; //using first in first out policy.

   pthread_t tid;
   pthread_attr_t tattr;
   int ret;
   int newprio = 1;
   struct sched_param param;

   ret = pthread_attr_init (&tattr);
     if(ret != 0) {
     printf("pthread_attr_setinheritsched() 1 failed \n ");
     return 1;}

   ret = pthread_attr_getschedparam (&tattr, &param);
 
   if(ret != 0) {
      printf("pthread_attr_setinheritsched() 3 failed \n");
      return 1;}


   param.sched_priority = newprio;


   ret = pthread_attr_setschedpolicy(&tattr, policy);
   if(ret != 0) {
     printf("pthread_attr_setinheritsched() 5 failed \n");
     return 1;}


   ret = pthread_attr_setschedparam (&tattr, &param);
   if(ret != 0) {
      printf("pthread_attr_setinheritsched() 7 failed \n");
   }
   int i, rc;
   /* create a thread_data_t argument array */
   /* create threads */


   if ((ret = pthread_create(&tid, &tattr, thr_func, NULL))) {
   fprintf(stderr, "error: pthread_create, ret: %d\n", ret);
   return EXIT_FAILURE;
 }


 printf("-------------------------------The End---------------------------\n");
 pthread_join(tid, NULL);
 return EXIT_SUCCESS;
}
