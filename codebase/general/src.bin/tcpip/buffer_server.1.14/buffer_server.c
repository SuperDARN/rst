/* buffer_server.c
   ===============
   Author: R.J.Barnes
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
#include <string.h>
#include "option.h"
 
#include "config.h"
#include "errstr.h"
#include "hlpstr.h"

/* This is the UNIX side server buffer - each time a client connect
 * itself the buffer forks to create a new process. The new process opens
 * a connection between itself and the root server, and between itself
 * and the UNIX (JAVA) client.
 *
 * The buffer waits until either the master or the client instigates a
 * read - and then buffers the input before writing it to the appropriate
 * process.
 *
 */

#define BUF_SIZE 8192

int msgin_sock=-1;
int msgout_sock=-1;

char in_buf[BUF_SIZE];
char out_buf[BUF_SIZE];
char tmp_buf[BUF_SIZE];
int in_sze=0;
int out_sze=0;

struct OptionData opt;

int read_csock() {
  /* reads in a message from a client */         
  int byte,delta;  
  delta=BUF_SIZE-in_sze;
  byte=read(msgout_sock,tmp_buf,BUF_SIZE);
  if (byte ==0) return -1;
  else if (byte <0) {
    if (errno ==EAGAIN) return 0;
    else return -1;
  }
  if (byte+out_sze > BUF_SIZE) {
     delta=byte+out_sze-BUF_SIZE;
     memmove(out_buf,out_buf+delta,out_sze-delta);
     out_sze=BUF_SIZE-byte;
  }
  memcpy(out_buf+out_sze,tmp_buf,byte);
  out_sze+=byte;
  return 0;
}

int read_ssock() {
  /* reads in a message from the server */         
 
  int byte,delta;  
  delta=BUF_SIZE-in_sze;
  byte=read(msgin_sock,tmp_buf,BUF_SIZE);
  if (byte ==0) return -1;
  else if (byte <0) {
    if (errno ==EAGAIN) return 0;
    else return -1;
  }
  if (byte+in_sze > BUF_SIZE) {
     delta=byte+in_sze-BUF_SIZE;
     memmove(in_buf,in_buf+delta,in_sze-delta);
     in_sze=BUF_SIZE-byte;
  }
  memcpy(in_buf+in_sze,tmp_buf,byte);
  in_sze+=byte;
  return 0;
}

int write_ssock() {
  int byte;
  fd_set write_set;
  struct timeval tv_poll;
    
  if (out_sze==0) return 0;
  tv_poll.tv_sec=0;
  tv_poll.tv_usec=0;

  FD_ZERO(&write_set);
  FD_SET(msgin_sock,&write_set);
  if (select(FD_SETSIZE,0,&write_set,0,&tv_poll) < 0) {
    perror("while doing write select on client");
    return -1;
  }
  if (FD_ISSET(msgin_sock,&write_set)) 
    byte=write(msgin_sock,out_buf,out_sze); 
  else byte=0;
  if ((byte <0) && (errno !=EAGAIN)) {
    perror("Writing to server");
    return -1;
  } else if (byte >0) {
    memmove(out_buf,out_buf+byte,out_sze-byte);
    out_sze-=byte;
    if (out_sze >0) return 1; 
  }
  return 0;
}

int write_csock() {
  int byte;
  fd_set write_set;
  struct timeval tv_poll;
    
  if (in_sze==0) return 0;
  tv_poll.tv_sec=0;
  tv_poll.tv_usec=0;

  FD_ZERO(&write_set);
  FD_SET(msgout_sock,&write_set);
  if (select(FD_SETSIZE,0,&write_set,0,&tv_poll) < 0) {
    perror("while doing write select on client");
    return -1;
  }
  if (FD_ISSET(msgout_sock,&write_set)) 
    byte=write(msgout_sock,in_buf,in_sze); 
  else byte=0;

  if ((byte <0) && (errno !=EAGAIN)) {
    perror("Writing to client");
    return -1;
  } else if (byte >0) {
    memmove(in_buf,in_buf+byte,in_sze-byte);
    in_sze-=byte;
    if (in_sze >0) return 1; 
  }
  return 0;
}

void operate(pid_t parent,int master,int client,char *name,char *log) {
  fd_set read_set;
  struct timeval tv;
  int poll=0;
  int status=0; 

  char *date;
  time_t ltime;
  struct tm *time_of_day;
  
  tv.tv_sec=0;
  tv.tv_usec=0;
 
  /* multiplex and buffer the read operations */

  time(&ltime);  

  time_of_day=localtime(&ltime);
  date=asctime(time_of_day);  
  fprintf(stdout,"%s : Open Connection. %s",name,date);

  if (log !=NULL) {
     FILE *fp;
     fp=fopen(log,"a");
     fprintf(fp,"%s : Open Connection. %s",name,date);
     fclose(fp);
  }

  while(1) {
    FD_ZERO(&read_set);
    FD_SET(master,&read_set);
    FD_SET(client,&read_set);
    if (poll==0) {
      if (select(FD_SETSIZE,&read_set,0,0,NULL) < 0) { 
         perror("while waiting client/master I/O");
         break;
      }
    } else {
      if (select(FD_SETSIZE,&read_set,0,0,&tv) < 0) { 
         perror("while waiting client/master I/O");
         break;
      }
    }
    poll=0;
    if ((FD_ISSET(master,&read_set)) && (read_ssock() <0)) break;
    else if ((FD_ISSET(client,&read_set)) && (read_csock() <0)) break;  
   
    status=write_csock();
    if (status <0) break;
    if (status ==1) poll=1;
    status=write_ssock();
    if (status <0) break;
    if (status ==1) poll=1;   
  }

  close(master);
  close(client);

  time(&ltime); 
 

  time_of_day=localtime(&ltime);
  date=asctime(time_of_day);  
  fprintf(stdout,"%s: Connection Closed. %s\n",name,date);

  if (log !=NULL) {
     FILE *fp;
     fp=fopen(log,"a");
     fprintf(fp,"%s : Close Connection. %s",name,date);
     fclose(fp);
  }

  _exit(EXIT_SUCCESS);
}

int rst_opterr(char *txt) {
  fprintf(stderr,"Option not recognized: %s\n",txt);
  fprintf(stderr,"Please try: buffer_server --help\n");
  return(-1);
}

int main(int argc,char *argv[]) {
  int arg;
  int sock;
  socklen_t length;
  socklen_t clength;  
  int sc_reuseaddr=1,temp;

  struct sockaddr_in server;
  struct sockaddr_in master;
  struct sockaddr_in client;


  fd_set ready;
  int port=0;
 
  unsigned char help=0;
  unsigned char option=0;
  unsigned char version=0;

  int remote_port=0;
  char chost[256];

  char *host=NULL;
  char *file=NULL;
  char *log=NULL;

  char *dflog=LOG_FILE;
  char *dffile=PORT_ID_FILE;

  int status=0;
  FILE *fp;
  struct hostent *hp,*gethostbyname();
 
  struct hostent *cname;
  pid_t root;

  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"-option",'x',&option);
  OptionAdd(&opt,"-version",'x',&version);
  OptionAdd(&opt,"l",'i',&port);
  OptionAdd(&opt,"f",'t',&file);
  OptionAdd(&opt,"lf",'t',&log);
  arg=OptionProcess(1,argc,argv,&opt,rst_opterr);

  if (arg==-1) {
    exit(-1);
  }
 
  if (help==1) {
    OptionPrintInfo(stdout,hlpstr);
    exit(0);
  }

  if (option==1) {
    OptionDump(stdout,&opt);
    exit(0);
  }

  if (version==1) {
    OptionVersion(stdout);
    exit(0);
  }

  if (argc-arg<2) {
    OptionPrintInfo(stderr,errstr);
    exit(-1);
  }
    
  host=argv[argc-2];
  remote_port=atoi(argv[argc-1]);

  if (file==NULL) file=dffile;
  if (log==NULL) log=dflog;


  /* get the address and port of the master server */
  
  master.sin_family=AF_INET;
  hp=gethostbyname(host);
  if (hp==0) {
    fprintf(stderr,"%s: unknown host0,\n",host);
    exit(2);
  }
  memcpy(&master.sin_addr,hp->h_addr,hp->h_length);
  master.sin_port=htons(remote_port);
 
  sock=socket(AF_INET,SOCK_STREAM,0); /* create our listening socket */
  if (sock<0) {
    perror("opening stream socket");
    exit(1);
  }

  /* set socket options */
  temp=setsockopt(sock,SOL_SOCKET,SO_REUSEADDR,&sc_reuseaddr,
                 sizeof(sc_reuseaddr));
  if (temp == -1)
  {
      fprintf(stderr, "Error: setsocketopt was unsuccessful in setting options\n");
      //TODO: read from errno here for more information
  }
  /* name and bind socket to an address and port number */

  server.sin_family=AF_INET;
  server.sin_addr.s_addr=INADDR_ANY;
  if (port !=0) server.sin_port=htons(port); 
  else server.sin_port=0;
  
  if (bind(sock,(struct sockaddr *) &server,sizeof(server))) {
     perror("binding stream socket");
     exit(1);
  }

  /* Find out assigned port number and print it out */

  length=sizeof(server);
  if (getsockname(sock,(struct sockaddr *) &server,&length)) {
     perror("getting socket name");
     exit(1);
  }

  fprintf(stdout,"Connected to host : %s:%d\n",host,remote_port);  
  fprintf(stdout,"Socket has port #%d\n",ntohs(server.sin_port));

  if (port==0) {
    fp=fopen(file,"w");
    fprintf(fp,"%d\n",ntohs(server.sin_port));
    fclose(fp);
    fprintf(stdout,"Recorded port number in file:%s\n",file);
  }

  fprintf(stdout,"Recording log in file:%s\n",log);

  root=getpid();

  fprintf(stdout,"Root PID is %d\n",root);

  /* if socket gets broken by the client we have to trap the SIGPIPE
   * signal
   */

   signal(SIGPIPE,SIG_IGN);
   signal(SIGCHLD,SIG_IGN); 
    

  /* the heart of the program */

  listen(sock,5); /* mark our socket willing to accept connections */
  
  do {

      /* block until someone wants to attach to us */

      FD_ZERO(&ready);
      FD_SET(sock,&ready);
      if (select(sock+1,&ready,0,0,NULL) < 0) { 
       perror("while testing for connections");
       continue;
      }
     
      /* Accept the connection from the client */

      fprintf(stdout,"Accepting a new connection...\n");
      clength=sizeof(client);
      msgout_sock=accept(sock,(struct sockaddr *) &client,&clength);
        
      if (msgout_sock==-1) {
         perror("accept"); 
         continue;
      }

      cname=gethostbyaddr((char *) &client.sin_addr,
                          sizeof (client.sin_addr),AF_INET);

      if ((cname !=NULL) && (cname->h_name !=NULL))
         sprintf(chost,"[%s (%s)]",cname->h_name,inet_ntoa(client.sin_addr));
      else sprintf(chost,"[%s]",inet_ntoa(client.sin_addr));

      msgin_sock=socket(AF_INET,SOCK_STREAM,0);
      if (msgin_sock<0) {
        perror("opening stream socket to master");        
        close(msgout_sock);
		continue;
      }      

      /* Make the sockets non-blocking */
      
      
      if ((status=fcntl(msgout_sock,F_GETFL))==-1) {
        close(msgin_sock);
        close(msgout_sock);
        msgout_sock=-1;
        msgin_sock=-1;
        continue;
      }
      
      status|=O_NONBLOCK;
      if ((status=fcntl(msgout_sock,F_SETFL,status))==-1) {
        close(msgin_sock);
        close(msgout_sock);
        msgin_sock=-1;
        msgout_sock=-1;
        continue;
      }
          
      fprintf(stdout,"Getting fcntl block...\n");
      if ((status=fcntl(msgin_sock,F_GETFL))==-1) {
        close(msgin_sock);
        close(msgout_sock);
        msgin_sock=-1;
        msgout_sock=-1;
        continue;
      }

      fprintf(stdout,"Making the socket non-blocking...\n");
      status|=O_NONBLOCK;
      if ((status=fcntl(msgin_sock,F_SETFL,status))==-1) {
        close(msgin_sock);
        close(msgout_sock);
        msgin_sock=-1;
        msgout_sock=-1;
        continue;
      } 
      
 
      fprintf(stdout,"Connecting socket to server...\n");
      if ((connect(msgin_sock,(struct sockaddr *) &master,
				  sizeof(master)) ==-1) && 
          (errno !=EINPROGRESS)) {
        perror("connecting to the master server");
        close(msgin_sock);
        close(msgout_sock);
        msgin_sock=-1;
        continue;
      }
      
      /* Connection to master server established */

      fprintf(stdout,"Creating child process...\n");
      
      if (fork() == 0) {
        close(sock);
        operate(root,msgin_sock,msgout_sock,chost,log);
      }
      
      
      fprintf(stderr,"closing the root files\n");

      close(msgin_sock);  /* close the new socket in the parent so */
      close(msgout_sock); /* that only the child has these files open */
  } while(1);

  return 0;
}
   

 











