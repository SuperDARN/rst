/* graphic.c
   =========
   Author: R.J.Barnes
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
#include <math.h>
#include "rfbuffer.h"
#include "fontdb.h"

#include "rtime.h"
#include "iplot.h"
#include "grplot.h"

#include "istp.h"

#define DAYSC (24*3600)

int txtbox(char *fntname,float sze,int num,char *txt,float *box,void *data) {

  struct FrameBufferFontDB *fontdb;
  struct FrameBufferFont *fnt;
  int tbox[3];

  fontdb=(struct FrameBufferFontDB *)data;
  if (fontdb==NULL) return -1;
  fnt=FrameBufferFontDBFind(fontdb,fntname,sze);
  
  FrameBufferTextBox(fnt,num,txt,tbox);
  
  box[0]=tbox[0];
  box[1]=tbox[1];
  box[2]=tbox[2];
 
  return 0;
}



char *mtxt[]={"JAN","FEB","MAR","APR","MAY","JUN","JUL",
                "AUG","SEP","OCT","NOV","DEC",0};

char *mltxt[]={"Jan","Feb","Mar","Apr","May","Jun","Jul",
                "Aug","Sep","Oct","Nov","Dec",0};


char *label_tme(double val,double min,double max,void *data) {
  char *txt=NULL;
  txt=malloc(32);
  sprintf(txt,"%.2d",((int) (val/3600)) % 24);
  return txt;
}


char *label_dte(double val,double min,double max,void *data) {

  int yr,mo,dy,hr,mt;
  double sc;
  char *txt=NULL;
  txt=malloc(32);

  /* 
  if ((((int) (val/3600)) % 24) !=0) {
    sprintf(txt,""); 
    return;
  }
  */

  TimeEpochToYMDHMS(val,&yr,&mo,&dy,&hr,&mt,&sc);
  sprintf(txt,"%.2d %s %.4d",dy,mltxt[mo-1],yr);
  return txt;
}



char *label_val(double val,double min,double max,void *data) {
  char *txt=NULL;
  txt=malloc(32); 
  sprintf(txt,"%.2d",(int) val);
  return txt;
}

char *label_km(double val,double min,double max,void *data) {
  char *txt=NULL;
  txt=malloc(32);
  sprintf(txt,"%g",val);
  return txt;
}




void plot_frame(struct Grplot *grplot,int pmax,double stime,
                double etime,int dflg,
                unsigned int color,float width,char *fontname,
                float fontsize) {
  int i=0;
  double xtime;

  double xmajor=3600;
  double xminor=600;
 
  xtime=etime-stime;
  while (xtime>DAYSC) {
     xtime=xtime/2;
     xmajor=xmajor*2;
     xminor=xminor*2;
     if (xminor==2400) xminor=1800;
     if (xminor==14400) xminor=DAYSC;
     if (xmajor==14400) xmajor=43200;
     
  }
  if ((dflg) && (xmajor<(43200))) xmajor=43200;

  for (i=0;i<pmax;i++) {
     GrplotPanel(grplot,i,color,0x0f,width);
     GrplotXaxis(grplot,i,stime,etime,xmajor,xminor,0x03,color,0x0f,width);
   }
  grplot->lbl_hgt=8;
  GrplotXaxisLabel(grplot,pmax-1,stime,etime,xmajor,0x01,label_tme,NULL,
                 fontname,fontsize,color,0x0f);
  if (dflg) {
    grplot->lbl_hgt=18;
    GrplotXaxisLabel(grplot,pmax-1,stime,etime,xmajor,0x01,label_dte,NULL,
                   fontname,fontsize,color,0x0f);
    grplot->lbl_hgt=8;
  }
  GrplotXaxisTitle(grplot,
                 pmax-1,0x01,strlen("UT (Hours)"),"UT (Hours)",fontname,
                 fontsize,color,0x0f);
}



void plot_pos(struct Grplot *grplot,int p,
              struct posdata *pos,double stime,double etime,int mode,
              unsigned int color,float width,char *fontname,
              float fontsize) {
 float *P;
 int i;
 int flg;
 float ylim=0;  
 double ymajor=100;
 double yminor=100/5;
 double y;   
 int km=0;
 float mult=1;
 float ox=0,oy=0,px=0,py=0;
 float tx,ty;
 unsigned int gry;
 unsigned int red;
 unsigned int blue;

 struct PlotDash *dashA;
 struct PlotDash *dashB;

 dashA=PlotMakeDashString("4 4");
 dashB=PlotMakeDashString("4 2 1 2");

 gry=PlotColor(128,128,128,255);
 red=PlotColor(255,0,0,255);
 blue=PlotColor(0,0,255,255);

 GrplotYzero(grplot,p,-1,1,gry,0x0f,width,NULL); 
 
 for (i=0;i<pos->cnt;i++) {
     if (mode==0) P=&pos->PGSM[3*i]; 
     else P=&pos->PGSE[3*i]; 
  
     if (fabs(P[0])>500) continue;
     if (fabs(P[1])>500) continue;
     if (fabs(P[2])>500) continue;

     if (fabs(P[0])>ylim) ylim=fabs(P[0]);
     if (fabs(P[1])>ylim) ylim=fabs(P[1]);
     if (fabs(P[2])>ylim) ylim=fabs(P[2]);
   }
   if (ylim<2) { /* plot in km */
     km=1;
     mult=6370.0;
     ylim=ylim*6370;
     y=0;
     while (y<ylim) y+=1000;
     ylim=y;
     ymajor=1000;
     yminor=1000/5;
     
   } else if (ylim<60) {
     y=0;
     while (y<ylim) y=y+20;
     ylim=y;
     ymajor=20;
     yminor=20/4;
   } else {
     y=0;
     while (y<ylim) y=y+50;
     ylim=y;
     ymajor=100;
     yminor=100/5;
   }

   GrplotYaxis(grplot,p,-ylim,ylim,ymajor,yminor,0x03,color,0x0f,width);
   if (km==0) {
      GrplotYaxisLabel(grplot,p,-ylim,ylim,ymajor,0x03,label_val,NULL,fontname,
                     fontsize,color,0x0f);
      GrplotYaxisTitle(grplot,p,0x03,strlen("Pos (RE)"),"Pos (RE)",
                     fontname,fontsize,color,0x0f);
   } else {
      GrplotYaxisLabel(grplot,p,-ylim,ylim,ymajor,0x03,label_km,NULL,fontname,
                     fontsize,color,0x0f);
      GrplotYaxisTitle(grplot,p,0x03,strlen("Pos (km)"),"Pos (km)",
                     fontname,fontsize,color,0x0f);
   }
   GrplotGetPoint(grplot,p,0,1,0,1,0,1,&tx,&ty);
   PlotLine(grplot->plot,tx+5,ty+5,tx+20,ty+5,color,0x0f,width,NULL);
   PlotLine(grplot->plot,tx+35,ty+5,tx+50,ty+5,red,0x0f,width,dashA);
   PlotLine(grplot->plot,tx+65,ty+5,tx+80,ty+5,blue,0x0f,width,dashB);

   PlotText(grplot->plot,NULL,fontname,fontsize,
             tx+22,ty+10,strlen("X"),"X",blue,0x0f,1);
   PlotText(grplot->plot,NULL,fontname,fontsize,
             tx+52,ty+10,strlen("Y"),"Y",blue,0x0f,1);
   PlotText(grplot->plot,NULL,fontname,fontsize,
             tx+82,ty+10,strlen("Z"),"Z",blue,0x0f,1);


  flg=0;
  for (i=0;i<pos->cnt;i++) { 
    if (mode==0) P=&pos->PGSM[3*i]; 
    else P=&pos->PGSE[3*i];  
    if (fabs(P[0])<500) {
      px=pos->time[i]-stime;
      py=P[0]*mult;
      if (flg==1) GrplotLine(grplot,p,ox,oy,px,py,
                           0,etime-stime,-ylim,ylim,
                           color,0x0f,width,NULL);
      ox=px;
      oy=py;
      flg=1;
    } else flg=0;
  }

  flg=0;
  for (i=0;i<pos->cnt;i++) { 
    if (mode==0) P=&pos->PGSM[3*i]; 
    else P=&pos->PGSE[3*i];  
    if (fabs(P[1])<500) {
      px=pos->time[i]-stime;
      py=P[1]*mult;
      if (flg==1) GrplotLine(grplot,p,ox,oy,px,py,
                           0,etime-stime,-ylim,ylim,
                           red,0x0f,width,dashA);
      ox=px;
      oy=py;
      flg=1;
    } else flg=0;
  }

  flg=0;
  for (i=0;i<pos->cnt;i++) { 
    if (mode==0) P=&pos->PGSM[3*i]; 
    else P=&pos->PGSE[3*i];  
    if (fabs(P[2])<500) {
      px=pos->time[i]-stime;
      py=P[2]*mult;
      if (flg==1) GrplotLine(grplot,p,ox,oy,px,py,
                           0,etime-stime,-ylim,ylim,
                           blue,0x0f,width,dashB);
      ox=px;
      oy=py;
      flg=1;
    } else flg=0;
  }
  PlotFreeDash(dashA);
  PlotFreeDash(dashB);

}



void plot_plasma(struct Grplot *grplot,int p,struct plasmadata *plasma,
              double stime,double etime,int mode,int *pflg,
              unsigned int color,float width,char *fontname,
              float fontsize) {

 int i;
 int flg=0;
 float *V;
 float ylim=50;
 float yxlim=1000;
 float ymajor=20;
 float yxmajor=200;
 float yplim=10.0;
 float ypmajor=2;
 int o=0;
 float ox=0,oy=0,px,py;

 unsigned int gry;
 gry=PlotColor(128,128,128,255);

 if (strcmp(plasma->sat,"ge")==0) {
   ylim=150;
   ymajor=50;
 }

 if (pflg[0]) o++;
 if (pflg[1]) o++;
 if (pflg[2]) {
   GrplotYzero(grplot,p+o,-1,1,gry,0x0f,width,NULL); 
   o++;
 }
 if (pflg[3]) {
   GrplotYzero(grplot,p+o,-1,1,gry,0x0f,width,NULL); 
   o++;
 }

 o=0;

 if (pflg[0]) {
   char *title="Pressure (nPa)";
   GrplotYaxis(grplot,p,0,yplim,ypmajor,0,0x03,color,0x0f,width);
   GrplotYaxisLabel(grplot,p,0,yplim,ypmajor,0x03,label_val,NULL,fontname,
                     fontsize,color,0x0f);
   GrplotYaxisTitle(grplot,p,0x03,strlen(title),title,
                     fontname,fontsize,color,0x0f);
   o++;
 }
 if (pflg[1]) {
   char *title="Vx (km/s)";
   GrplotYaxis(grplot,p+o,0,yxlim,yxmajor,0,0x03,color,0x0f,width);
   GrplotYaxisLabel(grplot,p+o,0,yxlim,yxmajor,0x03,label_val,NULL,fontname,
                     fontsize,color,0x0f);
   GrplotYaxisTitle(grplot,p+o,0x03,strlen(title),title,
                     fontname,fontsize,color,0x0f);
   o++;
 }
 if (pflg[2]) {
   char *title="Vy (km/s)";
   GrplotYaxis(grplot,p+o,-ylim,ylim,ymajor,0,0x03,color,0x0f,width);
   GrplotYaxisLabel(grplot,p+o,-ylim,ylim,ymajor,0x03,label_val,NULL,fontname,
                     fontsize,color,0x0f);
   GrplotYaxisTitle(grplot,p+o,0x03,strlen(title),title,
                     fontname,fontsize,color,0x0f);

   o++;
 }
 if (pflg[3]) {
   char *title="Vz (km/s)";
   GrplotYaxis(grplot,p+o,-ylim,ylim,ymajor,0,0x03,color,0x0f,width);
   GrplotYaxisLabel(grplot,p+o,-ylim,ylim,ymajor,0x03,label_val,NULL,fontname,
                     fontsize,color,0x0f);
   GrplotYaxisTitle(grplot,p+o,0x03,strlen(title),title,
                     fontname,fontsize,color,0x0f);
   o++;
 }


 o=0;

 if (pflg[0]) {  
   flg=0;
   for (i=0;i<plasma->cnt;i++) {
       
      if (fabs(plasma->pre[i])<1.e6) {
        px=plasma->time[i]-stime;
        py=plasma->pre[i];
        if (py>yplim) py=yplim;
        if (flg==1) GrplotLine(grplot,p,ox,oy,px,py,
                               0,etime-stime,0,yplim,
                               color,0x0f,width,NULL);
        ox=px;
        oy=py;  
        flg=1;
      } else flg=0;
    }
   o++;
 }

 if (pflg[1]) {
   flg=0;
   for (i=0;i<plasma->cnt;i++) {
      if (mode==0) V=&plasma->VGSM[3*i]; 
      else V=&plasma->VGSE[3*i]; 

      if (fabs(V[0])<1.e6) {
        px=plasma->time[i]-stime;
        py=fabs(V[0]);
        if (py>yxlim) py=yxlim;
        if (flg==1) GrplotLine(grplot,p+o,ox,oy,px,py,
                               0,etime-stime,0,yxlim,
                               color,0x0f,width,NULL);
        ox=px;
        oy=py;  
        flg=1;
      } else flg=0;
    }
   o++;
  }
  if (pflg[2]) {
    flg=0;
    for (i=0;i<plasma->cnt;i++) {
      if (mode==0) V=&plasma->VGSM[3*i]; 
      else V=&plasma->VGSE[3*i];   

      if (fabs(V[1])<1e6) {
        px=plasma->time[i]-stime;
        py=V[1];
        if (py>ylim) py=ylim;
        if (py<-ylim) py=-ylim;
        if (flg==1) GrplotLine(grplot,p+o,ox,oy,px,py,
                               0,etime-stime,-ylim,ylim,
                               color,0x0f,width,NULL);
        ox=px;
        oy=py;
        flg=1;  
      } else flg=0;
    }
    o++;
  }
  if (pflg[2]) {
    flg=0;
    for (i=0;i<plasma->cnt;i++) {
      if (mode==0) V=&plasma->VGSM[3*i]; 
      else V=&plasma->VGSE[3*i];   

      if (fabs(V[2])<1e6) {
        px=plasma->time[i]-stime;
        py=V[2];
        if (py>ylim) py=ylim;
        if (py<-ylim) py=-ylim;
        if (flg==1) GrplotLine(grplot,p+o,ox,oy,px,py,
                               0,etime-stime,-ylim,ylim,
                               color,0x0f,width,NULL);
        ox=px;
        oy=py;  
        flg=1;
      } else flg=0;
    }
    o++;
  }

}

void plot_imf(struct Grplot *grplot,int p,struct imfdata *imf,
              double stime,double etime,int mode,int *pflg,
              unsigned int color,float width,char *fontname,
              float fontsize) {
   int i;
   int flg=0;  
   float *B;
   float mod;
   float ylim=0;
   double ymajor=10;  
   double y;   
   int o=0;

   float ox=0,oy=0,px=0,py=0;

   unsigned int gry;
   gry=PlotColor(128,128,128,255);

   if (pflg[0]) o++;
   if (pflg[1]) {
     GrplotYzero(grplot,p+o,-1,1,gry,0x0f,width,NULL); 
     o++;
   }
   if (pflg[2]) {
     GrplotYzero(grplot,p+o,-1,1,gry,0x0f,width,NULL); 
     o++;
   }
   if (pflg[3]) {
     GrplotYzero(grplot,p+o,-1,1,gry,0x0f,width,NULL); 
     o++;
   }

   for (i=0;i<imf->cnt;i++) {
     if (mode==0) B=&imf->BGSMc[3*i]; 
     else B=&imf->BGSEc[3*i]; 
     if (fabs(B[0])>500) continue;
     if (fabs(B[1])>500) continue;
     if (fabs(B[2])>500) continue;

     if (fabs(B[0])>ylim) ylim=fabs(B[0]);
     if (fabs(B[1])>ylim) ylim=fabs(B[1]);
     if (fabs(B[2])>ylim) ylim=fabs(B[2]);
   }

   y=0;
   while (y<ylim) y+=5;
   ylim=y;
   if (ylim>50) ylim=50; 

   o=0;

   if (pflg[0]) {
     char *title="|B| (nT)";
     GrplotYaxis(grplot,p,0,1.5*ylim,ymajor,0,0x03,color,0x0f,width);
     GrplotYaxisLabel(grplot,p,0,1.5*ylim,ymajor,0x03,label_val,NULL,fontname,
                     fontsize,color,0x0f);
     GrplotYaxisTitle(grplot,p,0x03,strlen(title),title,
                     fontname,fontsize,color,0x0f);
     o++;
   }
   if (pflg[1]) {
     char *title="Bx (nT)";
     GrplotYaxis(grplot,p+o,-ylim,ylim,ymajor,0,0x03,color,0x0f,width);
     GrplotYaxisLabel(grplot,p+o,-ylim,ylim,ymajor,0x03,label_val,NULL,fontname,
                     fontsize,color,0x0f);
     GrplotYaxisTitle(grplot,p+o,0x03,strlen(title),title,
                     fontname,fontsize,color,0x0f);
     o++;
   }
   if (pflg[2]) {
     char *title="By (nT)";
     GrplotYaxis(grplot,p+o,-ylim,ylim,ymajor,0,0x03,color,0x0f,width);
     GrplotYaxisLabel(grplot,p+o,-ylim,ylim,ymajor,0x03,label_val,NULL,fontname,
                     fontsize,color,0x0f);
     GrplotYaxisTitle(grplot,p+o,0x03,strlen(title),title,
                     fontname,fontsize,color,0x0f);
     o++;
   }
   if (pflg[3]) {
     char *title="Bz (nT)";
     GrplotYaxis(grplot,p+o,-ylim,ylim,ymajor,0,0x03,color,0x0f,width);
     GrplotYaxisLabel(grplot,p+o,-ylim,ylim,ymajor,0x03,label_val,NULL,fontname,
                     fontsize,color,0x0f);
     GrplotYaxisTitle(grplot,p+o,0x03,strlen(title),title,
                     fontname,fontsize,color,0x0f);
     o++;
   }



 
  o=0; 
  if (pflg[0]) {
    flg=0;
    for (i=0;i<imf->cnt;i++) {
   
      if (mode==0) B=&imf->BGSMc[3*i]; 
      else B=&imf->BGSEc[3*i];  

      mod=sqrt(B[0]*B[0]+B[1]*B[1]+B[2]*B[2]);
      if (mod<100) {
        px=imf->time[i]-stime;
        py=mod;
        if (py>1.5*ylim) py=1.5*ylim;
        if (flg==1) GrplotLine(grplot,p+o,ox,oy,px,py,
                               0,etime-stime,0,1.5*ylim,
                               color,0x0f,width,NULL);
        ox=px;
        oy=py;  
        flg=1;
      } else flg=0;   
    }
    o++;
  }
  if (pflg[1]) {
    flg=0;
    for (i=0;i<imf->cnt;i++) {
      if (mode==0) B=&imf->BGSMc[3*i]; 
      else B=&imf->BGSEc[3*i];   
      if (fabs(B[0])<500) {
         px=imf->time[i]-stime;
         py=B[0];
         if (py>ylim) py=ylim;
         if (py<-ylim) py=-ylim;
         if (flg==1) GrplotLine(grplot,p+o,ox,oy,px,py,
                               0,etime-stime,-ylim,ylim,
                               color,0x0f,width,NULL);
        ox=px;
        oy=py;  
        flg=1;
      } else flg=0;
    }
    o++;
  }
  if (pflg[2]) {
    flg=0;
    for (i=0;i<imf->cnt;i++) {
      if (mode==0) B=&imf->BGSMc[3*i]; 
      else B=&imf->BGSEc[3*i]; 

      if (fabs(B[1])<100) {
         px=imf->time[i]-stime;
         py=B[1];
         if (py>ylim) py=ylim;
         if (py<-ylim) py=-ylim;
         if (flg==1) GrplotLine(grplot,p+o,ox,oy,px,py,
                               0,etime-stime,-ylim,ylim,
                               color,0x0f,width,NULL);
        ox=px;
        oy=py;  
        flg=1;
      } else flg=0;

    }
    o++;
  }
  if (pflg[3]) {
    flg=0;
    for (i=0;i<imf->cnt;i++) {
      if (mode==0) B=&imf->BGSMc[3*i]; 
      else B=&imf->BGSEc[3*i];

      if (fabs(B[2])<100) {
        px=imf->time[i]-stime;
        py=B[2];
        if (py>ylim) py=ylim;
        if (py<-ylim) py=-ylim;
        if (flg==1) GrplotLine(grplot,p+o,ox,oy,px,py,
                               0,etime-stime,-ylim,ylim,
                               color,0x0f,width,NULL);
        ox=px;
        oy=py;  
        flg=1;
      } else flg=0;
    }
    o++;
  }

}

void plot_title(struct Grplot *grplot,double stime,double etime,
                char *sat,char *plot,int mode,
                unsigned int color,float width,char *fontname,
                float fontsize) {
 char title[256];


 char *motxt[]={"GSM","GSE",0};
 int yr,mo,dy,hr,mt;
 int dayno;
 double sc;
 double yrsec;
 TimeEpochToYMDHMS(stime,&yr,&mo,&dy,&hr,&mt,&sc);
 
 yrsec=stime-TimeYMDHMSToEpoch(yr,1,1,0,0,0);
 dayno=1+yrsec/(24*3600);
 
 sprintf(title,"%s %s (%s) %s %.2d %.4d (Dayno=%.2d)",
         sat,plot,motxt[mode],mtxt[mo-1],dy,yr,dayno);
 
 GrplotXaxisTitle(grplot,0,0x02,strlen(title),title,
                fontname,fontsize,color,0x0f);


}



   


















