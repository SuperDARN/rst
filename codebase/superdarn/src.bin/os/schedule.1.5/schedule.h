/* schedule.h
   ==========
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


/* data structures for my scheduler routine */

#define SCHED_LINE_LENGTH 1024
#define SCHED_MAX_ENTRIES 1024
#define DELIM " \t\n"

/* each entry consists of time at which a command starts 
   + the load command */

struct scd_entry {
  double stime;
  char command[SCHED_LINE_LENGTH];
};

struct scd_blk {
  char name[256];
  int num; /* number of scheduled events */
  int cnt;
  char path[SCHED_LINE_LENGTH];
  struct scd_entry entry[SCHED_MAX_ENTRIES];
  char command[SCHED_LINE_LENGTH];
  int refresh;
  pid_t pid;
}; 
 
  
  
void print_schedule(struct scd_blk *ptr);
int set_schedule(struct scd_blk *ptr);
int load_schedule(FILE *fp,struct scd_blk *ptr);
int test_schedule(struct scd_blk *ptr);
