/* tcpipmsg.c
   ==========
   Author: J.Spaleta & R.J.Barnes
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



#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "tcpipmsg.h"

int TCPIPMsgOpen(char *hostip, int port) {

  /* DECLARE VARIABLES FOR IP CONNECTIONS */

  int sock,temp;
  struct sockaddr_in server;
  struct hostent *hp, *gethostbyname();
  int option;
  socklen_t optionlen;

  sock=socket(AF_INET, SOCK_STREAM, 0);
  if (sock<0) {
    TCPIPMsgErr=OPEN_FAIL;
    return -1;
  }

  server.sin_family=AF_INET;
  hp=gethostbyname(hostip);
  if(hp == 0) {
     TCPIPMsgErr=UNKNOWN_HOST;
     return -1;
  }	
	
  memcpy(&server.sin_addr, hp->h_addr, hp->h_length);
  server.sin_port=htons(port);
  temp=connect(sock, (struct sockaddr *)&server, sizeof(server));
  if(temp < 0) {
    TCPIPMsgErr=CONNECT_FAIL;
    return -1;
  }

  option=TCP_NODELAY;
  optionlen=4;
  temp=setsockopt(sock,6,TCP_NODELAY,&option,optionlen);
  temp=getsockopt(sock,6,TCP_NODELAY,&option,&optionlen);

  optionlen=4;
  option=32768;
  temp=setsockopt(sock,SOL_SOCKET,SO_SNDBUF,&option,optionlen);
  temp=getsockopt(sock,SOL_SOCKET,SO_SNDBUF,&option,&optionlen);

   optionlen=4;
   option=32768;
   temp=setsockopt(sock,SOL_SOCKET,SO_RCVBUF,&option,optionlen);
   temp=getsockopt(sock,SOL_SOCKET,SO_RCVBUF,&option,&optionlen);

   return sock;
}

int TCPIPMsgSend(int fd,void  *buf,size_t buflen) {
  int cc=0,total=0;
  while (buflen > 0) {
    cc = send(fd, buf, buflen, 0);
    if (cc <= 0) return cc;
    buf += cc;
    total += cc;
    buflen -= cc;
  }
  return total;
}

int TCPIPMsgRecv(int fd,void *buf,size_t buflen) {
  int cc=0,total=0;
  while (buflen > 0) {
    cc = recv(fd, buf, buflen, 0);
    if (cc <=0) return cc;
    buf += cc;
    total += cc;
    buflen -= cc;
  }
  return total;
}


