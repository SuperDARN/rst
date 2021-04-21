/* ConnexReadIP.c
   ==============
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
#include <sys/types.h>
#include <unistd.h>
#include "connex.h"

unsigned char ibuf[4096];
int isze=4096;
int iptr=0;

int main(int argc,char *argv[]) {
  int sock;
  int port=0;
  char *host=NULL;
  int size,status,cnt=0,i;
  unsigned char *buffer=NULL;

  fd_set read_set;
  
  
  if (argc<3) { 
    fprintf(stderr,"host and port must be specified.\n");
    exit(-1);
  }

  host=argv[1];
  port=atoi(argv[2]);
 
  sock=ConnexOpen(host,port); 
  if (sock<0) {
    fprintf(stderr,"Could not connect to host.\n");
    exit(-1);
  }
  do {

    FD_ZERO(&read_set);
    FD_SET(sock,&read_set);
    status=select(FD_SETSIZE,&read_set,0,0,NULL); 
   
    if (status==0) continue;
  if (FD_ISSET(sock,&read_set)) {
       /* start filling the input buffer */
       size=read(sock,ibuf+iptr,256);
       iptr+=size;
       fprintf(stderr,"Input buffer size=%d\n",iptr);
       fprintf(stderr,"Packet count=%d\n",cnt);
       cnt++;
    }

    if ((cnt>0) && ((cnt % 4)==0)) {
      /* decode the memory buffer */

      fprintf(stderr,"Decoding packets:\n");
      i=0;
      do {
        size=ConnexReadMem(ibuf+i,iptr-i,&buffer);
        if (size==0) break;
        fprintf(stderr,"Message:%s\n",buffer);
        i+=size+8;   
      } while (i<iptr);
      iptr=0;
    }
  } while(1);
  fprintf(stderr,"Connection failed.\n");
  return 0;

}
   

 










