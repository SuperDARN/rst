/* fork.c
   ======
   Author: R.J.Barnes
*/

/*
   See license.txt
*/




#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>
#include <fcntl.h>
#include "loginfo.h"
#include "socket.h"

extern char pname[256];
extern char pidname[256];
extern char logfname[256];  
extern char timefname[256];


int forkinet(int port) {
  FILE *fp;
  char logbuf[256];  

  int sock;
  int pipeid[2];
  pid_t cpid; /* process id of the child */
  pid_t pid;

  /* create the pipe */

  if (pipe(pipeid) <0) return -1;

  /*
    The pipe was non-blocking, but this causes a problem if the client
    cannot cope with the incoming data rate from multiplexed streams.
  
 
      if ((status=fcntl(pipeid[1],F_GETFL))==-1) return -1;
      status|=O_NONBLOCK;
      if ((status=fcntl(pipeid[1],F_SETFL,status))==-1) return -1; 
  */ 

  if ((cpid=fork()) !=0) {
    close(pipeid[0]);
    return pipeid[1];
  }

  close(pipeid[1]);

  loginfo(logfname,"Child Server Process Starting");

  sock=createsocket(&port);
 
  if (sock==-1) {
    loginfo(logfname,"Failed to create socket");
    close(pipeid[0]);
    exit(0);
  }

  fp=fopen(pname,"w");
  fprintf(fp,"%d\n",port);
  fclose(fp);
  sprintf(logbuf,"Listening on port %d.",port);
  loginfo(logfname,logbuf);
  sprintf(logbuf,"Port number recorded in file %s.",pname);
  loginfo(logfname,logbuf);
  sprintf(logbuf,"Time of last packet recorded in file %s.",timefname);
  loginfo(logfname,logbuf);
  fp=fopen(pidname,"w");
  pid=getppid();
  sprintf(logbuf,"Process ID recorded in file %s.",pidname);
  loginfo(logfname,logbuf);
  sprintf(logbuf,"Parent PID %d.",(int) pid);
  loginfo(logfname,logbuf);
  fprintf(fp,"%d\n",(int) pid);
  pid=getpid();
  sprintf(logbuf,"Child PID %d.",(int) pid);
  loginfo(logfname,logbuf);
  fprintf(fp,"%d\n",(int) pid);
  fclose(fp);

  processsocket(sock,pipeid[0]);
  close(sock);
  close(pipeid[0]);
  loginfo(logfname,"Child Server Process Terminating");
  exit(0); 
  return -1;
}    

