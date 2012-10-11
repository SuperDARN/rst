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
#include "option.h"

#include "tcpipmsg.h"
#include "errlog.h"

#include "errstr.h"
#include "hlpstr.h"

#define DEF_PORT 44000

char *radar=NULL;

struct OptionData opt;

char name[256];
int yday=0;

char *get_time() {
  char *str;
  time_t tclock;
  struct tm *gmt;

  tclock=time(NULL);
  gmt = gmtime(&tclock); 
  str = asctime(gmt);
  str[strlen(str)-1] = 0; /* get rid of new line */
  return str;
}

FILE *open_file() {

  FILE *fp=NULL;
  char  daynum[16];
  char mode[10];
  time_t tclock;
  struct tm *gmt;

  tclock=time(NULL);
  gmt = gmtime(&tclock);
				
  if ((gmt->tm_yday + 1) != yday)	{
     strcpy (name, getenv("SD_ERRLOG_PATH"));
	 strcat (name, "/errlog.");
         if (radar !=NULL) {
           strcat(name,radar);
           strcat(name,".");
	 }
	 sprintf(daynum, "%.4d%.2d%.2d",1900+gmt->tm_year,gmt->tm_mon+1,
                                         gmt->tm_mday);
	 strcat (name, daynum);	/* create name of new log file */
     if (access(name,F_OK) == -1) 
	    strcpy(mode,"w");
	    else strcpy(mode,"a");
        fp = fopen (name, mode);	/* open new file */
		yday = gmt->tm_yday + 1;
        if (fp !=NULL) {
		  fprintf(fp,"%s opened at %s",name,asctime(gmt));
          fprintf(stderr,"%s opened at %s",name,asctime(gmt));
        } 
	  } else {
	    fp = fopen(name,"a");
	  }
   return fp;
}




int operate(pid_t parent,int sock) {

  int s;
  int msg;
  size_t nlen,blen;
  char *name=NULL,*buf=NULL; 
  FILE *fp=NULL;


  while(1) {

    s=TCPIPMsgRecv(sock,&msg,sizeof(int));
    if (s !=sizeof(int)) break;

    s=TCPIPMsgRecv(sock,&nlen,sizeof(size_t)); 
   if (s !=sizeof(size_t)) break;

    s=TCPIPMsgRecv(sock,&blen,sizeof(size_t)); 
       if (s !=sizeof(size_t)) break;

    name=malloc(nlen);
    if (name !=NULL) buf=malloc(blen);

    if (buf !=NULL) {
      s=TCPIPMsgRecv(sock,name,nlen);
      if (s !=nlen) break;
      s=TCPIPMsgRecv(sock,buf,blen);
      if (s !=blen) break;
      msg=ERROR_OK;
    } else msg=ERROR_FAIL;

    s=TCPIPMsgSend(sock,&msg,sizeof(int));

    
    fp=open_file();
    if (fp==NULL) fprintf(stderr,"WARNING : Error log not recording\n"); 
   
    if (fp !=NULL) fprintf(fp,"%s : %s :%s\n",get_time(),name,buf);
    fprintf(stderr,"%s : %s :%s\n",get_time(),name,buf);
    if (fp !=NULL) fclose(fp);

    if (name !=NULL) free(name);
    if (buf !=NULL) free(buf);
  }

  return 0;
}

int main(int argc,char *argv[]) {
  
  int port=DEF_PORT,arg=0;
  int sock;
  int sc_reuseaddr=1,temp;



  unsigned char help=0; 
  unsigned char option=0; 

  socklen_t length;
  socklen_t clength;

  struct sockaddr_in server;
  struct sockaddr_in client;

  fd_set ready;

  struct hostent *gethostbyname();
  pid_t root;

  int msgsock=0;

  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"-option",'x',&option);
  OptionAdd(&opt,"name",'t',&radar);

  OptionAdd(&opt,"lp",'i',&port);

  arg=OptionProcess(1,argc,argv,&opt,NULL);

  if (help==1) {
    OptionPrintInfo(stdout,hlpstr);
    exit(0);
  }

  if (option==1) {
    OptionDump(stdout,&opt);
    exit(0);
  }

  signal(SIGCHLD,SIG_IGN); 
  signal(SIGPIPE,SIG_IGN);

  root=getpid();

  sock=socket(AF_INET,SOCK_STREAM,0); /* create our listening socket */
  if (sock<0) {
    perror("opening stream socket");
    exit(1);
  }

  /* set socket options */
  temp=setsockopt(sock,SOL_SOCKET,SO_REUSEADDR,&sc_reuseaddr,
                 sizeof(sc_reuseaddr));


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
      msgsock=accept(sock,(struct sockaddr *) &client,&clength);
        
      if (msgsock==-1) {
         perror("accept"); 
         continue;
      }

      if (fork() == 0) {
        close(sock);
        operate(root,msgsock);
        exit(0);
      }
      close (msgsock);
  } while(1);

 
  return 0;

}
   

 






















