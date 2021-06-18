/* getstring.c
   =========== 
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
#include <string.h>
#include "rxml.h"
#include "rxmldb.h"



char *getDBstring(struct XMLDBbuffer *ptr) {
  int s=0;
  char *str=NULL;
  char zero[1];
  struct XMLDBbuffer *decode=NULL;  

  zero[0]=0;
  decode=XMLDBCopyBuffer(ptr);
  if (decode==NULL) return NULL;
  s=XMLDBAddBuffer(decode,zero,1);
  if (s !=0) {
    XMLDBFreeBuffer(decode);
    return NULL;
  }
  str=decode->buf;
  free(decode);
  return str;
}

