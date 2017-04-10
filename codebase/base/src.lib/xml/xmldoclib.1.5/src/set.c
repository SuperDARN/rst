/* set.c
   ===== 
   Author: R.J.Barnes
*/

/* 
   See license.txt
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rxml.h"
#include "rxmldb.h"
#include "entity.h"
#include "tagdb.h"

#include "getstring.h"
#include "rxmldoc.h"



int XMLdocSetText(struct XMLdocdata *ptr,
             int (*text)(char *,int,
                         void *),void *data) {

  if (ptr==NULL) return -1;
  return EntitySetText(ptr->ent,text,data);
} 
