#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <wiringPi.h>
#include <stdint.h>
#define MAXTIMINGS 85
#define DHTPIN 7
#define PIRPin 25
int dht11_dat[5] = { 0, 0, 0, 0, 0 };

/* create thread argument struct for thr_func() */
typedef struct _thread_data_t {
   int tid;
   double stuff;
} thread_data_t;

/* thread function */

void *dht11(void *arg) {
      uint8_t laststate = HIGH;
     uint8_t counter = 0;
     uint8_t j = 0, i;
     float f;
     dht11_dat[0] = dht11_dat[1] = dht11_dat[2] = dht11_dat[3] =
     dht11_dat[4] = 0;
     wiringPiSetup () ;
     pinMode( DHTPIN, OUTPUT );
     digitalWrite( DHTPIN, LOW );
     delay( 18 );
     digitalWrite( DHTPIN, HIGH );
     delayMicroseconds( 40 );
     pinMode( DHTPIN, INPUT );
     for ( i = 0; i < MAXTIMINGS; i++ )
     {
     counter = 0;

     while ( digitalRead( DHTPIN ) == laststate )
     {
         counter++;
         delayMicroseconds( 1 );
           if ( counter == 255 )
           {
           break;
           }
          }
          laststate = digitalRead( DHTPIN );
          if ( counter == 255 )
          break;
          if ( (i >= 4) && (i % 2 == 0) )
          {
          dht11_dat[j / 8] <<= 1;
          if ( counter > 50 )
          dht11_dat[j / 8] |= 1;
          j++;
          }
      }
      if ( (j >= 40) &&
       (dht11_dat[4] == ( (dht11_dat[0] + dht11_dat[1] + dht11_dat[2] +
      dht11_dat[3]) & 0xFF) ) )
      {
        f = dht11_dat[2] * 9. / 5. + 32;

        printf( "<div><h3>Humidity = %d.%d %% Temperature = %d.
        %d C (%.1f F)</h3></div>\n",
        dht11_dat[0], dht11_dat[1], dht11_dat[2], dht11_dat[3], f );
        }else {
        printf( "Data not good, skip\n" );
        delay(10);
      }
       pthread_exit(NULL);
}
void *pir(void *arg) {
    wiringPiSetup () ;
   int t=1;
   //char command[] = "clear";
   if(wiringPiSetup() == -1){
     printf("setup wiringPi failed !");
     exit(1);
   }
   pinMode(PIRPin,INPUT);
   if(!(digitalRead(PIRPin))){
     printf("<div><h3>Motion not detected</div></h3>\n");
    }
   else{
     printf("<div><h3>Someone is around baby</div></h3>\n");
   }
    pthread_exit(NULL);

}
int main(int argc, char **argv) {
 int policy = SCHED_FIFO;

 pthread_t tid,tid1;
 pthread_attr_t tattr,tattr1;
 int ret,ret1;
 int newprio = 1;
 int newprio1 = 15;
 struct sched_param param,param1;

 ret = pthread_attr_init (&tattr);
 if(ret != 0) {
   printf("pthread_attr_setinheritsched() 1 failed \n ");
   return 1;}
 ret1 = pthread_attr_init (&tattr1);
 if(ret1 != 0) {
   printf("pthread_attr_setinheritsched() 2 failed \n");
   return 1;}

 ret = pthread_attr_getschedparam (&tattr, &param);
 if(ret != 0) {
   printf("pthread_attr_setinheritsched() 3 failed \n");
   return 1;}
 ret1 = pthread_attr_getschedparam (&tattr1, &param1);
 if(ret1 != 0) {
   printf("pthread_attr_setinheritsched() 4 failed \n");
   return 1;}

 param.sched_priority = newprio;
 param1.sched_priority = newprio1;

 ret = pthread_attr_setschedpolicy(&tattr, policy);
 if(ret != 0) {
   printf("pthread_attr_setinheritsched() 5 failed \n");
   return 1;}
 ret = pthread_attr_setschedpolicy(&tattr1, policy);
 if(ret != 0) {
   printf("pthread_attr_setinheritsched() 6 failed \n");
   return 1;}

 ret = pthread_attr_setschedparam (&tattr, &param);
 if(ret != 0) {
   printf("pthread_attr_setinheritsched() 7 failed \n");
   return 1;}
 ret1 = pthread_attr_setschedparam (&tattr1, &param1);
 if(ret1 != 0) {
   printf("pthread_attr_setinheritsched() 8 failed \n");
   return 1;}
 int i, rc;
 /* create a thread_data_t argument array */
 while (1) {

   /* create threads */


   if ((ret = pthread_create(&tid, &tattr, dht11, NULL))) {
     fprintf(stderr, "error: pthread_create, ret: %d\n", ret);
     return EXIT_FAILURE;
   }

   printf("priority=%d",param1.sched_priority);

   if ((ret1 = pthread_create(&tid1, &tattr1, pir, NULL))) {
     fprintf(stderr, "error: pthread_create, ret: %d\n", ret);
     return EXIT_FAILURE;
   }

   pthread_join(tid, NULL);
   pthread_join(tid1, NULL);
   printf("-------------------------------The End---------------------------\n");

 }
 return EXIT_SUCCESS;
}
