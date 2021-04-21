/* msg.c
   =====
   Author: R.J.Barnes
 LICENSE AND DISCLAIMER
 
 Copyright (c) 2012 The Johns Hopkins University/Applied Physics Laboratory
 
 This file is part of the Radar Software Toolkit (RST).
 
 RST is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with RST.  If not, see <http://www.gnu.org/licenses/>.
 
 
 
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include "connex.h"



int ConnexWriteMem(unsigned char *outbuf,unsigned int outsize,
                   unsigned char *buffer,unsigned int size) {
 
  struct {
     unsigned char msg_id[4];
     unsigned char length[4];
  } msg={{0x49,0x08,0x1e,0x00}, {0,0,0,0}};
 
  /* force little endian alignment */
  msg.length[0]=size & 0xff;
  msg.length[1]=(size & 0xff00) >>8;
  msg.length[2]=(size & 0xff0000) >>16;
  msg.length[3]=(size & 0xff000000) >> 24;
 
  if (outsize < 8) return -1;
  memcpy(outbuf,((char *) &msg),8);
  outsize-=8;
  if (outsize<size) return -1;
  memcpy(outbuf+8,buffer,size); 
  return size+8;
}  

int ConnexReadMem(unsigned char *inbuf,unsigned int insize,
                  unsigned char **buffer) {

  int cnt=0;

  unsigned char msg_id[4]={0x49,0x08,0x1e,0x00};
  unsigned char length[4];
  unsigned long int size;

  ConnexErr=0;
 
  while (cnt<4) { 
    if (inbuf[cnt] != msg_id[cnt]) {
       ConnexErr=MSGERR_OUT_OF_SYNC;
       break;
    }
    cnt++;
  }
  if (cnt<4) return -1;
  memcpy((char *) length,inbuf+4,4);

  /* decode the little endian length information */
  size=length[0];
  size|=length[1]<<8;
  size|=length[2]<<16;
  size|=length[3]<<24;
  
  if (size==0) {
    *buffer=NULL;
    return 0;
  }

  if ((*buffer=malloc(size)) ==NULL) {
    ConnexErr=MSGERR_NOMEM;
    return -1;
  }
  memcpy((*buffer),inbuf+8,size);
  return size;
}  



int ConnexWriteIP(int fildes,unsigned char *buffer,unsigned int size) {
  int byte=0;  
  int cnt=0;
 

  struct {
     unsigned char msg_id[4];
     unsigned char length[4];
  } msg={{0x49,0x08,0x1e,0x00}, {0,0,0,0}};
 
  /* force little endian alignment */
  msg.length[0]=size & 0xff;
  msg.length[1]=(size & 0xff00) >>8;
  msg.length[2]=(size & 0xff0000) >>16;
  msg.length[3]=(size & 0xff000000) >> 24;
  
  while (cnt<8) { 
    byte=write(fildes,((char*) &msg)+cnt,8-cnt);
    if (byte<=0) {
      ConnexErr=MSGERR_PIPE_CLOSED;
      break;
    }
    cnt+=byte;
  }
  if (cnt<8) return -1;

  cnt=0;  /* write out the message */
  while (cnt<size) {
    byte=write(fildes,buffer+cnt,size-cnt); 
    if (byte<=0) {
      ConnexErr=MSGERR_PIPE_CLOSED;
      break;
    }
    cnt+=byte;
  }
  return size;
}  

/* reads a message of the specified fildes */

int ConnexReadIP(int fildes,unsigned char **buffer) {

  int byte;
  int cnt=0;

  unsigned char msg_id[4]={0x49,0x08,0x1e,0x00};
  unsigned char byte_buf;
  unsigned char length[4];
  unsigned long int size;

  ConnexErr=0;
 
  while (cnt<4) { 
    byte=read(fildes,&byte_buf,1);
    if (byte<=0) {
       ConnexErr=MSGERR_PIPE_CLOSED;
       break;
    }
    if (byte_buf != msg_id[cnt]) {
       ConnexErr=MSGERR_OUT_OF_SYNC;
       break;
    }
    cnt++;
  }
  if (cnt<4) return -1;

   cnt=0;
  while (cnt<4) {
    byte=read(fildes,((char *) length)+cnt,4-cnt); 
    if (byte<=0) {
      ConnexErr=MSGERR_PIPE_CLOSED;
      break;
    }
    cnt+=byte;
  }
  if (cnt < 4)  return -1;

  /* decode the little endian length information */
  size=length[0];
  size|=length[1]<<8;
  size|=length[2]<<16;
  size|=length[3]<<24;
  
  if (size==0) {
    if (*buffer !=NULL) free(*buffer);
    *buffer=NULL;
    return 0;
  }

  if (*buffer !=NULL) {  
    unsigned char *tmp;  
    if ((tmp=realloc(*buffer,size)) ==NULL) {
      ConnexErr=MSGERR_NOMEM;
      return -1;
    }
    *buffer=tmp;
  } else {
    if ((*buffer=malloc(size)) ==NULL) {
      ConnexErr=MSGERR_NOMEM;
     return -1;
    }
  }

  cnt=0;  /* read in the message */
  while (cnt<size) {
    byte=read(fildes,(*buffer)+cnt,size-cnt); 
    if (byte<=0) {
      ConnexErr=MSGERR_PIPE_CLOSED;
      break;
    }
    cnt+=byte;
  }
  if (cnt<size) {
     free(*buffer);
     *buffer=NULL;
     return -1;
  }
  return size;
}  
