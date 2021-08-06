/* shmemrecv.c
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

