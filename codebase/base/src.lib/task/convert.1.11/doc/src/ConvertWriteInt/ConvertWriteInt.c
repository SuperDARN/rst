/* ConvertWriteInt.c
   =================
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



#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include "rtypes.h"
#include "rconvert.h"

#define FNAME "test.dat"

int main(int argc,char *argv[]) {
  int i;
  int32 val[8]={1,2,5,-5,6,2,18,-8};
  int fd;
  fd=open(FNAME,O_WRONLY | O_CREAT);
   
  for (i=0;i<8;i++) ConvertWriteInt(fd,val[i]);
  close(fd);
  
  return 0;
}
   

