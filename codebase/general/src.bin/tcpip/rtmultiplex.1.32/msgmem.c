/* msgmem.c
   ========
   Author: R.J.Barnes
*/

/*
   See license.txt
*/



#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>

#include <sys/uio.h>
#include <errno.h>

#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

#include "socket.h"
#include "connect.h"


extern struct client client[CLIENT_MAX];
extern int msgmax;

int writeraw(unsigned char *buf,int sze) { 
  /* this bit adds a message onto the end of the write buffer */
  int i;
  for (i=0;i<msgmax;i++) {
    if (client[i].sock !=0) {
      if (sze+client[i].out_sze > BUF_SIZE) {
        int delta;      
        delta=sze+client[i].out_sze-BUF_SIZE;
        memmove(client[i].out_buf,client[i].out_buf+delta,
                client[i].out_sze-delta);
        client[i].out_sze=BUF_SIZE-sze;
      } 
      memcpy(client[i].out_buf+client[i].out_sze,buf,sze);
      client[i].out_sze+=sze;
    }
  }
  return 0;
}

void readsock(fd_set *fdset,unsigned char *tmp_buf,int tmp_sze) {
  /* reads in a message from a client */         
  int i;
  int byte;

  for (i=0;i<msgmax;i++) 
    if ( (client[i].sock !=-1) && 
         (FD_ISSET(client[i].sock,fdset))) {
         byte=read(client[i].sock,tmp_buf,tmp_sze);
     
         if (byte ==0) closesock(i);
         else if ((byte <0) && (errno !=EAGAIN)) closesock(i);
         else if (byte > 0) {
           if (byte+client[i].in_sze > BUF_SIZE) {
             int delta;
             delta=byte+client[i].in_sze-BUF_SIZE;
             memmove(client[i].in_buf,client[i].in_buf+delta,
                   client[i].in_sze-delta);
             client[i].out_sze=BUF_SIZE-byte;
           } 
           memcpy(client[i].in_buf+client[i].in_sze,tmp_buf,byte);
           client[i].in_sze+=byte;
	 }
    }
}

int writesock() {
  int i;
  int byte,cnt=0;
  for (i=0;i<msgmax;i++) 
    if ((client[i].sock !=-1) && (client[i].out_sze !=0)) {   
      byte=write(client[i].sock,client[i].out_buf,client[i].out_sze);
   
      if (byte <0) {
        if (errno==EAGAIN) continue;
        else closesock(i);
      } else if (byte >0) {
        memmove(client[i].out_buf,client[i].out_buf+byte,
                client[i].out_sze-byte);
        client[i].out_sze-=byte;
        if (client[i].out_sze >0) cnt++; 
      }
    }
    return cnt; /* returns number of clients with outstanding data */
}




