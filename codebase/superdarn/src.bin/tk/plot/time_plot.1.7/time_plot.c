/* time_plot.c
   ===========
   Author: R.J.Barnes
*/


/*
 LICENSE AND DISCLAIMER
 
 Copyright (c) 2012 The Johns Hopkins University/Applied Physics Laboratory
 
 This file is part of the Radar Software Toolkit (RST).
 
 RST is free software: you can redistribute it and/or modify
 it under the terms of the GNU Lesser General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 any later version.
 
 RST is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU Lesser General Public License for more details.
 
 You should have received a copy of the GNU Lesser General Public License
 along with RST.  If not, see <http://www.gnu.org/licenses/>.
 
 
 
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <math.h>
#include <time.h>
#include <ctype.h>
#include <zlib.h>


#ifdef _XLIB_
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <X11/Xatom.h>
#endif

#include "rtypes.h"
#include "dmap.h"
#include "option.h"
#include "rxml.h"
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
#include "radar.h"
#include "rpos.h"
#include "rprm.h"
#include "fitdata.h"
#include "cfitdata.h"
#include "cfitread.h"
#include "fitread.h"
#include "fitindex.h"

#include "oldfitread.h"

#include "stdkey.h"

#include "tplot.h"
#include "fit.h"
#include "oldfit.h"
#include "smr.h"
#include "cfit.h"
#include "expr.h"
#include "hlpstr.h"
#include "errstr.h"

#include "graphics.h"
#include "key.h"

#include "version.h"




#define HEIGHT 640
#define WIDTH  540
#define MAX_RANGE 300

struct OptionData opt;
struct OptionFile *optf=NULL;

struct RadarNetwork *network;  
struct Radar *radar;
struct RadarSite *site;

struct RadarParm *prm;
struct FitData *fit;
struct CFitdata *cfit;

struct FitIndex *inx;

struct tplot tplot;
struct key fkey;
struct key nkey;
struct key pkey;
struct key vkey;
struct key wkey;

char *mstr[]={"January","February","March","April","May","June","July",
              "August","September","October","November","December",0};


char *label_km(double val,double min,double max,void *data) {
  char *txt=NULL;
  txt=malloc(32);
  sprintf(txt,"%g",val);
  return txt;
}



char *label_lat(double val,double min,double max,void *data) {
  char *txt=NULL;
  txt=malloc(32);
  sprintf(txt,"%.3g",val);
  return txt;
}


char *label_gate(double val,double min,double max,void *data) {
  char *txt=NULL;
  txt=malloc(32);
  sprintf(txt,"%.2d",(int) val);
  return txt;
}

char *label_tme(double val,double min,double max,void *data) {
  char *txt=NULL;
  txt=malloc(32);
  sprintf(txt,"%.2d:%.2d",((int) (val/3600)) % 24,((int) (val/60)) % 60);
  return txt;
}

char *label_pwr(double val,double min,double max,void *data) {
  char *txt=NULL;
  if ((val !=min) && (val !=max)) return NULL;
  txt=malloc(32);
  sprintf(txt,"%d dB",(int) val);
  return txt;
}

char *label_vel(double val,double min,double max,void *data) {
  char *txt=NULL;
  if ((val !=min) && (val !=max) && (val !=0)) return NULL;
  txt=malloc(32);
  sprintf(txt,"%d m/s",(int) val);
  return txt;
}

char *label_wdt(double val,double min,double max,void *data) {
  char *txt=NULL;
  if ((val !=min) && (val !=max)) return NULL;
  txt=malloc(32);
  sprintf(txt,"%d m/s",(int) val);
  return txt;
}

char *label_noise(double val,double min,double max,void *data) {
  char *txt=NULL;
  txt=malloc(32);
  sprintf(txt,"%d",(int) val);
  return txt;
}

char *label_freq(double val,double min,double max,void *data) {
  char *txt=NULL;
  txt=malloc(32);
  sprintf(txt,"%.2g",val*10);
  return txt;
}




int stream(char *buf,int sze,void *data) {
  FILE *fp;
  fp=(FILE *) data;
  fwrite(buf,sze,1,stdout);
  return 0;
}

int xmldecode(char *buf,int sze,void *data) {
  struct XMLdata *xmldata;
  xmldata=(struct XMLdata *) data;
  return XMLDecode(xmldata,buf,sze);
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

int main(int argc,char *argv[]) {

 /* File format transistion
   * ------------------------
   * 
   * When we switch to the new file format remove any reference
   * to "new". Change the command line option "new" to "old" and
   * remove "old=!new".
   */


  int old=0;
  int new=0;

  char *envstr;

#ifdef _XLIB_
  int xdf=0;
  int xd=0;
  struct XwinDisplay *dp;
  struct XwinWindow *win;
  char *display_name=NULL;
  char *wname="time_plot";
  int xdoff=-1;
  int ydoff=-1;
#endif

  int arg=0;  
  char *stmestr=NULL;
  char *etmestr=NULL;
  char *sdtestr=NULL;
  char *edtestr=NULL;
  char *exstr=NULL;

  double otime=-1;
  double atime=-1;

  double stime=-1;
  double etime=-1;
  double extime=0;
 
  double sdate=-1;
  double edate=-1;

  struct OldFitFp *oldfitfp=NULL;
  struct CFitfp *cfitfp=NULL;
  FILE *fitfp=NULL;
  FILE *smrfp=NULL;

  int bmnum=-1;
  char *chtxt=NULL;
  int chnum=-1;
  int cpid=-1; 
  int sflg=0;
  int scan=-8000;

  unsigned char allflg=0;
  unsigned char velflg=0;
  unsigned char pwrflg=0;
  unsigned char wdtflg=0;

  unsigned char gsflg=0;
  unsigned char gmflg=0;
  unsigned char nsflg=0;

  double pmin=0;
  double pmax=30;
 
  double vmin=-2000;
  double vmax=2000;
 
  double wmin=0;
  double wmax=500;
  
  double fmin=0.9;
  double fmax=1.9;
  
  double nmin=0;
  double nmax=30;

  unsigned char lrngflg=0;
  unsigned char kmflg=0;

  int frang=-1;
  int erang=-1;
  int rsep=0;

  int erng=MAX_RANGE;
  int srng=0;
  int sprng=1;
  int eprng=MAX_RANGE;

  unsigned char geoflg=0;
  unsigned char magflg=0;
  float latmin=50;
  float latmax=80;

  char *expr=NULL;

  unsigned int grdcolor=0;
  char *grdtxt=NULL;
  unsigned int txtcolor=0;
  char *txttxt=NULL;

  unsigned int gscolor=0;
  char *gsctxt=NULL;

  char *key_path=NULL;
  char kname[256];
  char *pkey_fname=NULL;
  char *vkey_fname=NULL;
  char *wkey_fname=NULL;
  char *fkey_fname=NULL;
  char *nkey_fname=NULL;


  struct FrameBuffer *img=NULL;
  struct PostScript *psdata=NULL;
  FILE *fontfp=NULL;
  char *fntdbfname=NULL;
  struct FrameBufferFontDB *fontdb=NULL;

  char *name=NULL;
  char *dname="tseries";
  char *bgtxt=NULL;
  int wdt=WIDTH;
  int hgt=HEIGHT;
  unsigned int bgcolor=0;

  float width=0.5;
  char *fontname=NULL;
  char *tfontname=NULL;
  char *deffont={"Helvetica"};
  float fontsize=10.0;
  float tfontsize=12.0;

  int lft,rgt,btm,top;

  struct Splot *splot=NULL;
  struct Plot *plot=NULL;
  struct Grplot *plt;

  struct Grplot *nfplt;

  float xpoff=40,ypoff=40;

  unsigned char psflg=0;
  unsigned char ppmflg=0;
  unsigned char xmlflg=0;
  unsigned char ppmxflg=0;
  unsigned char pngflg=0;

  unsigned char gflg=0;
  unsigned char pflg=0;

  unsigned char fitflg=0;
  unsigned char smrflg=0;
  unsigned char cfitflg=0;

  unsigned char help=0;
  unsigned char option=0;

  char *cfname=NULL;
  FILE *fp;

  int rng;
  int fbeam=8;

  int bwdt=WIDTH;
  int bhgt=HEIGHT;

  int x,y;
  unsigned char av,rv,gv,bv;

  int n,i;

  int c=0;
  double val=0;
  int cnt=0;

  float xmajor=0;
  float xminor=0;
  float ymajor=0;
  float yminor=0;
  int ytick=10;

  char *xmajorstr=NULL;
  char *xminorstr=NULL;

  int yr,mo,dy,hr,mt;
  double sc;

  int type[3];
  struct FrameBuffer *blk[3]={NULL,NULL,NULL};
  struct FrameBuffer *fblk;
  struct FrameBuffer *nblk;

  char revtxt[256];
  char txt[256],tmp[32];
  int stid=0;

  float box[3];
  float lnehgt;

  int cpnum=0;
  int cptab[256];

  prm=RadarParmMake();
  fit=FitMake();
  cfit=CFitMake();

  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"-option",'x',&option);

  OptionAdd(&opt,"new",'x',&new); /* new format switch */

  OptionAdd(&opt,"cf",'t',&cfname); /* config file */

#ifdef _XLIB_ 
  OptionAdd(&opt,"x",'x',&xd); /* plot to X display */
  OptionAdd(&opt,"display",'t',&display_name); /* display name */
  OptionAdd(&opt,"xoff",'i',&xdoff); /* X display X offset */
  OptionAdd(&opt,"yoff",'i',&ydoff); /* X display Y offset */
#endif

  OptionAdd(&opt,"ppm",'x',&ppmflg); /* PPM output */
  OptionAdd(&opt,"ppmx",'x',&ppmxflg); /* PPMX output */
  OptionAdd(&opt,"xml",'x',&xmlflg); /* XML output */
  OptionAdd(&opt,"png",'x',&pngflg); /* PNG output */
  OptionAdd(&opt,"ps",'x',&psflg);  /* PS output */

  OptionAdd(&opt,"xp",'f',&xpoff); /* X page offset */
  OptionAdd(&opt,"yp",'f',&ypoff); /* Y page offset */

  OptionAdd(&opt,"name",'t',&name); /* image name */
  OptionAdd(&opt,"wdt",'i',&wdt); /* width */
  OptionAdd(&opt,"hgt",'i',&hgt); /* height */ 
  OptionAdd(&opt,"bgcol",'t',&bgtxt); /* background color */ 

  OptionAdd(&opt,"grdcol",'t',&grdtxt); /* grid color */
  OptionAdd(&opt,"txtcol",'t',&txttxt); /* text color */

  OptionAdd(&opt,"gscol",'t',&gsctxt); /* ground scatter color */

  OptionAdd(&opt,"pkey",'t',&pkey_fname); /* power key */
  OptionAdd(&opt,"vkey",'t',&vkey_fname); /* velocity key */
  OptionAdd(&opt,"wkey",'t',&wkey_fname); /* spectral width key */
  OptionAdd(&opt,"fkey",'t',&fkey_fname); /* frequency key */
  OptionAdd(&opt,"nkey",'t',&nkey_fname); /* noise key */

  OptionAdd(&opt,"fontname",'t',&fontname); /* main font name */
  OptionAdd(&opt,"fontsize",'f',&fontsize); /* main font size */

  OptionAdd(&opt,"tfontname",'t',&tfontname); /* title font name */
  OptionAdd(&opt,"tfontsize",'f',&tfontsize); /* title font size */

  OptionAdd(&opt,"st",'t',&stmestr); /* start time */
  OptionAdd(&opt,"et",'t',&etmestr); /* end time */
  OptionAdd(&opt,"sd",'t',&sdtestr); /* start date */
  OptionAdd(&opt,"ed",'t',&edtestr); /* end date */
  OptionAdd(&opt,"ex",'t',&exstr); /* extent */

  OptionAdd(&opt,"tmajor",'t',&xmajorstr); /* major time tick */
  OptionAdd(&opt,"tminor",'t',&xminorstr); /* minor time tick */

  OptionAdd(&opt,"fit",'x',&fitflg); /* fit file */
  OptionAdd(&opt,"smr",'x',&smrflg); /* summary file */
  OptionAdd(&opt,"cfit",'x',&cfitflg); /* cfit file */

  OptionAdd(&opt,"b",'i',&bmnum); /* beam number */
  OptionAdd(&opt,"c",'t',&chtxt); /* channel number */
  OptionAdd(&opt,"cp",'i',&cpid); /* CP id */
  OptionAdd(&opt,"scan",'i',&scan); /* scan flag */

  OptionAdd(&opt,"a",'x',&allflg); /* plot all */
  OptionAdd(&opt,"p",'x',&pwrflg); /* plot power */
  OptionAdd(&opt,"v",'x',&velflg); /* plot velocity */
  OptionAdd(&opt,"w",'x',&wdtflg); /* plot spectral width */

  OptionAdd(&opt,"pmin",'d',&pmin); /* power minimum */
  OptionAdd(&opt,"pmax",'d',&pmax); /* power maximum */

  OptionAdd(&opt,"vmin",'d',&vmin); /* velocity minimum */
  OptionAdd(&opt,"vmax",'d',&vmax); /* velocity maximum */

  OptionAdd(&opt,"wmin",'d',&wmin); /* spectral width minimum */
  OptionAdd(&opt,"wmax",'d',&wmax); /* spectral width maximum */

  OptionAdd(&opt,"gs",'x',&gsflg); /* shade ground scatter */
  OptionAdd(&opt,"gm",'x',&gmflg); /* mask ground scatter */

  OptionAdd(&opt,"ns",'x',&nsflg); 

  OptionAdd(&opt,"km",'x',&kmflg); /* plot in km */
  OptionAdd(&opt,"frang",'i',&frang); /* first range (km) */
  OptionAdd(&opt,"erang",'i',&erang); /* end range (km) */
  OptionAdd(&opt,"rsep",'i',&rsep);  /* range separation (km) */
 
  OptionAdd(&opt,"srng",'i',&srng); /* first range */
  OptionAdd(&opt,"erng",'i',&erng); /* end range */
 
  OptionAdd(&opt,"geo",'x',&geoflg); /* plot in geographic */
  OptionAdd(&opt,"mag",'x',&magflg); /* plot in magnetic */
 
  OptionAdd(&opt,"latmin",'f',&latmin); /* latitude minimum */
  OptionAdd(&opt,"latmax",'f',&latmax); /* latitude maximum */
 
  OptionAdd(&opt,"ymajor",'f',&ymajor); /* Y axis major tick */
  OptionAdd(&opt,"yminor",'f',&yminor); /* Y axis minor tick */

  OptionAdd(&opt,"ytick",'i',&ytick); /* Number of Y axis tick marks */

  OptionAdd(&opt,"lr",'x',&lrngflg); /* limit ranges */

  OptionAdd(&opt,"fn",'t',&expr); /* apply function */

  OptionAdd(&opt,"fbeam",'i',&fbeam); /* first beam in summary file scan */

  arg=OptionProcess(1,argc,argv,&opt,NULL);  
  if (cfname !=NULL) { /* load the configuration file */
    int farg;
    do {
     fp=fopen(cfname,"r");
     if (fp==NULL) break;
      free(cfname);
      cfname=NULL;
      optf=OptionProcessFile(fp);
      if (optf !=NULL) {
        farg=OptionProcess(0,optf->argc,optf->argv,&opt,NULL);
        OptionFreeFile(optf);
      }   
      fclose(fp);
    } while (cfname !=NULL);
  } 


  old=!new;

  if (help==1) {
    OptionPrintInfo(stdout,hlpstr);
    exit(0);
  }

  if (option==1) {
    OptionDump(stdout,&opt);
    exit(0);
  }

  if (psflg) pflg=1;
  if (xmlflg) gflg=1;
  if (ppmflg) gflg=1;
  if (ppmxflg) gflg=1;
  if (pngflg) gflg=1;

  if ((wdt==0) || (hgt==0)) {
    fprintf(stderr,"Invalid image size.\n");
    exit(-1);
  }

  if (fontname==NULL) fontname=deffont;
  if (tfontname==NULL) tfontname=deffont;

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

  bgcolor=PlotColor(0xff,0xff,0xff,0xff);

  if (bgtxt !=NULL) sscanf(bgtxt,"%x",&bgcolor);

  gscolor=PlotColor(0xa0,0xa0,0xa0,0xff);

  if (gsctxt !=NULL) sscanf(gsctxt,"%x",&gscolor);


  grdcolor=PlotColor(0x00,0x00,0x00,0xff);

  if (grdtxt !=NULL) sscanf(grdtxt,"%x",&grdcolor);

  txtcolor=PlotColor(0x00,0x00,0x00,0xff);

  if (txttxt !=NULL)  sscanf(txttxt,"%x",&txtcolor);

  if (pkey_fname !=NULL) {
    key_path = getenv("COLOR_TABLE_PATH");
    if (key_path != NULL) {
      strcpy(kname, key_path);
      strcat(kname, pkey_fname);
    } else {
      fprintf(stderr, "No COLOR_TABLE_PATH set\n");
    }
    fp=fopen(kname,"r");
    if (fp !=NULL) {
      load_key(fp,&pkey);
      fclose(fp);
    }
  }

  if (vkey_fname !=NULL) {
    if (key_path == NULL) key_path = getenv("COLOR_TABLE_PATH");
    if (key_path != NULL) {
      strcpy(kname, key_path);
      strcat(kname, vkey_fname);
    } else {
      fprintf(stderr, "No COLOR_TABLE_PATH set\n");
    }
    fp=fopen(kname,"r");
    if (fp !=NULL) {
      load_key(fp,&vkey);
      fclose(fp);
    }
  }

  if (wkey_fname !=NULL) {
    if (key_path == NULL) key_path = getenv("COLOR_TABLE_PATH");
    if (key_path != NULL) {
      strcpy(kname, key_path);
      strcat(kname, wkey_fname);
    } else {
      fprintf(stderr, "No COLOR_TABLE_PATH set\n");
    }
    fp=fopen(kname,"r");
    if (fp !=NULL) {
      load_key(fp,&wkey);
      fclose(fp);
    }
  }

  if (fkey_fname !=NULL) {
    if (key_path == NULL) key_path = getenv("COLOR_TABLE_PATH");
    if (key_path != NULL) {
      strcpy(kname, key_path);
      strcat(kname, fkey_fname);
    } else {
      fprintf(stderr, "No COLOR_TABLE_PATH set\n");
    }
    fp=fopen(kname,"r");
    if (fp !=NULL) {
      load_key(fp,&fkey);
      fclose(fp);
    }
  }

  if (nkey_fname !=NULL) {
    if (key_path == NULL) key_path = getenv("COLOR_TABLE_PATH");
    if (key_path != NULL) {
      strcpy(kname, key_path);
      strcat(kname, nkey_fname);
    } else {
      fprintf(stderr, "No COLOR_TABLE_PATH set\n");
    }
    fp=fopen(kname,"r");
    if (fp !=NULL) {
      load_key(fp,&nkey);
      fclose(fp);
    }
  }


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
    SplotSetPostScript(splot,psdata,0,xpoff,ypoff);
  }

  if (exstr !=NULL) extime=strtime(exstr);
  if (stmestr !=NULL) stime=strtime(stmestr);
  if (etmestr !=NULL) etime=strtime(etmestr);
  if (sdtestr !=NULL) sdate=strdate(sdtestr);
  if (edtestr !=NULL) edate=strdate(edtestr);
  
  if (xmajorstr !=NULL) xmajor=strtime(xmajorstr);
  if (xminorstr !=NULL) xminor=strtime(xminorstr);


  if ((smrflg==0) && (cfitflg==0)) fitflg=1;
   
  if (scan !=-8000) sflg=1;

  if (chtxt !=NULL) {
    if (tolower(chtxt[0])=='a') chnum=1;
    if (tolower(chtxt[0])=='b') chnum=2;
  }
  
  if (pkey.max==0) {
      pkey.max=KeyLinearMax;
      pkey.a=KeyLinearA[0];
      pkey.r=KeyLinearR[0];
      pkey.g=KeyLinearG[0];
      pkey.b=KeyLinearB[0];
  }

  if (vkey.max==0) {
      vkey.max=KeyLinearMax;
      vkey.a=KeyLinearA[1];
      vkey.r=KeyLinearR[1];
      vkey.g=KeyLinearG[1];
      vkey.b=KeyLinearB[1];
  }

  if (wkey.max==0) {
      wkey.max=KeyLinearMax;
      wkey.a=KeyLinearA[0];
      wkey.r=KeyLinearR[0];
      wkey.g=KeyLinearG[0];
      wkey.b=KeyLinearB[0];
  }
 
  if (nkey.max==0) {
      nkey.max=KeyTenBlkMax;
      nkey.a=KeyTenBlkA[0];
      nkey.r=KeyTenBlkR[0];
      nkey.g=KeyTenBlkG[0];
      nkey.b=KeyTenBlkB[0];
  }

  if (fkey.max==0) {
      fkey.max=KeyTenBlkMax;
      fkey.a=KeyTenBlkA[0];
      fkey.r=KeyTenBlkR[0];
      fkey.g=KeyTenBlkG[0];
      fkey.b=KeyTenBlkB[0];
  }

  if (fitflg) {
    if (old) {
       if (arg==argc) {
       OptionPrintInfo(stderr,errstr);
        exit(-1);
      }
      if (argc-arg>1)  oldfitfp=OldFitOpen(argv[arg],argv[arg+1]);
      else oldfitfp=OldFitOpen(argv[arg],NULL);
      if (oldfitfp==NULL) {
        fprintf(stderr,"file %s not found\n",argv[arg]);
        exit(-1);
      }
      stime=oldfit_find(oldfitfp,prm,fit,sdate,stime);
    } else {
      if (arg==argc) fp=stdin;
      else {
         if (argc-arg>1)  {
	   fp=fopen(argv[arg+1],"r");
           if (fp==NULL) {
             fprintf(stderr,"Index not found.\n");
	   } else {
             inx=FitIndexFload(fp);
             fclose(fp);
             if (inx==NULL) {
               fprintf(stderr,"Error reading index.\n");
             }
	   }
	 }
         fitfp=fopen(argv[arg],"r");
         if (fitfp==NULL) {
          fprintf(stderr,"file %s not found\n",argv[arg]);
          exit(-1);
        }
      }
      stime=fit_find(fitfp,prm,fit,sdate,stime,inx);
     
    } 
  } else if (smrflg) {
    if (arg==argc) {
     OptionPrintInfo(stderr,errstr);
      exit(-1);
    }
    smrfp=fopen(argv[arg],"r");
    if (smrfp==NULL) {
      fprintf(stderr,"file %s not found\n",argv[arg]);
      exit(-1);
    }    
    stime=smr_find(smrfp,prm,fit,fbeam,sdate,stime);
  } else if (cfitflg) {
    if (arg==argc) {
     OptionPrintInfo(stderr,errstr);
      exit(-1);
    }
    cfitfp=CFitOpen(argv[arg]);
    if (cfitfp==NULL) {
      fprintf(stderr,"file %s not found\n",argv[arg]);
      exit(-1);
    } 
    stime=cfit_find(cfitfp,cfit,sdate,stime);
  }


  if (etime !=-1) {
    if (edate==-1) etime+=stime - ( (int) stime % (24*3600));
    else etime+=edate;
  } else etime=stime+24*3600;
  if (extime !=0) etime=stime+extime;    

  if (name==NULL) name=dname;
 
  if (fitflg) {
    if (old) atime=oldfit_scan(stime,oldfitfp,0,prm,
                             fit,bmnum,chnum,cpid,sflg,scan); 
    else atime=fit_scan(stime,fitfp,0,prm,
                             fit,bmnum,chnum,cpid,sflg,scan); 
  } else if (smrflg) atime=smr_scan(stime,smrfp,fbeam,0,prm,
                                  fit,bmnum,chnum,cpid,
                                  sflg,scan); 
  else if (cfitflg) atime=cfit_scan(atime,cfitfp,1,cfit,bmnum,chnum,cpid,
                                    sflg,scan);

  if ((fitflg) || (smrflg)) {
    stid=prm->stid;
    cptab[0]=prm->cp;
    cpnum=1;
    sprintf(revtxt,"Revision:%d.%d",fit->revision.major,
            fit->revision.minor);
  }
  if (cfitflg) {
    stid=cfit->stid;
    cptab[0]=cfit->cp;
    cpnum=1;
    sprintf(revtxt,"Revision:%d.%d",cfit->version.major,
            cfit->version.minor);
  }
  if (erng==MAX_RANGE) {
     if ((fitflg) || (smrflg)) erng=prm->nrang;
     if (cfitflg) erng=cfit->nrang;
  }

  if (kmflg) {
    if ((fitflg) || (smrflg)) {
      if (frang==-1) frang=prm->frang;
      if (rsep==0) rsep=prm->rsep;
      if (erang==-1) erang=frang+prm->nrang*rsep;
    } 
   if (cfitflg) {
      if (frang==-1) frang=cfit->frang;
      if (rsep==0) rsep=cfit->rsep;
      if (erang==-1) erang=frang+cfit->nrang*cfit->rsep;
    } 
  }
 

 envstr=getenv("SD_RADAR");
  if (envstr==NULL) {
    fprintf(stderr,"Environment variable 'SD_RADAR' must be defined.\n");
    exit(-1);
  }

  fp=fopen(envstr,"r");

  if (fp==NULL) {
    fprintf(stderr,"Could not locate radar information file.\n");
    exit(-1);
  }

  network=RadarLoad(fp);
  fclose(fp); 
  if (network==NULL) {
    fprintf(stderr,"Failed to read radar information.\n");
    exit(-1);
  }
 


  if (geoflg || magflg) { 
    envstr=getenv("SD_HDWPATH");
    if (envstr==NULL) {
      fprintf(stderr,"Environment variable 'SD_HDWPATH' must be defined.\n");
      exit(-1);
    }

    RadarLoadHardware(envstr,network);
    if ((fitflg) || (smrflg)) {
     radar=RadarGetRadar(network,prm->stid);
     site=RadarYMDHMSGetSite(radar,prm->time.yr,prm->time.mo,
		          prm->time.dy,prm->time.hr,prm->time.mt,
                          prm->time.sc);

    }
    if (cfitflg) {
      TimeEpochToYMDHMS(cfit->time,&yr,&mo,&dy,&hr,&mt,&sc);
      radar=RadarGetRadar(network,cfit->stid);
      site=RadarYMDHMSGetSite(radar,yr,mo,dy,hr,mt,sc);
    }


    if (site==NULL) { 
      fprintf(stderr,"Station not found.\n");
      exit(-1);
    }  
  }

  PlotDocumentStart(plot,name,NULL,wdt,hgt,24);
  PlotPlotStart(plot,name,wdt,hgt,24);
  PlotRectangle(plot,NULL,0,0,wdt,hgt,1,bgcolor,0x0f,0,NULL);

  if (allflg) {
    pwrflg=1;
    velflg=1;
    wdtflg=1;
  }

  cnt=0;
  if (pwrflg) {
    type[cnt]=0;
    cnt++;
  }
  if (velflg) {
    type[cnt]=1;
    cnt++;
  }
  if (wdtflg) {
    type[cnt]=2;
    cnt++;
  }

  plt=GrplotMake(wdt,hgt-150,1,cnt,60,80,0,25,0,120);
  GrplotSetPlot(plt,plot);
  GrplotSetTextBox(plt,txtbox,fontdb);  

  nfplt=GrplotMake(wdt,40,1,2,60,80,0,8,0,80);
  GrplotSetPlot(nfplt,plot);
  GrplotSetTextBox(nfplt,txtbox,fontdb);  

  bwdt=plt->box_wdt;
  bhgt=plt->box_hgt;
  
  nblk=FrameBufferMake("noise",bwdt,1,24);
  fblk=FrameBufferMake("frequency",bwdt,1,24);

  if (pwrflg) blk[0]=FrameBufferMake("power",bwdt,bhgt,24);
  if (velflg) blk[1]=FrameBufferMake("velocity",bwdt,bhgt,24);
  if (wdtflg) blk[2]=FrameBufferMake("width",bwdt,bhgt,24);

  FrameBufferClear(fblk,0x0f,bgcolor);
  FrameBufferClear(nblk,0x0f,bgcolor);
  
  for (n=0;n<3;n++) if (blk[n] !=NULL) FrameBufferClear(blk[n],0x0f,bgcolor);

  do {   

    otime=atime;
    if ((fitflg) || (smrflg)) fit_tplot(prm,fit,&tplot);
    if (cfitflg) cfit_tplot(cfit,&tplot);
  
    for (i=0;i<cpnum;i++) if (cptab[i]==tplot.cpid) break;
    if (i==cpnum) {
      cptab[i]=tplot.cpid;
      cpnum++;
    }

 
    if (fitflg) {
      if (old) atime=oldfit_scan(atime,oldfitfp,1,prm,fit,
                               bmnum,chnum,cpid,sflg,scan);
      else atime=fit_scan(atime,fitfp,1,prm,fit,
                               bmnum,chnum,cpid,sflg,scan); 
    } else if (smrflg) atime=smr_scan(atime,smrfp,fbeam,1,prm,fit,
                                    bmnum,chnum,cpid,
                                    sflg,scan);
    else if (cfitflg) atime=cfit_scan(atime,cfitfp,1,cfit,bmnum,chnum,cpid,
                                      sflg,scan);

    if (atime==-1) break;
    if ((tplot.scan<0) && (nsflg)) {
      if (atime>=etime) break;
      continue;
    }  
    
    if ((atime-otime)>120) otime=atime-120;
    
    lft=bwdt*(otime-stime)/(etime-stime);
    rgt=bwdt*(atime-stime)/(etime-stime);
    if (rgt==lft) rgt++;
    if (lft<0) lft=0;
    if (lft>=bwdt) lft=bwdt-1;
    if (rgt<0) rgt=0;
    if (rgt>=bwdt) rgt=bwdt-1;
    for (n=0;n<3;n++) {
      if (blk[n]==NULL) continue;
      if (lrngflg==0) {
        sprng=1;
        eprng=tplot.nrang;
      } else {
        sprng=srng+1;
        eprng=erng;
      }
      for (rng=sprng;rng<=eprng;rng++) {
        if ((expr !=NULL) && (eval_expr(expr,&tplot,rng)==0)) continue;
        if (tplot.qflg[rng]==0) continue;
        if ((tplot.gsct[rng]==1) && (gmflg)) continue;
   
        if (rng>0) btm=bhgt*(rng-1-srng)/(erng-srng);
        else btm=0; 
        top=bhgt*(rng-srng)/(erng-srng);


        if (kmflg) {

          btm=tplot.frang+(rng-1)*tplot.rsep;
          top=tplot.frang+rng*tplot.rsep;
          
          if (top<frang) continue;
          if (btm>erang) continue;
          if (top>=erang) top=erang;
          if (btm<frang) btm=frang;

          btm=bhgt*(btm-frang)/(erang-frang);
          top=bhgt*(top-frang)/(erang-frang);
        }

        if ((geoflg) || (magflg)) {
          double rho,blat,tlat,lon,tmp;
          if (magflg) RPosMag(0,tplot.bmnum,rng-1,site,tplot.frang,
                                 tplot.rsep,tplot.rxrise,300,&rho,
                                 &blat,&lon);   
          else RPosGeo(0,tplot.bmnum,rng-1,site,tplot.frang,tplot.rsep,
                        tplot.rxrise,300,&rho,&blat,&lon);   
   
          if (magflg) RPosMag(0,tplot.bmnum,rng,site,tplot.frang,tplot.rsep,
                               tplot.rxrise,300,&rho,&tlat,&lon);   
          else RPosGeo(0,tplot.bmnum,rng,site,tplot.frang,tplot.rsep,
                        tplot.rxrise,300,&rho,&tlat,&lon);   

          

          if (tlat<blat) {
            tmp=blat;
            blat=tlat;
            tlat=blat;
	  }
          btm=bhgt*(blat-latmin)/(latmax-latmin);
          top=bhgt*(tlat-latmin)/(latmax-latmin);
	}
        if (btm==top) top++;
        if (btm<0) btm=0;
        if (btm>=bhgt) btm=bhgt-1;
        if (top<0) top=0;
        if (top>=bhgt) top=bhgt-1;       

        if ((n==1) && (tplot.gsct[rng]==1) && (gsflg)) {
          av=(gscolor & 0xff000000) >> 24;
   	  rv=(gscolor & 0xff0000) >> 16;
	  gv=(gscolor & 0xff00) >> 8;
          bv=(gscolor & 0xff);
        } else {
          if (n==0) val=tplot.p_l[rng];
          if (n==1) val=tplot.v[rng];
          if (n==2) val=tplot.w_l[rng];
          
          if (c<0) c=0;
          if (n==0) {
            val=(val-pmin)/(pmax-pmin);
            c=val*pkey.max;
            if (c<0) c=0;
            if (c>=pkey.max) c=pkey.max-1; 
            if (pkey.a !=NULL) av=pkey.a[c];
            else av=255;
            rv=pkey.r[c];
            gv=pkey.g[c];
            bv=pkey.b[c];
	  } else if (n==1) {
            val=(val-vmin)/(vmax-vmin);
            c=val*vkey.max;
            if (c<0) c=0;
            if (c>=vkey.max) c=vkey.max-1;
            if (vkey.a !=NULL) av=vkey.a[c];
            else av=255; 
            rv=vkey.r[c];
            gv=vkey.g[c];
            bv=vkey.b[c];
	  } else {
            val=(val-wmin)/(wmax-wmin);
            c=val*wkey.max;
            if (c<0) c=0;
            if (c>=wkey.max) c=wkey.max-1; 
            if (wkey.a !=NULL) av=wkey.a[c];
		else av=255;
            rv=wkey.r[c];
            gv=wkey.g[c];
            bv=wkey.b[c];
	  }
        }  
	  
	for (x=lft;x<=rgt;x++) {
            for (y=btm;y<=top;y++) {
              blk[n]->img[(bhgt-1-y)*bwdt+x]=rv;
              blk[n]->img[bwdt*bhgt+(bhgt-1-y)*bwdt+x]=gv;
	      blk[n]->img[2*bwdt*bhgt+(bhgt-1-y)*bwdt+x]=bv;
              blk[n]->msk[(bhgt-1-y)*bwdt+x]=av;
	  }	  
        }


      }
    }
   
   
  
    if (tplot.noise>0) val=nmax*log10(tplot.noise/200.0)/3.0;
    else val=0;

    val=(val-nmin)/(nmax-nmin);   
    c=val*nkey.max;
    if (c<0) c=0;
    if (c>=nkey.max) c=nkey.max-1;
    
    for (x=lft;x<=rgt;x++) {
      nblk->img[x]=nkey.r[c];
      nblk->img[bwdt+x]=nkey.g[c];
      nblk->img[2*bwdt+x]=nkey.b[c];
      if (nkey.a !=NULL) nblk->msk[x]=nkey.a[c];
      else nblk->msk[x]=255;

    }
    
    val=tplot.tfreq/10000.0;

    val=(val-fmin)/(fmax-fmin);   
    c=val*fkey.max;
    if (c<0) c=0;
    if (c>=fkey.max) c=fkey.max-1;

    for (x=lft;x<=rgt;x++) {
      fblk->img[x]=fkey.r[c];
      fblk->img[bwdt+x]=fkey.g[c];
      fblk->img[2*bwdt+x]=fkey.b[c];
      if (fkey.a !=NULL) fblk->msk[x]=fkey.a[c];
	else fblk->msk[x]=255;
    }
     
  } while (atime<etime);
 
  if (fitflg){
    if (old) OldFitClose(oldfitfp);
    else if (fitfp !=stdin) fclose(fitfp);
  } else if (smrflg) fclose(smrfp);

  i=0;
  if (pwrflg) {
    GrplotFitImage(plt,i,blk[0],0x0f);
    i++;
  }
  if (velflg) {
    GrplotFitImage(plt,i,blk[1],0x0f);
    i++;
  }
  if (wdtflg) {
    GrplotFitImage(plt,i,blk[2],0x0f);
    i++;
  }

  if (xmajor==0) {
    xmajor=3*3600;    
    if ((etime-stime)<8*3600) xmajor=3600;
    if ((etime-stime)>48*3600) xmajor=12*3600;
    if ((etime-stime)>160*3600) xmajor=24*3600;
  }
   

  if (xminor==0) {
    xminor=15*60;
    if ((etime-stime)<8*3600) xminor=600;
    if ((etime-stime)>48*3600) xminor=3600;
    if ((etime-stime)>160*3600) xminor=3*3600;
  }

  if (ymajor==0) {
    if (kmflg) ymajor=(erang-frang)/ytick;
    else if ((geoflg) || (magflg)) ymajor=(latmax-latmin)/ytick; 
    else ymajor=15;
  }
  if (yminor==0) yminor=5;

  plt->major_wdt=3;
  plt->major_hgt=3;
  plt->ttl_wdt=30;
  if (kmflg) plt->ttl_wdt=40;
  if ((geoflg) || (magflg)) plt->ttl_wdt=40;
  plt->ttl_hgt=5;
  plt->lbl_hgt=8;
  for (i=0;i<cnt;i++) {
    GrplotPanel(plt,i,grdcolor,0x0f,width);
    GrplotXaxis(plt,i,stime,etime,xmajor,xminor,0x07,grdcolor,0x0f,width);
    if (kmflg) {
       GrplotYaxis(plt,i,frang,erang,ymajor,0,0x03,
                 grdcolor,0x0f,width);

       GrplotYaxisLabel(plt,i,frang,erang,ymajor,
                      0x01,label_km,NULL,fontname,
                     fontsize,txtcolor,0x0f);
       GrplotYaxisTitle(plt,i,0x01,strlen("Range (km)"),"Range (km)",
                     fontname,fontsize,txtcolor,0x0f);

    } else if ((geoflg) || (magflg)) {
      GrplotYaxis(plt,i,latmin,latmax,
                ymajor,0,0x03,grdcolor,0x0f,width);
      GrplotYaxisLabel(plt,i,latmin,latmax,ymajor,
                      0x01,label_lat,NULL,fontname,
                     fontsize,txtcolor,0x0f);
      if (magflg) GrplotYaxisTitle(plt,i,0x01,strlen("Magnetic Latitude"),
                      "Magnetic Latitude",
                     fontname,fontsize,txtcolor,0x0f);
      if (geoflg) GrplotYaxisTitle(plt,i,0x01,strlen("Geographic Latitude"),
                      "Geographic Latitude",
                     fontname,fontsize,txtcolor,0x0f);
    } else {
     GrplotYaxis(plt,i,srng,erng,yminor,0,0x03,grdcolor,0x0f,width);
     GrplotYaxisLabel(plt,i,srng,erng,ymajor,
                      0x01,label_gate,NULL,fontname,
                     fontsize,txtcolor,0x0f);
     GrplotYaxisTitle(plt,i,0x01,strlen("Range Gate"),"Range Gate",
                    fontname,fontsize,txtcolor,0x0f);
    }
    if (type[i]==0) {
      GrplotKey(plt,i,10,0,8,bhgt,pmin,pmax,(pmax-pmin)/10,0x02,0x00,NULL,
              label_pwr,NULL,fontname,fontsize,txtcolor,0x0f,
              width,pkey.max,pkey.a,pkey.r,pkey.g,pkey.b);

      GrplotXaxisTitle(plt,i,0x02,strlen("Power"),"Power",
                     fontname,fontsize,txtcolor,0x0f);
    }
    if (type[i]==1) {

      GrplotKey(plt,i,10,0,8,bhgt,vmin,vmax,vmax/5,0x02,0x00,NULL,
              label_vel,NULL,fontname,fontsize,txtcolor,0x0f,
              width,vkey.max,vkey.a,vkey.r,vkey.g,vkey.b);
      GrplotXaxisTitle(plt,i,0x02,strlen("Velocity"),"Velocity",
                     fontname,fontsize,txtcolor,0x0f);
    }

    if (type[i]==2) {

      GrplotKey(plt,i,10,0,8,bhgt,wmin,wmax,(wmax-wmin)/10,0x02,0x00,NULL,
              label_wdt,NULL,fontname,fontsize,txtcolor,0x0f,
              width,wkey.max,wkey.a,wkey.r,wkey.g,wkey.b);
      GrplotXaxisTitle(plt,i,0x02,strlen("Spectral Width"),"Spectral Width",
                     fontname,fontsize,txtcolor,0x0f);
    }
    if (i==cnt-1) {
        plt->ttl_hgt=20;
        GrplotXaxisLabel(plt,i,stime,etime,xmajor,
                       0x01,label_tme,NULL,fontname,
                       fontsize,txtcolor,0x0f);
        GrplotXaxisTitle(plt,i,0x01,strlen("UT Time"),"UT Time",
                    fontname,fontsize,txtcolor,0x0f);
    }


  }
  nfplt->ttl_yor=0;
  nfplt->ttl_wdt=3;

  GrplotFitImage(nfplt,0,fblk,0x0f);
  GrplotFitImage(nfplt,1,nblk,0x0f);
 
  GrplotPanel(nfplt,0,grdcolor,0x0f,width);
  GrplotPanel(nfplt,1,grdcolor,0x0f,width);
  GrplotYaxisTitle(nfplt,0,0x01,strlen("Frequency"),"Frequency",
                    fontname,fontsize,txtcolor,0x0f);
  GrplotYaxisTitle(nfplt,0,0x02,strlen(revtxt),revtxt,
                    fontname,fontsize,txtcolor,0x0f);
  GrplotYaxisTitle(nfplt,1,0x01,strlen("Noise dB"),"Noise dB",
                    fontname,fontsize,txtcolor,0x0f);

  GrplotKey(nfplt,0,10,-bwdt/4,
          10,bwdt*0.4,nmin,nmax,(nmax-nmin)/10,0x04,strlen("Noise"),"Noise",
              label_noise,NULL,fontname,fontsize,txtcolor,0x0f,
              width,nkey.max,nkey.a,nkey.r,nkey.g,nkey.b);
  GrplotKey(nfplt,0,10,+bwdt/4,
          10,bwdt*0.4,fmin,fmax,(fmax-fmin)/5,0x04,strlen("Frequency"),
          "Frequency",label_freq,NULL,fontname,fontsize,txtcolor,0x0f,
          width,fkey.max,fkey.a,fkey.r,fkey.g,fkey.b);

  sprintf(txt,"Xy");
  txtbox(tfontname,tfontsize,strlen(txt),txt,box,fontdb);
  lnehgt=1.5*(box[2]-box[1]);

  sprintf(txt,"Station:%s (%s)",RadarGetName(network,stid),
          RadarGetCode(network,stid,0));
  PlotText(plot,NULL,tfontname,tfontsize,2,lnehgt,
            strlen(txt),txt,txtcolor,0x0f,0);

  sprintf(txt,"Operated by:%s",RadarGetOperator(network,stid));
  PlotText(plot,NULL,tfontname,tfontsize,2,2*lnehgt,
            strlen(txt),txt,txtcolor,0x0f,0);

  if (sflg) {
    if (scan !=0) sprintf(txt,"Scan Type:%d",scan);
    else sprintf(txt,"Scan Type:Regular");
    PlotText(plot,NULL,tfontname,tfontsize,2,3*lnehgt,
            strlen(txt),txt,txtcolor,0x0f,0);
  }

  TimeEpochToYMDHMS(stime,&yr,&mo,&dy,&hr,&mt,&sc);
  sprintf(txt,"%s, %.2d %.4d (%.4d%.2d%.2d)",mstr[mo-1],dy,yr,yr,mo,dy);
  txtbox(tfontname,tfontsize,strlen(txt),txt,box,fontdb);
  PlotText(plot,NULL,tfontname,tfontsize,wdt-2-box[0],lnehgt,
            strlen(txt),txt,txtcolor,0x0f,0);

  strcpy(txt,"Program ID");
  if (cpnum>1) strcat(txt,"s");
  strcat(txt,":");
  for (i=0;i<cpnum;i++) {
    if (i !=0) strcat(txt,",");
    sprintf(tmp,"%d",cptab[i]);
    strcat(txt,tmp);
  }
  txtbox(tfontname,tfontsize,strlen(txt),txt,box,fontdb);
  PlotText(plot,NULL,tfontname,tfontsize,wdt-2-box[0],2*lnehgt,
            strlen(txt),txt,txtcolor,0x0f,0);

  if (expr !=NULL) {
    sprintf(txt,"Function:%s",expr);
    txtbox(tfontname,tfontsize,strlen(txt),txt,box,fontdb);
    PlotText(plot,NULL,tfontname,tfontsize,wdt-2-box[0],3*lnehgt,
            strlen(txt),txt,txtcolor,0x0f,0);
  }


  if (bmnum==-1) sprintf(txt,"Beam:All");
  else sprintf(txt,"Beam %.2d",bmnum);
 
  txtbox(tfontname,tfontsize,strlen(txt),txt,box,fontdb);
  PlotText(plot,NULL,tfontname,tfontsize,(wdt-box[0])/2,lnehgt,
            strlen(txt),txt,txtcolor,0x0f,0);

  if (chnum !=-1) {
    if (chnum==1) sprintf(txt,"Channel:A");
    else sprintf(txt,"Channel:B");
    txtbox(tfontname,tfontsize,strlen(txt),txt,box,fontdb);
    PlotText(plot,NULL,tfontname,tfontsize,(wdt-box[0])/2,2*lnehgt,
            strlen(txt),txt,txtcolor,0x0f,0);
  }

  if (cpid !=-1) {
    sprintf(txt,"Program ID Plotted:%d",cpid);
    txtbox(tfontname,tfontsize,strlen(txt),txt,box,fontdb);
    PlotText(plot,NULL,tfontname,tfontsize,(wdt-box[0])/2,3*lnehgt,
            strlen(txt),txt,txtcolor,0x0f,0);
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
    return 0;
}

