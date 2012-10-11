/* shmemsend.c
   =========== */


#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <string.h>

#include "shmem.h"


 char *memory="membuffer";
int shmemfd;
unsigned char *shm;

void endprog(int signum) {
  ShMemFree(shm,memory,256,1,shmemfd);
  exit(0);
}

int main(int argc,char *argv[]) {

  char *memory="IQbuffer";

  char *date;
  time_t ltime;
  struct tm *time_of_day;

  signal(SIGINT, endprog);


  shm=ShMemAlloc(memory,256,O_RDWR | O_CREAT,1,&shmemfd);

  while (1) {

    time(&ltime);  
    time_of_day=localtime(&ltime);

    date=asctime(time_of_day);  

    fprintf(stdout,date);
    
    strcpy( (char *) shm,date);
    
    sleep(1);
 
  }

  endprog(0);
  return 0;

}

