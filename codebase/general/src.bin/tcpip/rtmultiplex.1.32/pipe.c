/* pipe.c
   ======
   Author: R.J.Barnes
*/

/*
   See license.txt
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>
#include "connex.h"

int pipewrite(int fildes,unsigned char *buffer,unsigned int size) {
  int byte;
  int cnt=0;
  int hsze=0;   

  unsigned char hdr[sizeof(int)]; 

  *((int *)hdr)=size;
 
  hsze=sizeof(int);
  while (cnt<hsze) { 
    byte=write(fildes,hdr+cnt,hsze-cnt);
    if (byte<=0) {
      ConnexErr=MSGERR_PIPE_CLOSED;
      break;
    }
    cnt+=byte;
  }
  if (cnt<hsze) return -1;

  cnt=0;  /* write out the message */
  while (cnt<size) {
    byte=write(fildes,buffer+cnt,size-cnt); 
    if (byte<=0) {
      ConnexErr=MSGERR_PIPE_CLOSED;
      break;
    }
    cnt+=byte;
  }
  if (cnt<size) return -1;
  return size;
}  


int piperead(int fildes,unsigned char **buffer) {

  int byte;
  int cnt=0;

  int hsze=sizeof(int);
  unsigned char hdr[sizeof(int)];

  unsigned int size;   

  while (cnt<hsze) { 
    byte=read(fildes,hdr,hsze);
    if (byte<=0) {
       ConnexErr=MSGERR_PIPE_CLOSED;
       break;
    }
    cnt+=byte;
  }
  if (cnt<hsze) return -1;
 
  size=*((int *)hdr);

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
