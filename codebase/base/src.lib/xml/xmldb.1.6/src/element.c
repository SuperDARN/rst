/* element.c
   ========= 
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



struct XMLDBelement *XMLDBMakeElement(char *name,char end,int atnum, 
                                     char **atname,char **atval,int stp) {
  int s=0,i=0;
  struct XMLDBelement *ptr;

  ptr=malloc(sizeof(struct XMLDBelement));
  if (ptr==NULL) return NULL;
 
  ptr->name=malloc(strlen(name)+1);  
  if (ptr->name==NULL) {
    free(ptr);
    return NULL;
  }

  strcpy(ptr->name,name);
  ptr->data=NULL;
  ptr->atnum=atnum;
  ptr->atname=NULL;
  ptr->atval=NULL;
  ptr->end=end;


  if (atnum !=0) {
    ptr->atname=malloc(sizeof(char *)*atnum);
    if (ptr->atname==NULL) s=-1;
    if (s==0) ptr->atval=malloc(sizeof(char *)*atnum);
    if (ptr->atval==NULL) s=-1;

    if (s==0) for (i=0;i<atnum;i++) {
      ptr->atname[i]=NULL;
      ptr->atval[i]=NULL;
 
      if (atname[i] !=NULL) {
	ptr->atname[i]=malloc(strlen(atname[i])+1);
        if (ptr->atname[i]==NULL) break;
        strcpy(ptr->atname[i],atname[i]);
      }

      if (atval[i] !=NULL) {
	ptr->atval[i]=malloc(strlen(atval[i])+1);
        if (ptr->atval[i]==NULL) break;
        strcpy(ptr->atval[i],atval[i]);
      }
      
    }
    if (i<atnum) s=-1;
    ptr->atnum=atnum;
  }

  

  if (s==0) ptr->data=XMLDBMakeBuffer(stp);
  if (ptr->data==NULL) s=-1;

  if (s !=0) {
    for (i=0;i<atnum;i++) {
      if ((ptr->atname !=NULL) && (ptr->atname[i] !=NULL)) 
        free(ptr->atname[i]);
      if ((ptr->atval !=NULL) && (ptr->atval[i] !=NULL)) 
        free(ptr->atval[i]);
    }
    if (ptr->atname !=NULL) free(ptr->atname);
    if (ptr->atval !=NULL) free(ptr->atval);
    if (ptr->data !=NULL) XMLDBFreeBuffer(ptr->data);
    free(ptr->name);
    free(ptr);
    return NULL;
  }
 
  return ptr;
}

void XMLDBFreeElement(struct XMLDBelement *ptr) {
  int i;

  if (ptr==NULL) return;
  if (ptr->data !=NULL) XMLDBFreeBuffer(ptr->data);
  if (ptr->name !=NULL) free(ptr->name);

  if (ptr->atnum>0) {
    for (i=0;i<ptr->atnum;i++) {
      if ((ptr->atname !=NULL) && (ptr->atname[i] !=NULL)) 
          free(ptr->atname[i]);
      if ((ptr->atval !=NULL) && (ptr->atval[i] !=NULL)) free(ptr->atval[i]);
    }
    if (ptr->atname !=NULL) free(ptr->atname);
    if (ptr->atval !=NULL) free(ptr->atval);
  }
  free(ptr);

}







  


