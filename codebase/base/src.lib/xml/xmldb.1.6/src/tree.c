/* tree.c
   ====== 
   Author R.J.Barnes
*/

/*
   See license.txt
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rxml.h"
#include "rxmldb.h"



struct XMLDBtree *XMLDBMakeTree() {
  struct XMLDBtree *ptr;

  ptr=malloc(sizeof(struct XMLDBtree));
  if (ptr==NULL) return NULL;
  ptr->depth=0;
  ptr->name=NULL;

  return ptr;
}

void XMLDBFreeTree(struct XMLDBtree *ptr) {
  int i;

  if (ptr==NULL) return;
  if (ptr->name !=NULL) {
    for (i=0;i<ptr->depth;i++) if (ptr->name[i] !=NULL) free(ptr->name[i]);
    free(ptr->name);
  }
  free(ptr);
}

int XMLDBBuildTree(char *path,char delim,struct XMLDBtree *ptr) {

  int c=0;
  int s=0;
  int i;
  char **tmp;

  ptr->depth=0;
  if (ptr->name !=NULL) {
    for (i=0;i<ptr->depth;i++) if (ptr->name[i] !=NULL) free(ptr->name[i]);
    free(ptr->name);
    ptr->name=NULL;
  }

  while (c<strlen(path)) {
    if (path[c]==delim) {
      if ((c-s)>0) {
        if (ptr->name==NULL) ptr->name=malloc(sizeof(char *));
        else {
          tmp=realloc(ptr->name,sizeof(char *)*
                                 (ptr->depth+1));
          if (tmp==NULL) return -1;
          ptr->name=tmp;
	}
        if (ptr->name==NULL) return -1;
        ptr->name[ptr->depth]=malloc(c-s+1);
        if (ptr->name[ptr->depth]==NULL) return -1;
        memcpy(ptr->name[ptr->depth],path+s,c-s);
        ptr->name[ptr->depth][c-s]=0;
        ptr->depth++;
        s=c+1;
      }
    }
    c++;
  }    

  if ((c-s)>0) {
    if (ptr->name==NULL) ptr->name=malloc(sizeof(char *));
      else {
        tmp=realloc(ptr->name,sizeof(char *)*(ptr->depth+1));
        if (tmp==NULL) return -1;
        ptr->name=tmp;
      }
    if (ptr->name==NULL) return -1;

    ptr->name[ptr->depth]=malloc(c-s+1);
    if (ptr->name[ptr->depth]==NULL) return -1;
    memcpy(ptr->name[ptr->depth],path+s,c-s);
    ptr->name[ptr->depth][c-s]=0;
    ptr->depth++;
  }
  return 0;
}
