/* check_program.c
   ===============
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
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <errno.h>
#include "log_info.h"
#include "schedule.h"
#include "execute.h"

int confirm_program(char *path,char *prog) {
  char fname[2*SCHED_LINE_LENGTH+1];
  char *fptr;
  FILE *fp;
  sprintf(fname,"%s/%s",path,prog);
  fptr=strtok(fname," "); 
  fp=fopen(fptr,"r");
  if (fp !=NULL) fclose(fp);
  return (fp !=NULL);
}  
  



void check_program(struct scd_blk *ptr,int cnt) {
  int s;
  char txt[1024];
  if (cnt<0) cnt=0;

  if (ptr->pid !=-1) {
    errno=0;
    s=getpriority(PRIO_PROCESS,ptr->pid);
    if ((errno==0) &&
        (strcmp(ptr->command,ptr->entry[cnt].command)==0)) return;
  } 
  if (confirm_program(ptr->path,ptr->entry[cnt].command)==0) {
    sprintf(txt,"Program %s not found",ptr->entry[cnt].command);
    log_info(0,txt);
     return;
  }
  /* stop the old program here */

  if (ptr->pid !=-1) {
    log_info(0,"Stopping current program");
    terminate(ptr->pid);
  } 
  sprintf(txt,"Starting program:%s",ptr->entry[cnt].command);
  log_info(0,txt);
  if ((ptr->pid=execute(ptr->path,ptr->entry[cnt].command))==-1) {
    log_info(0,"Program failed to start - trying default program");
    if (confirm_program(ptr->path,ptr->entry[0].command)==0) {
      log_info(0,"Default program not found");  
      return;
    }
    if ((ptr->pid=execute(ptr->path,ptr->entry[0].command))==-1) {
      log_info(0,"Default program failed to start");
      return;
    }
  }  

  strcpy(ptr->command,ptr->entry[cnt].command);
  return;
}
