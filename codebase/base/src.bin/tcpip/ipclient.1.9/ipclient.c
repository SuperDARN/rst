/* ipclient.c
   ==========
   Author: R.J.Barnes
 LICENSE AND DISCLAIMER
 
 Copyright (c) 2012 The Johns Hopkins University/Applied Physics Laboratory
 
 This file is part of the Radar Software Toolkit (RST).
 
 RST is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with RST.  If not, see <http://www.gnu.org/licenses/>.
 
 
 
*/

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>
#include "option.h"
#include "connex.h"
#include "errstr.h"
#include "hlpstr.h"



struct OptionData opt;

int rst_opterr(char *txt) {
  fprintf(stderr,"Option not recognized: %s\n",txt);
  fprintf(stderr,"Please try: ipclient --help\n");
  return(-1);
}

int main(int argc,char *argv[]) {
  int arg;
  int sock;
  int remote_port=0;
  char *host;
  int flag,status,size=0;
  unsigned char help=0;
  unsigned char option=0;

  struct timeval tmout;

  int timeout=120;
  int tflg=0;
  int cflg=0;

  unsigned char *buffer=NULL;
 
  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"-option",'x',&option);
  OptionAdd(&opt,"connect",'x',&cflg);
  OptionAdd(&opt,"tmout",'i',&timeout);
  OptionAdd(&opt,"test",'x',&tflg);

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


  if (argc-arg<2) { 
    OptionPrintInfo(stderr,errstr);
    exit(-1);
  }

  host=argv[argc-2];
  remote_port=atoi(argv[argc-1]);

  tmout.tv_sec=timeout;
  tmout.tv_usec=0;

  fprintf(stderr,"opening connection %s %d\n",host,remote_port); 
  sock=ConnexOpen(host,remote_port,&tmout); 
  if (sock<0) {
    if (tflg) fprintf(stdout,"0\n");
    else fprintf(stderr,"Could not connect to host.\n");
    exit(255);
  }

  if (cflg) {
    fprintf(stdout,"0\n");
    exit(0);
  }

  do {

   status=ConnexRead(1,&sock,&buffer,&size,&flag,&tmout);

   if (status==-1) break;
   if (flag !=-1) {
     if (tflg) break;
     else { 
       fprintf(stderr,"Message size:%d\n",size);
       fprintf(stderr,"Message content:%s\n",buffer);
     }
   }           
  } while(tflg==0);
  if (tflg) {
    fprintf(stdout,"%d %d %d\n",size,status,flag);
    if ((status==-1) || (flag==-1)) return 255;
    if (flag==1) return 1;
    return 0;
  }  else fprintf(stderr,"Connection failed.\n");
  return 1;

}
   

 










