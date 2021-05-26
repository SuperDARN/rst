/* fitcnx.c
   =========
   Author: R.J.Barnes
*/

/*
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
*/ 


#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>
#include <zlib.h>
#include "rtypes.h"
#include "dmap.h"
#include "rprm.h"
#include "fitdata.h"
#include "connex.h"
#include "decodemsg.h"

int FitCnxRead(int num,int *sock,struct RadarParm *prm,
	       struct FitData *fit,int *flag,
	       struct timeval *tmout) {
  fd_set read_set;
  unsigned char *buffer=NULL;
  int status=0,size=0,i=0;
  
  FD_ZERO(&read_set);

  for (i=0;i<num;i++) {
    if (sock[i] !=-1) FD_SET(sock[i],&read_set);
    flag[i]=0;
  }

  if ((status=select(FD_SETSIZE,&read_set,0,0,tmout)) < 0) { 
     perror("while waiting client/master I/O");
     return -1;
  }

  if (status==0) return 0;

  for (i=0;i<num;i++) {
    if ((sock[i] !=-1) && (FD_ISSET(sock[i],&read_set))) {
      size=ConnexReadIP(sock[i],&buffer);
      if (size==-1) return flag[i]=-1;
      if (buffer !=NULL) {
        flag[i]=FitCnxDecodeIPMsg(&prm[i],&fit[i],buffer,size)==0;
        free(buffer);
      }
    } 
  }    
  return status;
}
   

 













