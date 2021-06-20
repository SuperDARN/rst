/* grplotstd_key.c
   ===============
   Author: R.J.Barnes
 Copyright (c) 2012 The Johns Hopkins University/Applied Physics Laboratory

This file is part of the Radar Software Toolkit (RST).

RST is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see <https://www.gnu.org/licenses/>.

Modifications:
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "rfbuffer.h"
#include "iplot.h"
#include "polygon.h"
#include "grplotraw.h"



int GrplotStdKey(struct Plot *ptr,
               float ox,float oy,float wdt,float hgt,
	       double min,double max,double step,
               int or,int txt_or,int tflg, 
               int ttl_num,char *ttl_text,
	       int  (*text_box)(char *,float,int,char *,float *,void *),
               void *boxdata,
               char * (*make_text)(double,double,double,void *),
               void *textdata,
               char *fntname,float fntsze,
               unsigned int color,unsigned char mask,float width,
               int ksze,unsigned char *a,
               unsigned char *r,unsigned char *g,unsigned char *b) {
  int s=0;
  


  float tstep,tlim;

  double tick_major=0;
  double tick_minor=0;
   
  float major_size=0;
  float minor_size=0;

  float txt_off=5;
  float ttl_off=5;
  
  int tick_flg=0x04;
  int txt_flg=0x02;
  int ttl_flg=0x01;
  
  int ttl_or=0;
 
  if (tflg==1) {
    txt_flg=0x01;
    if (or==0) ttl_flg=0x02;
  }

  tick_major=step;

  if (or==0) tstep=hgt*step/(max-min);
  else tstep=wdt*step/(max-min);

  if (make_text !=NULL) {
    float minbox[3];
    float maxbox[3];
    float w,h,t;
    char *mintxt=NULL;
    char *maxtxt=NULL;

    mintxt=(*make_text)(min,min,max,textdata);
    maxtxt=(*make_text)(max,min,max,textdata);

    if (mintxt !=NULL)
       s=(*text_box)(fntname,fntsze,strlen(mintxt),mintxt,minbox,boxdata);
    
    if ((s==0) && (maxtxt !=NULL)) 
       s=(*text_box)(fntname,fntsze,strlen(maxtxt),maxtxt,maxbox,boxdata);
    if (s !=0) {
      if (mintxt !=NULL) free(mintxt);
      if (maxtxt !=NULL) free(maxtxt);
      return s;
    }
    
    w=maxbox[0];
    h=(maxbox[1]+maxbox[2])/2;
   
    if (minbox[0]>w) w=minbox[0];
    t=(minbox[1]+minbox[2])/2;
    if (t>h) h=t;

    if (or==0) {
      if (txt_or==0) tlim=h;
      else tlim=w;
    } else {
      if (txt_or==0) tlim=w;
      else tlim=h;
    }

    if (tlim>tstep) {
      tick_major=max-min;
      tick_minor=step;
      tick_flg=0x08;
    }
    if (mintxt !=NULL) free(mintxt);
    if (maxtxt !=NULL) free(maxtxt);

  }

  return GrplotRawKey(ptr,ox,oy,wdt,hgt,min,max,tick_major,tick_minor,
                    major_size,minor_size,txt_off,ttl_off,tick_flg,
                    txt_flg,ttl_flg,txt_or,ttl_or,or,
                    ttl_num,ttl_text,text_box,
                    boxdata,make_text,textdata,fntname,fntsze,color,
                    mask,width,ksze,a,r,g,b);


}







int GrplotStdKeyLog(struct Plot *ptr,
               float ox,float oy,float wdt,float hgt,
	       double min,double max,double tick_major,double tick_minor,
	       int or,int txt_or,int tflg, 
               int ttl_num,char *ttl_text,
	       int  (*text_box)(char *,float,int,char *,float *,void *),
               void *boxdata,
               char * (*make_text)(double,double,double,void *),
               void *textdata,
               char *fntname,float fntsze,
               unsigned int color,unsigned char mask,float width,
               int ksze,unsigned char *a,
               unsigned char *r,unsigned char *g,unsigned char *b) {
  int s=0;


  float tstep,tlim;


  float major_size=0;
  float minor_size=2;

  float txt_off=5;
  float ttl_off=5;
  
  int tick_flg=0x07;
  int txt_flg=0x02;
  int ttl_flg=0x01;
  
  int ttl_or=0;
 
  if (tflg==1) {
    txt_flg=0x01;
    if (or==0) ttl_flg=0x02;
  }

  if (or==0) tstep=hgt*tick_major/(max-min);
  else tstep=wdt*tick_minor/(max-min);

  if (make_text !=NULL) {
    float minbox[3]={0,0,0};
    float maxbox[3]={0,0,0};
    float w,h,t;
    char *mintxt=NULL;
    char *maxtxt=NULL;

    mintxt=(*make_text)(min,min,max,textdata);
    maxtxt=(*make_text)(max,min,max,textdata);
   
    if (mintxt !=NULL) 
      s=(*text_box)(fntname,fntsze,strlen(mintxt),mintxt,minbox,boxdata);
    if ((s==0) && (maxtxt !=NULL)) 
      s=(*text_box)(fntname,fntsze,strlen(maxtxt),maxtxt,maxbox,boxdata);
    
    if (s !=0) {
      if (maxtxt !=NULL) free(maxtxt);
      if (mintxt !=NULL) free(mintxt);
    }    

    w=maxbox[0];
    h=(maxbox[1]+maxbox[2])/2;
   
    if (minbox[0]>w) w=minbox[0];
    t=(minbox[1]+minbox[2])/2;
    if (t>h) h=t;

    if (or==0) {
      if (txt_or==0) tlim=h;
      else tlim=w;
    } else {
      if (txt_or==0) tlim=w;
      else tlim=h;
    }

    if (tlim>tstep) {
       s=GrplotRawKeyLog(ptr,ox,oy,wdt,hgt,min,max,tick_major,tick_minor,
                    major_size,minor_size,txt_off,ttl_off,tick_flg,
                    txt_flg,ttl_flg,txt_or,ttl_or,or,
                    ttl_num,ttl_text,text_box,
                    boxdata,NULL,NULL,fntname,fntsze,color,
                    mask,width,ksze,a,r,g,b);
       if (or==0) {
	 if ((s==0) && (mintxt !=NULL))  
            s=GrplotRawYaxisLabelValue(ptr,ox,oy,wdt,hgt,min,max,min,txt_off,
                      txt_flg,txt_or,text_box,boxdata,
                      strlen(mintxt),mintxt,fntname,fntsze,color,mask);

         if ((s==0) && (maxtxt !=NULL)) 
            s=GrplotRawYaxisLabelValue(ptr,ox,oy,wdt,hgt,min,max,max,txt_off,
                      txt_flg,txt_or,text_box,boxdata,
                      strlen(maxtxt),maxtxt,fntname,fntsze,color,mask);

       } else {
         if ((s==0) && (mintxt !=NULL)) 
            s=GrplotRawXaxisLabelValue(ptr,ox,oy,wdt,hgt,min,max,min,txt_off,
                      txt_flg,txt_or,text_box,boxdata,
                      strlen(mintxt),mintxt,fntname,fntsze,color,mask);
         if ((s==0) && (maxtxt !=NULL))
            s=GrplotRawXaxisLabelValue(ptr,ox,oy,wdt,hgt,min,max,max,txt_off,
                      txt_flg,txt_or,text_box,boxdata,
                      strlen(maxtxt),maxtxt,fntname,fntsze,color,mask);
       }
       if (mintxt !=NULL) free(mintxt);
       if (maxtxt !=NULL) free(maxtxt);
       return s;
    
    }
    if (mintxt !=NULL) free(mintxt);
    if (maxtxt !=NULL) free(maxtxt);
  }

  return GrplotRawKeyLog(ptr,ox,oy,wdt,hgt,min,max,tick_major,tick_minor,
                    major_size,minor_size,txt_off,ttl_off,tick_flg,
                    txt_flg,ttl_flg,txt_or,ttl_or,or,
                    ttl_num,ttl_text,text_box,
                    boxdata,make_text,textdata,fntname,fntsze,color,
                    mask,width,ksze,a,r,g,b);


}





