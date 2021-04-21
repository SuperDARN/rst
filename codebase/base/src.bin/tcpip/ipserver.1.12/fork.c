/* fork.c
   ======
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

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>

#include "log_info.h"
#include "socket.h"




extern char *pname;
extern char *pidname;
  
int fork_inet(int port) {
  FILE *fp;
  char logbuf[256];  
  int status;
  int sock;
  int pipeid[2];
  pid_t cpid; /* process id of the child */

  /* create the pipe */

  if (pipe(pipeid) <0) return -1;

  /* make the pipe unblocking 
   *
   * as we are dealing with a pipe a message is
   * either sent or discarded. So there is no possibility
   * that the task will hang - however if the inet processing
   * fork does not empty the pipe quickly enough, it is possible
   * for a record to be lost.
   */

  if ((status=fcntl(pipeid[1],F_GETFL))==-1) return -1;
  status|=O_NONBLOCK;
  if ((status=fcntl(pipeid[1],F_SETFL,status))==-1) return -1; 
 
  if ((cpid=fork()) !=0) {
    close(pipeid[0]);
    return pipeid[1];
  }

  close(pipeid[1]);

  log_info("Child Server Process Starting");

  sock=create_socket(&port);
  if (sock !=-1) {
    pid_t pid;
    fp=fopen(pname,"w");
    fprintf(fp,"%d\n",port);
    fclose(fp);
    sprintf(logbuf,"Listening on port %d.",port);
    log_info(logbuf);
    sprintf(logbuf,"Port number recorded in file %s.",pname);
    log_info(logbuf);
    fp=fopen(pidname,"w");
    pid=getppid();
    sprintf(logbuf,"Process ID recorded in file %s.",pidname);
    log_info(logbuf);
    sprintf(logbuf,"Parent PID %d.",(int) pid);
    log_info(logbuf);
    fprintf(fp,"%d\n",(int) pid);
    pid=getpid();
    sprintf(logbuf,"Child PID %d.",(int) pid);
    log_info(logbuf);
    fprintf(fp,"%d\n",(int) pid);
    fclose(fp);

  } else log_info("Failed to create socket");


  if (sock !=-1) process_socket(sock,pipeid[0]);
 
  close(sock);
  close(pipeid[0]);
  log_info("Child Server Process Terminating");
  exit(0);
  
  return -1;
  
}    

