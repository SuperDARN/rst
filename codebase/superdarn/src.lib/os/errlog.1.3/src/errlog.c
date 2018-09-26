/* errlog.c
   ========
   Author: R.J.Barnes
*/

/*
 LICENSE AND DISCLAIMER
 
 Copyright (c) 2012 The Johns Hopkins University/Applied Physics Laboratory
 
 This file is part of the Radar Software Toolkit (RST).
 
 RST is free software: you can redistribute it and/or modify
 it under the terms of the GNU Lesser General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 any later version.
 
 RST is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU Lesser General Public License for more details.
 
 You should have received a copy of the GNU Lesser General Public License
 along with RST.  If not, see <http://www.gnu.org/licenses/>.
 
 
 
*/

#include <sys/types.h>
#include <sys/time.h>
#include <signal.h>
#include <errno.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "tcpipmsg.h"
#include "errlog.h"

char *ErrLogStrTime() {
  char *str;
  time_t clock;
  struct tm *gmt;

  time(&clock);
  gmt = gmtime(&clock); 
  str = asctime(gmt);
  str[strlen(str)-1] = 0; /* get rid of new line */
  return str;
}

int ErrLog(int sock,char *name,char *buffer) {

  int msg,s;
  size_t nlen,blen;

  nlen=strlen(name)+1;
  blen=strlen(buffer)+1;

  msg=ERROR_MSG;

  s=TCPIPMsgSend(sock,&msg,sizeof(int));

  if (s !=sizeof(int)) {
    fprintf(stderr,"WARNING: Error not logged\n");
    fprintf(stderr,"%s: %d : %s :%s\n",ErrLogStrTime(),getpid(),name,buffer);
    return -1;
  }

  s=TCPIPMsgSend(sock,&nlen,sizeof(size_t));
  s=TCPIPMsgSend(sock,&blen,sizeof(size_t));

  s=TCPIPMsgSend(sock,name,nlen);
  s=TCPIPMsgSend(sock,buffer,blen);

  s=TCPIPMsgRecv(sock,&msg,sizeof(int));

  fprintf(stderr,"%s: %d : %s :%s\n",ErrLogStrTime(),getpid(),name,buffer);

  if ((s !=sizeof(int))  || (msg !=ERROR_OK)) {
    fprintf(stderr,"WARNING: Error not logged\n");
    return -1;
  }
  return 0;
}






