/* pipe.c
   ======
   Author: R.J.Barnes
 Copyright (c) 2012 The Johns Hopkins University/Applied Physics Laboratory

This file is part of the Radar Software Toolkit (RST).

RST is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see <https://www.gnu.org/licenses/>.

Modifications:
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>
#include "connex.h"


int pipewrite(int fildes,unsigned char *buffer,unsigned int size) {
  int byte;
  int cnt=0;
  int hsze=0;

  unsigned char hdr[sizeof(int)];

  *((int *)hdr)=size;

  hsze=sizeof(int);
  while (cnt<hsze) {
    byte=write(fildes,hdr+cnt,hsze-cnt);
    if (byte<=0) {
      ConnexErr=MSGERR_PIPE_CLOSED;
      break;
    }
    cnt+=byte;
  }
  if (cnt<hsze) return -1;

  cnt=0;  /* write out the message */
  while (cnt<size) {
    byte=write(fildes,buffer+cnt,size-cnt);
    if (byte<=0) {
      ConnexErr=MSGERR_PIPE_CLOSED;
      break;
    }
    cnt+=byte;
  }
  if (cnt<size) return -1;
  return size;
}


int piperead(int fildes,unsigned char **buffer) {

  int byte;
  int cnt=0;

  int hsze=sizeof(int);
  unsigned char hdr[sizeof(int)];

  unsigned int size;

  while (cnt<hsze) {
    byte=read(fildes,hdr,hsze);
    if (byte<=0) {
      ConnexErr=MSGERR_PIPE_CLOSED;
      break;
    }
    cnt+=byte;
  }
  if (cnt<hsze) return -1;

  size=*((int *)hdr);

  if (size==0) {
    if (*buffer !=NULL) free(*buffer);
    *buffer=NULL;
    return 0;
  }

  if (*buffer !=NULL) {
    unsigned char *tmp;
    if ((tmp=realloc(*buffer,size)) ==NULL) {
      ConnexErr=MSGERR_NOMEM;
      return -1;
    }
    *buffer=tmp;
  } else {
    if ((*buffer=malloc(size)) ==NULL) {
      ConnexErr=MSGERR_NOMEM;
      return -1;
    }
  }

  cnt=0;  /* read in the message */
  while (cnt<size) {
    byte=read(fildes,(*buffer)+cnt,size-cnt);
    if (byte<=0) {
      ConnexErr=MSGERR_PIPE_CLOSED;
      break;
    }
    cnt+=byte;
  }

  if (cnt<size) {
    free(*buffer);
    *buffer=NULL;
    return -1;
  }

  return size;
}
