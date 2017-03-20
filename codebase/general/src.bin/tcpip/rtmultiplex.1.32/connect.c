/* connect.c
   =========
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
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>

#include "loginfo.h"
#include "socket.h"

extern unsigned char vb;
extern char logfname[256];
extern struct client client[CLIENT_MAX];
extern int msgmax;

void closesock(int i) {
  if ((i<msgmax) && (client[i].sock !=-1)) {
    char logbuf[256];
    sprintf(logbuf,"%s : Close Connection.",client[i].host);
    loginfo(logfname,logbuf);
    close(client[i].sock);
    client[i].sock=-1;
    if (i==msgmax-1) msgmax--;
  } 
}

int opensock(int sock,fd_set *fdset) {
  int i,status;
  char logbuf[256];
  int temp;
  socklen_t clength;

  
  struct sockaddr_in caddr;
 
  if (FD_ISSET(sock,fdset)==0) return -1;
  for (i=0;(i<msgmax) && (client[i].sock !=-1);i++);
  if (i>=CLIENT_MAX) { 
    /* dequeue the request here */

    loginfo(logfname,"Too many clients attached - refusing connection.");

    temp=accept(sock,0,0);
    if (temp !=-1) close(temp);
    return -1;
  }

  clength=sizeof(caddr);
  if ((client[i].sock=accept(sock,
      (struct sockaddr *) &caddr,&clength))==-1) {
    loginfo(logfname,"Accept failed.");

     return -1;
  }

  sprintf(client[i].host,"[%s]",inet_ntoa(caddr.sin_addr));

  if ((status=fcntl(client[i].sock,F_GETFL))==-1) {
    close(client[i].sock);
    client[i].sock=-1;
    loginfo(logfname,"Failed to read file control block.");
    return -1;
  }
  status|=O_NONBLOCK;
  if ((status=fcntl(client[i].sock,F_SETFL,status))==-1) {
    close(client[i].sock);
    client[i].sock=-1;
    loginfo(logfname,"Failed to write file control block.");
    return -1;
  } 

  sprintf(logbuf,"%s : Open Connection.",client[i].host);
  loginfo(logfname,logbuf);

  if (i==msgmax) msgmax++;
  return client[i].sock;
}





