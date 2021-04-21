/* table.c
   ======= 
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
#include "rxml.h"
#include "rxmldb.h"



struct XMLDBtable *XMLDBMakeTable() {
 
  struct XMLDBtable *ptr;

  ptr=malloc(sizeof(struct XMLDBtable));
  if (ptr==NULL) return NULL;
  ptr->num=0;
  ptr->element=NULL; 
  return ptr;
}

int XMLDBAddTable(struct XMLDBtable *ptr,struct XMLDBelement *element) {
  struct XMLDBelement **tmp;
  if (ptr==NULL) return -1;
  if (ptr->element==NULL) 
      ptr->element=malloc(sizeof(struct XMLDBelement *));
  else {
    tmp=realloc(ptr->element,
                sizeof(struct XMLDBelement *)*(ptr->num+1));
    if (tmp==NULL) return -1; 
    ptr->element=tmp;
  }

  if (ptr->element==NULL) return -1;
  ptr->element[ptr->num]=element;
  ptr->num++;
  return 0;
}


int XMLDBRemoveTable(struct XMLDBtable *ptr) {
  struct XMLDBelement **tmp;

  if (ptr==NULL) return -1;
  if (ptr->element==NULL) return -1;
  
  ptr->num--;
  if (ptr->num<0) return -1;
  
  if (ptr->element[ptr->num] !=NULL) XMLDBFreeElement(ptr->element[ptr->num]);
  

  if (ptr->num >0) {
     tmp=realloc(ptr->element,
                          sizeof(struct XMLDBelement *)*(ptr->num));
     if (tmp==NULL) return -1;
     ptr->element=tmp;
  } else {
    free(ptr->element);
    ptr->element=NULL;
  }
  return 0;
}




void XMLDBFreeTable(struct XMLDBtable *ptr) {
  int i;

  if (ptr==NULL) return;
  
  if (ptr->element !=NULL) {
    for (i=0;i<ptr->num;i++) 
      if (ptr->element[i] !=NULL) XMLDBFreeElement(ptr->element[i]);
    free (ptr->element);
  }
  free(ptr);

}







  
