/* fork.c
   ======
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

#include <sys/types.h>
#include <sys/time.h>
#include <sys/select.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include "rtypes.h"
#include "errlog.h"
#include "socket.h"
  
extern char *taskname;
extern int errsock;

int forkinet(int port) {
 
  char errbuf[256];  
  int status;
  int sock;
  int pipeid[2];
  pid_t cpid; /* process id of the child */

  /* create the pipe */

  if (pipe(pipeid) <0) return -1;

  if ((status=fcntl(pipeid[1],F_GETFL))==-1) return -1;
  status|=O_NONBLOCK;
  if ((status=fcntl(pipeid[1],F_SETFL,status))==-1) return -1; 
 
  if ((cpid=fork()) !=0) {
    close(pipeid[0]);
    return pipeid[1];
  }

  close(pipeid[1]);

  ErrLog(errsock,taskname,"Child server process starting");
 
  sock=createsocket(&port);
  if (sock !=-1) {
    pid_t pid;
    sprintf(errbuf,"Listening on port %d.",port);
    ErrLog(errsock,taskname,errbuf);
    pid=getppid();
    sprintf(errbuf,"Parent PID %d.",(int) pid);
    ErrLog(errsock,taskname,errbuf);
    pid=getpid();
    sprintf(errbuf,"Child PID %d.",(int) pid);
    ErrLog(errsock,taskname,errbuf);
 

  } else ErrLog(errsock,taskname,"Failed to create Socket");

  if (sock !=-1) procsocket(sock,pipeid[0]);
	   
  close(sock);
  close(pipeid[0]);

  ErrLog(errsock,taskname,"Child Server process terminating");

  exit(0);
  
  return -1;
  
}    

