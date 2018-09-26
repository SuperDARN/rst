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

 
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include "option.h"
#include "shmem.h"
#include "tcpipmsg.h"

#include "errstr.h"
#include "hlpstr.h"

#define DEF_CPPORT 44001
#define DEF_RSPORT 44002

#define MAX_NUM  256
#define BUF_SIZE (32*1024)

#define SHELL_POLL 'p'
#define SHELL_IGNORE 'i'
#define SHELL_SEND 's'
#define SHELL_REPLY 'r'
#define SHELL_OK '0'
#define SHELL_ERROR 'e'



struct OptionData opt;

char *shmemd="RadarShell";
char *shmem=NULL;
int shmemfd;
unsigned char *shm;

struct membuf {
  int cpstate;
  int rsstate;
  int num;
  size_t size;
  size_t offset[MAX_NUM];
  unsigned char buffer[BUF_SIZE/2];
};


void rootendprog(int signum) {
  ShMemFree(shm,shmem,BUF_SIZE,1,shmemfd);
  exit(0);
}

void childendprog(int signum) {
  ShMemFree(shm,shmem,BUF_SIZE,1,shmemfd);
  exit(0);
}

int operateCP(pid_t parent,int sock) {

  int s;
  int rmsg,smsg;
  struct membuf *ptr;

  signal(SIGINT,childendprog);
  
  shm=ShMemAlloc(shmem,BUF_SIZE,O_RDWR,0,&shmemfd);

  ptr=(struct membuf *) shm;

  ptr->cpstate=1;

  while(1) {

    s=TCPIPMsgRecv(sock,&rmsg,sizeof(int));
    if (s !=sizeof(int)) break;
    if (ptr->rsstate==0) {
      smsg=SHELL_IGNORE;
      s=TCPIPMsgSend(sock,&smsg,sizeof(int));
      if (s !=sizeof(int)) break;
    } else if (ptr->rsstate==1) {
      smsg=SHELL_SEND;
      s=TCPIPMsgSend(sock,&smsg,sizeof(int));
      if (s !=sizeof(int)) break;
      s=TCPIPMsgRecv(sock,&ptr->num,sizeof(int));
      if (s !=sizeof(int)) break;
      s=TCPIPMsgRecv(sock,&ptr->size,sizeof(size_t));
      if (s !=sizeof(size_t)) break;
      s=TCPIPMsgRecv(sock,ptr->offset,sizeof(size_t)*ptr->num);
      if (s !=sizeof(size_t)*ptr->num) break;
      s=TCPIPMsgRecv(sock,ptr->buffer,ptr->size);
      if (s !=ptr->size) break;
      ptr->cpstate=2;
    } else if (ptr->rsstate==2) {
      smsg=SHELL_REPLY;
      s=TCPIPMsgSend(sock,&smsg,sizeof(int));
      if (s !=sizeof(int)) break;
      s=TCPIPMsgSend(sock,&ptr->num,sizeof(int));
      if (s !=sizeof(int)) break;
      s=TCPIPMsgSend(sock,&ptr->size,sizeof(size_t));
      if (s !=sizeof(size_t)) break;
      s=TCPIPMsgSend(sock,ptr->offset,sizeof(size_t)*ptr->num);
      if (s !=sizeof(size_t)*ptr->num) break;
      s=TCPIPMsgSend(sock,ptr->buffer,ptr->size);
      if (s !=ptr->size) break;
      ptr->cpstate=1;
    }

  }
  ptr->cpstate=0;
  ShMemFree(shm,shmem,BUF_SIZE,0,shmemfd);

  
  return 0;
}

int operateRS(pid_t parent,int sock) {

  int s;
  int rmsg,smsg;

  struct membuf *ptr;

  signal(SIGINT,childendprog);

  shm=ShMemAlloc(shmem,BUF_SIZE,O_RDWR,0,&shmemfd);

  ptr=(struct membuf *) shm;
  ptr->rsstate=0;
  
  while(1) {

    s=TCPIPMsgRecv(sock,&rmsg,sizeof(int));
    if (s !=sizeof(int)) break;
     
    if (rmsg==SHELL_SEND) {
      ptr->rsstate=1;
      while (ptr->cpstate !=2) {
        if (ptr->cpstate==0) break;
        usleep(50000);
      }
      if (ptr->cpstate==0) {
	smsg=SHELL_ERROR;
        s=TCPIPMsgSend(sock,&smsg,sizeof(int));
        if (s !=sizeof(int)) break;
        break;
      }
      smsg=SHELL_OK;
      s=TCPIPMsgSend(sock,&smsg,sizeof(int));
      if (s !=sizeof(int)) break;
      s=TCPIPMsgSend(sock,&ptr->num,sizeof(int));
      if (s !=sizeof(int)) break;
      s=TCPIPMsgSend(sock,&ptr->size,sizeof(size_t));
      if (s !=sizeof(size_t)) break;
      s=TCPIPMsgSend(sock,ptr->offset,sizeof(size_t)*ptr->num);
      if (s !=sizeof(size_t)*ptr->num) break;
      s=TCPIPMsgSend(sock,ptr->buffer,ptr->size);
      if (s !=ptr->size) break;
      ptr->rsstate=0; 
     
    } else if (rmsg==SHELL_REPLY) {
      if (ptr->cpstate==0) {
        smsg=SHELL_ERROR;
        s=TCPIPMsgSend(sock,&smsg,sizeof(int));
        if (s !=sizeof(int)) break;
      }
      smsg=SHELL_OK;
      s=TCPIPMsgSend(sock,&smsg,sizeof(int));
      if (s !=sizeof(int)) break;
      s=TCPIPMsgRecv(sock,&ptr->num,sizeof(int));
      if (s !=sizeof(int)) break;
      s=TCPIPMsgRecv(sock,&ptr->size,sizeof(size_t));
      if (s !=sizeof(size_t)) break;
      s=TCPIPMsgRecv(sock,ptr->offset,sizeof(size_t)*ptr->num);
      if (s !=sizeof(size_t)*ptr->num) break;
      s=TCPIPMsgRecv(sock,ptr->buffer,ptr->size);
      if (s !=ptr->size) break;
      ptr->rsstate=2;
      while (ptr->cpstate !=1) {
        if (ptr->cpstate==0) break;
        usleep(50000);
      }
      break;
    }
  }
  ptr->rsstate=0;
  ShMemFree(shm,shmem,BUF_SIZE,0,shmemfd);

  return 0;
}


int main(int argc,char *argv[]) {
  
  int arg,n;
  int port[2]={DEF_CPPORT,DEF_RSPORT};
  int sock[2];

  int sc_reuseaddr=1,temp;




  unsigned char help=0; 
  unsigned char option=0; 

  socklen_t length[2];
  socklen_t clength;

  struct sockaddr_in server[2];
  struct sockaddr_in client;

  fd_set ready;

  struct hostent *gethostbyname();
  pid_t root;

  int msgsock=0;

  struct membuf *ptr;
  
  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"-option",'x',&option);

  OptionAdd(&opt,"cp",'i',&port[0]);
  OptionAdd(&opt,"sp",'i',&port[1]);

  OptionAdd(&opt,"sh",'t',&shmem);

  arg=OptionProcess(1,argc,argv,&opt,NULL);

  if (help==1) {
    OptionPrintInfo(stdout,hlpstr);
    exit(0);
  }

  if (option==1) {
    OptionDump(stdout,&opt);
    exit(0);
  }

  if (shmem==NULL) shmem=shmemd;

  signal(SIGCHLD,SIG_IGN); 
  signal(SIGPIPE,SIG_IGN);

  signal(SIGINT, rootendprog);

  shm=ShMemAlloc(shmem,BUF_SIZE,O_RDWR | O_CREAT,1,&shmemfd);

  ptr=(struct membuf *) shm;

  ptr->cpstate=0;
  ptr->rsstate=0;
  ptr->size=0;

  root=getpid();
  
  for (n=0;n<2;n++) {

    sock[n]=socket(AF_INET,SOCK_STREAM,0); /* create our listening socket */
    if (sock[n]<0) {
      perror("opening stream socket");
      exit(1);
    }

    /* set socket options */
    temp=setsockopt(sock[n],SOL_SOCKET,SO_REUSEADDR,&sc_reuseaddr,
                 sizeof(sc_reuseaddr));


    /* name and bind socket to an address and port number */

    server[n].sin_family=AF_INET;
    server[n].sin_addr.s_addr=INADDR_ANY;
    if (port[n] !=0) server[n].sin_port=htons(port[n]); 
    else server[n].sin_port=0;
  
    if (bind(sock[n],(struct sockaddr *) &server[n],sizeof(server[n]))) {
       perror("binding stream socket");
       exit(1);
    }

    length[n]=sizeof(server[n]);
    if (getsockname(sock[n],(struct sockaddr *) &server[n],&length[n])) {
       perror("getting socket name");
       exit(1);
    }

    listen(sock[n],5); /* mark our socket willing to accept connections */
  }
  do {
    
    /* block until someone wants to attach to us */

   FD_ZERO(&ready);
   for (n=0;n<2;n++) FD_SET(sock[n],&ready);
   if (select(sock[1]+1,&ready,0,0,NULL) < 0) { 
     perror("while testing for connections");
     continue;
   }
     
   /* Accept the connection from control program client */
   if (FD_ISSET(sock[0],&ready)) {

     fprintf(stdout,"Accepting a new control program connection.\n");
     clength=sizeof(client);
     msgsock=accept(sock[0],(struct sockaddr *) &client,&clength);
        
     if (msgsock==-1) {
       perror("accept"); 
       continue;
     }


     if (fork() == 0) {
       close(sock[0]);
       operateCP(root,msgsock);
       exit(0);
     }
     close(msgsock);
   }

   if (FD_ISSET(sock[1],&ready)) {
       fprintf(stdout,"Accepting a new radar shell connection.\n");
       clength=sizeof(client);
       msgsock=accept(sock[1],(struct sockaddr *) &client,&clength);

      if (msgsock==-1) {
        perror("accept");
        continue;
      }

      if (fork() == 0) {
        close(sock[1]);
        operateRS(root,msgsock);
        exit(0);
      }
      close(msgsock);
    }
  } while(1);

  return 0;

}
   

 






















