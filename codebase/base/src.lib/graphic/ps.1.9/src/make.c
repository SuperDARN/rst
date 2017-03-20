/* make.c
   ====== 
   Author: R.J.Barnes
*/

/*
   See license.txt
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rfbuffer.h"
#include "rps.h"



struct PostScript *PostScriptMake() {
  struct PostScript *ptr;
  ptr=malloc(sizeof(struct PostScript));
  ptr->text.func=NULL;
  ptr->text.data=NULL;
  return ptr;
}

void PostScriptFree(struct PostScript *ptr) {
  if (ptr==NULL) return;
  free(ptr);
};

int PostScriptSetText(struct PostScript *ptr,
                      int (*text)(char *,int,void *),void *data) {
  if (ptr==NULL) return -1;
  ptr->text.func=text;
  ptr->text.data=data;
  return 0;
}

