/* document.c
   ========== 
   Author: R.J.Barnes
*/


/*
   See license.txt
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



