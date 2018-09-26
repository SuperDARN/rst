/* print_schedule.c
   ================
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
#include <sys/types.h>
#include "rtime.h"
#include "log_info.h"
#include "schedule.h"


void print_schedule(struct scd_blk *ptr) {/* prints out the schedule */
  int c;
  char txt[256];
  if (ptr->num==0) {
    log_info(1,"No schedule file loaded");
    return;
  }
  
  sprintf(txt,"Schedule file %s loaded",ptr->name);
  log_info(1,txt);
  sprintf(txt,"Command path -> %s",ptr->path);
  log_info(1,txt);
  if (ptr->entry[0].stime==-1) {
    sprintf(txt,"Default Program -> %s\n",ptr->entry[0].command);
    log_info(1,txt);
  }
  if ((ptr->cnt >0) && (ptr->cnt<ptr->num)) {
    int yr,mo,dy,hr,mt;
    double sc;
    log_info(1,"Pending programs :\n");    		 
    for (c=ptr->cnt;c<ptr->num;c++) {
      if (ptr->entry[c].stime==-1) continue;
      TimeEpochToYMDHMS(ptr->entry[c].stime,&yr,&mo,&dy,&hr,&mt,&sc);
      sprintf(txt,"%d : %d %d %d : %d %d -> %s",c,yr,mo,dy,hr,mt,
	          ptr->entry[c].command);
      log_info(1,txt);       
    } 
  } else log_info(1,"There are no pending programs");
  sprintf(txt,"\nCurrent program ->%s",ptr->command);
  log_info(1,txt);
  sprintf(txt,"Schedule reloaded every %d seconds",ptr->refresh);
  
  log_info(1,txt);
}  			
