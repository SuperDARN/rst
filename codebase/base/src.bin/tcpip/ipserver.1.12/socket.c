/* socket.c
   ========
   Author: R.J.Barnes
*/

/*
   See license.txt
*/

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <errno.h>

#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>

#include "socket.h"
#include "connect.h"
#include "msg_mem.h"




struct client client[CLIENT_MAX];
int msgmax=0;
int runloop=1;

char tmpbuf[BUF_SIZE];
char mbuf[BUF_SIZE];

void trap_reset(int signal) {
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

int poll_sock(int sock,struct timeval *tv,fd_set *fdset) {
  int i;
  FD_SET(sock,fdset);
  for (i=0;i<msgmax;i++) 
    if (client[i].sock !=-1) FD_SET(client[i].sock,fdset);

  if (select(FD_SETSIZE,fdset,0,0,tv) < 0) return -1;
  return 0;
}

int create_socket(int *port) {

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

int process_socket(int sock,int inpipe) {

  fd_set fdset; /* selected file descriptors */  
  int poll,i;

  struct timeval tv;

  signal(SIGPIPE,SIG_IGN);
  signal(SIGUSR1,trap_reset);
  listen(sock,5);
 
  tv.tv_sec=0;
  tv.tv_usec=0;

  poll=0;  
  runloop=1; 
  do {

    FD_ZERO(&fdset);
    FD_SET(inpipe,&fdset);  
    if (poll==0) {
      if (poll_sock(sock,NULL,&fdset) !=0) continue;
    } else poll_sock(sock,&tv,&fdset);

    /* open any new connections if possible */

    open_sock(sock,&fdset);

    poll=0;

    /* check to see if the root server has sent any data */

    if (FD_ISSET(inpipe,&fdset)) {
      int size;
      size=read(inpipe,mbuf,BUF_SIZE);
      if (size==0) break;
      write_raw(mbuf,size);
    }
    
    /* send the data to the clients */

    if (write_sock() !=0) poll=1;
   
    /* read back any data from the clients */

    read_sock(&fdset,tmpbuf,BUF_SIZE); 
    
    /* decode the buffers here */

  } while(runloop);
  
  /* close all the clients down */
 
  for (i=0;i<msgmax;i++) {
    if (client[i].sock !=0) close(client[i].sock);
  }
  close(sock);
  return -1;
}



