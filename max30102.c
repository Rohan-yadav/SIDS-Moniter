#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include<fcntl.h>
#include<unistd.h>
#include<termios.h> // using the termios.h library
#include<string>
#include<iostream>
#include <wiringPi.h>
#define NUM_THREADS 2
/* create thread argument struct for thr_func() */
typedef struct _thread_data_t {
   
   int tid;
   double stuff;
} thread_data_t;

/* thread function */
void *thr_func(void *arg) {
          if(wiringPiSetup() == -1){
         printf("setup wiringPi failed !");
         exit(1);
      }
      pinMode( 29, OUTPUT );
      digitalWrite( 29, HIGH );
      sleep(1);
       unsigned char hr[100];
      int file, count;
      if ((file = open("/dev/ttyAMA0", O_RDWR | O_NOCTTY | O_NDELAY))<0) {
          perror("UART: Failed to open the file.\n");
          pthread_exit(NULL);
      }
      struct termios options; // the termios structure is vital
      tcgetattr(file, &options); // sets the parameters for the file
      // Set up the communications options:
      // 115200 baud, 8-bit, enable receiver, no modem control lines
      options.c_cflag = B115200 | CS8 | CREAD | CLOCAL;
      options.c_iflag = IGNPAR | ICRNL; // ignore parity errors
      tcflush(file, TCIFLUSH); // discard file information
      tcsetattr(file, TCSANOW, &options); // changes occur immmediately
      //unsigned char transmit[20] = "Hello Raspberry Pi!"; // send string

      sleep(29); // give the Arduino a chance to respond
      unsigned char receive[100]; // declare a buffer for receiving data
      printf("<div><h3>Give 30s to respond:/ Thank you for your paitence</div></h3>\n");

      if ((count = read(file, (void*)receive, 100))<0){ //receive data
           perror("Failed to read from the input\n");
           pthread_exit(NULL);
      }
      else if (count==0) printf("There was no data available to read!\n");
      else printf("<div><h3>The following was read in [%d]:
      %s</div></h3></para></html>\n",count,receive);
      close(file);


      digitalWrite( 29, LOW );

       pthread_exit(NULL);
}
                  
int main(int argc, char **argv) {
 
     int policy = SCHED_FIFO;

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
     while (1) {
    
     /* create threads */


     if ((ret = pthread_create(&tid, &tattr, thr_func, NULL))) {
     fprintf(stderr, "error: pthread_create, ret: %d\n", ret);
     return EXIT_FAILURE;
     }


     printf("-------------------------------The End---------------------------\n");
     pthread_join(tid, NULL);


     }
     return EXIT_SUCCESS;
}
