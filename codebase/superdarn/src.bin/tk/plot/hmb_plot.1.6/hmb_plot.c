/* hmb_plot.c
   =========== 
   Author: R.J.Barnes
*/

/*
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
#include <time.h>
#include <sys/types.h>
#ifdef _XLIB_
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <X11/Xatom.h>
#endif

#include "rtypes.h"
#include "option.h"

#include "rfbuffer.h"
#include "rps.h"
#include "fontdb.h"
#include "iplot.h"
#include "splot.h"

#ifdef _XLIB_
#include "xwin.h"
#endif

#include "grplot.h"

#include "rtime.h"
#include "rfile.h"
#include "griddata.h"
#include "cnvmap.h"
#include "cnvmapread.h"
#include "oldcnvmapread.h"

#include "hlpstr.h"
#include "version.h"




#define WIDTH (558-58)
#define HEIGHT (734-58)

struct GridData *grd;
struct CnvMapData  *map;

struct OptionData opt;
struct OptionFile *optf=NULL;

int rst_opterr(char *txt) {
  fprintf(stderr,"Option not recognized: %s\n",txt);
  fprintf(stderr,"Please try: hmb_plot --help\n");
  return(-1);
}

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

double strtime(char *text) {
  int hr,mn;
  int i;
  for (i=0;(text[i] !=':') && (text[i] !=0);i++);
  if (text[i]==0) return atoi(text)*3600L;
  text[i]=0;
  hr=atoi(text);
  mn=atoi(text+i+1);
  return (double) hr*3600L+mn*60L;
}   

int stream(char *buf,int sze,void *data) {
  FILE *fp;
  fp=(FILE *) data;
  fwrite(buf,sze,1,fp);
  return 0;
} 

char *xlabel(double val,double min,double max,void *data) {
  char *txt=NULL;
  txt=malloc(32);
  sprintf(txt,"%.2d:%.2d",((int) val/3600) % 24, ((int) val % 3600)/60);
  return txt;
}


char *ylabel(double val,double min,double max,void *data) {
  char *txt=NULL;
  txt=malloc(32);
  sprintf(txt,"%.2d",(int) val);

  return txt;
}

int main(int argc,char *argv[]) {

  int old=0;

#ifdef _XLIB_
  int xdf=0;
  int xd=0;
  struct XwinDisplay *dp;
  struct XwinWindow *win;
  char *display_name=NULL;
  char *wname="hmb_plot";
  int xdoff=-1;
  int ydoff=-1;
#endif

  char name[256]="hmb";

  struct FrameBuffer *img=NULL;
  struct PostScript *psdata=NULL;
  FILE *fontfp;
  char *fntdbfname;
  struct FrameBufferFontDB *fontdb=NULL;
  unsigned int bgcol;
  unsigned int fgcol;
  float lne=0.5;

  float ox,oy,px,py;

  int arg;
 
  char *cfname=NULL;
  FILE *fp;

  unsigned char land=0;
  float wdt=WIDTH,hgt=HEIGHT;
 
  struct Plot *plot=NULL;
  struct Splot *splot=NULL;
  struct Grplot *plt;

  float xpoff=40,ypoff=40;
  unsigned char psflg=0;
  unsigned char xmlflg=0;
  unsigned char ppmflg=0;
  unsigned char ppmxflg=0;
  unsigned char pngflg=0;

  unsigned char gflg=0;
  unsigned char pflg=0;


  unsigned char help=0;
  unsigned char option=0;
  unsigned char version=0;

  unsigned char vb=0;

  char *fname=NULL;
  char *extime_txt=NULL;

  char title[256];  
  int yr,mo,dy,hr,mt;
  double sc;

  double extime=24*3600;
  double stime=-1;
  double mtime=0;

  int xnum=1,ynum=1;
  float lpad=40,rpad=40,tpad=80,bpad=40;
  float xoff=0,yoff=0;

  double xmin=0,xmax=24*3600;
  double xmajor=3*3600,xminor=15*60;
  int xtick_flg=0x03;
  int xlbl_flg=0x03;
 
  char *xtitle="Time";
  int xttl_flg=0x03;
 
  double ymin=40,ymax=80;
  double ymajor=5,yminor=1;
  int ytick_flg=0x03;
  int ylbl_flg=0x03;
 
  char *ytitle="Latitude";
  int yttl_flg=0x03;

  int status=0;

  grd=GridMake();
  map=CnvMapMake();  

 OptionAdd(&opt,"-help",'x',&help);
 OptionAdd(&opt,"-option",'x',&option);
 OptionAdd(&opt,"-version",'x',&version);

 OptionAdd(&opt,"old",'x',&old);
 OptionAdd(&opt,"vb",'x',&vb);

#ifdef _XLIB_ 
  OptionAdd(&opt,"x",'x',&xd);
  OptionAdd(&opt,"display",'t',&display_name);
  OptionAdd(&opt,"xoff",'i',&xdoff);
  OptionAdd(&opt,"yoff",'i',&ydoff);
#endif

  OptionAdd(&opt,"ppm",'x',&ppmflg);
  OptionAdd(&opt,"ppmx",'x',&ppmxflg);
  OptionAdd(&opt,"xml",'x',&xmlflg);
  OptionAdd(&opt,"png",'x',&pngflg);


  OptionAdd(&opt,"ps",'x',&psflg);

  OptionAdd(&opt,"xp",'f',&xpoff);
  OptionAdd(&opt,"yp",'f',&ypoff);
 
  OptionAdd(&opt,"l",'x',&land); 

  OptionAdd(&opt,"w",'f',&wdt);
  OptionAdd(&opt,"h",'f',&hgt);

  OptionAdd(&opt,"ex",'t',&extime_txt);

  OptionAdd(&opt,"cf",'t',&cfname);

  arg=OptionProcess(1,argc,argv,&opt,rst_opterr);

  if (arg==-1) {
    exit(-1);
  }

  if (arg<argc) fname=argv[arg];
  if (cfname !=NULL) { /* load the configuration file */
    do {
      fp=fopen(cfname,"r");
      if (fp==NULL) break;
      free(cfname);
      cfname=NULL;
      optf=OptionProcessFile(fp);
      if (optf !=NULL) {
        arg=OptionProcess(0,optf->argc,optf->argv,&opt,rst_opterr);
        if (arg==-1) {
          fclose(fp);
          OptionFreeFile(optf);
          exit(-1);
        }
        if (arg<optf->argc) {
	  fname=malloc(strlen(optf->argv[arg]+1));
	  strcpy(fname,optf->argv[arg]);
        }
        OptionFreeFile(optf);
      }   
      fclose(fp);
    } while (cfname !=NULL);
  }

  if (help==1) {
    OptionPrintInfo(stdout,hlpstr);
    exit(0);
  }

  if (option==1) {
    OptionDump(stdout,&opt);
    exit(0);
  }

  if (version==1) {
    OptionVersion(stdout);
    exit(0);
  }



 
  if (extime_txt !=NULL) extime=strtime(extime_txt);

  if (fname !=NULL) {
    fp=fopen(fname,"r");
    if (fp==NULL) {
      fprintf(stderr,"Map file not found.\n");
      exit(-1);
    }
  } else fp=stdin;


  xmax=extime;
 
  if (xmax>24*3600) {
    xmajor=12*3600;
    xminor=3*3600;
  } else if (xmax<12*3600) {
    xmajor=3600;
    xminor=5*60;
  } else if (xmax<3600) {
    xmajor=15*60;
    xminor=60;
  }


  if (old) status=OldCnvMapFread(fp,map,grd);
    else status=CnvMapFread(fp,map,grd);
  stime=map->st_time;

  TimeEpochToYMDHMS(stime,&yr,&mo,&dy,&hr,&mt,&sc);
  sprintf(title,"%.4d%.2d%.2d",yr,mo,dy);
  sprintf(name,"%.4d%.2d%.2d.hmb",yr,mo,dy);



 if ((land==1) && (wdt== WIDTH) && (hgt==HEIGHT)) {
    int t;
    t=hgt;
    hgt=wdt;
    wdt=t;
  } 

  fntdbfname=getenv("FONTDB");
  fontfp=fopen(fntdbfname,"r");
  if (fontfp !=NULL) {
   fontdb=FrameBufferFontDBLoad(fontfp);
   fclose(fontfp);
  }
 
  if (fontdb==NULL) {
   fprintf(stderr,"Could not load fonts.\n");
   exit(-1);
  }

  /* now determing our output type */

  if (psflg) pflg=1;
  if (xmlflg) gflg=1;
  if (ppmflg) gflg=1;
  if (ppmxflg) gflg=1;
  if (pngflg) gflg=1;

#ifdef _XLIB_ 
   if (xd !=0) {
     pflg=0; 
     gflg=1;
   }
#endif

  if (pflg) gflg=0;
  if ((!gflg) && (!pflg)) pflg=1;

  plot=PlotMake();
  splot=SplotMake();
  SplotSetPlot(plot,splot);

  if (gflg) SplotSetFrameBuffer(splot,&img,fontdb,NULL,NULL);
  if (pflg) {
    psdata=PostScriptMake();
    PostScriptSetText(psdata,stream,stdout);
    SplotSetPostScript(splot,psdata,land,xpoff,ypoff);
  }




  psdata=PostScriptMake();
  PostScriptSetText(psdata,stream,stdout);
      
  bgcol=PlotColor(255,255,255,255);
  fgcol=PlotColor(0,0,0,255);
 
  PlotDocumentStart(plot,name,NULL,wdt,hgt,24);
  PlotPlotStart(plot,name,wdt,hgt,24);
  PlotRectangle(plot,NULL,0,0,wdt,hgt,1,bgcol,0x0f,0,NULL);

  plt=GrplotMake(wdt,hgt,xnum,ynum,lpad,rpad,bpad,tpad,xoff,yoff);
  GrplotSetPlot(plt,plot);
  GrplotSetTextBox(plt,txtbox,fontdb);  

  plt->major_hgt=8;
  plt->minor_hgt=5;
  plt->ttl_hgt=20;
  plt->lbl_hgt=10;

  plt->ttl_wdt=20;
  plt->lbl_wdt=10;

  if (map->hemisphere==-1) {
    ymin=-80;
    ymax=-40;
  }

  GrplotPanel(plt,0,fgcol,0x0f,lne);
  GrplotXaxis(plt,0,xmin,xmax,xmajor,xminor,xtick_flg,fgcol,0x0f,lne);

  GrplotXaxisLabel(plt,0,xmin+stime,xmax+stime,xmajor,xlbl_flg,xlabel,NULL,
                 "Helvetica",10.0,fgcol,0x0f);
  GrplotXaxisTitle(plt,0,xttl_flg,strlen(xtitle),xtitle,"Helvetica",
                 10.0,fgcol,0x0f);

  GrplotYaxis(plt,0,ymin,ymax,ymajor,yminor,ytick_flg,fgcol,0x0f,lne);

  GrplotYaxisLabel(plt,0,ymin,ymax,ymajor,ylbl_flg,ylabel,NULL,
                 "Helvetica",10.0,fgcol,0x0f);
  GrplotYaxisTitle(plt,0,yttl_flg,strlen(ytitle),ytitle,"Helvetica",
                 10.0,fgcol,0x0f);


  ox=0;
  oy=map->latmin;

  if (old) status=OldCnvMapFread(fp,map,grd);
  else status=CnvMapFread(fp,map,grd);

  while (status !=-1) {          
    mtime=map->st_time;
    TimeEpochToYMDHMS(mtime,&yr,&mo,&dy,&hr,&mt,&sc);
    
    if (vb==1) 
      fprintf(stderr,"%.2d-%.2d-%.4d %.2d:%.2d\n",dy,mo,dy,hr,mt);

    px=mtime-stime;
    py=map->latmin;
    GrplotLine(plt,0,ox,oy,px,py,xmin,xmax,ymin,ymax,fgcol,0x0f,lne,NULL);

    ox=px;
    oy=py;
    
    if ((mtime-stime)>extime) break;
    if (old) status=OldCnvMapFread(fp,map,grd);
    else status=CnvMapFread(fp,map,grd);

  }  

  plt->ttl_hgt=40;
  GrplotXaxisTitle(plt,0,0x02,strlen(title),title,
                "Helvetica",20.0,fgcol,0x0f);

  PlotPlotEnd(plot);   
  PlotDocumentEnd(plot);

  if (!gflg) exit(0);
  if (img==NULL) {
    fprintf(stderr,"Nothing to plot.\n");
    exit(-1);
  }

#ifdef _XLIB_
  if (xd !=0) {
    dp=XwinOpenDisplay(display_name,&xdf);
 
    if (dp==NULL) {
      fprintf(stderr,"Could not open display.\n");
      exit(-1);
    }

    if (xdoff==-1) xdoff=(dp->wdt-img->wdt)/2;
    if (ydoff==-1) ydoff=(dp->hgt-img->hgt)/2;


    win=XwinMakeWindow(xdoff,ydoff,img->wdt,img->hgt,0,
                       dp,wname,
                       wname,argv[0],wname,argc,argv,&xdf);
    if (win==NULL) {
      fprintf(stderr,"Could not create window.\n");
      exit(-1);
    }

    XwinFrameBufferWindow(img,win);

    XwinShowWindow(win);

    XwinDisplayEvent(dp,1,&win,1,NULL);

    XwinFreeWindow(win);
    XwinCloseDisplay(dp);
  } else {
    if (xmlflg==1) FrameBufferSaveXML(img,stream,stdout);
    else if (ppmflg==1) FrameBufferSavePPM(img,stdout);
    else if (ppmxflg==1) FrameBufferSavePPMX(img,stdout);
    else FrameBufferSavePNG(img,stdout);
  }
  #else 
    if (xmlflg==1) FrameBufferSaveXML(img,stream,stdout);
    else if (ppmflg==1) FrameBufferSavePPM(img,stdout);
    else if (ppmxflg==1) FrameBufferSavePPMX(img,stdout);
    else FrameBufferSavePNG(img,stdout);
  #endif

  if ((cfname !=NULL) && (arg<optf->argc)) {
    free(fname);
  }
  return 0;


}


