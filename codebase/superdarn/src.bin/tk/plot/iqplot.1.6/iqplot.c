/* iqplot.c
   ======== 
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
    2021-04-22 Marina Schmidt chaged delay to 0 as a default 
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <string.h>
#include <ctype.h>
#include <zlib.h>

#ifdef _XLIB_
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <X11/Xatom.h>
#endif

#include "rtypes.h"
#include "rtime.h"
#include "dmap.h"
#include "option.h"

#include "rfbuffer.h"
#include "fontdb.h"
#include "iplot.h"
#include "splot.h"
#include "xwin.h"
#include "grplot.h"

#include "rprm.h"
#include "iq.h"
#include "iqread.h"
#include "iqindex.h"
#include "iqseek.h"

#include "hlpstr.h"
#include "errstr.h"
#include "version.h"

#define WIDTH (720)
#define HEIGHT (500)

struct OptionData opt;
struct OptionFile *optf=NULL;

struct RadarParm *prm;
struct IQ *iq;
unsigned int *badtr;
int16 *samples;
int16 *ptr;


char *mktext(double value,double min,double max,void *data) {
  char *txt=NULL;
  txt=malloc(32);
  sprintf(txt,"%g",value);
  return txt;
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


void plot_time(struct Plot *plot,
               float xoff,float yoff,float wdt,float hgt,
               double stime,
               unsigned int color,unsigned char mask,
               char *fontname,float fontsize,
               void *txtdata) {
  int i;
  char txt[256];
  float txbox[3]={0,0,0};
  char *month[]={"Jan","Feb","Mar","Apr","May","Jun",
               "Jul","Aug","Sep","Oct","Nov","Dec",0};
  char *tmeA="00:00:00.0000 UT";

  char *tmeC="0";
  float cwdt;
  float x,y;
  int yr,mo,dy,shr,smt,ssc,fsec;
  double sec;

  txtbox(fontname,fontsize,strlen(tmeC),tmeC,txbox,txtdata);
  cwdt=txbox[0];

  TimeEpochToYMDHMS(stime,&yr,&mo,&dy,&shr,&smt,&sec);
  ssc=sec;

  fsec=10000.0*(sec-ssc);

  sprintf(txt,"%.2d %s %d",dy,month[mo-1],yr);
  txtbox(fontname,fontsize,strlen(txt),txt,txbox,txtdata);
  x=xoff;
  y=yoff+txbox[2];
  for (i=0;txt[i] !=0;i++) {
    txtbox(fontname,fontsize,1,txt+i,txbox,txtdata);
    PlotText(plot,NULL,fontname,fontsize,x,
              y,1,txt+i,color,mask,1);
    if (isdigit(txt[i])) x+=cwdt;
    else x+=txbox[0];
  }

  sprintf(txt,"%.2d:%.2d:%.2d.%.4d UT",shr,smt,ssc,fsec);

  txtbox(fontname,fontsize,strlen(tmeA),tmeA,txbox,txtdata);

  x=xoff+wdt-txbox[0];
  y=yoff+txbox[2];
  for (i=0;txt[i] !=0;i++) {
    txtbox(fontname,fontsize,1,txt+i,txbox,txtdata);
    PlotText(plot,NULL,fontname,fontsize,x,
              y,1,txt+i,color,mask,1);
    if (isdigit(txt[i])) x+=cwdt;
    else x+=txbox[0];
  }
}


void plot_ephem(struct Plot *plot,
               float xoff,float yoff,float wdt,float hgt,int interfer,
               int bmnum,int channel,int tfreq,float noise,int nave,int ave,
               unsigned int color,unsigned char mask,
               char *fontname,float fontsize,
               void *txtdata) {
  int i;
  char txt[256];
  float txbox[3]={0,0,0};
  char antenna[256];

  char *dig="0";

  float cwdt;
  float x,y;

  txtbox(fontname,fontsize,strlen(dig),dig,txbox,txtdata);
  cwdt=txbox[0];

  if (interfer) sprintf(antenna,"Interferometer");
  else          sprintf(antenna,"Main Array");

  sprintf(txt,"bmnum=%.2d channel=%.1d tfreq=%.5d noise=%4.2f smp=%.3d/%.3d (%s)",
          bmnum,channel,tfreq,noise,ave,nave,antenna);
  txtbox(fontname,fontsize,strlen(txt),txt,txbox,txtdata);
  x=xoff;
  y=yoff+txbox[2];
  for (i=0;txt[i] !=0;i++) {
    txtbox(fontname,fontsize,1,txt+i,txbox,txtdata);
    PlotText(plot,NULL,fontname,fontsize,x,
              y,1,txt+i,color,mask,1);
    if (isdigit(txt[i])) x+=cwdt;
    else x+=txbox[0];
  }
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
  if (text[i]==0) {
    fprintf(stderr,"Warning: must include ':' in '-t hr:mn' input - your date/time is probably incorrect!\n");
    return atoi(text)*3600L;
  }
  text[i]=0;
  hr=atoi(text);
  mn=atoi(text+i+1);

  return hr*3600L+mn*60L;
}


int rst_opterr(char *txt) {
  fprintf(stderr,"Option not recognized: %s\n",txt);
  fprintf(stderr,"Please try: iqplot --help\n");
  return(-1);
}


int main(int argc,char *argv[]) {

  char *fntdbfname;
  struct FrameBufferFontDB *fontdb=NULL;
  float lne=0.5;

  struct timeval tmout;
  float delay=0;

  int xdf=0;
  struct XwinDisplay *dp;
  struct XwinWindow *win;
  char *display_name=NULL;
  char *wname="iqplot";
  int xdoff=-1;
  int ydoff=-1;

  int xpnum=1,ypnum=1;
  float lpad=40,rpad=40,tpad=40,bpad=40;
  float xoff=0,yoff=0;

  struct Splot *splot=NULL;
  struct Plot *plot=NULL;
  struct Grplot *plt;

  unsigned char pflg=0;
  unsigned char rflg=0;
  unsigned char iflg=0;

  int bmnum=-1;
  int chnum=-1;
  unsigned char interfer=0;

  unsigned char help=0;
  unsigned char option=0;
  unsigned char version=0;

  int status=0;
  double atime;
  int offset;

  char *stmestr=NULL;
  char *etmestr=NULL;
  char *sdtestr=NULL;
  char *edtestr=NULL;
  char *exstr=NULL;

  double sdate=-1;
  double edate=-1;

  double stime=-1; /* specified start time*/
  double etime=-1; /* end time */
  double extime=0;

  int yr,mo,dy,hr,mt;
  double sc;

  float wdt=WIDTH,hgt=HEIGHT;
  int ymin=-200,ymax=200;
  int xnum=300;
  int xmin=0,xmax=0;
  int x;

  int pxmin,pxmax;

  int xmajor=0;
  int xminor=0;
  int ymajor=0;
  int yminor=0;

  int arg;

  float ax,ay,bx,by;
  int n;
  char *cfname=NULL;
  FILE *fp=NULL;
  double tval;

  struct FrameBuffer *img=NULL;

  unsigned txtcol=0xffffffff;
  unsigned bgcol=0xff000000;
  unsigned pbgcol=0xff000000;
  unsigned fgcol=0xffffffff;
  unsigned gcol=0xff606060;
  unsigned dgcol=0xff404040;
  unsigned rcol=0xff00ffff;
  unsigned icol=0xffff0000;

  char *txtcol_txt=NULL;
  char *bgcol_txt=NULL;
  char *pbgcol_txt=NULL;
  char *fgcol_txt=NULL;
  char *gcol_txt=NULL;
  char *dgcol_txt=NULL;
  char *rcol_txt=NULL;
  char *icol_txt=NULL;

  char *fontname=NULL;
  char *tfontname=NULL;
  char *deffont={"Helvetica"};
  float fontsize=10.0;
  float tfontsize=12.0;

  prm=RadarParmMake();
  iq=IQMake();

  fntdbfname=getenv("FONTDB");
  fp=fopen(fntdbfname,"r");
  if (fp !=NULL) {
    fontdb=FrameBufferFontDBLoad(fp);
    fclose(fp);
  }

  if (fontdb==NULL) {
    fprintf(stderr,"Could not load fonts.\n");
    exit(-1);
  }

  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"-option",'x',&option);
  OptionAdd(&opt,"-version",'x',&version);

  OptionAdd(&opt,"wdt",'f',&wdt);
  OptionAdd(&opt,"hgt",'f',&hgt);
  OptionAdd(&opt,"display",'t',&display_name);
  OptionAdd(&opt,"xoff",'i',&xdoff);
  OptionAdd(&opt,"yoff",'i',&ydoff);
  OptionAdd(&opt,"delay",'f',&delay);
  OptionAdd(&opt,"m",'i',&ymax);
  OptionAdd(&opt,"s",'i',&xmin);
  OptionAdd(&opt,"n",'i',&xnum);
  OptionAdd(&opt,"r",'x',&rflg);
  OptionAdd(&opt,"i",'x',&iflg);
  OptionAdd(&opt,"p",'x',&pflg);

  OptionAdd(&opt,"b",'i',&bmnum);
  OptionAdd(&opt,"c",'i',&chnum);
  OptionAdd(&opt,"int",'x',&interfer);

  OptionAdd(&opt,"st",'t',&stmestr);
  OptionAdd(&opt,"et",'t',&etmestr);
  OptionAdd(&opt,"sd",'t',&sdtestr);
  OptionAdd(&opt,"ed",'t',&edtestr);
  OptionAdd(&opt,"ex",'t',&exstr);

  OptionAdd(&opt,"rcol",'t',&rcol_txt);
  OptionAdd(&opt,"icol",'t',&icol_txt);

  OptionAdd(&opt,"txtcol",'t',&txtcol_txt);
  OptionAdd(&opt,"bgcol",'t',&bgcol_txt);
  OptionAdd(&opt,"pbgcol",'t',&pbgcol_txt);
  OptionAdd(&opt,"fgcol",'t',&fgcol_txt);
  OptionAdd(&opt,"gcol",'t',&gcol_txt);
  OptionAdd(&opt,"dgcol",'t',&dgcol_txt);

  OptionAdd(&opt,"fontname",'t',&fontname);
  OptionAdd(&opt,"fontsize",'f',&fontsize);

  OptionAdd(&opt,"tfontname",'t',&tfontname);
  OptionAdd(&opt,"tfontsize",'f',&tfontsize);

  OptionAdd(&opt,"xmaj",'i',&xmajor);
  OptionAdd(&opt,"xmin",'i',&xminor);
  OptionAdd(&opt,"ymaj",'i',&ymajor);
  OptionAdd(&opt,"ymin",'i',&yminor);

  OptionAdd(&opt,"cf",'t',&cfname);

  arg=OptionProcess(1,argc,argv,&opt,rst_opterr);

  if (arg==-1) {
    exit(-1);
  }

  if (cfname !=NULL) { /* load the configuration file */
    int farg;
    do {
      fp=fopen(cfname,"r");
      if (fp==NULL) break;
      free(cfname);
      cfname=NULL;
      optf=OptionProcessFile(fp);
      if (optf !=NULL) {
        farg=OptionProcess(0,optf->argc,optf->argv,&opt,rst_opterr);
        if (farg==-1) {
          fclose(fp);
          OptionFreeFile(optf);
          exit(-1);
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

  if (exstr !=NULL) extime=strtime(exstr);
  if (stmestr !=NULL) stime=strtime(stmestr);
  if (etmestr !=NULL) etime=strtime(etmestr);
  if (sdtestr !=NULL) sdate=strdate(sdtestr);
  if (edtestr !=NULL) edate=strdate(edtestr);

  if (rcol_txt !=NULL) rcol=PlotColorStringRGBA(rcol_txt);
  if (icol_txt !=NULL) icol=PlotColorStringRGBA(icol_txt);

  if (txtcol_txt !=NULL) txtcol=PlotColorStringRGBA(txtcol_txt);
  if (bgcol_txt !=NULL) bgcol=PlotColorStringRGBA(bgcol_txt);
  if (pbgcol_txt !=NULL) pbgcol=PlotColorStringRGBA(pbgcol_txt);
  if (fgcol_txt !=NULL) fgcol=PlotColorStringRGBA(fgcol_txt);
  if (gcol_txt !=NULL) gcol=PlotColorStringRGBA(gcol_txt);
  if (dgcol_txt !=NULL) dgcol=PlotColorStringRGBA(dgcol_txt);

  if (fontname==NULL) fontname=deffont;
  if (tfontname==NULL) tfontname=deffont;

  if (arg<argc) fp=fopen(argv[arg],"r");
  else fp=stdin;

  if (fp==NULL) {
    fprintf(stderr,"File not found.\n");
    exit(-1);
  }

  if (IQFread(fp,prm,iq,&badtr,&samples)==-1) {
    fprintf(stderr,"Error reading file.\n");
    exit(-1);
  }

  if (delay == 0){
      fprintf(stderr, "Click to view next plot. Ctrl+c to exit. Use -delay option to cycle through all frames sequentially.\n");
  }

  if ((interfer) && (prm->xcf==0)) {
    fprintf(stderr,"Warning: Interferometer samples may not be present in this file.\n");
  }


  if ((wdt==0) || (hgt==0)) {
    fprintf(stderr,"Invalid plot size.\n");
    exit(-1);
  }

  if ((rflg==0) && (iflg==0) && (pflg==0)) {
    rflg=1;
    iflg=1;
  }

  splot=SplotMake();

  SplotSetFrameBuffer(splot,&img,fontdb,NULL,NULL);

  plot=PlotMake();
  SplotSetPlot(plot,splot);

  dp=XwinOpenDisplay(display_name,&xdf);

  if (dp==NULL) {
    fprintf(stderr,"Could not open display.\n");
    exit(-1);
  }

  if (xdoff==-1) xdoff=(dp->wdt-wdt)/2;
  if (ydoff==-1) ydoff=(dp->hgt-hgt)/2;

  win=XwinMakeWindow(xdoff,ydoff,wdt,hgt,0,
                     dp,wname,
                     wname,argv[0],wname,argc,argv,&xdf);
  if (win==NULL) {
    fprintf(stderr,"Could not create window.\n");
    exit(-1);
  }
  XwinShowWindow(win);

  PlotDocumentStart(plot,"image",NULL,wdt,hgt,24);

  plt=GrplotMake(wdt,hgt,xpnum,ypnum,lpad,rpad,bpad,tpad,xoff,yoff);
  GrplotSetPlot(plt,plot);
  GrplotSetTextBox(plt,txtbox,fontdb);

  plt->major_hgt=8;
  plt->minor_hgt=5;
  plt->ttl_hgt=20;
  plt->lbl_hgt=10;

  plt->ttl_wdt=20;
  plt->lbl_wdt=10;

  ymin=-ymax;
  if (pflg) {
    ymin=0;
    ymax=10;
    rflg=0;
    iflg=0;
  }

  xmax=xmin+xnum;

  if (xmajor==0) xmajor=(xmax-xmin)/6;
  if (xminor==0) xminor=(xmax-xmin)/30;

  if (ymajor==0) ymajor=ymax/4;
  if (yminor==0) yminor=ymax/20;

  atime=TimeYMDHMSToEpoch(prm->time.yr,prm->time.mo,prm->time.dy,
                          prm->time.hr,prm->time.mt,prm->time.sc+prm->time.us/1.0e6);

  if ((stime !=-1) || (sdate !=-1)) {
    if (stime==-1) stime= ( (int) atime % (24*3600));
    if (sdate==-1) stime+=atime - ( (int) atime % (24*3600));
    else stime+=sdate;

    TimeEpochToYMDHMS(stime,&yr,&mo,&dy,&hr,&mt,&sc);
    status=IQFseek(fp,yr,mo,dy,hr,mt,sc,NULL,NULL);

    if (status == -1) {
      fprintf(stderr,"File does not contain the requested interval.\n");
      exit(-1);
    }
    if (IQFread(fp,prm,iq,&badtr,&samples)==-1) {
      fprintf(stderr,"Error reading file.\n");
      exit(-1);
    }
  } else stime=atime;

  if (etime !=-1) {
    if (edate==-1) etime+=atime - ( (int) atime % (24*3600));
    else etime+=edate;
  }

  if (extime !=0) etime=stime+extime;

  while(IQFread(fp,prm,iq,&badtr,&samples)==0) {

    if ((interfer) && (prm->xcf==0)) continue;
    if ((bmnum !=-1) && (prm->bmnum !=bmnum)) continue;
    if ((chnum !=-1) && (prm->channel !=chnum)) continue;

    atime=TimeYMDHMSToEpoch(prm->time.yr,prm->time.mo,prm->time.dy,
                            prm->time.hr,prm->time.mt,prm->time.sc+prm->time.us/1.0e6);
    if ((etime !=-1) && (atime>=etime)) break;

    for (n=0;n<iq->seqnum;n++) {
      tval=iq->tval[n].tv_sec+(1.0*iq->tval[n].tv_nsec)/1.0e9;
      if (tval == 0) tval=atime;
      PlotPlotStart(plot,"image",wdt,hgt,24);
      PlotRectangle(plot,NULL,0,0,wdt,hgt,1,bgcol,0x0f,0,NULL);
      PlotRectangle(plot,NULL,plt->xoff,plt->yoff,
                    plt->box_wdt,plt->box_hgt,1,pbgcol,0x0f,0,NULL);
      pxmin=xmin;
      pxmax=xmax;
      if (pxmax>=iq->smpnum) pxmax=iq->smpnum;
      if (pxmin>=iq->smpnum) pxmin=iq->smpnum;

      if (iq->offset[n] == 0) offset=n*iq->smpnum*2*2;
      else                    offset=iq->offset[n];

      if (interfer) ptr=samples+offset+2*iq->smpnum;
      else          ptr=samples+offset;

      GrplotXaxis(plt,0,xmin,xmax,xmajor,xminor,0x08,dgcol,0x0f,lne);
      GrplotYaxis(plt,0,ymin,ymax,ymajor,yminor,0x08,dgcol,0x0f,lne);
      GrplotXaxis(plt,0,xmin,xmax,xmajor,xminor,0x04,gcol,0x0f,lne);
      GrplotYaxis(plt,0,ymin,ymax,ymajor,yminor,0x04,gcol,0x0f,lne);

      if (pflg) {
        for (x=pxmin+1;x<pxmax;x++) {
          ax=x-1;
          ay=ptr[2*(x-1)+1]*ptr[2*(x-1)+1]+ptr[2*(x-1)]*ptr[2*(x-1)];
          if (ay !=0) ay=log10(ay);
          bx=x;
          by=ptr[2*x+1]*ptr[2*x+1]+ptr[2*x]*ptr[2*x];
          if (by !=0) by=log10(by);
          GrplotLine(plt,0,ax,ay,bx,by,xmin,xmax,ymin,ymax,fgcol,0x0f,lne,NULL);
        }
      }

      if (iflg) {
        for (x=pxmin+1;x<pxmax;x++) {
          ax=x-1;
          ay=ptr[2*(x-1)+1];
          bx=x;
          by=ptr[2*x+1];
          GrplotLine(plt,0,ax,ay,bx,by,xmin,xmax,ymin,ymax,icol,0x0f,lne,NULL);
        }
      }

      if (rflg) {
        for (x=pxmin+1;x<pxmax;x++) {
          ax=x-1;
          ay=ptr[2*(x-1)];
          bx=x;
          by=ptr[2*x];
          GrplotLine(plt,0,ax,ay,bx,by,xmin,xmax,ymin,ymax,rcol,0x0f,lne,NULL);
        }
      }

      GrplotPanel(plt,0,fgcol,0x0f,lne);

      GrplotXaxis(plt,0,xmin,xmax,xmajor,xminor,0x03,fgcol,0x0f,lne);
      GrplotYaxis(plt,0,ymin,ymax,ymajor,yminor,0x03,fgcol,0x0f,lne);
      GrplotXaxisLabel(plt,0,xmin,xmax,xmajor,0x03,mktext,NULL,
                       fontname,fontsize,txtcol,0x0f);
      GrplotXaxisTitle(plt,0,0x01,strlen("Sample Number"),"Sample Number",
                       fontname,fontsize,txtcol,0x0f);

      GrplotYaxisLabel(plt,0,ymin,ymax,ymajor,0x03,mktext,NULL,
                       fontname,fontsize,txtcol,0x0f);

      plot_time(plot,2,2,wdt-4,hgt-4,tval,txtcol,0x0f,tfontname,tfontsize,fontdb);

      plot_ephem(plot,100+2,2,wdt-4,hgt-4,interfer,
                 prm->bmnum,prm->channel,prm->tfreq,
                 prm->noise.search,
                 prm->nave,n,txtcol,0x0f,tfontname,tfontsize,fontdb);

      PlotPlotEnd(plot);
      PlotDocumentEnd(plot);
      if (img==NULL) continue;

      XwinFrameBufferWindow(img,win);
      tmout.tv_sec=(int) delay;
      tmout.tv_usec=(delay-(int) delay)*1e6;
      if (delay !=0) XwinDisplayEvent(dp,1,&win,1,&tmout);
      else XwinDisplayEvent(dp,1,&win,1,NULL);

      FrameBufferFree(img);
      img=NULL;
    }

  }

  PlotDocumentEnd(plot);
  XwinFreeWindow(win);
  XwinCloseDisplay(dp);
  return 0;

}
