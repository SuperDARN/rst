/* grid_plot.c
   =========== 
   Author: R.J.Barnes and others

   Issues:
     - Assumes 300 km altitude for AACGM transformations.

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
    2021-03-04 Marina Schmidt chaged delay to 0 as a default
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

#include "rmath.h"
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
#include "rtime.h"
#include "rmap.h"
#include "contour.h"
#include "grplotraw.h"
#include "grplotstd.h"
#include "aacgm.h"
#include "mlt.h"
#include "aacgmlib_v2.h"
#include "mlt_v2.h"

#include "rfile.h"

#include "radar.h" 

#include "griddata.h"
#include "gridread.h"
#include "oldgridread.h"
#include "gridindex.h"
#include "gridseek.h"

#include "key.h"
#include "text_box.h"
#include "plot_time.h"
#include "make_grid.h"
#include "sza.h"
#include "szamap.h"

#include "scandata.h"
#include "geobeam.h"
#include "plot_cell.h"
#include "plot_raw.h"
#include "plot_logo.h"
#include "plot_time_label.h"
#include "plot_vec.h"
#include "make_fov.h"

#include "hlpstr.h"
#include "errstr.h"
#include "version.h"

char *fsfx[]={"xml","ppm","ppmx","png","ps",0};

unsigned char gry[256];

struct key vkey;
struct key xkey;

struct OptionData opt;
struct OptionFile *optf=NULL;

struct PolygonData *map;
struct PolygonData *nmap;
struct PolygonData *pmap;
struct PolygonData *rmap; 

struct PolygonData *bnd;
struct PolygonData *nbnd;
struct PolygonData *pbnd;
struct PolygonData *rbnd;

struct PolygonData *grd;
struct PolygonData *ngrd;
struct PolygonData *pgrd;
struct PolygonData *rgrd;

struct PolygonData *igrd;
struct PolygonData *nigrd;
struct PolygonData *pigrd;
struct PolygonData *rigrd;

struct PolygonData *tmk;
struct PolygonData *ntmk;
struct PolygonData *ptmk;
struct PolygonData *rtmk;

struct PolygonData *fov;
struct PolygonData *nfov;
struct PolygonData *pfov;
struct PolygonData *rfov;

struct PolygonData *trm;
struct PolygonData *ntrm;
struct PolygonData *ptrm;

struct PolygonData *clip;

struct GridData *rgrid;
struct GridData *rgridmrg;
struct GridData *rgridavg;

struct RadarNetwork *network=NULL;

int circle_clip(struct Plot *plot,
                float xoff,float yoff,float wdt,float hgt) {
  int i;
  float sf=0.55;
  float px[12];
  float py[12];
  int t[12];

  px[0]=wdt;
  py[0]=0;
  px[1]=wdt;
  py[1]=hgt*sf;
  px[2]=wdt*sf;
  py[2]=hgt;
  px[3]=0;
  py[3]=hgt;
  px[4]=-wdt*sf;
  py[4]=hgt;
  px[5]=-wdt;
  py[5]=hgt*sf;
  px[6]=-wdt;
  py[6]=0;
  px[7]=-wdt;
  py[7]=-hgt*sf;
  px[8]=-wdt*sf;
  py[8]=-hgt;
  px[9]=0;
  py[9]=-hgt;
  px[10]=wdt*sf;
  py[10]=-hgt;
  px[11]=wdt;
  py[11]=-hgt*sf;

  for (i=0;i<12;i++) {
    t[i]=1;
    px[i]+=xoff;
    py[i]+=yoff;
  }
  return PlotClip(plot,12,px,py,t);
}

int square_clip(struct Plot *plot,
                float xoff,float yoff,float wdt,float hgt) {

  float px[4];
  float py[4];
  int t[4]={0,0,0,0};

  px[0]=xoff;
  py[0]=yoff;
  px[1]=xoff+wdt;
  py[1]=yoff;
  px[2]=xoff+wdt;
  py[2]=yoff+hgt;
  px[3]=xoff;
  py[3]=yoff+hgt;
  return PlotClip(plot,4,px,py,t);
}

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

int rotate(int ssze,void *src,int dsze,void *dst,void *data) {

  float *arg=NULL;
  float *pnt;
  float px,py;
  float rad;
  arg=(float *) data;
  if (arg[0] > 0) rad=arg[1]*PI/180.0;
  else rad=-arg[1]*PI/180;
 
  pnt=(float *) src;
  px=pnt[0];
  py=pnt[1];
  pnt=(float *) dst;
  
  pnt[0]=0.5+(px-0.5)*cos(rad)-(py-0.5)*sin(rad);
  pnt[1]=0.5+(px-0.5)*sin(rad)+(py-0.5)*cos(rad);
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

unsigned int mag_color(double v,void *data) {
  struct key *key;
  int i;

  key=(struct key *) data;
  if (key->num==0) return key->defcol;

  i=key->num*fabs(v)/key->max;
  if (i>=key->num) {
    if (key->num==256) i=key->num-2;
    else i=key->num-1;
  }

  return (key->a[i]<<24) | (key->r[i]<<16) | (key->g[i]<<8) | key->b[i];
}

char *label_vel(double val,double min,double max,void *data) {
  char *txt=NULL;
  if ((val !=max) && (val !=min)) return NULL;
  txt=malloc(32); 
  if (val==max) sprintf(txt,"%g",val);
  if (val==min) sprintf(txt,"%g m/s",val); 
  return txt;
}

char *label_wdt(double val,double min,double max,void *data) {
  char *txt=NULL;
  if ((val !=max) && (val !=min)) return NULL;
  txt=malloc(32); 
  if (val==max) sprintf(txt,"%g",val);
  if (val==min) sprintf(txt,"%g m/s (sw)",val); 
  return txt;
}


char *label_pwr(double val,double min,double max,void *data) {
  char *txt=NULL;
  if ((val !=max) && (val !=min)) return NULL;
  txt=malloc(32); 
  if (val==max) sprintf(txt,"%g",val);
  if (val==min) sprintf(txt,"%g dB (pwr)",val); 
   
  return txt;
}

float find_hemisphere(struct GridData *ptr) {
  int i;
  int h=1;
  for (i=0;i<ptr->vcnum;i++) {
    if (ptr->data[i].mlat<0) h=-1;
    break;
  }
  return h;
}

int rst_opterr(char *txt) {
  fprintf(stderr,"Option not recognized: %s\n",txt);
  fprintf(stderr,"Please try: grid_plot --help\n");
  return(-1);
}

int main(int argc,char *argv[]) {

  int old=0;
  int old_aacgm=0;

  char filename[256];
  FILE *outfp=NULL;
  unsigned char dnflg=0;
  unsigned char tnflg=0;
  unsigned char mnflg=0;
  unsigned char stdioflg=0;

  char *sfx=NULL;
  int cnt=0;

  int s=0;

#ifdef _XLIB_
  int xdf=0;
  int xd=0;
  struct XwinDisplay *dp=NULL;
  struct XwinWindow *win=NULL;
  char *display_name=NULL;
  char *wname="grid_plot";
  int xdoff=-1;
  int ydoff=-1;
  struct timeval tmout;
  float delay=0;
  int xstat=0;
#endif

  struct RfileIndex *oinx=NULL;
  struct GridIndex *inx=NULL;
  struct FrameBuffer *img=NULL;
  struct PostScript *psdata=NULL;
  FILE *fontfp=NULL;
  char *fntdbfname=NULL;
  struct FrameBufferFontDB *fontdb=NULL;
  unsigned int bgcol;
  unsigned int txtcol;

  int arg;
  char *fname=NULL;
  char *envstr=NULL;
  char *cfname=NULL;
  FILE *fp;

  float wdt=540,hgt=540;
  float pad=-1;

  float khgt=80;
  float kwdt=44;
  float apad=22;
  float px=2;

  struct Splot *splot=NULL;
  struct Plot *plot=NULL;

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
  char *vkey_path=NULL;
  char *xkey_path=NULL;
  char kname[256];
  char *vkey_fname=NULL;
  char *xkey_fname=NULL;
  FILE *keyfp=NULL;
  size_t len;

  MapTFunction  tfunc;

  unsigned char sqflg=0;

  unsigned char flip=0;
  unsigned char ortho=0;
  int hemisphere=1;
  float lat=1e10,lon=0;
  float latmin=50.0;
  float sf=1.0;

  unsigned char magflg=0;
  unsigned char rotflg=0;

  unsigned char mapflg=0;
  unsigned char fmapflg=0;
  unsigned char bndflg=0;
  unsigned char grdflg=0;
  unsigned char igrdflg=0;
  unsigned char tmkflg=0;

  int tmtick=3;

  unsigned char lstflg=0;

  unsigned char trmflg=0;
  unsigned char ftrmflg=0;

  unsigned char defflg=0;

  char *grdcol_txt=NULL;
  char *igrdcol_txt=NULL;
  char *cstcol_txt=NULL;
  char *bndcol_txt=NULL;
  char *lndcol_txt=NULL;
  char *seacol_txt=NULL;
  char *ftrmcol_txt=NULL;
  char *trmcol_txt=NULL;
  char *tmkcol_txt=NULL;

  char *veccol_txt=NULL;
  char *fovcol_txt=NULL;
  char *ffovcol_txt=NULL;

  unsigned int grdcol;
  unsigned int igrdcol;
  unsigned int cstcol;
  unsigned int bndcol;
  unsigned int lndcol;
  unsigned int seacol;
  unsigned int trmcol;
  unsigned int ftrmcol;
  unsigned int tmkcol;

  unsigned int veccol;
  unsigned int fovcol;
  unsigned int ffovcol;

  FILE *mapfp;
  float marg[6];
  int i;

  int flg=0;

  char *stmestr=NULL;
  char *etmestr=NULL;
  char *sdtestr=NULL;
  char *edtestr=NULL;
  char *exstr=NULL;

  double sdte=-1;
  double edte=-1;

  double ssec=-1; /* specified start time */
  double esec=-1; /* end time */
  double exsec=0;

  int yr,mo,dy,hr,mt;
  double sc;
  int yrsec;
  double tval=0;
  float tme_shft;

  float grdlat=10;
  float grdlon=15;

  float igrdlat=10;
  float igrdlon=15;

  unsigned char grdtop=0;
  unsigned char igrdtop=0;

  unsigned char vkeyflg=0;
  unsigned char xkeyflg=0;

  unsigned char vecflg=0;  

  unsigned char rawflg=0;
  unsigned char mrgflg=0;

  unsigned char fovflg=0;
  unsigned char ffovflg=0;
  unsigned char gfovflg=0;
  unsigned char pwrflg=0;
  unsigned char wdtflg=0;

  unsigned char tlblflg=0;
  unsigned char logoflg=0;
  unsigned char tmeflg=0;

  unsigned char maxflg=0;
  unsigned char minflg=0;
  unsigned char avgflg=0;

  unsigned char avflg=0;
  int aval=0;
 
  unsigned char celflg=0;
  int cprm=0;

  double pmax=30;
  double wmax=500;
  double vmax=1000;
  float vsf=2.0;
  float vradius=2.0;

  unsigned char poleflg=0;
 
  unsigned char frmflg=0; 
  unsigned char ovrflg=0; 

  float lnewdt=0.5;

  char tsfx[16];

  int chisham=0;  
  
  /* function pointers for file reading (old and new) and MLT */
  int (*Grid_Read)(FILE *, struct GridData *);
  double (*MLTCnv)(int, int, double);

  rgrid=GridMake();
  rgridmrg=GridMake();
  rgridavg=GridMake();

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

  envstr=getenv("MAPDATA");

  mapfp=fopen(envstr,"r");
  map=MapFread(mapfp);
  fclose(mapfp);   

  envstr=getenv("BNDDATA");
  mapfp=fopen(envstr,"r");
  bnd=MapBndFread(mapfp);
  fclose(mapfp);

  for (i=0;i<256;i++) gry[i]=i;

  bgcol=PlotColor(0xff,0xff,0xff,0xff);
  txtcol=PlotColor(0x00,0x00,0x00,0xff);

  grdcol=PlotColor(0xc0,0xc0,0xc0,0xff);
  igrdcol=PlotColor(0xc0,0xc0,0xc0,0xff);
  cstcol=PlotColor(0xa0,0xa0,0xa0,0xff);
  bndcol=PlotColor(0x80,0x80,0x80,0xff);
  lndcol=PlotColor(0xff,0xff,0xf0,0xff);
  seacol=PlotColor(0xe0,0xf0,0xff,0xff);
  trmcol=PlotColor(0xc0,0xc0,0xc0,0xff);
  ftrmcol=PlotColor(0xe0,0xe0,0xe0,0xff);

  tmkcol=PlotColor(0x00,0x00,0x00,0xff);

  veccol=PlotColor(0x00,0x00,0x00,0xff);
  fovcol=PlotColor(0x00,0x00,0x00,0xff);
  ffovcol=PlotColor(0xc0,0xc0,0xc0,0xff);
 
  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"-option",'x',&option);
  OptionAdd(&opt,"-version",'x',&version);

  OptionAdd(&opt,"old",'x',&old);
  OptionAdd(&opt,"old_aacgm",'x',&old_aacgm);

  OptionAdd(&opt,"cf",'t',&cfname);

#ifdef _XLIB_ 
  OptionAdd(&opt,"x",'x',&xd);
  OptionAdd(&opt,"display",'t',&display_name);
  OptionAdd(&opt,"xoff",'i',&xdoff);
  OptionAdd(&opt,"yoff",'i',&ydoff);
  OptionAdd(&opt,"delay",'f',&delay);
#endif

  OptionAdd(&opt,"ppm",'x',&ppmflg);
  OptionAdd(&opt,"ppmx",'x',&ppmxflg);
  OptionAdd(&opt,"xml",'x',&xmlflg);
  OptionAdd(&opt,"png",'x',&pngflg);

  OptionAdd(&opt,"ps",'x',&psflg); 

  OptionAdd(&opt,"tn",'x',&tnflg);
  OptionAdd(&opt,"dn",'x',&dnflg); 
  OptionAdd(&opt,"mn",'x',&mnflg); 

  OptionAdd(&opt,"stdout",'x',&stdioflg); 

  OptionAdd(&opt,"xp",'f',&xpoff);
  OptionAdd(&opt,"yp",'f',&ypoff);
  OptionAdd(&opt,"wdt",'f',&wdt);
  OptionAdd(&opt,"hgt",'f',&hgt);
  OptionAdd(&opt,"pad",'f',&pad);
  OptionAdd(&opt,"lnewdt",'f',&lnewdt);

  OptionAdd(&opt,"st",'t',&stmestr);
  OptionAdd(&opt,"et",'t',&etmestr);
  OptionAdd(&opt,"sd",'t',&sdtestr);
  OptionAdd(&opt,"ed",'t',&edtestr);
  OptionAdd(&opt,"ex",'t',&exstr);

  OptionAdd(&opt,"t",'t',&stmestr);
  OptionAdd(&opt,"d",'t',&sdtestr);

  OptionAdd(&opt,"bgcol",'t',&bgcol_txt);
  OptionAdd(&opt,"txtcol",'t',&txtcol_txt);
  OptionAdd(&opt,"vkey",'t',&vkey_fname);
  OptionAdd(&opt,"vkey_path",'t',&vkey_path);
  OptionAdd(&opt,"xkey",'t',&xkey_fname);
  OptionAdd(&opt,"xkey_path",'t',&xkey_path);

  OptionAdd(&opt,"square",'x',&sqflg);

  OptionAdd(&opt,"ortho",'x',&ortho);
  OptionAdd(&opt,"lat",'f',&lat);
  OptionAdd(&opt,"lon",'f',&lon);
  OptionAdd(&opt,"latmin",'f',&latmin);
  OptionAdd(&opt,"sf",'f',&sf);
  OptionAdd(&opt,"mag",'x',&magflg);
  OptionAdd(&opt,"rotate",'x',&rotflg);
  OptionAdd(&opt,"flip",'x',&flip);

  OptionAdd(&opt,"coast",'x',&mapflg);
  OptionAdd(&opt,"fcoast",'x',&fmapflg);
  OptionAdd(&opt,"bnd",'x',&bndflg);
  OptionAdd(&opt,"grd",'x',&grdflg);
  OptionAdd(&opt,"grdlat",'f',&grdlat);
  OptionAdd(&opt,"grdlon",'f',&grdlon);

  OptionAdd(&opt,"igrd",'x',&igrdflg);
  OptionAdd(&opt,"igrdlat",'f',&igrdlat);
  OptionAdd(&opt,"igrdlon",'f',&igrdlon);

  OptionAdd(&opt,"grdontop",'x',&grdtop);
  OptionAdd(&opt,"igrdontop",'x',&igrdtop);

  OptionAdd(&opt,"tmk",'x',&tmkflg);

  OptionAdd(&opt,"tmtick",'i',&tmtick);

  OptionAdd(&opt,"lst",'x',&lstflg);

  OptionAdd(&opt,"term",'x',&trmflg);
  OptionAdd(&opt,"fterm",'x',&ftrmflg);

  OptionAdd(&opt,"grdcol",'t',&grdcol_txt);
  OptionAdd(&opt,"igrdcol",'t',&igrdcol_txt);
  OptionAdd(&opt,"cstcol",'t',&cstcol_txt);
  OptionAdd(&opt,"bndcol",'t',&bndcol_txt);
  OptionAdd(&opt,"lndcol",'t',&lndcol_txt);
  OptionAdd(&opt,"seacol",'t',&seacol_txt);
  OptionAdd(&opt,"trmcol",'t',&trmcol_txt);
  OptionAdd(&opt,"ftrmcol",'t',&ftrmcol_txt);

  OptionAdd(&opt,"tmkcol",'t',&tmkcol_txt);

  OptionAdd(&opt,"veccol",'t',&veccol_txt);
  OptionAdd(&opt,"fovcol",'t',&fovcol_txt);
  OptionAdd(&opt,"ffovcol",'t',&ffovcol_txt);

  OptionAdd(&opt,"raw",'x',&rawflg);
  OptionAdd(&opt,"mrg",'x',&mrgflg);

  OptionAdd(&opt,"fov",'x',&fovflg);
  OptionAdd(&opt,"ffov",'x',&ffovflg);
  OptionAdd(&opt,"gfov",'x',&gfovflg);

  OptionAdd(&opt,"pwr",'x',&pwrflg);
  OptionAdd(&opt,"swd",'x',&wdtflg);
  
  OptionAdd(&opt,"avg",'x',&avgflg);
  OptionAdd(&opt,"max",'x',&maxflg);
  OptionAdd(&opt,"min",'x',&minflg);
 
  OptionAdd(&opt,"vkeyp",'x',&vkeyflg);
  OptionAdd(&opt,"xkeyp",'x',&xkeyflg);

  OptionAdd(&opt,"vecp",'x',&vecflg);
  OptionAdd(&opt,"vsf",'f',&vsf);
  OptionAdd(&opt,"vrad",'f',&vradius);
 
  OptionAdd(&opt,"tmlbl",'x',&tlblflg);
  OptionAdd(&opt,"logo",'x',&logoflg);
  OptionAdd(&opt,"time",'x',&tmeflg);

  OptionAdd(&opt,"vmax",'d',&vmax);
  OptionAdd(&opt,"pmax",'d',&pmax);
  OptionAdd(&opt,"wmax",'d',&wmax);

  OptionAdd(&opt,"frame",'x',&frmflg);
  OptionAdd(&opt,"over",'x',&ovrflg);

  OptionAdd(&opt,"def",'x',&defflg);

  OptionAdd(&opt,"chisham",'x',&chisham); /* Data mapped using Chisham virtual height model */

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

  if (arg==argc) {
    OptionPrintInfo(stderr,errstr);
    exit(-1);
  }

  fname=argv[arg];
  if (old) {
    if (argc-arg>1) {
      fp=fopen(argv[arg+1],"r");
      if (fp !=NULL) { 
         oinx=RfileLoadIndex(fp);
         fclose(fp);
      }
    }
  } else {
    if (argc-arg>1) {
      fp=fopen(argv[arg+1],"r");
      if (fp !=NULL) { 
         inx=GridIndexFload(fp);
         fclose(fp);
      }
    } 
  }

  if (defflg) { /* set default plot */
    magflg=1;
    rotflg=1;
    rawflg=1;
    fmapflg=1;
    grdflg=1;
    grdtop=1;
    vkey_fname="color.key";
    vkeyflg=1;
    vecflg=1;
    tmeflg=1;
  }

  if (exstr !=NULL) exsec=strtime(exstr);
  if (stmestr !=NULL) ssec=strtime(stmestr);
  if (etmestr !=NULL) esec=strtime(etmestr);
  if (sdtestr !=NULL) sdte=strdate(sdtestr);
  if (edtestr !=NULL) edte=strdate(edtestr);

  fp=fopen(fname,"r");

  if (fp==NULL) {
    fprintf(stderr,"Error opening grid file: %s\n",fname);
    exit(-1);
  }
  
  if (delay == 0){
      fprintf(stderr, "Click to view next plot. Ctrl+c to exit. Use -delay option to cycle through all frames sequentially.\n");
  }


  if (magflg && old_aacgm) magflg = 2; /* set to 2 for old AACGM */

  /* set function pointer to compute MLT or MLT_v2 */
  if (old_aacgm) MLTCnv = &MLTConvertYrsec;
  else           MLTCnv = &MLTConvertYrsec_v2;

  if (old) Grid_Read = &OldGridFread;
  else     Grid_Read = &GridFread;
  s = (*Grid_Read)(fp,rgrid);

  hemisphere=find_hemisphere(rgrid);

  if (s !=-1) {
    if (ssec==-1) ssec=((int) rgrid->st_time % (24*3600));
    if (sdte==-1) ssec+=rgrid->st_time - ((int) rgrid->st_time % (24*3600));
    else ssec+=sdte;
    if (esec !=-1) {
      if (edte==-1) esec+=rgrid->st_time - ((int) rgrid->st_time % (24*3600));
      else esec+=edte;
    }
    if (exsec !=0) esec=ssec+exsec;
  }
  TimeEpochToYMDHMS(ssec,&yr,&mo,&dy,&hr,&mt,&sc);
  if (rgrid->st_time<ssec) {
    if (old) s = OldGridFseek(fp,yr,mo,dy,hr,mt,(int) sc,oinx,NULL);
    else     s =    GridFseek(fp,yr,mo,dy,hr,mt,(int) sc,NULL,inx);
    s = (*Grid_Read)(fp,rgrid);
  }
 
  if (!old_aacgm) {
    AACGM_v2_SetDateTime(yr,mo,dy,hr,mt,(int)sc); /* required */
    if (magflg) AACGM_v2_Lock();
  }

  if (!sqflg) clip=MapCircleClip(10);
  else clip=MapSquareClip();

  if (lat>90) lat=90*hemisphere;

  if ((lat<0) && (latmin>0)) latmin=-latmin;
  if ((lat>0) && (latmin<0)) latmin=-latmin;

  if ((fovflg || ffovflg) && !gfovflg) {
    fov=make_grid_fov(rgrid->st_time,network,chisham,old_aacgm);
    if (!magflg) {
      if (old_aacgm) MapModify(fov,AACGMtransform,&flg);
      else           MapModify(fov,AACGM_v2_transform,&flg);
    }
  }

  if (tmtick<1) tmtick=1;
  if (tmtick>6) tmtick=6;

  if (grdflg) grd=make_grid(grdlon,grdlat,0);   
  if (igrdflg) igrd=make_grid(igrdlon,igrdlat,0);   

  if (tmkflg) tmk=make_grid(30*tmtick,10,0);

  if (magflg) {
    if (old_aacgm) {
      MapModify(map,AACGMtransform,NULL);
      MapModify(bnd,AACGMtransform,NULL);
    } else {
      MapModify(map,AACGM_v2_transform,NULL);
      MapModify(bnd,AACGM_v2_transform,NULL);
    }
    if (igrdflg) {
      if (old_aacgm) MapModify(igrd,AACGMtransform,NULL);
      else           MapModify(igrd,AACGM_v2_transform,NULL);
    }
  } else {
    if (igrdflg) {
      if (old_aacgm) MapModify(igrd,AACGMtransform,marg);
      else           MapModify(igrd,AACGM_v2_transform,marg);
    }
  }

  marg[0]=lat;
  marg[1]=0;
  if (ortho) marg[2]=sf;
  else marg[2]=1.25*0.5*sf*90.0/(90-fabs(latmin));
  marg[3]=flip;
  marg[4]=0.0;
  marg[5]=0.0;

  tfunc=MapStereographic;
  if (ortho) tfunc=MapOrthographic;

  strcpy(tsfx,"LT");
  if (magflg) strcpy(tsfx,"MLT");
  else if (lstflg) strcpy(tsfx,"LST");

  if (lat==90) poleflg=1;
  if (lat==-90) poleflg=1;

  if (poleflg) {
    if (mapflg || fmapflg) {
      nmap=MapTransform(map,2*sizeof(float),PolygonXYbbox,tfunc,marg);
      pmap=PolygonClip(clip,nmap); 
      PolygonFree(map);
      PolygonFree(nmap);
    }
    if (bndflg) {
       nbnd=MapTransform(bnd,2*sizeof(float),PolygonXYbbox,tfunc,marg);
       pbnd=PolygonClip(clip,nbnd);
       PolygonFree(bnd);
       PolygonFree(nbnd);
    }
    if (grdflg) {
       ngrd=MapTransform(grd,2*sizeof(float),PolygonXYbbox,tfunc,marg);
       pgrd=PolygonClip(clip,ngrd);
       PolygonFree(grd);
       PolygonFree(ngrd);
    }

    if (igrdflg) {
       nigrd=MapTransform(igrd,2*sizeof(float),PolygonXYbbox,tfunc,marg);
       pigrd=PolygonClip(clip,nigrd);
       PolygonFree(igrd);
       PolygonFree(nigrd);
    }

    if ((fovflg || ffovflg) && !gfovflg) {
       nfov=MapTransform(fov,2*sizeof(float),PolygonXYbbox,tfunc,marg);
       pfov=PolygonClip(clip,nfov);
       PolygonFree(fov);
       PolygonFree(nfov);
    }
    if (tmkflg) {
       ntmk=MapTransform(tmk,2*sizeof(float),PolygonXYbbox,tfunc,marg);
       ptmk=PolygonClip(clip,ntmk);
       PolygonFree(tmk);
       PolygonFree(ntmk);
    }
  }
   
   
  if (bgcol_txt !=NULL) bgcol=PlotColorStringRGBA(bgcol_txt);
  if (txtcol_txt !=NULL) txtcol=PlotColorStringRGBA(txtcol_txt);

  if (grdcol_txt !=NULL) grdcol=PlotColorStringRGBA(grdcol_txt);
  if (igrdcol_txt !=NULL) igrdcol=PlotColorStringRGBA(igrdcol_txt);
  if (cstcol_txt !=NULL) cstcol=PlotColorStringRGBA(cstcol_txt);
  if (bndcol_txt !=NULL) bndcol=PlotColorStringRGBA(bndcol_txt);
  if (lndcol_txt !=NULL) lndcol=PlotColorStringRGBA(lndcol_txt);
  if (seacol_txt !=NULL) seacol=PlotColorStringRGBA(seacol_txt);
  if (trmcol_txt !=NULL) trmcol=PlotColorStringRGBA(trmcol_txt);
  if (ftrmcol_txt !=NULL) ftrmcol=PlotColorStringRGBA(ftrmcol_txt);

  if (tmkcol_txt !=NULL) tmkcol=PlotColorStringRGBA(tmkcol_txt);
  if (veccol_txt !=NULL) veccol=PlotColorStringRGBA(veccol_txt);
  if (fovcol_txt !=NULL) fovcol=PlotColorStringRGBA(fovcol_txt);
  if (ffovcol_txt !=NULL) ffovcol=PlotColorStringRGBA(ffovcol_txt);

  if (vkey_fname !=NULL) {
    if (vkey_path == NULL) vkey_path = getenv("COLOR_TABLE_PATH");
    if (vkey_path != NULL) {
      strcpy(kname, vkey_path);
      len = strlen(vkey_path);
      if (vkey_path[len-1] != '/') strcat(kname, "/");
      strcat(kname, vkey_fname);
    } else {
      fprintf(stderr, "No COLOR_TABLE_PATH set\n");
    }
    keyfp=fopen(kname,"r");
    if (keyfp !=NULL) {
      load_key(keyfp,&vkey);
      fclose(keyfp);
    } else {
      fprintf(stderr, "Velocity color table %s not found\n", kname);
    }
  }
  vkey.max=vmax;
  vkey.defcol=veccol;

  if (xkey_fname !=NULL) {
    if (xkey_path == NULL) xkey_path = getenv("COLOR_TABLE_PATH");
    if (xkey_path != NULL) {
      strcpy(kname, xkey_path);
      len = strlen(xkey_path);
      if (xkey_path[len-1] != '/') strcat(kname, "/");
      strcat(kname, xkey_fname);
    } else {
      fprintf(stderr, "No COLOR_TABLE_PATH set\n");
    }
    keyfp=fopen(kname,"r");
    if (keyfp !=NULL) {
      load_key(keyfp,&xkey);
      fclose(keyfp);
    } else {
      fprintf(stderr, "Extra color table %s not found\n", kname);
    }
  }

  if (pwrflg) xkey.max=pmax;
  else xkey.max=wmax;
 
  if (pwrflg || wdtflg) {
     celflg=1;
     if (pwrflg) cprm=0;
     else cprm=2;
  }

  if (avgflg || maxflg || minflg) {
     avflg=1;
     if (maxflg==1) aval=1;
     if (minflg==1) aval=4;
  }

  if ((wdt==0) || (hgt==0)) {
    fprintf(stderr,"invalid plot size.\n");
    exit(-1);
  }

  if (pad==-1) {
    if (tlblflg) pad=22;
    if (wdt>400) {
       if (tlblflg==0) pad=12;
       else pad=20;
       apad=30;
    } else pad=6;
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
    SplotSetPostScript(splot,psdata,0,xpoff,ypoff);
  }

  sfx=fsfx[0];
  if (gflg) {
    if (xmlflg) sfx=fsfx[0];
    else if (ppmflg) sfx=fsfx[1];
    else if (ppmxflg) sfx=fsfx[2];
    else sfx=fsfx[3];
  }
  if (pflg) sfx=fsfx[4];
  

#ifdef _XLIB_
  if (xd !=0) {
    dp=XwinOpenDisplay(display_name,&xdf);
 
    if (dp==NULL) {
      fprintf(stderr,"Could not open display.\n");
      exit(-1);
    }

    if (xdoff==-1) xdoff=(dp->wdt-wdt)/2;
    if (ydoff==-1) ydoff=(dp->hgt-hgt)/2;

    win=XwinMakeWindow(xdoff,ydoff,wdt,hgt,0, dp,wname,
                       wname,argv[0],wname,argc,argv,&xdf);
    if (win==NULL) {
      fprintf(stderr,"Could not create window.\n");
      exit(-1);
    }
    XwinShowWindow(win);
  }
  #endif

  do {

    if (mnflg) tval=(rgrid->st_time+rgrid->ed_time)/2;
    else tval=rgrid->st_time;

    TimeEpochToYMDHMS(tval,&yr,&mo,&dy,&hr,&mt,&sc);
    yrsec=TimeYMDHMSToYrsec(yr,mo,dy,hr,mt,sc);

    if (stdioflg) {
      sprintf(filename,"%.4d%.2d%.2d.%.2d%.2d.%.2d.rp",
                       yr,mo,dy,hr,mt,(int) sc);
      if (pflg) PostScriptSetText(psdata,stream,stdout);
    } else {
      fprintf(stderr,"%d-%d-%d %d:%d:%d\n",dy,mo,yr,hr,mt,(int) sc);
      if (tnflg) sprintf(filename,"%.2d%.2d.%.2d.%s",hr,mt,(int) sc,sfx); 
      else if (dnflg) sprintf(filename,"%.4d%.2d%.2d.%.2d%.2d.%.2d.%s",
                       yr,mo,dy,hr,mt,(int) sc,sfx);
      else sprintf(filename,"%.4d.%s",cnt,sfx);

      if (pflg) {
        outfp=fopen(filename,"w");
        PostScriptSetText(psdata,stream,outfp);
      }
    }
    if (mrgflg) GridMerge(rgrid,rgridmrg);
    if (avflg) GridAverage(rgrid,rgridavg,aval+cprm*(aval !=0)); 

    if ((fovflg || ffovflg) && gfovflg) {
      fov=make_grid_fov_data(rgrid,network,chisham,old_aacgm);
      if (!magflg) {
        if (old_aacgm) MapModify(fov,AACGMtransform,&flg);
        else           MapModify(fov,AACGM_v2_transform,&flg);
      }
      if (poleflg) {
        marg[0]=lat;
        marg[1]=0;
        if (ortho) marg[2]=sf;
        else marg[2]=1.25*0.5*sf*90.0/(90-fabs(latmin));
        marg[3]=flip;
        nfov=MapTransform(fov,2*sizeof(float),PolygonXYbbox,tfunc,marg);
        pfov=PolygonClip(clip,nfov);
        PolygonFree(fov);
        PolygonFree(nfov);
      }
    }

    if (trmflg || ftrmflg) {
      if (lat>0) trm=SZATerminator(yr,mo,dy,hr,mt,sc,1,magflg, 1.0,90.0);
      if (lat<0) trm=SZATerminator(yr,mo,dy,hr,mt,sc,-1,magflg, 1.0,90.0);
    }

    if (magflg) tme_shft=-(*MLTCnv)(yr,yrsec,0.0)*15.0; 
    else {
      double eqt,LsoT,LT,Hangle;
      if (lstflg) {
        eqt=SZAEqOfTime(yr,mo,dy,hr,mt,sc);
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
    if (rotflg) marg[1]=lon+tme_shft;
    else marg[1]=lon;
    if (poleflg) {
      if ((rotflg) && (flip)) marg[1]=-lon-tme_shft;
      if (pmap !=NULL)
        rmap=MapTransform(pmap,2*sizeof(float),PolygonXYbbox,rotate,marg);
      if (pbnd !=NULL)
        rbnd=MapTransform(pbnd,2*sizeof(float),PolygonXYbbox,rotate,marg);
      if (pgrd !=NULL)
        rgrd=MapTransform(pgrd,2*sizeof(float),PolygonXYbbox,rotate,marg);
      if (pigrd !=NULL)
       rigrd=MapTransform(pigrd,2*sizeof(float),PolygonXYbbox,rotate,marg);
      if (pfov !=NULL) {
        rfov=MapTransform(pfov,2*sizeof(float),PolygonXYbbox,rotate,marg);
        if (gfovflg) PolygonFree(pfov);
      }
      if (ptmk !=NULL) {
        if (rotflg) marg[1]=0;
        else marg[1]=lon-tme_shft;
        rtmk=MapTransform(ptmk,2*sizeof(float),PolygonXYbbox,rotate,marg);
        if (rotflg) marg[1]=lon+tme_shft;
        else marg[1]=lon;
      }
      if ((rotflg) && (flip)) marg[1]=lon+tme_shft;
    } else {
      if (mapflg || fmapflg) {
        nmap=MapTransform(map,2*sizeof(float),PolygonXYbbox, tfunc,marg);
        rmap=PolygonClip(clip,nmap);
        PolygonFree(nmap);
      }
      if (bndflg) {
        nbnd=MapTransform(bnd,2*sizeof(float),PolygonXYbbox, tfunc,marg);
        rbnd=PolygonClip(clip,nbnd);
        PolygonFree(nbnd);
      }
      if (grdflg) {
        ngrd=MapTransform(grd,2*sizeof(float),PolygonXYbbox, tfunc,marg);
        rgrd=PolygonClip(clip,ngrd);
        PolygonFree(ngrd);
      }
      if (igrdflg) {
        nigrd=MapTransform(igrd,2*sizeof(float),PolygonXYbbox, tfunc,marg);
        rigrd=PolygonClip(clip,nigrd);
        PolygonFree(nigrd);
      }
      if (fovflg || ffovflg) {
        nfov=MapTransform(fov,2*sizeof(float),PolygonXYbbox, tfunc,marg);
        rfov=PolygonClip(clip,nfov);
        PolygonFree(nfov);
      }
      if (tmkflg) {
        if (rotflg) marg[1]=0;
        else marg[1]=lon-tme_shft;
        ntmk=MapTransform(tmk,2*sizeof(float),PolygonXYbbox, tfunc,marg);
        rtmk=PolygonClip(clip,ntmk);
        PolygonFree(ntmk);
        if (rotflg) marg[1]=lon+tme_shft;
        else marg[1]=lon;
      }
    }
    if (trmflg || ftrmflg) {
       ntrm=MapTransform(trm,2*sizeof(float),PolygonXYbbox, tfunc,marg);
       ptrm=PolygonClip(clip,ntrm);
       PolygonFree(ntrm);
       PolygonFree(trm);
       trm=NULL;
       ntrm=NULL;
    }

    PlotDocumentStart(plot,filename,NULL,wdt,hgt,24);
    PlotPlotStart(plot,filename,wdt,hgt,24);
 
    if (!ovrflg) PlotRectangle(plot,NULL,0,0,wdt,hgt,1,bgcol,0x0f,0,NULL);
    if (!sqflg) circle_clip(plot,wdt/2,hgt/2,wdt/2-pad,hgt/2-pad);
    else square_clip(plot,pad,pad,wdt-2*pad,hgt-2*pad);

    if (fmapflg) {
      if (!sqflg) PlotEllipse(plot,NULL,wdt/2,hgt/2,wdt/2-pad,hgt/2-pad,
                 1,seacol,0x0f,0,NULL);
      else PlotRectangle(plot,NULL,pad,pad,wdt-2*pad,hgt-2*pad,
                 1,seacol,0x0f,0,NULL);
    }

    if ((grdflg) && (grdtop==0)) {
       MapPlotPolygon(plot,NULL,pad,pad,wdt-2*pad,hgt-2*pad,0,
                                grdcol,0x0f,0.5,NULL,
                                rgrd,1);
    }

    if ((igrdflg) && (igrdtop==0)) {
       MapPlotPolygon(plot,NULL,pad,pad,wdt-2*pad,hgt-2*pad,0,
                                igrdcol,0x0f,0.5,NULL,
                                rigrd,1);
    }

    if (fmapflg) {
      MapPlotPolygon(plot,NULL,pad,pad,wdt-2*pad,hgt-2*pad,
                     1,lndcol,0x0f,0,NULL,rmap,1);

      MapPlotPolygon(plot,NULL,pad,pad,wdt-2*pad,hgt-2*pad,
                     1,lndcol,0x0f,0,NULL,rmap,3);
  
      MapPlotPolygon(plot,NULL,pad,pad,wdt-2*pad,hgt-2*pad,
                     1,seacol,0x0f,0,NULL,rmap,0);
    }
     
    if (ftrmflg) 
       MapPlotPolygon(plot,NULL,pad,pad,wdt-2*pad,hgt-2*pad,1,
                               ftrmcol,0x0f,0.5,NULL,
                               ptrm,1);

    if (ffovflg) MapPlotPolygon(plot,NULL,pad,pad,wdt-2*pad,hgt-2*pad,1,
			       ffovcol,0x0f,0.5,NULL,
			       rfov,1);
 
    if (celflg) {
      if (avflg) 
        plot_grid_cell(plot,rgridavg,latmin,magflg,pad,pad,wdt-2*pad,
                       hgt-2*pad,tfunc,marg,mag_color,&xkey,cprm,old_aacgm);
      else 
        plot_grid_cell(plot,rgrid,latmin,magflg,pad,pad,wdt-2*pad,
                       hgt-2*pad,tfunc,marg,mag_color,&xkey,cprm,old_aacgm);
    }

    if (rawflg) 
        plot_raw(plot,rgrid,latmin,magflg,pad,pad,wdt-2*pad,hgt-2*pad,
               vsf,vradius,tfunc,marg,mag_color,&vkey,lnewdt,old_aacgm);

    if (mrgflg) 
        plot_raw(plot,rgridmrg,latmin,magflg,
                 pad,pad,wdt-2*pad,hgt-2*pad,
                 vsf,vradius,tfunc,marg,mag_color,&vkey,lnewdt,old_aacgm);

    if (mapflg) {
       MapPlotPolygon(plot,NULL,pad,pad,wdt-2*pad,hgt-2*pad,0,cstcol,0x0f,
                    lnewdt,NULL,
                    rmap,1);
  
       MapPlotPolygon(plot,NULL,pad,pad,wdt-2*pad,hgt-2*pad,0,cstcol,0x0f,
                    lnewdt,NULL,
                    rmap,0);

       MapPlotOpenPolygon(plot,NULL,pad,pad,wdt-2*pad,hgt-2*pad,cstcol,0x0f,
                    lnewdt,NULL,
                    rmap,2);

    }

    if (bndflg) MapPlotOpenPolygon(plot,NULL,pad,pad,wdt-2*pad,hgt-2*pad,
                                bndcol,0x0f,lnewdt,NULL,
                                rbnd,-1);
  
    if (fovflg) MapPlotPolygon(plot,NULL,pad,pad,wdt-2*pad,hgt-2*pad,0,
                             fovcol,0x0f,lnewdt,NULL,
                             rfov,1);

    if (trmflg)
      MapPlotPolygon(plot,NULL,pad,pad,wdt-2*pad,hgt-2*pad,0,
		     trmcol,0x0f,lnewdt,NULL,
		     ptrm,1);

   if ((grdflg) && (grdtop==1)) {
       MapPlotPolygon(plot,NULL,pad,pad,wdt-2*pad,hgt-2*pad,0,
                                grdcol,0x0f,0.5,NULL,
                                rgrd,1);
    }

    if ((igrdflg) && (igrdtop==1)) {
       MapPlotPolygon(plot,NULL,pad,pad,wdt-2*pad,hgt-2*pad,0,
                                igrdcol,0x0f,0.5,NULL,
                                rigrd,1);
    }

    if (tmkflg) MapPlotPolygon(plot,NULL,pad,pad,wdt-2*pad,hgt-2*pad,0,
                                tmkcol,0x0f,0.5,NULL,
                                rtmk,1);
   
    PlotClip(plot,0,NULL,NULL,NULL);
    if (frmflg) {
      if (! sqflg) PlotEllipse(plot,NULL,wdt/2,hgt/2,wdt/2-pad,hgt/2-pad,
                              0,grdcol,0x0f,0.5,NULL);
      else PlotRectangle(plot,NULL,pad,pad,wdt-2*pad,hgt-2*pad,
                              0,grdcol,0x0f,0.5,NULL);
    }
    if (tmeflg) plot_time_range(plot,5,5,wdt-10,hgt-10,rgrid->st_time,rgrid->ed_time,
                                txtcol,0x0f,"Helvetica",12.0,fontdb);
    px=2;
    if ((vkeyflg) && (vkey.num !=0)) {
      double kstp;
      if (khgt<80) kstp=vmax/5.0;
      else kstp=vmax/10.0;
      GrplotStdKey(plot,px,apad,8,khgt, 0,vmax,kstp, 0,0,2, 0,NULL,
                   txtbox,fontdb,label_vel,NULL,
                   "Helvetica",10.0,txtcol,0x0f,0.5,
                   vkey.num,vkey.a,vkey.r,vkey.g,vkey.b);
      px+=kwdt;
    }
    if ((xkeyflg) && (xkey.num !=0)) {
      double kstp,max;
      if (cprm==0) max=pmax;
      else max=wmax;
      if (khgt<80) kstp=max/5.0;
      else kstp=max/10.0;
      if (cprm==0) GrplotStdKey(plot,px,apad,8,khgt, 0,pmax,kstp, 0,0,2,
                                0,NULL, txtbox,fontdb,label_pwr,NULL,
                                "Helvetica",10.0,txtcol,0x0f,0.5,
                                xkey.num,xkey.a,xkey.r,xkey.g,xkey.b);
      else GrplotStdKey(plot,px,apad,8,khgt, 0,pmax,kstp, 0,0,2, 0,NULL,
                        txtbox,fontdb,label_wdt,NULL,
                        "Helvetica",10.0,txtcol,0x0f,0.5,
                        xkey.num,xkey.a,xkey.r,xkey.g,xkey.b);

      px+=kwdt;
    }


    if (vecflg) {
      if (px==2) px+=10;
      if (ortho) plot_grid_vec(plot,px,1.8*apad,0,vmax,magflg,
                               pad,pad,wdt-2*pad,hgt-2*pad,
                               vsf,vradius,tfunc,marg,txtcol,0x0f,0.5,
                               "Helvetica",10.0,fontdb,old_aacgm);
      else plot_grid_vec(plot,px,1.8*apad,0,vmax,magflg,
                         pad,pad,wdt-2*pad,hgt-2*pad,
                         vsf,vradius,MapStereographic,marg,txtcol,0x0f,0.5,
                         "Helvetica",10.0,fontdb,old_aacgm);
   }

  if (magflg || (!magflg && igrdflg)) {
    plot_aacgm(plot,4,4,wdt-8,wdt-8,txtcol,0x0f,"Helvetica",
               7.0,fontdb,old_aacgm);
  }

   if (logoflg==1) {
      plot_logo(plot,4,4,wdt-8,wdt-8,txtcol,0x0f,"Helvetica",10.0,fontdb);
/*      plot_web(plot,4,4,wdt-8,wdt-8,txtcol,0x0f,"Helvetica",10.0,fontdb);*/
/*      plot_credit(plot,4,4,wdt-8,wdt-8,txtcol,0x0f,"Helvetica",8.0,fontdb);*/
    }

    if (tlblflg) plot_time_label(plot,pad,pad,wdt-2*pad,hgt-2*pad,
                                 90*hemisphere,flip,tsfx,
                                 lon-tme_shft*(! rotflg),
                                 (wdt/2)-pad,6,
                                 txtcol,0x0f,"Helvetica",10.0,fontdb);
    PlotPlotEnd(plot);  
    PlotDocumentEnd(plot);
   
    if (!stdioflg) {
      if (pflg) fclose(outfp);
    }
    if (gflg) { /* image file */
       

#ifdef _XLIB_
       if (xd !=0) {
         XwinFrameBufferWindow(img,win);
         tmout.tv_sec=(int) delay;
         tmout.tv_usec=(delay-(int) delay)*1e6;
         if (delay!=0) xstat=XwinDisplayEvent(dp,1,&win,1,&tmout);
         else xstat=XwinDisplayEvent(dp,1,&win,1,NULL);
         if (xstat==1) break;
       } else {
         if (stdioflg) {
           if (xmlflg) FrameBufferSaveXML(img,stream,stdout);
           else if (ppmflg) FrameBufferSavePPM(img,stdout);
           else if (ppmxflg) FrameBufferSavePPMX(img,stdout);
           else FrameBufferSavePNG(img,stdout);
         } else {
           outfp=fopen(filename,"w");
           if (xmlflg) FrameBufferSaveXML(img,stream,outfp);
           else if (ppmflg) FrameBufferSavePPM(img,outfp);
           else if (ppmxflg) FrameBufferSavePPMX(img,outfp);
           else FrameBufferSavePNG(img,outfp);
           fclose(outfp);
	 }
       }
#else
       if (stdioflg) {
         if (xmlflg) FrameBufferSaveXML(img,stream,stdout);
         else if (ppmflg) FrameBufferSavePPM(img,stdout);
         else if (ppmxflg) FrameBufferSavePPMX(img,stdout);
         else FrameBufferSavePNG(img,stdout);
       } else {
         outfp=fopen(filename,"w");
         if (xmlflg) FrameBufferSaveXML(img,stream,outfp);
         else if (ppmflg) FrameBufferSavePPM(img,outfp);
         else if (ppmxflg) FrameBufferSavePPMX(img,outfp);
         else FrameBufferSavePNG(img,outfp);
         fclose(outfp);
       }
#endif
       FrameBufferFree(img);
       img=NULL;
    }
    if (rmap !=NULL) PolygonFree(rmap);
    if (rbnd !=NULL) PolygonFree(rbnd);
    if (rgrd !=NULL) PolygonFree(rgrd);
    if (rfov !=NULL) PolygonFree(rfov);
    if (rtmk !=NULL) PolygonFree(rtmk);
    if (ptrm !=NULL) PolygonFree(ptrm);
    ptrm=NULL;
    if (stdioflg) break;

    s = (*Grid_Read)(fp,rgrid);
    if ((esec !=-1) && (rgrid->ed_time>esec)) break;
    cnt++;
  } while (s !=-1);
  fclose(fp);
 

#ifdef _XLIB_
  if (xd !=0) {
    XwinFreeWindow(win);
    XwinCloseDisplay(dp);
  }
#endif

  return 0;
  
}  

