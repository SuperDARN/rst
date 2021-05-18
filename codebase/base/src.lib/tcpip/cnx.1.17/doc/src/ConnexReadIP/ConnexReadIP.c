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
#include "connex.h"

int main(int argc,char *argv[]) {
  int sock;
  int port=0;
  char *host=NULL;
  int size;
  unsigned char *buffer=NULL;
 
  
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
   size=ConnexReadIP(sock,&buffer);
   if (size==-1) break;
   fprintf(stderr,"Message size:%d\n",size);
     
  } while(1);
  fprintf(stderr,"Connection failed.\n");
  return 0;

}
   

 










