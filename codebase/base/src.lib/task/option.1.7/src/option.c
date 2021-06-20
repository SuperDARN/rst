/* option.c
   ========
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
#include <string.h>
#include <stdlib.h>
#include "option.h"




char *OptionScanFileArg(FILE *file) {

  int c=0;
  int chr;
  int stp=256;
  int max=256;
  int quote=0;
  char *txt=NULL;
  char *tmp=NULL;

  txt=malloc(max);
  if (txt==NULL) return NULL;
    
  do {
    while ( ((chr=fgetc(file)) != EOF) && 
          ( (chr==' ') || (chr == '\n') || (chr== '\t') ) );
 
   if (chr=='#') { /* skip this line its a comment */
      while ( ((chr=fgetc(file)) !=EOF) && (chr !='\n'));
      chr='#'; 
    }

  } while (chr=='#');

  if (chr=='"') quote=!quote;
  else {
    txt[0]=(char) chr;
    c++;
  }

  while ((chr=fgetc(file)) != EOF) {

    if ((quote==0) && 
	((chr==' ') || (chr== '\n') || (chr== '\t'))) break;

    if (chr=='"') {
      quote=!quote;
      continue;
    }

    txt[c]=(char) chr;
    c++;
    if (c>=max) {
      char *tmp;
      max+=stp;
      tmp=realloc(txt,max);
      if (tmp==NULL) {
        free(txt);
        return NULL;
      }
      txt=tmp;
    }
  }
  
  if ((chr==EOF) && (c==1)) {
    free(txt);
    return NULL;
  }
  txt[c]=0;

  tmp=realloc(txt,c+1);
  if (tmp==NULL) {
    free(txt);
    return NULL;
  }
  txt=tmp;
  return txt;
}

struct OptionFile *OptionProcessFile(FILE *fp) {
  char *txt=NULL;
  int n;
  struct OptionFile *ptr=NULL;

  ptr=malloc(sizeof(struct OptionFile));
  if (ptr==NULL) return NULL;
  ptr->argc=0;
  ptr->argv=NULL;

  while ((txt=OptionScanFileArg(fp)) !=NULL) {
    if (ptr->argv==NULL) {
      ptr->argv=malloc(sizeof(char **));
      if (ptr->argv==NULL) break;
    } else {
      char **tmp=NULL;
      tmp=realloc(ptr->argv,sizeof(char **)*(ptr->argc+1));
      if (tmp==NULL) break;
      ptr->argv=tmp;
    } 
    ptr->argv[ptr->argc]=txt;
    ptr->argc++;
   
  }
  if (txt !=NULL) {
    if (ptr->argv !=NULL) {
      for (n=0;n<ptr->argc;n++) if (ptr->argv[n] !=NULL) free(ptr->argv[n]);
      free(ptr->argv);
    } 
    free(ptr);
    return NULL;
  }
  return ptr;
}

void OptionFreeFile(struct OptionFile *ptr) {
  int n;
  if (ptr==NULL) return;
  if (ptr->argv !=NULL) {
    for (n=0;n<ptr->argc;n++) if (ptr->argv[n] !=NULL) free(ptr->argv[n]);
    free(ptr->argv);
  } 
  free(ptr);
  return;
}

void OptionFreeText(struct OptionText *ptr) {
  int n;
  if (ptr==NULL) return;
  if (ptr->txt !=NULL) {
    for (n=0;n<ptr->num;n++) if (ptr->txt[n] !=NULL) free(ptr->txt[n]);
    free(ptr->txt);
  } 
  free(ptr);
  return;
}

int OptionProcess(int offset,int argc,char *argv[], struct OptionData *opt,
					int (*opterr)(char *)) {

  int i=offset,k=offset;
  while ((i<argc) && (argv[i][0]=='-')) {
    int j=0;
    while ((j<opt->num) && 
           (strcmp(&argv[i][1],opt->ptr[j].name) !=0) ) j++;

    if (j==opt->num) {
      int s=0;
      if (opterr !=NULL) s=(*opterr)(&argv[i][1]);
      i++;
      if (s==0) continue;
      else return -1;
    }
    i++;
    opt->ptr[j].set=1;
    switch (opt->ptr[j].type) {
    case 'x'  : /* a flag */
        if (opt->ptr[j].ptr !=NULL) 
        *( (unsigned char *) opt->ptr[j].ptr)=(unsigned char) 1;
        break;
      case 'i' : /* vanilla integer */ 
        if (i==argc) return -1;
        if (opt->ptr[j].ptr !=NULL)
        *( (int *) opt->ptr[j].ptr)=(int) atoi(argv[i]);
        i++;
        break;
      case 's' : /* short integer */
        if (i==argc) return -1;    
        if (opt->ptr[j].ptr !=NULL)
        *( (short int *) opt->ptr[j].ptr)=(short int) atoi(argv[i]);
        i++;
        break;
      case 'l' : /* long integer */
        if (i==argc) return -1;
        if (opt->ptr[j].ptr !=NULL) 
        *( (long int *) opt->ptr[j].ptr)=(long int) atol(argv[i]);
        i++;
        break;
      case 'f' : /* float */
        if (i==argc) return -1;
        if (opt->ptr[j].ptr !=NULL) 
        *( (float *) opt->ptr[j].ptr)=(float) atof(argv[i]);
        i++;
        break;
      case 'd' : /* double */
        if (i==argc) return -1;
        if (opt->ptr[j].ptr !=NULL) 
        *( (double *) opt->ptr[j].ptr)=(double) atof(argv[i]);
        i++;
        break;
      case 't' : /* string */
        if (i==argc) return -1;
        if (opt->ptr[j].ptr !=NULL) {
	  char **tptr;
          tptr=(char **) opt->ptr[j].ptr;
          if (*tptr !=NULL) free(*tptr);
          *tptr=malloc(strlen(argv[i])+1);
          if (*tptr==NULL) return -1;
   	  strcpy(*tptr,argv[i]);
	}
        opt->ptr[j].set=i; 
        i++;
        break;
        case 'a' : /* array of strings */
        if (i==argc) return -1;
        if (opt->ptr[j].ptr !=NULL) {
          struct OptionText *ptr;
          char **tmp;
          ptr=*(struct OptionText **) opt->ptr[j].ptr;
          if (ptr==NULL) {
	    ptr=malloc(sizeof(struct OptionText));
            if (ptr==NULL) return -1;
            ptr->num=0;
            ptr->txt=NULL;
            *(struct OptionText **) opt->ptr[j].ptr=ptr;
          }
          if (ptr->txt==NULL) ptr->txt=malloc(sizeof(char *));
          else {
            tmp=realloc(ptr->txt,sizeof(char *)*ptr->num+1);
	    if (tmp==NULL) return -1;
            ptr->txt=tmp;
	  }
          if (ptr->txt==NULL) return -1;
          ptr->txt[ptr->num]=malloc(strlen(argv[i])+1);
	  if (ptr->txt[ptr->num]==NULL) return -1;
          strcpy(ptr->txt[ptr->num],argv[i]);
          ptr->num++;
	}
        opt->ptr[j].set=i; 
        i++;
        break;


      default :
        break;
     } 
     /* skip to next option beginning with "-" */
     k=i;
     while ((i<argc) && (argv[i][0] !='-')) i++;
  }
  return k;
}


int OptionAdd(struct OptionData *opt,char *name,char type,void *data) {
  
  struct OptionEntry *tmp; 
  char *buf;

  if (opt==NULL) return -1;

  buf=malloc(strlen(name)+1);
  if (buf==NULL) return -1;
  strcpy(buf,name);

 
  if (opt->ptr !=NULL) {
    tmp=realloc(opt->ptr,sizeof(struct OptionEntry)*(opt->num+1));
    if (tmp==NULL) {
      free(buf);
      return -1;
    }
    opt->ptr=tmp;
  } else {
    opt->ptr=malloc(sizeof(struct OptionEntry));
    if (opt->ptr==NULL) {
      free(buf);
      return -1;
    }
    opt->num=0; 
  }

  opt->ptr[opt->num].name=buf;
  opt->ptr[opt->num].set=0;
  opt->ptr[opt->num].type=type;
  opt->ptr[opt->num].ptr=data;
  opt->num++;
  return 0;
}


int OptionDump(FILE *fp,struct OptionData *opt) {
  int i;
  for (i=0;i<opt->num;i++) {
    switch (opt->ptr[i].type) {
    case 'x'  : /* a flag */
       fprintf(fp,"-%s\n",opt->ptr[i].name);
       break;
    case 'i' : /* vanilla integer */ 
      fprintf(fp,"-%s <int>\n",opt->ptr[i].name);     
      break;
    case 's' : /* short integer */
      fprintf(fp,"-%s <short>\n",opt->ptr[i].name);     
      break;
    case 'l' : /* long integer */
      fprintf(fp,"-%s <long>\n",opt->ptr[i].name);     
      break;
    case 'f' : /* float */
      fprintf(fp,"-%s <float>\n",opt->ptr[i].name);     
      break;
    case 'd' : /* double */
      fprintf(fp,"-%s <double>\n",opt->ptr[i].name);     
      break;
    case 't' : /* string */
      fprintf(fp,"-%s <string>\n",opt->ptr[i].name);     
      break;
    case 'a' : /* array of strings */
      fprintf(fp,"[-%s <text>]...\n",opt->ptr[i].name);
      break;
    default:
      break;
    }
  }
  return 0;
}

      
void OptionFree(struct OptionData *opt) {
  if (opt->ptr !=NULL) {
    int i;
    for (i=0;i<opt->num;i++) free(opt->ptr[i].name);
    for (i=0;i<opt->num;i++) if (opt->ptr[i].type=='t') {
      char **tptr;
      tptr=(char **) opt->ptr[i].ptr;
      if (*tptr !=NULL) free(*tptr);
    }
    free(opt->ptr);
  }
  

  opt->ptr=NULL;
}

 
int OptionVersion(FILE *fp) {

  char *rst_path=NULL;
  char vname[256];
  char buff[100];
  FILE *vfp=NULL;

  rst_path=getenv("RSTPATH");
  strcpy(vname, rst_path);
  strcat(vname, "/.rst.version");

  vfp=fopen(vname, "r");

  if (vfp != NULL) {
    while (fscanf(vfp,"%s",buff)==1);
    fprintf(fp, "RST version: %s\n",buff);
    fclose(vfp);
  } else {
    fprintf(fp, "RST version file %s not found\n", vname);
    return -1;
  }

  return 0;
}

