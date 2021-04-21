/* istp_plot.c
   ===========
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



/* This is a simple test program to demonstrate how to read a CDF file */

#ifdef _XLIB_
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <X11/Xatom.h>
#endif


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <math.h>
#include <time.h>

#include "rtypes.h"
#include "option.h"

#include "rfbuffer.h"
#include "iplot.h"
#include "rps.h"
#include "fontdb.h"
#include "splot.h"

#ifdef _XLIB_
#include "xwin.h"
#endif

#include "grplot.h"

#include "rtime.h"

#include "cdf.h"
#include "rcdf.h"

#include "locate.h"

#include "istp.h"
#include "wind.h"
#include "ace.h"
#include "imp.h"
#include "geotail.h"

#include "graphic.h"

#include "errstr.h"
#include "hlpstr.h"
#include "version.h"

#define ONE_MONTH (28*24*3600)

#define WIDTH (558-58)
#define HEIGHT (734-58)
  
struct imfdata imf;
struct posdata pos;
struct plasmadata plasma;

unsigned char mfi=0;
unsigned char swe=0;
unsigned char mag=0;
unsigned char pla=0;
unsigned char mgf=0;
unsigned char lep=0;
unsigned char cpi=0;

char sat[256];
char inst[256];

char dpath[256]={"/data"};
 
double st_time=-1;
double ed_time=-1;

struct file_list *fptr;

struct OptionData opt;
struct OptionFile *optf=NULL;

int plasma_f[4]={1,1,1,1};
int imf_f[4]={1,1,1,1};

int plasma_n=4;
int imf_n=4;

char *imf_s[]={"Bm","Bx","By","Bz",0};
char *plasma_s[]={"P","Vx","Vy","Vz",0};

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
  return hr*3600L+mn*60L;
}  


int load_wind() {
  int i;
  char path[256];

  CDFid id;
  CDFstatus status;
 

  sprintf(sat,"WIND");
  sprintf(path,"%s/%s",dpath,"wind");

  if ((mfi+swe)==0) mfi=1; /* default to imf if not given */

  if (swe==1) {
    strcpy(inst,"SWE");
    fptr=locate_files(path,"swe",st_time,ed_time);

    for (i=0;i<fptr->cnt;i++) {
      fprintf(stderr,"%s\n",fptr->fname[i]);

      status=CDFopen(fptr->fname[i],&id);
      if (status !=CDF_OK) {
        fprintf(stderr,"Could not open cdf file.\n");
        continue;
      }
   
      status=windswe_pos(id,&pos,st_time,ed_time); 
      status=windswe_plasma(id,&plasma,st_time,ed_time);
      CDFclose(id);
    }
    free_locate(fptr);
  }


  if (mfi==1) {
    if (swe==0) strcpy(inst,"MFI");
    else strcat(inst,"+MFI");
    fptr=locate_files(path,"mfi",st_time,ed_time);

    for (i=0;i<fptr->cnt;i++) {
      fprintf(stderr,"%s\n",fptr->fname[i]);

      status=CDFopen(fptr->fname[i],&id);
      if (status !=CDF_OK) {
        fprintf(stderr,"Could not open cdf file.\n");
        continue;
      }
   
      if (swe==0) status=windmfi_pos(id,&pos,st_time,ed_time); 
      status=windmfi_imf(id,&imf,st_time,ed_time);


      CDFclose(id);
    }
    free_locate(fptr);
  }
  return 0;
}
 
int load_ace() {
  int i;
  char path[256];

  CDFid id;
  CDFstatus status;
 
  if ((mfi+swe)==0) mfi=1; /* default to imf if not given */

  sprintf(sat,"ACE");
  sprintf(path,"%s/%s",dpath,"ace");
 

  if (swe==1) strcpy(inst,"SWE");



  fptr=locate_files(path,"h0_swe",st_time,ed_time);



  for (i=0;i<fptr->cnt;i++) {
    fprintf(stderr,"%s\n",fptr->fname[i]);

    status=CDFopen(fptr->fname[i],&id);
    if (status !=CDF_OK) {
       fprintf(stderr,"Could not open cdf file.\n");
       continue;
    }
    status=aceswe_pos(id,&pos,st_time,ed_time); 
    if (swe==1) status=aceswe_plasma(id,&plasma,st_time,ed_time);
    CDFclose(id);
  }
  free_locate(fptr);
  if (mfi==1) {
    if (swe==0) strcpy(inst,"MFI");
    else strcat(inst,"+MFI");

    fptr=locate_files(path,"h0_mfi",st_time,ed_time);

      if (fptr->cnt !=0) {
       for (i=0;i<fptr->cnt;i++) {
         fprintf(stderr,"%s\n",fptr->fname[i]);
         status=CDFopen(fptr->fname[i],&id);
         if (status !=CDF_OK) {
          fprintf(stderr,"Could not open cdf file.\n");
          continue;
        }
	 status=acemfi_imf(id,&imf,st_time,ed_time,0);
    
        CDFclose(id);
      }
      free_locate(fptr);
    } else {
      free_locate(fptr);    
      fptr=locate_files(path,"k1_mfi",st_time,ed_time);

      for (i=0;i<fptr->cnt;i++) {
        fprintf(stderr,"%s\n",fptr->fname[i]);

         status=CDFopen(fptr->fname[i],&id);
        if (status !=CDF_OK) {
          fprintf(stderr,"Could not open cdf file.\n");
          continue;
        }
    
        status=acemfi_imf(id,&imf,st_time,ed_time,1);
    
        CDFclose(id);
      }
      free_locate(fptr);
    }
  }
  return 0;
}

int load_imp() {
  int i;
  char path[256];

  CDFid id;
  CDFstatus status;
 

  sprintf(sat,"IMP8");
  sprintf(path,"%s/%s",dpath,"imp8");

  if ((mag+pla)==0) mag=1; /* default to imf if not given */

  if (pla==1) {

    strcpy(inst,"PLA");
   
    fptr=locate_files(path,"pla",st_time,ed_time);

    for (i=0;i<fptr->cnt;i++) {
      fprintf(stderr,"%s\n",fptr->fname[i]);

      status=CDFopen(fptr->fname[i],&id);
      if (status !=CDF_OK) {
        fprintf(stderr,"Could not open cdf file.\n");
        continue;
      }
  
      status=imppla_pos(id,&pos,st_time,ed_time); 
      status=imppla_plasma(id,&plasma,st_time,ed_time);
      CDFclose(id);
    }
    free_locate(fptr);
  }

  if (mag==1) {
    if (pla==0) strcpy(inst,"MAG");
    else strcat(inst,"+MAG");
    fptr=locate_files(path,"mag",st_time,ed_time);

    for (i=0;i<fptr->cnt;i++) {
      fprintf(stderr,"%s\n",fptr->fname[i]);

      status=CDFopen(fptr->fname[i],&id);
      if (status !=CDF_OK) {
        fprintf(stderr,"Could not open cdf file.\n");
        continue;
      }
   
      if (pla==0) status=impmag_pos(id,&pos,st_time,ed_time); 
      status=impmag_imf(id,&imf,st_time,ed_time);
      CDFclose(id);
    }
    free_locate(fptr);
  }

  return 0;
}
 
int load_geo() {
  int i;
  char path[256];

  CDFid id;
  CDFstatus status;
 

  sprintf(sat,"GEOTAIL");
  sprintf(path,"%s/%s",dpath,"geotail");

  if ((mgf+lep+cpi)==0) mgf=1; /* default to imf if not given */

  if (lep==1) strcpy(inst,"LEP");

  if ((lep==1) || (mgf==0)) {
    fptr=locate_files(path,"lep",st_time,ed_time);

    for (i=0;i<fptr->cnt;i++) {
        fprintf(stderr,"%s\n",fptr->fname[i]);

        status=CDFopen(fptr->fname[i],&id);
        if (status !=CDF_OK) {
          fprintf(stderr,"Could not open cdf file.\n");
          continue;
        }
  
        status=geolep_pos(id,&pos,st_time,ed_time);
        if (lep==1) status=geolep_plasma(id,&plasma,st_time,ed_time);
        CDFclose(id);
      }
      free_locate(fptr);
  }

  if (cpi==1) {

    if (lep==0) strcpy(inst,"CPI");
    else strcat(inst,"+CPI");
    fptr=locate_files(path,"cpi",st_time,ed_time);

    for (i=0;i<fptr->cnt;i++) {
      fprintf(stderr,"%s\n",fptr->fname[i]);

      status=CDFopen(fptr->fname[i],&id);
      if (status !=CDF_OK) {
        fprintf(stderr,"Could not open cdf file.\n");
        continue;
      }
  
      status=geocpi_plasma(id,&plasma,st_time,ed_time);
      CDFclose(id);
    }
    free_locate(fptr);
  }

  if (mgf==1) {
    if ((lep+cpi)==0) strcpy(inst,"MGF");
    else strcat(inst,"+MGF");
    fptr=locate_files(path,"mgf",st_time,ed_time);

    for (i=0;i<fptr->cnt;i++) {
      fprintf(stderr,"%s\n",fptr->fname[i]);

      status=CDFopen(fptr->fname[i],&id);
      if (status !=CDF_OK) {
        fprintf(stderr,"Could not open cdf file.\n");
        continue;
      }
   
      if (lep==0) status=geomgf_pos(id,&pos,st_time,ed_time); 
      status=geomgf_imf(id,&imf,st_time,ed_time);
      CDFclose(id);
    }
    free_locate(fptr);
  }
  return 0;
}


int stream(char *buf,int sze,void *data) {
  FILE *fp;
  fp=(FILE *) data;
  fwrite(buf,sze,1,fp);
  return 0;
} 

int rst_opterr(char *txt) {
  fprintf(stderr,"Option not recognized: %s\n",txt);
  fprintf(stderr,"Please try: istp_plot --help\n");
  return(-1);
}

int main(int argc,char *argv[]) {

#ifdef _XLIB_
  int xdf=0;
  int xd=0;
  struct XwinDisplay *dp;
  struct XwinWindow *win;
  char *display_name=NULL;
  char *wname="istp_plot";
  int xdoff=-1;
  int ydoff=-1;
#endif

  struct FrameBuffer *img=NULL;
  struct PostScript *psdata=NULL;

  char *fntdbfname;
  struct FrameBufferFontDB *fontdb=NULL;
  unsigned int bgcol;
  unsigned int fgcol;
  float lne=0.5;

  int arg;
  char *envstr;
  char *cfname=NULL;
  FILE *fp;

  unsigned char wind=0;
  unsigned char ace=0;
  unsigned char imp=0;
  unsigned char geo=0;
  unsigned char posf=0;
  unsigned char help=0;
  unsigned char option=0;
  unsigned char version=0;

  char *st_time_txt=NULL;
  char *extime_txt=NULL;
  char *ed_time_txt=NULL;
  char *sdate_txt=NULL;
  char *edate_txt=NULL;
  char *dpath_txt=NULL;

  double extime=-1;
  double sdate=-1;
  double edate=-1;

  char *plot=NULL;
  
  int yr,mo,dy,hr,mt;
  double sc;

  unsigned char mode=0; 
  int pnum=0;
  int p=0;
  int dflg=0;
  
  unsigned char land=0;
  int wdt=WIDTH,hgt=HEIGHT;


  struct Plot *Plot=NULL;
  struct Splot *splot=NULL;
  struct Grplot *grplot=NULL;

  float xpoff=40,ypoff=40;
  unsigned char psflg=0;
  unsigned char xmlflg=0;
  unsigned char ppmflg=0;
  unsigned char ppmxflg=0;
  unsigned char pngflg=0;

  unsigned char gflg=0;
  unsigned char pflg=0;

  char name[256];  

  envstr=getenv("ISTP_PATH");
  if (envstr !=NULL) strcpy(dpath,envstr);


  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"-option",'x',&option);
  OptionAdd(&opt,"-version",'x',&version);

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

  OptionAdd(&opt,"w",'i',&wdt);
  OptionAdd(&opt,"h",'i',&hgt);

  OptionAdd(&opt,"st",'t',&st_time_txt);
  OptionAdd(&opt,"et",'t',&ed_time_txt);
  OptionAdd(&opt,"ex",'t',&extime_txt);
  OptionAdd(&opt,"sd",'t',&sdate_txt);
  OptionAdd(&opt,"ed",'t',&edate_txt);

  OptionAdd(&opt,"gse",'x',&mode);

  OptionAdd(&opt,"pos",'x',&posf);
  OptionAdd(&opt,"mfi",'x',&mfi);
  OptionAdd(&opt,"swe",'x',&swe);
  OptionAdd(&opt,"mag",'x',&mag);
  OptionAdd(&opt,"pla",'x',&pla);
  OptionAdd(&opt,"mgf",'x',&mgf);
  OptionAdd(&opt,"lep",'x',&lep);
  OptionAdd(&opt,"cpi",'x',&cpi);

  OptionAdd(&opt,"plot",'t',&plot);
  OptionAdd(&opt,"path",'t',&dpath_txt);

  OptionAdd(&opt,"ace",'x',&ace);
  OptionAdd(&opt,"wind",'x',&wind);
  OptionAdd(&opt,"imp8",'x',&imp);
  OptionAdd(&opt,"geotail",'x',&geo);
  OptionAdd(&opt,"cf",'t',&cfname);

  if (argc>1) { 
    arg=OptionProcess(1,argc,argv,&opt,rst_opterr);
    if (arg==-1) {
      exit(-1);
    }
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
          OptionFreeFile(optf);
	}   
        fclose(fp);
      } while (cfname !=NULL);
    }
  } else {
    OptionPrintInfo(stderr,errstr);
    exit(-1);
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



  if (plot !=NULL) {
    char *tok;
    int i,j;
    for (i=0;i<4;i++) {
       plasma_f[i]=0;
       imf_f[i]=0;
    }
    plasma_n=0;
    imf_n=0;
    tok=strtok(plot,",");
    do {
      for (j=0;plasma_s[j] !=NULL;j++) 
        if (strcasecmp(plasma_s[j],tok)==0) break;
      if (plasma_s[j] !=NULL) {
        plasma_f[j]=1;
        plasma_n++;
      }
     

      for (j=0;imf_s[j] !=NULL;j++) 
        if (strcasecmp(imf_s[j],tok)==0) break;
      if (imf_s[j] !=NULL) {
        imf_f[j]=1;
        imf_n++;
      }
    } while ((tok=strtok(NULL,",")) !=NULL);
  } 
    


  if ((ace+wind+imp+geo)==0) ace=1;

  if ((ace+wind+imp+geo)>1) { 
    fprintf(stderr,"Only one satellite can be specified.\n");
    exit(-1);
  }

  if (dpath_txt !=NULL) strcpy(dpath,dpath_txt);


  if (st_time_txt !=NULL) st_time=strtime(st_time_txt);
  if (ed_time_txt !=NULL) ed_time=strtime(ed_time_txt);
  if (extime_txt !=NULL) extime=strtime(extime_txt);
  if (sdate_txt !=NULL) sdate=strdate(sdate_txt);
  if (edate_txt !=NULL) edate=strdate(edate_txt);

  if (sdate==-1) sdate=TimeYMDHMSToEpoch(1997,1,1,0,0,0);
  if (st_time==-1) st_time=0;

  st_time+=sdate;

  if (edate==-1) edate=sdate; 
  if (ed_time==-1) ed_time=0;  

  ed_time+=edate;
  if (extime !=-1) ed_time=st_time+extime;


  if (ed_time<=st_time) ed_time=st_time+(24*3600); /* plot a day */
    
  TimeEpochToYMDHMS(st_time,&yr,&mo,&dy,&hr,&mt,&sc);
  sprintf(name,"%.4d%.2d%.2d.%.2d%.2d.%.2d.%s",yr,mo,dy,hr,mt,(int) sc,sat);
  fprintf(stderr,"Start time:%d-%d-%d %d:%d:%d\n",yr,mo,dy,hr,mt,(int) sc);
  TimeEpochToYMDHMS(ed_time,&yr,&mo,&dy,&hr,&mt,&sc);
  fprintf(stderr,"End time:%d-%d-%d %d:%d:%d\n",yr,mo,dy,hr,mt,(int) sc);


 if ((ed_time-st_time)>=(48*3600)) dflg=1;


 
 
  if (ace==1) load_ace();
  else if (wind==1)load_wind();
  else if (imp==1) load_imp();
  else if (geo==1) load_geo();
 

  if (posf==0) pos.cnt=0;
  
  pnum=(pos.cnt>0)+imf_n*(imf.cnt>0)+plasma_n*(plasma.cnt>0);
  
  if (pnum==0) {
    fprintf(stderr,"Nothing to plot.\n");
    exit(-1);
  }
  
  TimeEpochToYMDHMS(st_time,&yr,&mo,&dy,&hr,&mt,&sc);
  sprintf(name,"%.4d%.2d%.2d.%.2d%.2d.%.2d.%s",yr,mo,dy,hr,mt,(int) sc,sat);
 

  if ((land==1) && (wdt== WIDTH) && (hgt==HEIGHT)) {
    int t;
    t=hgt;
    hgt=wdt;
    wdt=t;
  } 

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

  if (!gflg) pflg=1;
  if (pflg) gflg=0;

  Plot=PlotMake();
  splot=SplotMake();
  SplotSetPlot(Plot,splot);

  
  if (gflg) SplotSetFrameBuffer(splot,&img,fontdb,NULL,NULL);
  if (pflg) {
    psdata=PostScriptMake();
    PostScriptSetText(psdata,stream,stdout);
    SplotSetPostScript(splot,psdata,land,xpoff,ypoff);
  }

  bgcol=PlotColor(255,255,255,255);
  fgcol=PlotColor(0,0,0,255);
 
  PlotDocumentStart(Plot,name,NULL,wdt,hgt,24);
  PlotPlotStart(Plot,name,wdt,hgt,24);
 
  PlotRectangle(Plot,NULL,0,0,wdt,hgt,1,bgcol,0x0f,0,NULL);

  grplot=GrplotMake(wdt,hgt-60-(10*dflg),1,pnum,40,40,5,5,0,30);
  GrplotSetPlot(grplot,Plot);
  GrplotSetTextBox(grplot,txtbox,fontdb);  
 
  grplot->major_hgt=4;
  grplot->minor_hgt=2;
  grplot->lbl_hgt=8;
  grplot->ttl_hgt=20+(10*dflg);
 
  grplot->ttl_wdt=30;
  grplot->lbl_wdt=10;
 
  
  plot_frame(grplot,pnum,st_time,ed_time,dflg,fgcol,lne,"Helvetica",10.0); 
  
  if (pos.cnt !=0) {
    plot_pos(grplot,p,&pos,st_time,ed_time,mode,fgcol,lne,"Helvetica",10.0);   
    p++;
  } 
  
  if (plasma.cnt !=0) {
     plot_plasma(grplot,p,&plasma,st_time,ed_time,mode,plasma_f,fgcol,lne,
                 "Helvetica",10.0);  
     p+=plasma_n;;
  }
  
  if (imf.cnt !=0) {
     plot_imf(grplot,p,&imf,st_time,ed_time,mode,imf_f,fgcol,lne,
              "Helvetica",10.0); 
     p+=imf_n;
  }
  
  grplot->ttl_hgt=6;
  plot_title(grplot,st_time,ed_time,
             sat,inst,mode,fgcol,lne,"Helvetica",12.0);  
  PlotPlotEnd(Plot);   
  PlotDocumentEnd(Plot);
 
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
    return 0;
}
   


















