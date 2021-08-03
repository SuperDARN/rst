/* socket.c
   ========
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
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/uio.h>
#include <errno.h>

#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include "rtime.h"
#include "socket.h"
#include "connect.h"
#include "msgmem.h"


extern char timefname[256];

struct client client[CLIENT_MAX];
int msgmax=0;
int runloop=1;

char tmpbuf[BUF_SIZE];
char mbuf[BUF_SIZE];


void logtime(char *fname,int nbytes) {
  char txt[256];
  int mask=S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
  int fid;
  int yr,mo,dy,hr,mt,sc,us,s;

  TimeReadClock(&yr,&mo,&dy,&hr,&mt,&sc,&us);

  fid=open(fname,O_WRONLY | O_TRUNC | O_CREAT,mask);

  if (fid !=0) {
    sprintf(txt,"%d %d %d %d %d %d %d %d\n",yr,mo,dy,hr,mt,sc,us,nbytes);
    s=write(fid,txt,strlen(txt)+1);
    if (s == -1)
    {
        fprintf(stderr, "Error: Write was not Successful\n");
        //TODO: read errno for more information
    }

    close(fid);
  }
}


void trapreset(int signal) {
   runloop=0;
}

void reset() {
  int i;
  for (i=0;i<CLIENT_MAX;i++) {
     client[i].sock=-1;
     client[i].in_sze=0;
     client[i].out_sze=0;
  }
}

int pollsock(int sock,struct timeval *tv,fd_set *fdset) {
  int i;
  FD_SET(sock,fdset);
  for (i=0;i<msgmax;i++) 
    if (client[i].sock !=-1) FD_SET(client[i].sock,fdset);

  if (select(FD_SETSIZE,fdset,0,0,tv) < 0) return -1;
  return 0;
}

int createsocket(int *port) {

  socklen_t length;
  int sock;
  int sc_reuseaddr=1,temp;


  struct sockaddr_in server;

  struct hostent *gethostbyname();
 
  /* reset the client array */
  reset();
  
  sock=socket(AF_INET,SOCK_STREAM,0); /* create our listening socket */
  if (sock<0) return -1;


  /* set socket options */
  temp=setsockopt(sock,SOL_SOCKET,SO_REUSEADDR,&sc_reuseaddr,
                 sizeof(sc_reuseaddr));
  if (temp == -1)
  {
      fprintf(stderr, "Error: setsockopt is unable to read in all options\n");
      // TODO: add errno here to get why it failed 
      return -1; 

  }
  /* name and bind socket to an address and port number */

  server.sin_family=AF_INET;
  server.sin_addr.s_addr=INADDR_ANY;
  if (*port !=0) server.sin_port=htons(*port); 
  else server.sin_port=0;
  
  if (bind(sock,(struct sockaddr *) &server,sizeof(server))) return -1;
   
  /* Find out assigned port number and print it out */

  length=sizeof(server);
  if (getsockname(sock,(struct sockaddr *) &server,&length)) return -1;
    
  *port=ntohs(server.sin_port);
  return sock;
}

int processsocket(int sock,int inpipe) {

  fd_set fdset; /* selected file descriptors */  
  int poll,i;

  struct timeval tv;

   sigset_t set;
  struct sigaction act;

  sigemptyset(&set);
  sigaddset(&set,SIGUSR1);

  act.sa_flags=0;
  act.sa_mask=set;
  act.sa_handler=trapreset;
  sigaction(SIGUSR1,&act,NULL);

  signal(SIGPIPE,SIG_IGN);

  listen(sock,5);
 
  tv.tv_sec=0;
  tv.tv_usec=0;

  poll=0;  
  runloop=1; 
  do {

    FD_ZERO(&fdset);
    FD_SET(inpipe,&fdset);  
    if (poll==0) {
      if (pollsock(sock,NULL,&fdset) !=0) continue;
    } else pollsock(sock,&tv,&fdset);

    /* open any new connections if possible */

    opensock(sock,&fdset);

    poll=0;

    /* check to see if the root server has sent any data */

    if (FD_ISSET(inpipe,&fdset)) {
      int size;
      size=read(inpipe,mbuf,BUF_SIZE);
      if (size==0) break;

      /* log the time */

      logtime(timefname,size);
      writeraw(mbuf,size);
    }
    

    /* send the data to the clients */

    if (writesock() !=0) poll=1;
   
    /* read back any data from the clients */

    readsock(&fdset,tmpbuf,BUF_SIZE); 
    
    /* decode the buffers here */

  } while(runloop);
  
  /* close all the clients down */
 
  for (i=0;i<msgmax;i++) {
    if (client[i].sock !=0) close(client[i].sock);
  }
  close(sock);
  return -1;
}



