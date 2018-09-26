
/* radarshell.c
   ============
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
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include "tcpipmsg.h"
#include "radarshell.h"

#define DELIM  " "

#define SHELL_POLL 'p'
#define SHELL_IGNORE 'i'
#define SHELL_SEND 's'
#define SHELL_REPLY 'r'

#include "radarshell.h"
 
struct RShellBuffer *RShellBufferMake() {

  struct RShellBuffer *ptr=NULL;
  ptr=malloc(sizeof(struct RShellBuffer));
  
  if (ptr==NULL) return NULL;
  ptr->buf=0;
  ptr->off=NULL;
  ptr->num=0;
  ptr->len=0;
  return ptr; 

}

int RShellBufferAlloc(struct RShellBuffer *ptr,void *buf,int sze) {
  void *tmp;

  if (ptr->buf==NULL) ptr->buf=malloc(sze);
  else {
    tmp=realloc(ptr->buf,ptr->len+sze);
    if (tmp !=NULL) ptr->buf=tmp;
    else return -1;
  }
  
  if (ptr->off==NULL) ptr->off=malloc(sizeof(size_t));
  else {
    tmp=realloc(ptr->off,(ptr->num+1)*sizeof(size_t));
    if (tmp !=NULL) ptr->off=tmp;
    else return -1;
  }

  memcpy(ptr->buf+ptr->len,buf,sze);
  ptr->off[ptr->num]=ptr->len;
  ptr->num++;
  ptr->len+=sze;
  return 0;
}

void *RShellBufferRead(struct RShellBuffer *ptr,int num) {
  if (ptr==NULL) return NULL;
  if (ptr->buf==NULL) return NULL;
  if (ptr->off==NULL) return NULL;
  if (num>=ptr->num) return NULL;
  return ptr->buf+ptr->off[num];
}

  
void RShellBufferFree(struct RShellBuffer *ptr) {
  if (ptr->buf !=NULL) free(ptr->buf);
  if (ptr->off !=NULL) free(ptr->off);
  free(ptr);
}



int RadarShell(int sock,struct RShellTable *ptr) {

  char tmp[STR_MAX];
  struct RShellBuffer *buf=NULL;
  void *data=NULL;

  int smsg,rmsg,s,n,rnum;
  size_t rlen;

  /* see if the shell server  wants a copy of the variables */
  smsg=SHELL_POLL;
  s=TCPIPMsgSend(sock,&smsg,sizeof(int));
  if (s !=sizeof(int)) return -1;
  
  s=TCPIPMsgRecv(sock,&rmsg,sizeof(int));
  if (s !=sizeof(int)) return -1;

  if (rmsg==SHELL_IGNORE) return 0;

  /* decode the variable list */

  buf=RShellBufferMake();

  for (n=0;n<ptr->num;n++) {
    RShellBufferAlloc(buf,ptr->ptr[n].name,strlen(ptr->ptr[n].name)+1);
    RShellBufferAlloc(buf,&ptr->ptr[n].type,sizeof(ptr->ptr[n].type));
    switch (ptr->ptr[n].type) {
    case var_SHORT:
      RShellBufferAlloc(buf,ptr->ptr[n].data,sizeof(short));
      break;
    case var_INT:
      RShellBufferAlloc(buf,ptr->ptr[n].data,sizeof(int));
      break;
    case var_LONG:
      RShellBufferAlloc(buf,ptr->ptr[n].data,sizeof(int));
      break;
    case var_FLOAT:
      RShellBufferAlloc(buf,ptr->ptr[n].data,sizeof(float));
      break;
    case var_DOUBLE:
      RShellBufferAlloc(buf,ptr->ptr[n].data,sizeof(double));
      break;
    case var_STRING:
      strncpy(tmp,ptr->ptr[n].data,STR_MAX);
      RShellBufferAlloc(buf,tmp,STR_MAX);
      break;
    default:
      break;
     
    }        

  }
  
  if (rmsg==SHELL_SEND) {
    s=TCPIPMsgSend(sock,&buf->num,sizeof(int));
    s=TCPIPMsgSend(sock,&buf->len,sizeof(size_t));
    if (buf->num !=0) s=TCPIPMsgSend(sock,buf->off,sizeof(size_t)*buf->num);
    if (buf->len !=0) s=TCPIPMsgSend(sock,buf->buf,buf->len);
  } else if (rmsg==SHELL_REPLY) {
    s=TCPIPMsgRecv(sock,&rnum,sizeof(int));
    s=TCPIPMsgRecv(sock,&rlen,sizeof(size_t));
    if ((rnum !=buf->num) || (rlen != buf->len)) {
      RShellBufferFree(buf);
      return -1;
    }
    s=TCPIPMsgRecv(sock,buf->off,sizeof(size_t)*buf->num);
    s=TCPIPMsgRecv(sock,buf->buf,buf->len);
    /* unpack here */

   for (n=0;n<ptr->num;n++) {
     data=RShellBufferRead(buf,3*n+2);

     switch (ptr->ptr[n].type) {
     case var_SHORT:
       *((short int *) ptr->ptr[n].data)=*((short int *) data);
       break;
     case var_INT:
       *((int *) ptr->ptr[n].data)=*((int *) data);
       break;
     case var_LONG:
       *((int *) ptr->ptr[n].data)=*((int *) data);
       break;
     case var_FLOAT:
       *((float *) ptr->ptr[n].data)=*((float *) data);
       break;
     case var_DOUBLE:
       *((double *) ptr->ptr[n].data)=*((double *) data);
       break;
    case var_STRING:
      strncpy((char *) ptr->ptr[n].data,(char *) data,STR_MAX);

	 break;
       default:
	 break;
     }

   }

  }
  RShellBufferFree(buf);

  return 0;
}
 


int RadarShellAdd(struct RShellTable *rptr,
	      char *name,int type,void *data) {
  
  if (rptr->ptr !=NULL) {
    rptr->ptr=realloc(rptr->ptr,sizeof(struct RShellEntry)*(rptr->num+1));
    if (rptr->ptr==NULL) return -1;
  } else {
    rptr->ptr=malloc(sizeof(struct RShellEntry));
    if (rptr->ptr==NULL) return -1;
    rptr->num=0; 
  }

  rptr->ptr[rptr->num].name=malloc(strlen(name)+1);
  strcpy(rptr->ptr[rptr->num].name,name);
  rptr->ptr[rptr->num].type=type;
  rptr->ptr[rptr->num].data=data;
  rptr->num++;
  return 0;
}

void RadarShellFree(struct RShellTable *rptr) {
  if (rptr->ptr !=NULL) {
    int i;
    for (i=0;(rptr->ptr[i].name !=NULL);i++) free(rptr->ptr[i].name);
    free(rptr->ptr);
  }
  rptr->ptr=NULL;
  rptr->num=0;
}

struct RShellEntry *RadarShellRead(struct RShellTable *rptr,int num) {
  if (rptr==NULL) return NULL;
  if (num>=rptr->num) return NULL;
  return &rptr->ptr[num];
}



struct RShellEntry *RadarShellFind(struct RShellTable *rptr,char *name) {
  int num;
  if (rptr==NULL) return NULL;
  if (name==NULL) return NULL;
  if (rptr->num==0) return NULL;

  for (num=0;num<rptr->num;num++) 
    if (strcmp(rptr->ptr[num].name,name)==0) break;    

  if (num>=rptr->num) return NULL;
  return &rptr->ptr[num];
}

  

int RadarShellParse(struct RShellTable *rptr,char *name,...) {
  int status=0;
  va_list ap;

  char *lname=NULL;
  char *vname=NULL;
  char *vtype=NULL;
  void *data=NULL;
  int type=0;

  lname=malloc(strlen(name)+1);
  strcpy(lname,name);

  if (name==NULL) return 0;
  va_start(ap,name);

  vname=strtok(lname," ");

  while (vname !=NULL) {

    vtype=strtok(NULL,DELIM);
    if (vtype==NULL) break;
    
    type=var_STRING;
    if (strcmp(vtype,"s")==0) type=var_SHORT;
    if (strcmp(vtype,"i")==0) type=var_INT;
    if (strcmp(vtype,"l")==0) type=var_LONG;
    if (strcmp(vtype,"f")==0) type=var_FLOAT;
    if (strcmp(vtype,"d")==0) type=var_DOUBLE;
    data=va_arg(ap,void *);
  
    if (data !=NULL) status=RadarShellAdd(rptr,vname,type,data);
    
    if (status !=0) return -1;
    
    vname=strtok(NULL,DELIM);
  }
  va_end(ap);
  return 0;
}

  
  










