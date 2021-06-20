/* connex.c
   ========
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
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include "connex.h"



int ConnexOpen(char *host,int remote_port,struct timeval *tmout) {
  long arg;
  int msgin_sock;
  struct sockaddr_in master;
  struct hostent *hp,*gethostbyname();
  fd_set cnx_set;
  fd_set exp_set;

  int valopt=0;
  socklen_t optlen;


  int s=0;

  optlen=sizeof(int);

  /* get the address and port of the master server */
  
  master.sin_family=AF_INET;

  hp=gethostbyname(host);

  if (hp==0) {
    /* unknown host error */
    ConnexErr=UNKNOWN_HOST;
    return -1;
  }

  memcpy(&master.sin_addr,hp->h_addr,hp->h_length);
  master.sin_port=htons(remote_port);

  msgin_sock=socket(AF_INET,SOCK_STREAM,0);
 
  if (msgin_sock<0) {
    /*opening stream connection to master*/
     ConnexErr=OPEN_FAIL;
     return -1;
  }

  
  arg=fcntl(msgin_sock,F_GETFL,NULL);
  arg |= O_NONBLOCK;
  fcntl(msgin_sock,F_SETFL,arg);


  s=connect(msgin_sock,(struct sockaddr *) &master,
	      sizeof(master));

  if (s<0) {
    if (errno==EINPROGRESS) {

      FD_ZERO(&cnx_set);
      FD_SET(msgin_sock,&cnx_set);
      FD_ZERO(&exp_set);
      FD_SET(msgin_sock,&exp_set);

      s=select(FD_SETSIZE,NULL,&cnx_set,&exp_set,tmout);
      if (s>0) {

        s=getsockopt(msgin_sock,SOL_SOCKET,SO_ERROR,&valopt,&optlen);

        if ((s<0) || (valopt !=0)) {
          close(msgin_sock);
          ConnexErr=CONNECT_FAIL;
          return -1;    
        }
      } else {
        close(msgin_sock);
        ConnexErr=CONNECT_FAIL;
        return -1;    
      }
    } else {
      close(msgin_sock);
      ConnexErr=CONNECT_FAIL;
      return -1;
    }
  }

  arg=fcntl(msgin_sock,F_GETFL,NULL);
  arg &= (~O_NONBLOCK);
  fcntl(msgin_sock,F_SETFL,arg);
  
  return msgin_sock;
}

void ConnexClose(int msgin_sock) {
 shutdown(msgin_sock,2); 
 close(msgin_sock);
}

int ConnexRead(int num,int *sock,unsigned char **buffer,int *size,int *flag,
                struct timeval *tmout) {
  fd_set read_set;
  fd_set excp_set;
  int status=0,i=0;
  
  FD_ZERO(&read_set);
  FD_ZERO(&excp_set);

  for (i=0;i<num;i++) {
    if (sock[i] !=-1) FD_SET(sock[i],&read_set);
    if (sock[i] !=-1) FD_SET(sock[i],&excp_set);

    flag[i]=0;
  }

  if ((status=select(FD_SETSIZE,&read_set,0,&excp_set,
		     tmout)) < 0) { 
     perror("while waiting client/master I/O");
     return -1;
  }

  if (status==0) return 0;

  for (i=0;i<num;i++) {
    if ((sock[i] !=-1) && (FD_ISSET(sock[i],&excp_set))) {
   
      flag[i]=-1;
      continue;
    }
    if ((sock[i] !=-1) && (FD_ISSET(sock[i],&read_set))) {
      size[i]=ConnexReadIP(sock[i],&buffer[i]);
      if (size[i]==-1) flag[i]=-1;
      else flag[i]=1;
    } 

  }    
  return status;
}
   

 















