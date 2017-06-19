/* shmemrecv.c
   =========== */


#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <string.h>

#include "shmem.h"


char *memory="membuffer";
int shmemfd;
unsigned char *shm;

int main(int argc,char *argv[]) {

  char date[256];

  while (1) {
  
    shm=ShMemAlloc(memory,256,O_RDWR,0,&shmemfd);
    if (shm==NULL) break;
    strcpy( date,(char *) shm); 

    ShMemFree(shm,memory,256,0,shmemfd);

    fprintf(stdout,"%s",date);
    
    sleep(1);

  }

  return 0;

}

