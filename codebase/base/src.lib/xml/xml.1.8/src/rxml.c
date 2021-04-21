/* rxml.c
   ===== 
   Author R.J.Barnes
*/


/*
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
#include <ctype.h>
#include "rxml.h"



struct XMLdata *XMLMake() {
  struct XMLdata *ptr;
  ptr=malloc(sizeof(struct XMLdata));
  if (ptr==NULL) return NULL;
  ptr->sze=0;
  ptr->max=0;
  ptr->stp=100;
  ptr->buf=NULL;
  ptr->interface.text.func=NULL;
  ptr->interface.text.data=NULL; 
  ptr->interface.start.func=NULL;
  ptr->interface.start.data=NULL;
  ptr->interface.end.func=NULL;
  ptr->interface.end.data=NULL;
  ptr->interface.comment.func=NULL;
  ptr->interface.comment.data=NULL;
  ptr->interface.process.func=NULL;
  ptr->interface.process.data=NULL;
  return ptr;
}

void XMLFree(struct XMLdata *ptr) {
  if (ptr==NULL) return;
  if (ptr->buf !=NULL) free(ptr->buf);
  free(ptr);
}

int XMLSetText(struct XMLdata *ptr,
             int (*func)(char *,int,void *),void *data) {
  if (ptr==NULL) return -1;
  ptr->interface.text.func=func;
  ptr->interface.text.data=data;
  return 0;
} 

int XMLSetProcess(struct XMLdata *ptr,
             int (*func)(char *,int,void *),void *data) {
  if (ptr==NULL) return -1;
  ptr->interface.process.func=func;
  ptr->interface.process.data=data;
  return 0;
} 

int XMLSetComment(struct XMLdata *ptr,
             int (*func)(char *,int,void *),void *data) {
  if (ptr==NULL) return -1;
  ptr->interface.comment.func=func;
  ptr->interface.comment.data=data;
  return 0;
} 

int XMLSetStart(struct XMLdata *ptr,
		int (*func)(char *,char,int,char **,char **,
                         char *,int,void *),void *data) {
  if (ptr==NULL) return -1;
  ptr->interface.start.func=func;
  ptr->interface.start.data=data;
  return 0;
} 

int XMLSetEnd(struct XMLdata *ptr,
             int (*func)(char *,char *,int,void *),void *data) {
  if (ptr==NULL) return -1;
  ptr->interface.end.func=func;
  ptr->interface.end.data=data;
  return 0;
} 




int XMLDecode(struct XMLdata *ptr,char *buf,int sze) {
  int c=0;
  int s=0;
  int status=0;

  char *tbuf=NULL;
  int tsze=0;

  char *name;
  char endchar=0;
  int atnum=0;
  char **atmp=NULL;
  char **atname=NULL;
  char **atval=NULL;

  int i,j;

  if (buf==NULL) { /* flush */
    status=(ptr->interface.text.func)(NULL,0,
            ptr->interface.text.data);
    return -1;
  }

  while(c<sze) {

    if (buf[c]=='<') { /* start of a tag */

      if (((c-s) !=0) && (ptr->interface.text.func !=NULL)) 
         status=(ptr->interface.text.func)(buf+s,c-s,
                 ptr->interface.text.data);
      
      if (status !=0) return status;

      if (ptr->buf !=NULL) return -1; /* bad XML syntax */
      ptr->buf=malloc(ptr->stp);
      if (ptr->buf==NULL) return -1; /* memory error */
      ptr->max=ptr->stp;
      ptr->sze=0;
    } else if (buf[c]=='>') { /* end of a tag */
      if (ptr->buf == NULL) return -1;

      /* at this point we can decode the tag */

      /* copy the location and size of the buffered tag data */

      tbuf=ptr->buf;
      tsze=ptr->sze;

      /* reset the structure */

      ptr->buf=NULL;
      ptr->max=0;
      ptr->sze=0;

     
      switch (tbuf[0]) {
      case '?':
        if (ptr->interface.process.func !=NULL) 
	  status=(ptr->interface.process.func)(tbuf,tsze,
                  ptr->interface.process.data);
        if (status !=0) return status;
        break;
      case '!':
        if (ptr->interface.comment.func !=NULL) 
	  status=(ptr->interface.comment.func)(tbuf,
                  tsze,ptr->interface.comment.data);
        if (status !=0) return status;
        break;
      case '/':

        for (i=1;(i<tsze) && (isspace(tbuf[i]));i++);
        for (j=i;(j<tsze) && (!isspace(tbuf[j]));j++);
	name=malloc(j-i+1);
        if (name==NULL) return -1;
        memcpy(name,tbuf+i,j-i);
        name[j-i]=0;

        if (ptr->interface.end.func !=NULL) 
	  status=(ptr->interface.end.func)(name,
                  tbuf,tsze,ptr->interface.end.data);
         
        free(name);
        if (status !=0) return status;
        break;
      default:
        /* extract the tag name */

        for (i=0;(i<tsze) && (isspace(tbuf[i]));i++);
        for (j=i;(j<tsze) && (!isspace(tbuf[j]));j++);
	name=malloc(j-i+1);
        if (name==NULL) return -1;
        memcpy(name,tbuf+i,j-i);
        name[j-i]=0;
       
        /* further decode to get out any attribute pairs */
	
        atnum=0;
	
        while (j<tsze) {
          
        
	  for (i=j;(i<tsze) && (isspace(tbuf[i]));i++);
          if (tbuf[i]=='/') {
	    j=i;
            break;
	  }
          for (j=i;(j<tsze) && (tbuf[j] !='=') && (!isspace(tbuf[j]));j++);
          
          if ((j-i) !=0) { 
	      
	    if (atname==NULL) atname=malloc(sizeof(char *));
            else {
              atmp=realloc(atname,sizeof(char *)*(atnum+1));           
              if (atmp==NULL) break;
              atname=atmp; 
	    }
            if (atname==NULL) break;

	    if (atval==NULL) atval=malloc(sizeof(char *));
            else {
              atmp=realloc(atval,sizeof(char *)*(atnum+1));           
	      if (atmp==NULL) break;
              atval=atmp;
	    }
            if (atval==NULL) break;    
 
            atname[atnum]=NULL;
            atval[atnum]=NULL;

	    
            atname[atnum]=malloc(j-i+1);
            if (atname[atnum]==NULL) break;

            memcpy(atname[atnum],tbuf+i,j-i);
            atname[atnum][j-i]=0;
           
          
            /* search for attribute value after the space */
     
            for (i=j;(i<tsze) && (tbuf[i] !='"');i++);
            j=i;
            if (j>=tsze) continue; /* no value */
            i++;
            for (j=i;(j<tsze) && 
                       (tbuf[j] !='"');j++);

            if ((j-i) !=0) {
              atval[atnum]=malloc(j-i+1);
              if (atval[atnum]==NULL) break;
              memcpy(atval[atnum],tbuf+i,j-i);
              atval[atnum][j-i]=0;
	    }
            j++;	    
            atnum++;
	       
	  }
	  
	}
              
        if ((j<tsze) && (tbuf[j] !='/')) status=-1;
        if ((j<tsze) && (tbuf[j] =='/')) endchar='/';
        else endchar=0;

        if ((status==0) && (ptr->interface.start.func !=NULL)) 
	  status=(ptr->interface.start.func)(name,endchar,atnum,atname,atval,
                  tbuf,tsze,ptr->interface.start.data);
       
        for (i=0;i<atnum;i++) {
	  if ((atname !=NULL) && (atname[i] !=NULL)) free(atname[i]);
          if ((atval !=NULL) &&  (atval[i] !=NULL)) free(atval[i]);
	}
        if (atname !=NULL) free(atname);
        if (atval !=NULL) free(atval);
        free(name);

        name=NULL;
        atname=NULL;
        atval=NULL;     	  

        if (status !=0) return status;
        break;
      }


      /* free the buffer */
      free(tbuf);
      tbuf=NULL;
      tsze=0;
      s=c+1;
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

  if ((ptr->buf==NULL) && ((c-s) !=0) && (ptr->interface.text.func !=0)) 
     status=(ptr->interface.text.func)(buf+s,c-s,ptr->interface.text.data);
  
  return status;
}





