/* rxml.c
   ======
   Author: R.J.Barnes
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
#include "rxml.h"



int XMLCallEnd(struct XMLdata *ptr,char *name,char *buf,int sze) {
  if (ptr->interface.end.func !=NULL) 
      return (ptr->interface.end.func)(name,buf,sze,ptr->interface.end.data);
  return 0;   
}

int XMLCallStart(struct XMLdata *ptr,
                 char *name,char end,int atnum,char **atname,char **atval,
                 char *buf,int sze) {
  
 if (ptr->interface.start.func !=NULL)
   return (ptr->interface.start.func)(name,end,atnum,atname,atval,
                               buf,sze,ptr->interface.start.data);
  return 0;

}






