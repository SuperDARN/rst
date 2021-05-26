/* ConnexRead.c
   ============
   Author: R.J.Barnes
 Copyright (c) 2012 The Johns Hopkins University/Applied Physics Laboratory

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
along with this program. If not, see <https://www.gnu.org/licenses/>.

Modifications:



#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include "connex.h"

int main(int argc,char *argv[]) {
  int n,num;
  int sock[10];
  int port[10];
  char *host[10];
  int flag[10],status,size[10];
  unsigned char *buffer[10];
  struct timeval tout;
 
  
  if (argc<3) { 
    fprintf(stderr,"host and port must be specified.\n");
    exit(-1);
  }

  num=0;
  for (n=1;n<argc;n+=2) {
    host[num]=argv[n];
    port[num]=atoi(argv[n+1]);
    flag[num]=0;
    size[num]=0;
    buffer[num]=0;
    num++;
  } 

  for (n=0;n<num;n++) {
    sock[n]=ConnexOpen(host[n],port[n]); 
    if (sock[n]<0) fprintf(stderr,"Could not connect to host.\n");
  }

  do {
   tout.tv_sec = 5;
   tout.tv_usec = 0;
   status=ConnexRead(num,sock,buffer,size,flag,&tout);
   if (status==-1) break;
   for (n=0;n<num;n++) 
     if (flag[n]==1) fprintf(stderr,"Server %s:%d Size:%d\n",
                            host[n],port[n],size[n]);     
  } while(1);
  fprintf(stderr,"Connection failed.\n");
  return 0;

}
   

 










