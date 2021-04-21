/* shmem.c
   =======
   Author R.J.Barnes
*/

/*
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
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <sys/mman.h>

unsigned char *ShMemAlloc(char *memname,int size,int flags,int unlink,
			  int *fdptr) {

  int s;
  unsigned char *p;
  int fd;

  if (unlink) {
    s=shm_unlink(memname); /* ignore errors on this */
  }
  
  fd=shm_open(memname,flags,0777);
  if (fd==-1) {
    return NULL;
  }

  if (flags & O_CREAT) {
    s=ftruncate(fd,size);
    if (s==-1) {
      close(fd);
      return NULL;
    }
  }

  p=mmap(0,size,PROT_READ | PROT_WRITE,MAP_SHARED,fd,0);
  if (p==MAP_FAILED) {
    close(fd);
    return NULL;
  }

  if (fdptr !=NULL) *fdptr=fd;
  return p;

}

int ShMemFree(unsigned char *p,char *memname,int size,int unlink,int fd) {
  int s=0;
  munmap(p,size);
  close(fd);
  if (unlink) s=shm_unlink(memname);
  return s;
}

