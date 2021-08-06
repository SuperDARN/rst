/* rfile.c
   =======
   Author: R.J.Barnes
 Copyright (c) 2012 The Johns Hopkins University/Applied Physics Laboratory

This file is part of the Radar Software Toolkit (RST).

RST is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with this program. If not, see <https://www.gnu.org/licenses/>.

Modifications:
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <zlib.h>
#include "rtypes.h"
#include "rconvert.h"
#include "rtime.h"
#include "rfile.h"




int RfileReadLine(FILE *fp,int buflen,char *buffer) {
  char *s=NULL;
  do {
    s=fgets(buffer,buflen,fp);
    if (s==NULL) break;
  } while ((buffer[0]=='#') || (buffer[0]=='%'));
  if (s==NULL) return -1;
  return 0;
}

int RfileReadRawLine(FILE *fp,int buflen,char *buffer) {
  int s;
  int j=0;
  do {
    s=fgetc(fp);
    if (s==EOF) break;
    buffer[j]=s;
    j++;
  } while (s !=0);
  if (s==EOF) return -1;
  return 0;
}



int RfileReadRaw(FILE *fp,int num,RfileDecoder *decode,
              void **ptr) {

  char line[LMAX];
  char name[LMAX];
  char type[LMAX];
  char unit[LMAX];
  char *tcmp[]={"text","char","float","double","short","long","int",0};
  int txt=0;
  int sptr;
 
  double st_time,ed_time;

  int i,j,k,l,s;
  int32 nblks;
  int32 nprm=0,npnt=0;

  struct RfileData *data=NULL;
  sptr=ftell(fp)-1;

  ConvertFreadDouble(fp,&st_time);
  ConvertFreadDouble(fp,&ed_time);
  
  if (num==0) { /* no decoders - but we may still want the time */
    if (ptr !=NULL) {
      ((double *) ptr[0])[0]=st_time;
      ((double *) ptr[0])[1]=ed_time;
    }
  }

  ConvertFreadInt(fp,&nblks);
 
  for (i=0;i<nblks;i++) {
    ConvertFreadInt(fp,&npnt);
    ConvertFreadInt(fp,&nprm);

    if (data !=NULL) data=realloc(data,sizeof(struct RfileData)*nprm);
    else data=malloc(sizeof(struct RfileData)*nprm);
    memset(data,0,sizeof(struct RfileData)*nprm);
   
    /* now get the title string */
    name[0]=0;    
    for (j=0;j<nprm;j++) {
       if (RfileReadRawLine(fp,LMAX,line) !=0) break;
       if (j>0) strcat(name," ");
       strcat(name,line);
    }
    unit[0]=0;
    for (j=0;j<nprm;j++) {
       if (RfileReadRawLine(fp,LMAX,line) !=0) break;
       if (j>0) strcat(unit," ");
       strcat(unit,line);
    }
    type[0]=0;
    for (j=0;j<nprm;j++) {
       if (RfileReadRawLine(fp,LMAX,line) !=0) break;
       if (j>0) strcat(type," ");
       strcat(type,line);
       for (k=0;(tcmp[k] !=0) && (strcmp(line,tcmp[k]) !=0);k++);
       if (tcmp[k]==0) k=0;
       data[j].type=k;
       if (k==0) txt=1;

    }
    
   
    if (npnt==0) { /* tell loaders about empty records */
       for (k=0;k<num;k++) {
         s=(decode[k])(name,unit,type,st_time,ed_time,
                          npnt,nprm,0,data,ptr[k]);
         if (s==-1) break;
       }
       if (k !=num) break;
    }
  

    for (l=0;l<npnt;l++) {
      for (j=0;j<nprm;j++) {
        if (data[j].type==0) { /* get string */
          if (RfileReadRawLine(fp,LMAX,line) !=0) break;
          if (data[j].data.tval !=NULL) 
          data[j].data.tval=realloc(data[j].data.tval,strlen(line)+1);
          else data[j].data.tval=malloc(strlen(line)+1);
          strcpy(data[j].data.tval,line);
        } else if (data[j].type==1) {
           data[j].data.cval=fgetc(fp);
	}
        else if (data[j].type==2) ConvertFreadFloat(fp,&data[j].data.fval);
        else if (data[j].type==3) ConvertFreadDouble(fp,&data[j].data.dval);
        else if (data[j].type==4) ConvertFreadShort(fp,&data[j].data.sval);
        else if (data[j].type==5) ConvertFreadInt(fp,&data[j].data.lval);
        else if (data[j].type==6) ConvertFreadInt(fp,&data[j].data.ival);
      }

     if (j !=nprm) break;
      
     for (k=0;k<num;k++) {
       s=(decode[k])(name,unit,type,st_time,ed_time,
                     npnt,nprm,l,data,ptr[k]);
       if (s==-1) break;
     }
     if (k !=num) break;
    }

    /* free text strings */
    if (num !=0) {
      if (txt==1) {
        for (k=0;k<nprm;k++) if ((data[k].type==0) &&
        (data[k].data.tval !=NULL)) free(data[k].data.tval);
      }
      if (l != npnt) break; 
    }     
  }
      
  if (num !=0) free(data);
  if (i<nblks) return -1;
  return ftell(fp)-sptr;


}

int RfileRead(FILE *fp,int num,RfileDecoder *decode,
              void **ptr) {

  double st_time,ed_time;
  int syr,smo,sdy,shr,smt,ssc;
  int eyr,emo,edy,ehr,emt,esc;
  int i,j,k,l,ka,kb,s;
  int nblks;

  int nprm=0,npnt=0;
  
  int sptr;
  int txt=0;
  
  char line[LMAX];
  char name[LMAX];
  char type[LMAX];
  char unit[LMAX];


  char *tcmp[]={"text","char","float","double","short","long","int",0};
  char *tcnv[]={"%s","%c","%g","%lg","%d","%d","%d",0};
  
  char tmp[1024]; /* buffer to store parameters */

  struct RfileData *data=NULL;
  
  sptr=ftell(fp);

  s=fgetc(fp);
  if (s==0) return RfileReadRaw(fp,num,decode,ptr);
  line[0]=s;
  
  if (RfileReadLine(fp,LMAX-1,line+1) !=0) return -1;
 
  if (sscanf(line,"%d %d %d %d %d %d %d %d %d %d %d %d",
                  &syr,&smo,&sdy,&shr,&smt,&ssc,
                  &eyr,&emo,&edy,&ehr,&emt,&esc) !=12) return -1;


  st_time=TimeYMDHMSToEpoch(syr,smo,sdy,shr,smt,ssc);
  ed_time=TimeYMDHMSToEpoch(eyr,emo,edy,ehr,emt,esc);

  if (num==0) { /* no decoders - but we may still want the time */
    if (ptr !=NULL) {
      ((double *) ptr[0])[0]=st_time;
      ((double *) ptr[0])[1]=ed_time;
    }
  }

  if (RfileReadLine(fp,LMAX,line) !=0) return -1;

  if (sscanf(line,"%d",&nblks) !=1) return -1;
 
  for (i=0;i<nblks;i++) {

    if (RfileReadLine(fp,LMAX,line) !=0) break;
    if (sscanf(line,"%d %d",&npnt,&nprm) !=2) break;
    if (num !=0) {    
      if (data !=NULL) data=realloc(data,sizeof(struct RfileData)*nprm);
      else data=malloc(sizeof(struct RfileData)*nprm);
      memset(data,0,sizeof(struct RfileData)*nprm);
    }

    if (RfileReadLine(fp,LMAX,line) !=0) break;
    if (num !=0) {
      ka=0;
      kb=0;
      name[0]=0;
      for (j=0;j<nprm;j++) {

        /* skip spaces */
        for (k=ka;((line[k]==' ') || (line[k]=='\t') || (line[k]=='\n')) && 
                   (line[k] !=0);k++);
        ka=k;
        for (k=ka;(line[k]!=' ')  && (line[k]!='\t') &&(line[k]!='\n') && 
                 (line[k] !=0);k++);
        kb=k;
        if (j>0) strcat(name," ");
        strncat(name,line+ka,kb-ka);
        ka=kb;
      }
    }
    if (RfileReadLine(fp,LMAX,line) !=0) break;
    if (num !=0) {
      ka=0;
      kb=0;
      unit[0]=0;
      for (j=0;j<nprm;j++) {

        /* skip spaces */
        for (k=ka;((line[k]==' ')  || (line[k]=='\t') || (line[k]=='\n')) &&
                   (line[k] !=0);k++);
        ka=k;
        for (k=ka;(line[k]!=' ')  &&  (line[k]!='\t') && (line[k]!='\n') && 
                  (line[k] !=0);k++);
        kb=k;
        if (j>0) strcat(unit," ");
        strncat(unit,line+ka,kb-ka);
        ka=kb;
      }
    }
    if (RfileReadLine(fp,LMAX,line) !=0) break;
    if (num !=0) {
      ka=0;
      kb=0;
      type[0]=0;
      for (j=0;j<nprm;j++) {

        /* skip spaces */
        for (k=ka;((line[k]==' ') || (line[k]=='\t') || (line[k]=='\n')) && 
            (line[k] !=0);k++);
        ka=k;
        for (k=ka;(line[k]!=' ')  && (line[k]!='\t')  && (line[k]!='\n') && 
            (line[k] !=0);k++);
        kb=k;
        if (j>0) strcat(type," ");
        strncat(type,line+ka,kb-ka);
        ka=kb;
      }

      txt=0;
      ka=0;
      kb=0;
      for (j=0;j<nprm;j++) {
    
        for (k=ka;(type[k]!=' ')  && (type[k]!='\t') && (type[k]!='\n') && 
            (type[k] !=0);k++);
        kb=k;
        for (k=0;(tcmp[k] !=0) && (strncmp(type+ka,tcmp[k],kb-ka) !=0);k++);
        if (tcmp[k]==0) k=0;
      
        data[j].type=k;
        if (k==0) txt=1;
        ka=kb+1;
      }

      if (npnt==0) { /* tell loaders about empty records */
         for (k=0;k<num;k++) {
            s=(decode[k])(name,unit,type,st_time,ed_time,
                          npnt,nprm,0,data,ptr[k]);
            if (s==-1) break;
         }
         if (k !=num) break;
      }
    }
    for (l=0;l<npnt;l++) {
      
      if (RfileReadLine(fp,LMAX,line) !=0) break;
      if (num !=0) {
        ka=0;
    
        for (j=0;j<nprm;j++) {

          for (k=ka;((line[k]==' ') || (line[k]=='\t') || (line[k]=='\n')) && 
              (line[k] !=0);k++);
          ka=k;
          for (k=ka;(line[k]!=' ')  && (line[k]!='\t') && (line[k]!='\n') && 
              (line[k] !=0);k++);
        
     
          if (sscanf(line+ka,tcnv[data[j].type],tmp) !=1) break;
        
          if (data[j].type==1) data[j].data.cval=*tmp;
          else if (data[j].type==2) data[j].data.fval=*((float *) tmp);
          else if (data[j].type==3) data[j].data.dval=*((double *) tmp);
          else if (data[j].type==4) data[j].data.ival=*((int *) tmp);
          else if (data[j].type==5) data[j].data.ival=*((int *) tmp);
          else if (data[j].type==6) data[j].data.ival=*((int *) tmp);
          else {
            if (data[j].data.tval !=NULL) 
              data[j].data.tval=realloc(data[j].data.tval,strlen(tmp)+1);
            else data[j].data.tval=malloc(strlen(tmp)+1);
            strcpy(data[j].data.tval,tmp);
          }
          ka=k;        
        }
        if (j !=nprm) break;
      
        for (k=0;k<num;k++) {
            s=(decode[k])(name,unit,type,st_time,ed_time,
                          npnt,nprm,l,data,ptr[k]);
          if (s==-1) break;
        }
        if (k !=num) break;
      }
    }
    
    /* free text strings */
    if (num !=0) {
      if (txt==1) {
        for (k=0;k<nprm;k++) if ((data[k].type==0) &&
        (data[k].data.tval !=NULL)) free(data[k].data.tval);
      }
      if (l != npnt) break; 
    }     
  }
      
  if (num !=0) free(data);
  if (i<nblks) return -1;
  return ftell(fp)-sptr;
}
  

struct RfileIndex *RfileLoadIndex(FILE *fp) {

  int syr,smo,sdy,shr,smt,eyr,emo,edy,ehr,emt;
  double ssc,esc;
  double tme;
  int off; 
  char line[LMAX];

  struct RfileIndex *ptr;
  ptr=malloc(sizeof(struct RfileIndex));

  ptr->time.start=malloc(sizeof(double)*FILE_STEP);
  ptr->time.end=malloc(sizeof(double)*FILE_STEP);
  ptr->offset=malloc(sizeof(int)*FILE_STEP);
  ptr->num=0;


  while (RfileReadLine(fp,LMAX,line)==0) {
    if (sscanf(line,"%d %d %d %d %d %lg %d %d %d %d %d %lg %d",
        &syr,&smo,&sdy,&shr,&smt,&ssc,
        &eyr,&emo,&edy,&ehr,&emt,&esc,&off) !=13) 
      break;
    tme=TimeYMDHMSToEpoch(syr,smo,sdy,shr,smt,ssc);
    if (ptr->num==0) ptr->st_time=tme;
    ptr->time.start[ptr->num]=tme;

    tme=TimeYMDHMSToEpoch(eyr,emo,edy,ehr,emt,esc);
    ptr->ed_time=tme;
    ptr->time.end[ptr->num]=tme;

    ptr->offset[ptr->num]=off;

    ptr->num++;
    if ((ptr->num % FILE_STEP)==0) {
      int inc;
      inc=FILE_STEP*(ptr->num/FILE_STEP+1);
      ptr->time.start=realloc(ptr->time.start,sizeof(double)*inc);
      ptr->time.end=realloc(ptr->time.end,sizeof(double)*inc);

      ptr->offset=realloc(ptr->offset,sizeof(int)*inc);
    }
  }
  ptr->time.start=realloc(ptr->time.start,sizeof(double)*ptr->num);
  ptr->time.end=realloc(ptr->time.end,sizeof(double)*ptr->num);

  ptr->offset=realloc(ptr->offset,sizeof(int)*ptr->num);

  return ptr;
}




       

int RfileSeek(FILE *fp,
	      int yr,int mo,int dy,int hr,int mt,double sc,
              struct RfileIndex *inx,
              double *aval) {

  
  int status=0;

  int tval;

  tval=TimeYMDHMSToEpoch(yr,mo,dy,hr,mt,sc);
  
  if (inx !=NULL) {
    int rec=0,prec=-1;
    int st_rec;
    int ed_rec;

    /* search using index records */

    double st_time,ed_time;
    st_time=inx->st_time;
    ed_time=inx->ed_time;

    st_rec=0;
    ed_rec=inx->num;

    if (tval<st_time) { /* before start of file */
      if (aval !=NULL) *aval=st_time;
      fseek(fp,inx->offset[st_rec],SEEK_SET);
      return 0;
    } else if (tval>ed_time) { /* after end of file */
      if (aval !=NULL) *aval=st_time;
      fseek(fp,inx->offset[ed_rec-1],SEEK_SET);
      return 0;
    }

    do {
     
      prec=rec;
      rec=st_rec+(int) ((tval-st_time)*(ed_rec-st_rec))/(ed_time-st_time);
      if (inx->time.start[rec]==tval) break;
      if (inx->time.start[rec]<tval) {
         st_rec=rec;
         st_time=inx->time.start[rec];
      } else {
         ed_rec=rec;
         ed_time=inx->time.end[rec];
      }
    } while (prec != rec);
    if (aval !=NULL) *aval=inx->time.start[rec]; 
    fseek(fp,inx->offset[rec],SEEK_SET);
    return 0;
  } else {
    int ptr=0,pptr=0;
    double tme[2];
    void *tptr[2];
    tptr[0]=tme;
 
    /* search manually */
    ptr=ftell(fp);
    status=RfileRead(fp,0,NULL,tptr);
    if (status==0) {
      if (tme[0]>tval) fseek(fp,0,SEEK_SET); /* need to rewind to start */
    } else {
      fseek(fp,0,SEEK_SET); /* set to the start of the file */
      status=0;
    }

    if (aval !=NULL) *aval=tme[0];
    while ((tval>tme[0]) && (status !=-1)) {
      pptr=ptr;
      ptr=ftell(fp);
      status=RfileRead(fp,0,NULL,tptr);
      if (aval !=NULL) *aval=tme[0];
    }
    
    if (status==-1) {
      fseek(fp,pptr,SEEK_SET);
      return -1;
    }
    fseek(fp,ptr,SEEK_SET);
   
  }
  return 0;
}





