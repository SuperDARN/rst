/* shmemsend.c
   =========== 
   Copyright (C) <year>  <name of author>

This file is part of the Radar Software Toolkit (RST).

RST is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.

Modifications:
   */


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

    fprintf(stdout,"%s",date);
    
    strcpy( (char *) shm,date);
    
    sleep(1);
 
  }

  endprog(0);
  return 0;

}

