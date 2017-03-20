/* rscript.c
   ======== 
   Author: R.J.Barnes
*/

/*
   See license.txt
*/

#include <stdio.h>
#include <stdlib.h>
#include "rscript.h"



struct ScriptData *ScriptMake() {
  struct ScriptData *ptr;
  ptr=malloc(sizeof(struct ScriptData));
  if (ptr==NULL) return NULL;
  ptr->buf=NULL;
  ptr->depth=0;
  ptr->sze=0;
  ptr->max=0;
  ptr->stp=100;
  ptr->text.func=NULL;
  return ptr;
}

void ScriptFree(struct ScriptData *ptr) {
  if (ptr==NULL) return;
  if (ptr->buf !=NULL) free(ptr->buf);
  free(ptr);
}

int ScriptSetText(struct ScriptData *ptr,
             int (*text)(char *,int,void *),void *data) {

  if (ptr==NULL) return -1;
  ptr->text.func=text;
  ptr->text.data=data;
  return 0;
} 


int ScriptDecode(struct ScriptData *ptr,char *buf,int sze) {
  int status=0;
  int c=0;
  int s=0;
          
  while(c<sze) {

    if (buf[c]=='<') { 
      
      if ((ptr->depth==0) && ((c-s) !=0) && (ptr->text.func !=NULL)) 
         status=(ptr->text.func)(buf+s,c-s,ptr->text.data);
      
      if (status !=0) return status;
      if (ptr->buf==NULL) { 
        ptr->buf=malloc(ptr->stp);
        if (ptr->buf==NULL) return -1;
        ptr->max=ptr->stp;
        ptr->sze=0;
      }
      ptr->buf[ptr->sze]='<';
      ptr->sze++;
      ptr->depth++;
    
    } else if (buf[c]=='>') {
    
      if (ptr->buf == NULL) return -1;

      ptr->buf[ptr->sze]='>';
      ptr->sze++;
      ptr->depth--;

      /* at this point we can user the tag on */

      if (ptr->depth==0) {   
        char *tmpbuf;
        int tmpsze;

        /* make temporary copies of the buffer pointer and its size */

        tmpbuf=ptr->buf;
        tmpsze=ptr->sze;

        /* reset the structure so that recursion is possible */

        ptr->sze=0;
        ptr->buf=NULL;
        ptr->max=0;

        status=(ptr->text.func)(tmpbuf,tmpsze,ptr->text.data);

        if (tmpbuf !=NULL) free(tmpbuf);
        if (status !=0) return status;
        s=c+1;
      }
    } else { /* text data or tag contents */
      if (ptr->buf !=NULL) { /* part of a tag */
        ptr->buf[ptr->sze]=buf[c];
        ptr->sze++;
        if (ptr->sze==ptr->max) {
          char *tmp;
          ptr->max+=ptr->stp;
          tmp=realloc(ptr->buf,ptr->max);
          if (tmp==NULL) return -1;
          ptr->buf=tmp;
	}
      } 
    }
    c++;
  }
  if ((ptr->buf==NULL) && ((c-s) !=0) && (ptr->text.func !=0)) 
     status=(ptr->text.func)(buf+s,c-s,ptr->text.data);
  
  return status;
}

