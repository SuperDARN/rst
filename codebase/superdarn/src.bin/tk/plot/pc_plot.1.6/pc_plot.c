/* pc_plot.c
   ========= 
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

#ifdef _XLIB_
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <X11/Xatom.h>
#endif

#include "rtypes.h"
#include "rxml.h"
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
#include "cnvmapindex.h"
#include "cnvmapseek.h"
#include "oldcnvmapread.h"


#include "hlpstr.h"
#include "version.h"



#define WIDTH (558-58)
#define HEIGHT (734-58)

struct GridData *grd;
struct CnvMapData  *map;

struct OptionData opt;
struct OptionFile *optf=NULL;

int count=0;
double tval[10000];
double polar[10000];
int scatter[10000];
int delayval[10000];
double bx[10000];
double by[10000];
double bz[10000];

int rst_opterr(char *txt) {
  fprintf(stderr,"Option not recognized: %s\n",txt);
  fprintf(stderr,"Please try: pc_plot --help\n");
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

double strdate(char *text) {
  double tme;
  int val;
  int yr,mo,dy;
  val=atoi(text);
  dy=val % 100;
  mo=(val / 100) % 100;
  yr=(val / 10000);
  if (yr<1970) yr+=1900;  
  tme=TimeYMDHMSToEpoch(yr,mo,dy,0,0,0);

  return tme;
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
  char *wname="pc_plot";
  int xdoff=-1;
  int ydoff=-1;
#endif

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

  struct Splot *splot=NULL; 
  struct Plot *plot=NULL;
  struct Grplot *plt=NULL;

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
 
  char *st_time_txt=NULL;
  char *extime_txt=NULL;
  char *ed_time_txt=NULL;
  char *sdate_txt=NULL;
  char *edate_txt=NULL;
  
  char title[256];  
  char name[256];
  int yr,mo,dy,hr,mt;
  double sc;

  int status;

  unsigned char nodelay=0;

  double extime=0;
  double stime=-1;
  double etime=-1;
  double mtime=0;
  double sdate=-1;
  double edate=-1;

  int i,j;
  int xnum=1,ynum=1;
  float pwdt,phgt;
  float lpad=40,rpad=40,tpad=30,bpad=30;
  float xoff=0,yoff=40;

  double xmin=0,xmax=0;
  double xmajor=3*3600,xminor=30*60;
  int xtick_flg=0x03;
  int xlbl_flg=0x03;
 
  char *xtitle="Time";
  int xttl_flg=0x03;
  
  unsigned char sflg=0;
  unsigned char  bflg=0;

  double ypmin=0,ypmax=100;
  double ypmajor=10,ypminor=5;
  double ysmin=0,ysmax=1200;
  double ysmajor=100,ysminor=20;

  double bxmin=-20,bxmax=20;
  double bxmajor=4,bxminor=4;
  double bymin=-20,bymax=20;
  double bymajor=4,byminor=4;
  double bzmin=-20,bzmax=20;
  double bzmajor=4,bzminor=4;

  int ytick_flg=0x03;
  int ylbl_flg=0x03;
 
  char *yptitle="Cross Polar Cap Potential";
  char *ystitle="Data Points";
  char *bxtitle="Bx (nT)";
  char *bytitle="By (nT)";
  char *bztitle="Bz (nT)";
  int yttl_flg=0x03;

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

  OptionAdd(&opt,"st",'t',&st_time_txt);
  OptionAdd(&opt,"et",'t',&ed_time_txt);
  OptionAdd(&opt,"ex",'t',&extime_txt);
  OptionAdd(&opt,"sd",'t',&sdate_txt);
  OptionAdd(&opt,"ed",'t',&edate_txt);

  OptionAdd(&opt,"pmin",'d',&ypmin);
  OptionAdd(&opt,"pmax",'d',&ypmax);

  OptionAdd(&opt,"smin",'d',&ysmin);
  OptionAdd(&opt,"smax",'d',&ysmax);

  OptionAdd(&opt,"bxmin",'d',&bxmin);
  OptionAdd(&opt,"bxmax",'d',&bxmax);
  OptionAdd(&opt,"bymin",'d',&bymin);
  OptionAdd(&opt,"bymax",'d',&bymax);
  OptionAdd(&opt,"bzmin",'d',&bzmin);
  OptionAdd(&opt,"bzmax",'d',&bzmax);

  OptionAdd(&opt,"s",'x',&sflg);
  OptionAdd(&opt,"b",'x',&bflg);
  OptionAdd(&opt,"nodelay",'x',&nodelay);

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

  if (st_time_txt !=NULL) stime=strtime(st_time_txt);
  if (ed_time_txt !=NULL) etime=strtime(ed_time_txt);
  if (extime_txt !=NULL) extime=strtime(extime_txt);
  if (sdate_txt !=NULL) sdate=strdate(sdate_txt);
  if (edate_txt !=NULL) edate=strdate(edate_txt);

  if ((etime==-1) && (extime==0)) extime=24*3600;

  if (fname !=NULL) {
    fp=fopen(fname,"r");
    if (fp==NULL) {
      fprintf(stderr,"Map file not found.\n");
      exit(-1);
    }
  } else fp=stdin;

  if (old) status=OldCnvMapFread(fp,map,grd);
  else  status=CnvMapFread(fp,map,grd);

  if (stime !=-1) { /* we must skip the start of the files */
    int yr,mo,dy,hr,mt;
    double sc;  

    if (stime==-1) stime= ( (int) map->st_time % (24*3600));
    if (sdate==-1) stime+=map->st_time - ( (int) map->st_time % (24*3600));
    else stime+=sdate;

    TimeEpochToYMDHMS(stime,&yr,&mo,&dy,&hr,&mt,&sc);
    if (old) status=OldCnvMapFseek(fp,yr,mo,dy,hr,mt,sc,NULL,NULL);
    else status=CnvMapFseek(fp,yr,mo,dy,hr,mt,sc,NULL,NULL);
    if (status ==-1) {
      fprintf(stderr,"File does not contain the requested interval.\n");
      exit(-1);
    }
    if (old) status=OldCnvMapFread(fp,map,grd);
    else  status=CnvMapFread(fp,map,grd);
  } else stime=grd->st_time;



  if (etime !=-1) {
    if (edate==-1) etime+=grd->st_time - ( (int) grd->st_time % (24*3600));
    else etime+=edate;
  } 
  if (extime !=0)  etime=stime+extime;

  if ((land==1) && (wdt== WIDTH) && (hgt==HEIGHT)) {
    int t;
    t=hgt;
    hgt=wdt;
    wdt=t;
  } 

  phgt=hgt-yoff;
  pwdt=wdt;

  if (sflg==1) {
     ynum++;  
     tpad=15;
     bpad=15;
     phgt-=20;
     xttl_flg=0x02;
     xlbl_flg=0x02;
  } 

  if (bflg==1) {
     ynum+=3;  
     tpad=8;
     bpad=8;
     xttl_flg=0x02;
     xlbl_flg=0x02;
     if (sflg==0) phgt-=25;
     else phgt-=5;
     yoff+=5;
  } 

  xmax=etime-stime;
 
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

  /* now determine our output type */

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

  bgcol=PlotColor(255,255,255,255);
  fgcol=PlotColor(0,0,0,255);
 
  TimeEpochToYMDHMS(stime,&yr,&mo,&dy,&hr,&mt,&sc);
  sprintf(name,"%.4d%.2d%.2d.pc",yr,mo,dy);

  PlotDocumentStart(plot,name,NULL,wdt,hgt,24);
  PlotPlotStart(plot,name,wdt,hgt,24);
  PlotRectangle(plot,NULL,0,0,wdt,hgt,1,bgcol,0x0f,0,NULL);

  plt=GrplotMake(pwdt,phgt,xnum,ynum,lpad,rpad,bpad,tpad,xoff,yoff);
  GrplotSetPlot(plt,plot);
  GrplotSetTextBox(plt,txtbox,fontdb);  

  plt->major_hgt=8;
  plt->minor_hgt=5;
  plt->ttl_hgt=20;
  plt->lbl_hgt=10;

  plt->ttl_wdt=30;
  plt->lbl_wdt=10;
  
  GrplotPanel(plt,0,fgcol,0x0f,lne);
  GrplotXaxis(plt,0,xmin,xmax,xmajor,xminor,xtick_flg,fgcol,0x0f,lne);

  GrplotXaxisLabel(plt,0,xmin+stime,xmax+stime,xmajor,xlbl_flg,xlabel,NULL,
                 "Helvetica",10.0,fgcol,0x0f);
  GrplotXaxisTitle(plt,0,xttl_flg,strlen(xtitle),xtitle,"Helvetica",
                 10.0,fgcol,0x0f);

  GrplotYaxis(plt,0,ypmin,ypmax,ypmajor,ypminor,ytick_flg,fgcol,0x0f,lne);

  GrplotYaxisLabel(plt,0,ypmin,ypmax,ypmajor,ylbl_flg,ylabel,NULL,
                 "Helvetica",10.0,fgcol,0x0f);
  GrplotYaxisTitle(plt,0,yttl_flg,strlen(yptitle),yptitle,"Helvetica",
                 10.0,fgcol,0x0f);

  if (ynum==2) {
    xttl_flg=0x01; 
    xlbl_flg=0x01;

    GrplotPanel(plt,1,fgcol,0x0f,lne);
    GrplotXaxis(plt,1,xmin,xmax,xmajor,xminor,xtick_flg,fgcol,0x0f,lne);

    GrplotXaxisLabel(plt,1,xmin+stime,xmax+stime,xmajor,xlbl_flg,xlabel,NULL,
                 "Helvetica",10.0,fgcol,0x0f);
    GrplotXaxisTitle(plt,1,xttl_flg,strlen(xtitle),xtitle,"Helvetica",
                 10.0,fgcol,0x0f);

    GrplotYaxis(plt,1,ysmin,ysmax,ysmajor,ysminor,ytick_flg,fgcol,0x0f,lne);

    GrplotYaxisLabel(plt,1,ysmin,ysmax,ysmajor,ylbl_flg,ylabel,NULL,
                 "Helvetica",10.0,fgcol,0x0f);
    GrplotYaxisTitle(plt,1,yttl_flg,strlen(ystitle),ystitle,"Helvetica",
                 10.0,fgcol,0x0f);

  }
  if (ynum>2) {
    xttl_flg=0x01;
    xlbl_flg=0x01;
    for (i=1;i<ynum;i++) {
      GrplotPanel(plt,i,fgcol,0x0f,lne);
      GrplotXaxis(plt,i,xmin,xmax,xmajor,xminor,xtick_flg,fgcol,0x0f,lne);
    }
    GrplotXaxisLabel(plt,ynum-1,
                   xmin+stime,xmax+stime,xmajor,xlbl_flg,xlabel,NULL,
                 "Helvetica",10.0,fgcol,0x0f);
    GrplotXaxisTitle(plt,ynum-1,xttl_flg,strlen(xtitle),xtitle,"Helvetica",
                 10.0,fgcol,0x0f);
    i=1;
    if (sflg) {
      GrplotYaxis(plt,i,ysmin,ysmax,ysmajor,ysminor,ytick_flg,fgcol,0x0f,lne);

      GrplotYaxisLabel(plt,i,ysmin,ysmax,ysmajor,ylbl_flg,ylabel,NULL,
                 "Helvetica",10.0,fgcol,0x0f);
      GrplotYaxisTitle(plt,i,yttl_flg,strlen(ystitle),ystitle,"Helvetica",
                 10.0,fgcol,0x0f);
      i++;
    }
    GrplotYaxis(plt,i,bxmin,bxmax,bxmajor,bxminor,ytick_flg,fgcol,0x0f,lne);
    GrplotYzero(plt,i,bxmin,bxmax,fgcol,0x0f,lne,NULL);
    GrplotYaxisLabel(plt,i,bxmin,bxmax,bxmajor,ylbl_flg,ylabel,NULL,
                 "Helvetica",10.0,fgcol,0x0f);
    GrplotYaxisTitle(plt,i,yttl_flg,strlen(bxtitle),bxtitle,"Helvetica",
                 10.0,fgcol,0x0f);

    GrplotYaxis(plt,i+1,bymin,bymax,bymajor,byminor,ytick_flg,fgcol,0x0f,lne);
    GrplotYzero(plt,i+1,bymin,bymax,fgcol,0x0f,lne,NULL);
    GrplotYaxisLabel(plt,i+1,bymin,bymax,bymajor,ylbl_flg,ylabel,NULL,
                 "Helvetica",10.0,fgcol,0x0f);
    GrplotYaxisTitle(plt,i+1,yttl_flg,strlen(bytitle),bytitle,"Helvetica",
                 10.0,fgcol,0x0f);

    GrplotYaxis(plt,i+2,bzmin,bzmax,bzmajor,bzminor,ytick_flg,fgcol,0x0f,lne);
    GrplotYzero(plt,i+2,bzmin,bzmax,fgcol,0x0f,lne,NULL);
    GrplotYaxisLabel(plt,i+2,bzmin,bzmax,bzmajor,ylbl_flg,ylabel,NULL,
                 "Helvetica",10.0,fgcol,0x0f);
    GrplotYaxisTitle(plt,i+2,yttl_flg,strlen(bztitle),bztitle,"Helvetica",
                 10.0,fgcol,0x0f);


  }

  plt->ttl_hgt=40;

  TimeEpochToYMDHMS(stime,&yr,&mo,&dy,&hr,&mt,&sc);
  sprintf(title,"%.4d%.2d%.2d",yr,mo,dy);
  GrplotXaxisTitle(plt,0,0x02,strlen(title),title,"Helvetica",
                 20.0,fgcol,0x0f);
  do {
    tval[count]=map->st_time;
    mtime=map->st_time;
    TimeEpochToYMDHMS(mtime,&yr,&mo,&dy,&hr,&mt,&sc);
    
    if (vb==1) 
      fprintf(stderr,"%.2d-%.2d-%.4d %.2d:%.2d\n",dy,mo,dy,hr,mt);

    polar[count]=map->pot_drop;
    scatter[count]=grd->vcnum;
    bx[count]=map->Bx;
    by[count]=map->By;
    bz[count]=map->Bz;
    delayval[count]=map->imf_delay;

    count++;
    if (mtime>etime) break;
    if (old) status=OldCnvMapFread(fp,map,grd);
    else  status=CnvMapFread(fp,map,grd);

  } while (status !=-1);


  ox=0;
  oy=polar[0]/1000;
  for (j=1;j<count;j++) {
    px=tval[j]-stime;
    py=polar[j]/1000;
    GrplotLine(plt,0,ox,oy,px,py,xmin,xmax,ypmin,ypmax,fgcol,0x0f,lne,NULL);
    ox=px;
    oy=py;

  }
  i=1;
  if (sflg==1) {
    ox=0;
    oy=scatter[0];
    for (j=1;j<count;j++) {
      px=tval[j]-stime;
      py=scatter[j];
      GrplotLine(plt,1,ox,oy,px,py,xmin,xmax,ysmin,ysmax,fgcol,0x0f,lne,NULL);
      ox=px;
      oy=py;
    }
    i=2;
  } 

  if (bflg==1) {
    if (nodelay==0) {
      ox=0;
      oy=bx[0];
      for (j=1;j<count;j++) {
        px=tval[j]-stime;
        py=bx[j];
        GrplotLine(plt,i,ox,oy,px,py,xmin,xmax,bxmin,bxmax,
                   fgcol,0x0f,lne,NULL);
        ox=px;
        oy=py;
      }
      ox=0;
      oy=by[0];
      for (j=1;j<count;j++) {
        px=tval[j]-stime;
        py=by[j];
        GrplotLine(plt,i+1,ox,oy,px,py,xmin,xmax,
                 bymin,bymax,fgcol,0x0f,lne,NULL);
        ox=px;
        oy=py;
      }
      ox=0;
      oy=bz[0];
      for (j=1;j<count;j++) {
        px=tval[j]-stime;
        py=bz[j];
        GrplotLine(plt,i+2,ox,oy,px,py,xmin,xmax,
                 bzmin,bzmax,fgcol,0x0f,lne,NULL);
        ox=px;
        oy=py;
      }
    } else {
      int off=0;
      for (j=0;j<count;j++) if (((tval[j]-stime)/60)>delayval[0]) break;
      off=j;
      ox=0;
      oy=bx[off];
      for (j=off+1;j<count;j++) {
        px=tval[j]-60*delayval[j]-stime;
        py=bx[j];
        GrplotLine(plt,i,ox,oy,px,py,xmin,xmax,
                 bxmin,bxmax,fgcol,0x0f,lne,NULL);
        ox=px;
        oy=py;
      }
      ox=0;
      oy=by[off];
      for (j=off+1;j<count;j++) {
        px=tval[j]-60*delayval[j]-stime;
        py=by[j];
        GrplotLine(plt,i+1,ox,oy,px,py,xmin,xmax,
                 bymin,bymax,fgcol,0x0f,lne,NULL);
        ox=px;
        oy=py;
      }
      ox=0;
      oy=bz[off];
      for (j=off+1;j<count;j++) {
        px=tval[j]-60*delayval[j]-stime;
        py=bz[j];
        GrplotLine(plt,i+2,ox,oy,px,py,xmin,xmax,
                 bzmin,bzmax,fgcol,0x0f,lne,NULL);
        ox=px;
        oy=py;
      }

    }
  }

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


