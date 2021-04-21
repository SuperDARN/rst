/* document.c
   ========== 
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
#include "rfbuffer.h"
#include "fontdb.h"
#include "rps.h"
#include "iplot.h"
#include "splot.h"



int SplotDocumentStart(struct Splot *ptr,char *name,
                      char *pages,float wdt,float hgt,int depth) {



  if (ptr==NULL) return -1;
  if (ptr->ps.ptr !=NULL) 
     return PostScriptDocumentStart(ptr->ps.ptr,ptr->ps.xpoff,ptr->ps.ypoff,
                            wdt,hgt,ptr->ps.land);


  return 0;
}

int SplotDocumentEnd(struct Splot *ptr) {
 
  if (ptr==NULL) return -1;
  if (ptr->ps.ptr !=NULL) return PostScriptDocumentEnd(ptr->ps.ptr);

  return 0;
}



