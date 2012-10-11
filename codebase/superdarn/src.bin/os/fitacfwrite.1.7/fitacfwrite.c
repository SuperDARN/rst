/* fitacfwrite
    =========== 
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
#include <zlib.h>
#include "rtypes.h"
#include "option.h"
#include "tcpipmsg.h"

#include "dmap.h"
#include "rprm.h"
#include "fitdata.h"
#include "radar.h"
#include "rmsg.h"
#include "rmsgrcv.h"
#include "fio.h"
#include "fitwrite.h"
#include "errlog.h"

#include "errstr.h"
#include "hlpstr.h"

#include "version.h"

#define DEF_PORT 44102

struct OptionData opt;

struct RadarNetwork *network;  
struct Radar *radar;
struct RadarSite *site;

struct RMsgBlock rblk;
unsigned char *store=NULL; 

struct DMsg {
  int tag;
  void *pbuf;
  void *fbuf;
};

int dnum=0;
int dptr=0;
struct DMsg dmsg[32];


struct RadarParm *prm;
struct FitData *fit;


char *chn=NULL;

char *taskname="fitacfwrite";

char *errhost=NULL;
char *derrhost="127.0.0.1";
int errport=44000;
int errsock=-1;

char errbuf[1024];

int operate(pid_t parent,int sock) {
  FILE *fp;
  int s,i;
  int msg,rmsg;
  int flg=0;

  char *filename=NULL;

  unsigned char *cbufadr=NULL;
  size_t cbuflen;

  while(1) {

    s=TCPIPMsgRecv(sock,&msg,sizeof(int));

    if (s !=sizeof(int)) break;
    rmsg=TASK_OK;
    switch (msg) {
    
    case TASK_OPEN:
      ErrLog(errsock,taskname,"Opening file.");
      rmsg=RMsgRcvDecodeOpen(sock,&cbuflen,&cbufadr);

      if (rmsg==TASK_OK) flg=1;
      break;
    case TASK_CLOSE:
      ErrLog(errsock,taskname,"Closing file.");
      if (filename !=NULL) free(filename);
      filename=NULL;
      break;
    case TASK_RESET:
      ErrLog(errsock,taskname,"Reset.");
      if (filename !=NULL) free(filename);
      filename=NULL;
      break;
    case TASK_QUIT:
      ErrLog(errsock,taskname,"Stopped.");
      TCPIPMsgSend(sock,&rmsg,sizeof(int));
      exit(0);
      break;
    case TASK_DATA:
      ErrLog(errsock,taskname,"Received Data.");
      rmsg=RMsgRcvDecodeData(sock,&rblk,&store);    
    default:
      break;  
    }
    TCPIPMsgSend(sock,&rmsg,sizeof(int));

    if (msg==TASK_DATA) {
      dnum=0;
      for (i=0;i<rblk.num;i++) {
        for (dptr=0;dptr<dnum;dptr++) 
          if (dmsg[dptr].tag==rblk.data[i].tag) break;
          if (dptr==dnum) {
            dmsg[dptr].tag=rblk.data[i].tag;
            dmsg[dptr].pbuf=NULL;
            dmsg[dptr].fbuf=NULL;
            dnum++;
	  }
          switch (rblk.data[i].type) {
	  case PRM_TYPE:
            dmsg[dptr].pbuf=rblk.ptr[rblk.data[i].index];
	    break;
          case FIT_TYPE:
            dmsg[dptr].fbuf=rblk.ptr[rblk.data[i].index];
	    break;
          default:
            break;
	  }
      }


      for (dptr=0;dptr<dnum;dptr++) {
	if (dmsg[dptr].pbuf==NULL) continue;
        if (dmsg[dptr].fbuf==NULL) continue;
        RadarParmExpand(prm,dmsg[dptr].pbuf);
        FitExpand(fit,prm->nrang,dmsg[dptr].fbuf);

        if ((filename==NULL) && (flg !=0)) {
          flg=0;
          filename=FIOMakeFile(getenv("SD_FIT_PATH"),
			       prm->time.yr,
                               prm->time.mo,
                               prm->time.dy,
                               prm->time.hr,
                               prm->time.mt,
                               prm->time.sc,
			       RadarGetCode(network,prm->stid,0),
			       chn,"fitacf",0,0);
	}
        if (filename !=NULL) {
          fp=fopen(filename,"a");
          if (fp==NULL) ErrLog(errsock,taskname,"Error opening file.");
	  else {
            s=FitFwrite(fp,prm,fit);  
            if (s==-1) break;
	  }
          if (s==-1) ErrLog(errsock,taskname,"Error writing record.");
          fclose(fp);
	}
      }
      if (store !=NULL) free(store);
      store=NULL;
    }  
  }


  return 0;
}

int main(int argc,char *argv[]) {
  FILE *fp=NULL;
  char *envstr=NULL;
  
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

  prm=RadarParmMake();
  fit=FitMake();

  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"-option",'x',&option);

  OptionAdd(&opt,"lp",'i',&port);
  OptionAdd(&opt,"eh",'t',&errhost);
  OptionAdd(&opt,"ep",'i',&errport);
  OptionAdd(&opt,"c",'t',&chn);

  arg=OptionProcess(1,argc,argv,&opt,NULL);

  if (help==1) {
    OptionPrintInfo(stdout,hlpstr);
    exit(0);
  }

  if (option==1) {
    OptionDump(stdout,&opt);
    exit(0);
  }

  envstr=getenv("SD_RADAR");
  if (envstr==NULL) {
    fprintf(stderr,"Environment variable 'SD_RADARNAME' must be defined.\n");
    exit(-1);
  }

  fp=fopen(envstr,"r");

  if (fp==NULL) {
    fprintf(stderr,"Could not locate radar information file.\n");
    exit(-1);
  }

  network=RadarLoad(fp);
  fclose(fp); 
  if (network==NULL) {
    fprintf(stderr,"Failed to read radar information.\n");
    exit(-1);
  }

  if (errhost==NULL) errhost=derrhost;
  errsock=TCPIPMsgOpen(errhost,errport);

  sprintf(errbuf,"Started (version %s.%s) listening on port %d",
          MAJOR_VERSION,MINOR_VERSION,port);
  ErrLog(errsock,taskname,errbuf);  


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
   

 






















