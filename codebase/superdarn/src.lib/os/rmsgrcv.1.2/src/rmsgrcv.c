/* rmsgrcv.c
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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <fcntl.h>
#include "rtypes.h"
#include "rmsg.h"
#include "tcpipmsg.h"

char RMsgRcvDecodeOpen(int sock,size_t *size,unsigned char **store) {

  int s;
  size_t buflen=0;
  unsigned char *bufadr=NULL;

  s=TCPIPMsgRecv(sock,&buflen,sizeof(size_t));
  if (s !=sizeof(size_t)) return TASK_ERR;
  bufadr=malloc(buflen+1); 

  if (bufadr==NULL) return TASK_ERR;
  s=TCPIPMsgRecv(sock,bufadr,buflen);


  if (s !=buflen) {
    free(bufadr);
    return TASK_ERR;
  } 

  *store=bufadr;
  *size=buflen;
  return TASK_OK;
}

char RMsgRcvDecodeData(int sock,struct RMsgBlock *blk,
		       unsigned char **store) {
  int s;
  int i;
  unsigned char *dptr;
  
  blk->num=0;

  s=TCPIPMsgRecv(sock,&blk->num,sizeof(int));
  if (s !=sizeof(int)) return TASK_ERR;
  s=TCPIPMsgRecv(sock,&blk->tsize,sizeof(size_t));
  if (s !=sizeof(size_t)) return TASK_ERR;
  s=TCPIPMsgRecv(sock,&blk->data,sizeof(struct RMsgData)*blk->num);
  if (s !=sizeof(struct RMsgData)*blk->num) return TASK_ERR;
  if (*store !=NULL) {
    free(*store);
    *store=NULL;
  }  

  if (blk->tsize==0) return TASK_OK;

  *store=malloc(blk->tsize);
  if (*store==NULL) return TASK_ERR;
   
    s=TCPIPMsgRecv(sock,*store,blk->tsize);
  if (s !=blk->tsize) return TASK_ERR;

  dptr=*store;
  for (i=0;i<blk->num;i++) {
    blk->ptr[i]=dptr;
    dptr+=blk->data[i].size;
  }
  return TASK_OK;
}

