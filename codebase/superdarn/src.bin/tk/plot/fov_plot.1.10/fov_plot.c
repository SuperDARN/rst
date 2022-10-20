/* fov_plot.c
   ========== 
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
  2021-06 E.G.Thomas (Dartmouth College), bug fixes, multi-radar FOVs, etc
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

#include "polygon.h"


#ifdef _XLIB_
#include "xwin.h"
#endif
#include "raster.h"
#include "rtime.h"
#include "rmap.h"

#include "aacgm.h"
#include "mlt.h"
#include "aacgmlib_v2.h"
#include "mlt_v2.h"

#include "radar.h" 

#include "griddata.h"
#include "make_fov.h"
#include "text_box.h"
#include "plot_time_label.h"
#include "make_grid.h"
#include "sza.h"
#include "szamap.h"
#include "plot_time.h"
#include "plot_logo.h"


#include "hlpstr.h"
#include "errstr.h"
#include "version.h"





struct OptionData opt;
struct OptionFile *optf=NULL;

struct PolygonData *map;
struct PolygonData *nmap;
struct PolygonData *pmap;
 
struct PolygonData *bnd;
struct PolygonData *nbnd;
struct PolygonData *pbnd;

struct PolygonData *grd;
struct PolygonData *ngrd;
struct PolygonData *pgrd;

struct PolygonData *tmk;
struct PolygonData *ntmk;
struct PolygonData *ptmk;

struct PolygonData *trm;
struct PolygonData *ntrm;
struct PolygonData *ptrm;

struct PolygonData *fov;
struct PolygonData *nfov;
struct PolygonData *pfov;

struct PolygonData *clip;

struct RadarNetwork *network;

double eqn_of_time(double mean_lon,int yr);
int solar_loc(int yr,int t1,double *mean_lon, double *dec);

int stream(char *buf,int sze,void *data) {
  FILE *fp;
  fp=(FILE *) data;
  fwrite(buf,sze,1,fp);
  return 0;
} 

int xmldecode(char *buf,int sze,void *data) {
  struct XMLdata *xmldata;
  xmldata=(struct XMLdata *) data;
  return XMLDecode(xmldata,buf,sze);
} 

int AACGM_v2_transform(int ssze,void *src,int dsze,void *dst,void *data) {

  float *pnt;
  int s;
  double mlon,mlat;
  double glon,glat,r;

  pnt=(float *)src;

  if (data==NULL) {
    glat=pnt[0];
    glon=pnt[1];
    s=AACGM_v2_Convert(glat,glon,300,&mlat,&mlon,&r,0);
    pnt=(float *)dst;
    pnt[0]=mlat;
    pnt[1]=mlon;
  } else {
    mlat=pnt[0];
    mlon=pnt[1];
    s=AACGM_v2_Convert(mlat,mlon,300,&glat,&glon,&r,1);
    pnt=(float *)dst;
    pnt[0]=glat;
    pnt[1]=glon;
  }
  return s;

}

int AACGMtransform(int ssze,void *src,int dsze,void *dst,void *data) {

  float *pnt;
  int s;
  double mlon,mlat;
  double glon,glat,r;

  pnt=(float *)src;

  if (data==NULL) {
    glat=pnt[0];
    glon=pnt[1];
    s=AACGMConvert(glat,glon,300,&mlat,&mlon,&r,0);
    pnt=(float *)dst;
    pnt[0]=mlat;
    pnt[1]=mlon;
  } else {
    mlat=pnt[0];
    mlon=pnt[1];
    s=AACGMConvert(mlat,mlon,300,&glat,&glon,&r,1);
    pnt=(float *)dst;
    pnt[0]=glat;
    pnt[1]=glon;
  }
  return s;

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
  double sc;
  int i,j;
  for (i=0;(text[i] !=':') && (text[i] !=0);i++);
  if (text[i]==0) return atoi(text)*3600L;
  text[i]=0;
  hr=atoi(text);
  for (j=i+1;(text[j] !=':') && (text[j] !=0);j++);
  if (text[j]==0) { 
    mn=atoi(text+i+1);
    return (double) hr*3600L+mn*60L;
  }
  text[j]=0;
  mn=atoi(text+i+1);
  sc=atof(text+j+1);
  return (double) hr*3600L+mn*60L+sc;
}  


struct PolygonData *wrap(struct PolygonData *src) {
  struct PolygonData *dst;
  dst=MapWraparound(src);
  PolygonFree(src);
  return dst;
}

int rst_opterr(char *txt) {
  fprintf(stderr,"Option not recognized: %s\n",txt);
  fprintf(stderr,"Please try: fov_plot --help\n");
  return(-1);
}

int main(int argc,char *argv[]) {



#ifdef _XLIB_
  int xdf=0;
  int xd=0;
  struct XwinDisplay *dp;
  struct XwinWindow *win;
  char *display_name=NULL;
  char *wname="fov_plot";
  int xdoff=-1;
  int ydoff=-1;
#endif

  struct FrameBuffer *img=NULL;
  struct PostScript *psdata=NULL;
  FILE *fontfp=NULL;
  char *fntdbfname=NULL;
  struct FrameBufferFontDB *fontdb=NULL;
  unsigned int bgcol;
  unsigned int txtcol;

  int arg;

  char *envstr=NULL;
  char *cfname=NULL;
  FILE *fp;

  struct RadarSite *site;

  float wdt=540,hgt=540;
  float pad=0;

  char *fontname=NULL;
  float fontsize=10.0;
  char *dfontname="Helvetica";

  float width=0.5;

  struct Plot *plot=NULL;
  struct Splot *splot=NULL;

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

  char *bgcol_txt=NULL;
  char *txtcol_txt=NULL;


  unsigned char sqflg=0;
  MapTFunction  tfunc;

  unsigned char flip=0;
  unsigned char gvp=0;
  unsigned char ortho=0;
  unsigned char stereo=0;
  unsigned char cylind=0;

  float lat=90,lon=0;
  float latmin=50.0;
  float sf=1.0;
  float alt=0.0; 

  unsigned char magflg=0;
  unsigned char rotflg=0;

  unsigned char mapflg=0;
  unsigned char fmapflg=0;
  unsigned char bndflg=0;
  unsigned char grdflg=0;
  unsigned char tmkflg=0;
  unsigned char tlblflg=0;
  unsigned char ofovflg=0;
  unsigned char fofovflg=0;

  unsigned char cfovflg=0;
  unsigned char fcfovflg=0;

  unsigned char fovflg=0;
  unsigned char ffovflg=0;

  unsigned char grdontop=0;

  unsigned char dotflg=0;
  unsigned char tmeflg=0;

  int tmtick=3;

  unsigned char lstflg=0;
  unsigned char trmflg=0;
  unsigned char ftrmflg=0;

  char *grdcol_txt=NULL;
  char *cstcol_txt=NULL;
  char *bndcol_txt=NULL;
  char *lndcol_txt=NULL;
  char *seacol_txt=NULL;
  char *trmcol_txt=NULL;
  char *ftrmcol_txt=NULL;
  char *tmkcol_txt=NULL;
  char *ofovcol_txt=NULL;
  char *fofovcol_txt=NULL;
  char *fovcol_txt=NULL;
  char *ffovcol_txt=NULL;

  char *cfovcol_txt=NULL;
  char *fcfovcol_txt=NULL;


  unsigned int grdcol;
  unsigned int cstcol;
  unsigned int bndcol;
  unsigned int lndcol;
  unsigned int seacol;
  unsigned int trmcol;
  unsigned int ftrmcol;
  unsigned int tmkcol;
  unsigned int ofovcol;
  unsigned int fofovcol;
  unsigned int fovcol;
  unsigned int ffovcol;

  unsigned int cfovcol;
  unsigned int fcfovcol;
  

  FILE *mapfp;
  float marg[4];
  int i;

  char *tmetxt=NULL;
  char *dtetxt=NULL;

  double tval=-1;
  double dval=-1;

  int yr,mo,dy,hr,mt,sec,dno;
  double sc;
  int yrsec;
  float tme_shft;



  double LsoT,LT,Hangle;
  //doubledec;
  double eqt;
  
  float dotr=2; 
 
  int stid=-1;
  int st_cnt=0;
  char *ststr=NULL;
  int stnum[64];
  int *rad=NULL;

  char tsfx[16];

  int chisham=0;
  int old_aacgm=0;

  /* function pointer for MLT */
  double (*MLTCnv)(int, int, double);

  envstr=getenv("MAPDATA");

  mapfp=fopen(envstr,"r");
  map=MapFread(mapfp);
  fclose(mapfp);   

  envstr=getenv("BNDDATA");
  mapfp=fopen(envstr,"r");
  bnd=MapBndFread(mapfp);
  fclose(mapfp);

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

  envstr=getenv("SD_HDWPATH");
  if (envstr==NULL) {
    fprintf(stderr,"Environment variable 'SD_HDWPATH' must be defined.\n");
    exit(-1);
  }

  RadarLoadHardware(envstr,network);


  bgcol=PlotColor(0xff,0xff,0xff,0xff);
  txtcol=PlotColor(0x00,0x00,0x00,0xff);

  grdcol=PlotColor(0x20,0x40,0x60,0xff);
  cstcol=PlotColor(0xa0,0xa0,0xa0,0xff);
  bndcol=PlotColor(0x80,0x80,0x80,0xff);
  lndcol=PlotColor(0xff,0xff,0xf0,0xff);
  seacol=PlotColor(0xe0,0xf0,0xff,0xff);
  tmkcol=PlotColor(0x00,0x00,0x00,0xff);
  ofovcol=PlotColor(0x00,0x00,0x00,0xff);
  fofovcol=PlotColor(0xff,0xff,0xff,0xff);
  fovcol=PlotColor(0x00,0x00,0x00,0xff);
  ffovcol=PlotColor(0xc0,0x00,0x00,0xff);
  trmcol=PlotColor(0x80,0x80,0x80,0xff);
  ftrmcol=PlotColor(0x80,0x80,0x80,0xff);

  cfovcol=PlotColor(0x00,0x00,0x00,0xff);
  fcfovcol=PlotColor(0xff,0xff,0xff,0xff);
 
  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"-option",'x',&option);
  OptionAdd(&opt,"-version",'x',&version);

  OptionAdd(&opt,"cf",'t',&cfname);


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
  OptionAdd(&opt,"wdt",'f',&wdt);
  OptionAdd(&opt,"hgt",'f',&hgt);
  OptionAdd(&opt,"pad",'f',&pad);

  OptionAdd(&opt,"bgcol",'t',&bgcol_txt);
  OptionAdd(&opt,"txtcol",'t',&txtcol_txt);

  OptionAdd(&opt,"square",'x',&sqflg);

  OptionAdd(&opt,"ortho",'x',&ortho);
  OptionAdd(&opt,"stereo",'x',&stereo);
  OptionAdd(&opt,"gvp",'x',&gvp);

  OptionAdd(&opt,"lat",'f',&lat);
  OptionAdd(&opt,"lon",'f',&lon);
  OptionAdd(&opt,"latmin",'f',&latmin);
  OptionAdd(&opt,"sf",'f',&sf);
  OptionAdd(&opt,"mag",'x',&magflg);
  OptionAdd(&opt,"rotate",'x',&rotflg);
  OptionAdd(&opt,"alt",'f',&alt);
  OptionAdd(&opt,"flip",'x',&flip);

  OptionAdd(&opt,"coast",'x',&mapflg);
  OptionAdd(&opt,"fcoast",'x',&fmapflg);
  OptionAdd(&opt,"bnd",'x',&bndflg);
  OptionAdd(&opt,"grd",'x',&grdflg);
  OptionAdd(&opt,"tmk",'x',&tmkflg);

  OptionAdd(&opt,"grdontop",'x',&grdontop);

  OptionAdd(&opt,"fov",'x',&fovflg);
  OptionAdd(&opt,"ffov",'x',&ffovflg);

  OptionAdd(&opt,"ofov",'x',&ofovflg);
  OptionAdd(&opt,"fofov",'x',&fofovflg);

  OptionAdd(&opt,"cfov",'x',&cfovflg);
  OptionAdd(&opt,"fcfov",'x',&fcfovflg);

  OptionAdd(&opt,"tmtick",'i',&tmtick);
  OptionAdd(&opt,"lst",'x',&lstflg);
  OptionAdd(&opt,"term",'x',&trmflg);
  OptionAdd(&opt,"fterm",'x',&ftrmflg);

  OptionAdd(&opt,"grdcol",'t',&grdcol_txt);
  OptionAdd(&opt,"cstcol",'t',&cstcol_txt);
  OptionAdd(&opt,"bndcol",'t',&bndcol_txt);
  OptionAdd(&opt,"lndcol",'t',&lndcol_txt);
  OptionAdd(&opt,"seacol",'t',&seacol_txt);
  OptionAdd(&opt,"trmcol",'t',&trmcol_txt);
  OptionAdd(&opt,"ftrmcol",'t',&ftrmcol_txt);
  OptionAdd(&opt,"tmkcol",'t',&tmkcol_txt);
 
  OptionAdd(&opt,"fovcol",'t',&fovcol_txt);
  OptionAdd(&opt,"ffovcol",'t',&ffovcol_txt);
  OptionAdd(&opt,"ofovcol",'t',&ofovcol_txt);
  OptionAdd(&opt,"fofovcol",'t',&fofovcol_txt);

  OptionAdd(&opt,"cfovcol",'t',&cfovcol_txt);
  OptionAdd(&opt,"fcfovcol",'t',&fcfovcol_txt);

  OptionAdd(&opt,"st",'t',&ststr);

  OptionAdd(&opt,"t",'t',&tmetxt);
  OptionAdd(&opt,"d",'t',&dtetxt);
  
  OptionAdd(&opt,"tmlbl",'x',&tlblflg);

  OptionAdd(&opt,"fontname",'t',&fontname);
  OptionAdd(&opt,"fontsize",'f',&fontsize);
  OptionAdd(&opt,"lnewdt",'f',&width);

  OptionAdd(&opt,"dotr",'f',&dotr);
  OptionAdd(&opt,"dot",'x',&dotflg);

  OptionAdd(&opt,"time",'x',&tmeflg);

  OptionAdd(&opt,"old_aacgm",'x',&old_aacgm);

  OptionAdd(&opt,"chisham",'x',&chisham); /* use Chisham virtual height model */

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


  if (tmetxt !=NULL) tval=strtime(tmetxt);
  if (dtetxt !=NULL) dval=strdate(dtetxt);

  if (magflg && old_aacgm) magflg = 2; /* set to 2 for old AACGM */

  /* set function pointer to compute MLT or MLT_v2 */
  if (old_aacgm) MLTCnv = &MLTConvertYrsec;
  else           MLTCnv = &MLTConvertYrsec_v2;

  if (dval !=-1) {
    if (tval !=-1) {
      tval+=dval;
    } else {
      tval=dval;
    }
  } else {
    fprintf(stderr,"\nDate must be set for fov_plot, using today's date.\n");
    AACGM_v2_SetNow();
    AACGM_v2_GetDateTime(&yr,&mo,&dy,&hr,&mt,&sec,&dno);
    if (tval !=-1) {
      dval=TimeYMDHMSToEpoch(yr,mo,dy,0,0,0);
      tval+=dval;
    } else {
      tval=TimeYMDHMSToEpoch(yr,mo,dy,0,0,0);
    }
  }
  TimeEpochToYMDHMS(tval,&yr,&mo,&dy,&hr,&mt,&sc);
  yrsec=TimeYMDHMSToYrsec(yr,mo,dy,hr,mt,sc);

  if (!old_aacgm) {
    AACGM_v2_SetDateTime(yr,mo,dy,hr,mt,(int)sc); /* required */
    if (magflg) AACGM_v2_Lock();
  }

  if (magflg) {
    if (old_aacgm) {
      MapModify(map,AACGMtransform,NULL);
      MapModify(bnd,AACGMtransform,NULL);
    } else {
      MapModify(map,AACGM_v2_transform,NULL);
      MapModify(bnd,AACGM_v2_transform,NULL);
    }
  }

  fov=make_fov(tval,network,alt,chisham); 

  if (magflg) {
    if (old_aacgm) MapModify(fov,AACGMtransform,NULL);
    else           MapModify(fov,AACGM_v2_transform,NULL);
  }


  if (tmtick<1) tmtick=1;
  if (tmtick>6) tmtick=6;

  if ((ortho==0) && (stereo==0)) cylind=1;

  if (grdflg) grd=make_grid(15,10,cylind);   
 
  if (tmkflg) tmk=make_grid(30*tmtick,10,cylind);

  rad=calloc(network->rnum,sizeof(int));

  if (ststr !=NULL) {
    char *tmp;
    tmp=strtok(ststr,",");
    do {
      stid=RadarGetID(network,tmp);
      if (stid !=-1) {
        for (i=0;i<network->rnum;i++) {
          if (network->radar[i].id==stid) {
            if ((lat<0) != (network->radar[i].site[0].geolat<0)) {
              lat=-lat;
            }
            rad[i]=1;
            stnum[st_cnt]=i;
            break;
          }
        }
        if (i==network->rnum) stnum[st_cnt]=0;
        st_cnt++;
      }
    } while ((tmp=strtok(NULL,",")) !=NULL);
  }

  if ((lat<0) && (latmin>0)) latmin=-latmin;
  if ((lat>0) && (latmin<0)) latmin=-latmin;

  if (trmflg || ftrmflg) {
    if ((cylind) || (ortho) || (gvp))
       trm=SZATerminator(yr,mo,dy,hr,mt,sc,0,magflg,
                           1.0,90.0);
     else if (lat>0) trm=SZATerminator(yr,mo,dy,hr,mt,sc,1,magflg,
                                     1.0,90.0);
     else trm=SZATerminator(yr,mo,dy,hr,mt,sc,-1,magflg,1.0,90.0);
  }
 
  marg[0]=lat;
  marg[1]=lon;

  tfunc=MapCylindrical;
  if (ortho) tfunc=MapOrthographic;
  if (stereo) tfunc=MapStereographic;
  if (gvp) tfunc=MapGeneralVerticalPerspective;

  if ((ortho) || (gvp)) marg[2]=sf;
  else if (stereo) marg[2]=1.25*0.5*sf*90.0/(90-fabs(latmin));
  else marg[2]=1;
  marg[3]=flip;

  strcpy(tsfx,"LT");
  if (magflg) strcpy(tsfx,"MLT");
  else if (lstflg) strcpy(tsfx,"LST");

  //dec=SZASolarDec(yr,mo,dy,hr,mt,sc);
  eqt=SZAEqOfTime(yr,mo,dy,hr,mt,sc);

  if (magflg) tme_shft=-(*MLTCnv)(yr,yrsec,0.0)*15.0;
    else {
      if (lstflg) {
        LsoT=(hr*3600+mt*60+sc)+eqt;
        Hangle=15*(LsoT/3600);
        tme_shft=-Hangle;
      } else {
        LT=(hr*3600+mt*60+sc);
        Hangle=15*(LT/3600);
        tme_shft=-Hangle;
      }
    }
  if (lat<0) tme_shft+=180.0;

  if (rotflg)  marg[1]=lon+tme_shft;
  
  if ((cylind) || (sqflg)) clip=MapSquareClip(); 
  else clip=MapCircleClip(10); 

  if (mapflg || fmapflg) {
     nmap=MapTransform(map,2*sizeof(float),PolygonXYbbox,
                           tfunc,marg);
     if (cylind) nmap=wrap(nmap);
     pmap=PolygonClip(clip,nmap); 
  }
  if (bndflg) {
     nbnd=MapTransform(bnd,2*sizeof(float),PolygonXYbbox,
                      tfunc,marg);
     if (cylind) nbnd=wrap(nbnd);
     pbnd=PolygonClip(clip,nbnd);
  }
  if (grdflg) {
     ngrd=MapTransform(grd,2*sizeof(float),PolygonXYbbox,
                      tfunc,marg);
     if (cylind) ngrd=wrap(ngrd);
     pgrd=PolygonClip(clip,ngrd);
  }
  if (trmflg || ftrmflg) {
     ntrm=MapTransform(trm,2*sizeof(float),PolygonXYbbox,
                           tfunc,marg);
     if (cylind) ntrm=wrap(ntrm);
     ptrm=PolygonClip(clip,ntrm); 
  }

  nfov=MapTransform(fov,2*sizeof(float),PolygonXYbbox,
                     tfunc,marg);
  if (cylind) nfov=wrap(nfov);
  pfov=PolygonClip(clip,nfov);

  if (tmkflg) {
     float tmp;
     tmp=marg[1];
     if (rotflg) marg[1]=0;
     else marg[1]=lon-tme_shft;
     ntmk=MapTransform(tmk,2*sizeof(float),PolygonXYbbox,
                        tfunc,marg);
     ptmk=PolygonClip(clip,ntmk);
     marg[1]=tmp;
  }


  if (bgcol_txt !=NULL) bgcol=PlotColorStringRGBA(bgcol_txt);
  if (txtcol_txt !=NULL) txtcol=PlotColorStringRGBA(txtcol_txt);

  if (grdcol_txt !=NULL) grdcol=PlotColorStringRGBA(grdcol_txt);
  if (cstcol_txt !=NULL) cstcol=PlotColorStringRGBA(cstcol_txt);
  if (bndcol_txt !=NULL) bndcol=PlotColorStringRGBA(bndcol_txt);
  if (lndcol_txt !=NULL) lndcol=PlotColorStringRGBA(lndcol_txt);
  if (seacol_txt !=NULL) seacol=PlotColorStringRGBA(seacol_txt);
  if (trmcol_txt !=NULL) trmcol=PlotColorStringRGBA(trmcol_txt);
  if (ftrmcol_txt !=NULL) ftrmcol=PlotColorStringRGBA(ftrmcol_txt);

  if (tmkcol_txt !=NULL) tmkcol=PlotColorStringRGBA(tmkcol_txt);

  if (fovcol_txt !=NULL) fovcol=PlotColorStringRGBA(fovcol_txt);
  if (ofovcol_txt !=NULL) ofovcol=PlotColorStringRGBA(ofovcol_txt);
  if (cfovcol_txt !=NULL) cfovcol=PlotColorStringRGBA(cfovcol_txt);
 
  if (ffovcol_txt !=NULL) ffovcol=PlotColorStringRGBA(ffovcol_txt);
  if (fofovcol_txt !=NULL) fofovcol=PlotColorStringRGBA(fofovcol_txt);
  if (fcfovcol_txt !=NULL) fcfovcol=PlotColorStringRGBA(fcfovcol_txt);


  if ((wdt==0) || (hgt==0)) {
    fprintf(stderr,"invalid plot size.\n");
    exit(-1);
  }


  if (tlblflg) pad=10+fontsize;
  
  if (fontname==NULL) fontname=dfontname;

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
    SplotSetPostScript(splot,psdata,0,xpoff,ypoff);
  }



  PlotDocumentStart(plot,"fov_plot",NULL,wdt,hgt,24);
  PlotPlotStart(plot,"fov_plot",wdt,hgt,24);

  PlotRectangle(plot,NULL,0,0,wdt,hgt,1,bgcol,0x0f,0,NULL);
     
  if (fmapflg) {
      if ((cylind) || (sqflg)) PlotRectangle(plot,NULL,pad,pad,
                               wdt-2*pad,hgt-2*pad,  
                             1,seacol,0x0f,0,NULL);
      PlotEllipse(plot,NULL,wdt/2,hgt/2,wdt/2-pad,hgt/2-pad,
                   1,seacol,0x0f,0,NULL);
  }

  if ((grdflg) && (!grdontop)) {
    MapPlotPolygon(plot,NULL,pad,pad,wdt-2*pad,hgt-2*pad,0,
                                grdcol,0x0f,width,NULL,
                                pgrd,1);
  }

  if (fmapflg) {
    MapPlotPolygon(plot,NULL,pad,pad,wdt-2*pad,hgt-2*pad,1,
                   lndcol,0x0f,0,NULL,pmap,1);
  
    MapPlotPolygon(plot,NULL,pad,pad,wdt-2*pad,hgt-2*pad,1,
                   lndcol,0x0f,0,NULL,pmap,3);
  
    MapPlotPolygon(plot,NULL,pad,pad,wdt-2*pad,hgt-2*pad,1,
                   seacol,0x0f,0,NULL,pmap,0);
  }

  if (ftrmflg) MapPlotPolygon(plot,NULL,pad,pad,wdt-2*pad,hgt-2*pad,
                             1,ftrmcol,0x0f,0,NULL,ptrm,1);

  
  if (fofovflg) {
    for (i=0;i<network->rnum;i++) {
      if (rad[i]==1) continue;
      if (network->radar[i].status !=1) continue;
      MapPlotPolygon(plot,NULL,pad,pad,
                   wdt-2*pad,hgt-2*pad,1,fofovcol,0x0f,0,NULL,pfov,i); 
    }
  }

  if (fcfovflg) {
    for (i=0;i<network->rnum;i++) {
      if (rad[i]==1) continue;
      if (network->radar[i].status!=0) continue;
      MapPlotPolygon(plot,NULL,pad,pad,wdt-2*pad,hgt-2*pad,1,
                       fcfovcol,0x0f,0,NULL,pfov,i); 
    }
  }


   if (ffovflg) {
     for (i=0;i<st_cnt;i++) {
       MapPlotPolygon(plot,NULL,pad,pad,
                      wdt-2*pad,hgt-2*pad,1,ffovcol,0x0f,0,NULL,pfov,stnum[i]);
     }
   }
  

  if (mapflg) {
     MapPlotPolygon(plot,NULL,pad,pad,wdt-2*pad,hgt-2*pad,0,cstcol,0x0f,
                    width,NULL,
                    pmap,1);
  
     MapPlotOpenPolygon(plot,NULL,pad,pad,wdt-2*pad,hgt-2*pad,cstcol,0x0f,
                    width,NULL,
                    pmap,2);
  
     MapPlotPolygon(plot,NULL,pad,pad,wdt-2*pad,hgt-2*pad,0,cstcol,0x0f,
                    width,NULL,
                    pmap,0);
  }

  if (ofovflg)  {
    for (i=0;i<network->rnum;i++) {
      if (rad[i]==1) continue;
      if (network->radar[i].status !=1) continue;
      MapPlotPolygon(plot,NULL,pad,pad,wdt-2*pad,hgt-2*pad,0,
                    ofovcol,0x0f,width,NULL,
                    pfov,i);
    } 
  }

 if (cfovflg)  {
   
    for (i=0;i<network->rnum;i++) {
      if (rad[i]==1) continue;
      if (network->radar[i].status !=0) continue;
      MapPlotPolygon(plot,NULL,pad,pad,wdt-2*pad,hgt-2*pad,0,
                    cfovcol,0x0f,width,NULL,
                    pfov,i);
    } 
  }

 if (dotflg) {
   int s=0;
   float pnt[2];
   double mlat,mlon,r;
   if (cfovflg | fcfovflg)  {
     for (i=0;i<network->rnum;i++) {
       if (rad[i]==1) continue;
       if (network->radar[i].status !=0) continue;
       site=RadarYMDHMSGetSite(&(network->radar[i]),yr,mo,dy,hr,mt,sc);
       if (site == NULL) continue;
       if (magflg) {
         if (old_aacgm) {
           s=AACGMConvert(site->geolat,site->geolon,300,&mlat,&mlon,&r,0);
           pnt[0]=mlat;
           pnt[1]=mlon;
         } else {
           s=AACGM_v2_Convert(site->geolat,site->geolon,300,&mlat,&mlon,&r,0);
           pnt[0]=mlat;
           pnt[1]=mlon;
         }
       } else {
         pnt[0]=site->geolat;
         pnt[1]=site->geolon;
       }
       s=(*tfunc)(sizeof(float)*2,pnt,2*sizeof(float),pnt,marg);
       if (s==0) PlotEllipse(plot,NULL,pad+pnt[0]*(wdt-2*pad),
                    pad+pnt[1]*(hgt-2*pad),dotr,dotr,
                    1,fcfovcol,0x0f,0,NULL);
           
     } 
   }
   if (ofovflg | fofovflg)  {
     for (i=0;i<network->rnum;i++) {
       if (rad[i]==1) continue;
       if (network->radar[i].status !=1) continue;
       site=RadarYMDHMSGetSite(&(network->radar[i]),yr,mo,dy,hr,mt,sc);
       if (site == NULL) continue;
       if (magflg) {
         if (old_aacgm) {
           s=AACGMConvert(site->geolat,site->geolon,300,&mlat,&mlon,&r,0);
           pnt[0]=mlat;
           pnt[1]=mlon;
         } else {
           s=AACGM_v2_Convert(site->geolat,site->geolon,300,&mlat,&mlon,&r,0);
           pnt[0]=mlat;
           pnt[1]=mlon;
         }
       } else {
         pnt[0]=site->geolat;
         pnt[1]=site->geolon;
       }
       s=(*tfunc)(sizeof(float)*2,pnt,2*sizeof(float),pnt,marg);
       if (s==0) PlotEllipse(plot,NULL,pad+pnt[0]*(wdt-2*pad),
                    pad+pnt[1]*(hgt-2*pad),dotr,dotr,
                    1,fofovcol,0x0f,0,NULL);
           
     } 
   }

   if (fovflg) {
     for (i=0;i<st_cnt;i++) {
       site=RadarYMDHMSGetSite(&(network->radar[stnum[i]]),yr,mo,dy,hr,mt,sc);
       if (site !=NULL) {
         if (magflg) {
           if (old_aacgm) {
             s=AACGMConvert(site->geolat,site->geolon,300,&mlat,&mlon,&r,0);
             pnt[0]=mlat;
             pnt[1]=mlon;
           } else {
             s=AACGM_v2_Convert(site->geolat,site->geolon,300,&mlat,&mlon,&r,0);
             pnt[0]=mlat;
             pnt[1]=mlon;
           }
         } else {
           pnt[0]=site->geolat;
           pnt[1]=site->geolon;
         }
         s=(*tfunc)(sizeof(float)*2,pnt,2*sizeof(float),pnt,marg);
         if (s==0) PlotEllipse(plot,NULL,pad+pnt[0]*(wdt-2*pad),
                        pad+pnt[1]*(hgt-2*pad),dotr,dotr,
                        1,ffovcol,0x0f,0,NULL);
       }
     }
   }


 }

  if (fovflg) {
    for (i=0;i<st_cnt;i++) {
      MapPlotPolygon(plot,NULL,pad,pad,wdt-2*pad,hgt-2*pad,0,
                     fovcol,0x0f,width,NULL,
                     pfov,stnum[i]);
    }
  }

  if (bndflg) MapPlotOpenPolygon(plot,NULL,pad,pad,wdt-2*pad,hgt-2*pad,
                                bndcol,0x0f,width,NULL,
                                pbnd,-1);

  if (trmflg) MapPlotOpenPolygon(plot,NULL,pad,pad,wdt-2*pad,hgt-2*pad,
                                trmcol,0x0f,width,NULL,
                                ptrm,1);

  if (tmkflg) MapPlotPolygon(plot,NULL,pad,pad,wdt-2*pad,hgt-2*pad,0,
                                tmkcol,0x0f,width,NULL,
                                ptmk,1);

  if ((grdflg) && (grdontop)) {
    MapPlotPolygon(plot,NULL,pad,pad,wdt-2*pad,hgt-2*pad,0,
                                grdcol,0x0f,width,NULL,
                                pgrd,1);
  }


  if ((cylind) || (sqflg)) 
     PlotRectangle(plot,NULL,pad,pad,wdt-2*pad,hgt-2*pad,
               0,grdcol,0x0f,width,NULL);
  else PlotEllipse(plot,NULL,wdt/2,hgt/2,wdt/2-pad,hgt/2-pad,
               0,grdcol,0x0f,width,NULL);

  if (tlblflg){
      if (lat>0) plot_time_label(plot,pad,pad,wdt-2*pad,hgt-2*pad,
				 90,flip,tsfx,
                                 lon-tme_shft*(! rotflg),
                                 (wdt/2)-pad,6,
                                 txtcol,0x0f,fontname,fontsize,fontdb);
      else plot_time_label(plot,pad,pad,wdt-2*pad,hgt-2*pad,
			   -90,flip,tsfx,
                           lon-tme_shft*(! rotflg),
                           (wdt/2)-pad,6,
                           txtcol,0x0f,fontname,fontsize,fontdb);
  }

  if (tmeflg) plot_time(plot,5,5,wdt-10,hgt-10,tval,
                        txtcol,0x0f,"Helvetica",12.0,fontdb);

  if (magflg) plot_aacgm(plot,4,4,wdt-8,wdt-8,txtcol,0x0f,"Helvetica",
                         7.0,fontdb,old_aacgm);

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
    else if (ppmxflg==1)  FrameBufferSavePPMX(img,stdout);
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


