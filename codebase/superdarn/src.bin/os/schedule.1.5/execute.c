/* execute.c
   =========
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

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <libgen.h>
#include <time.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/wait.h>
#include "schedule.h"

#define TIME_OUT 10

int resetflg=0;

void terminate_signal_handler(int sig_number) { /* null handler for time out */
  resetflg=1;
}

void terminate(pid_t prog_id) {
  
  
  sigset_t set;
  struct sigaction act;
  struct sigaction oldact; 

  struct itimerval timer;

  sigemptyset(&set);
  sigaddset(&set,SIGALRM);
 
  act.sa_flags=0;
  act.sa_mask=set;
  act.sa_handler=terminate_signal_handler;
  sigaction(SIGALRM,&act,&oldact);

  /* kill off a running program */

  timer.it_interval.tv_sec=0;
  timer.it_interval.tv_usec=0;
  timer.it_value.tv_sec=TIME_OUT;
  timer.it_value.tv_usec=0;

  setitimer(ITIMER_REAL,&timer,NULL);

  kill(prog_id,SIGUSR1);
  waitpid(prog_id,NULL,0);
  
  if (resetflg==1) {  
    fprintf(stderr,"killing task.\n");
    kill(prog_id,SIGKILL);
    waitpid(prog_id,NULL,0);
    resetflg=0;
  }

  timer.it_value.tv_sec=0;
  timer.it_value.tv_usec=0;
  setitimer(ITIMER_REAL,&timer,NULL);

  sigaction(SIGALRM,&oldact,NULL);

}
   


int execute(char *path,char *prog) {
  int pid,i;
  int s;
  char command[2*SCHED_LINE_LENGTH+1];
  char *name;
  char *argv[256];

  /* build the argument vector */

  sprintf(command,"%s/%s",path,prog);
  name=strtok(command," ");
  argv[0]=basename(name);
  for (i=1;(argv[i]=strtok(NULL," ")) !=NULL;i++);
  pid=fork();

  if (pid==0) { 
    s=execv(name,argv);
    exit(0);
  }
  return pid;   

}
  
  



