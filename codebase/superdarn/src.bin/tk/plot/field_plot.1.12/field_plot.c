/* field_plot.c
   ============ 
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
#include <time.h>
#include <zlib.h>

#ifdef _XLIB_
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <X11/Xatom.h>
#endif

#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "rmath.h"
#include "rtypes.h"
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
#include "dmap.h"
#include "stdkey.h"
#include "radar.h" 
#include "rprm.h"
#include "fitdata.h"
#include "cfitdata.h"
#include "scandata.h"
#include "fitread.h"
#include "fitindex.h"
#include "fitseek.h"
#include "cfitread.h"
#include "cfitindex.h"
#include "cfitseek.h"

#include "fitscan.h"
#include "fitindex.h"
#include "cfitscan.h"

#include "oldfitread.h"
#include "oldfitscan.h"

#include "snddata.h"
#include "sndread.h"
#include "sndseek.h"
#include "sndscan.h"

#include "key.h"
#include "text_box.h"
#include "plot_time.h"
#include "make_grid.h"
#include "sza.h"
#include "szamap.h"

#include "griddata.h"
#include "geobeam.h"
#include "plot_cell.h"
#include "plot_vec.h"
#include "plot_refvec.h"
#include "plot_outline.h"

#include "plot_logo.h"
#include "plot_time_label.h"
#include "make_fov.h"

#include "expr.h"

#include "hlpstr.h"
#include "errstr.h"
#include "version.h"



char *fsfx[]={"xml","ppm","ppmx","png","ps",0};

unsigned char gry[256];

struct key vkey;
struct key key;

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

struct CFitdata *cfit;
struct RadarParm *prm;
struct FitData *fit;
struct SndData *snd;

struct RadarScan *scn;

struct GeoLocScan geol;

struct FitIndex *inx=NULL;

struct RadarNetwork *network;
struct Radar *radar;
struct RadarSite *site;

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

unsigned int find_color(double v,void *data) {
  struct key *key;
  int i;
  key=(struct key *) data;
  if (key->num==0) return key->defcol;
  i=key->num*(v-key->min)/(key->max-key->min);
  if (i<0) i=0;
  if (i>=key->num) i=key->num-1;
  return (key->a[i]<<24) | (key->r[i]<<16) | (key->g[i]<<8) | key->b[i];
}

char *label_vec(double val,double min,double max,void *data) {
  char *txt=NULL;
  if ((val !=max) && (val !=min)) return NULL;
  txt=malloc(32); 
  if (val==max) sprintf(txt,"%g",val);
  if (val==min) sprintf(txt,"%g m/s (vmag)",val);
  return txt;
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

char *label_phi(double val,double min,double max,void *data) {
  char *txt=NULL;
  if ((val !=max) && (val !=min)) return NULL;
  txt=malloc(32);
  if (val==max) sprintf(txt,"%g",val);
  if (val==min) sprintf(txt,"%g rad",val);
  return txt;
}

char *label_elv(double val,double min,double max,void *data) {
  char *txt=NULL;
  if ((val !=max) && (val !=min)) return NULL;
  txt=malloc(32);
  if (val==max) sprintf(txt,"%g",val);
  if (val==min) sprintf(txt,"%g deg",val); 
  return txt;
}

int rst_opterr(char *txt) {
  fprintf(stderr,"Option not recognized: %s\n",txt);
  fprintf(stderr,"Please try: field_plot --help\n");
  return(-1);
}

int main(int argc,char *argv[]) {

  int old=0;

  char filename[256];
  FILE *outfp=NULL;
  unsigned char dnflg=0;
  unsigned char tnflg=0;
  char *sfx=NULL;
  int cnt=0;
  int bm=0;

  int state=0;
  int s=0;

#ifdef _XLIB_
  int xdf=0;
  int xd=0;
  struct XwinDisplay *dp=NULL;
  struct XwinWindow *win=NULL;
  char *display_name=NULL;
  char *wname="field_plot";
  int xdoff=-1;
  int ydoff=-1;
  struct timeval tmout;
  float delay=0;
  int xstat=0;
#endif

  struct FrameBuffer *img=NULL;
  struct PostScript *psdata=NULL;
  FILE *fontfp=NULL;
  char *fntdbfname=NULL;
  struct FrameBufferFontDB *fontdb=NULL;
  unsigned int bgcol;
  unsigned int txtcol;

  int farg;

  char *envstr=NULL;
  char *cfname=NULL;
  FILE *fp;

  float wdt=540,hgt=640;
  float pad=6;
  float tpad=0;

  float khgt=80;
  float kwdt=80;
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
  char *key_path=NULL;
  char *vkey_path=NULL;
  char kname[256];
  char *key_fname=NULL;
  char *vkey_fname=NULL;
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
  char *fovcol_txt=NULL;
  char *ffovcol_txt=NULL;
  char *fancol_txt=NULL;
  char *ffancol_txt=NULL;

  char *gscol_txt=NULL;

  unsigned int grdcol;
  unsigned int igrdcol;

  unsigned int cstcol;
  unsigned int bndcol;
  unsigned int lndcol;
  unsigned int seacol;
  unsigned int trmcol;
  unsigned int ftrmcol;
  unsigned int tmkcol;

  unsigned int fovcol;
  unsigned int ffovcol;

  unsigned int fancol;
  unsigned int ffancol;

  unsigned int gscol;

  FILE *mapfp;
  float marg[4];
  int i,n,c;

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

  unsigned char keyflg=0;
  unsigned char vkeyflg=0;
  unsigned char refvecflg=0;

  unsigned char fovflg=0;
  unsigned char ffovflg=0;
  unsigned char pwrflg=0;
  unsigned char velflg=0;
  unsigned char wdtflg=0;
  unsigned char phiflg=0;
  unsigned char elvflg=0;
  unsigned char verrflg=0;
  unsigned char werrflg=0;
  unsigned char pwr0flg=0;
  unsigned char vecflg=0;

  unsigned char tlblflg=0;
  unsigned char logoflg=0;
  unsigned char tmeflg=0;

  unsigned char frmflg=0; 
  unsigned char ovrflg=0; 

  unsigned char fanflg=0;
  unsigned char ffanflg=0;

  int pprm=0;

  double pmin=0;
  double pmax=30;

  double vmin=-2000;
  double vmax=2000;

  double wmin=0;
  double wmax=500;

  double phimin=-3;
  double phimax=3;

  double emin=0;
  double emax=50;

  double vemin=0;
  double vemax=200;

  double wemin=0;
  double wemax=100;

  double p0min=0;
  double p0max=30;

  float vsf=1.0;
  float vecr=2.0;

  unsigned char gsflg=0;
  unsigned char gmflg=0;

  unsigned char poleflg=0;

  unsigned char cfitflg=0,fitflg=0;
  unsigned char sndflg=0;

  int minbeam=0;
  int maxbeam=-1;
  unsigned char databeam=0;

  char *chnstr=NULL;

  char *dname=NULL,*iname=NULL;
  FILE *fitfp=NULL;
  struct OldFitFp *oldfitfp=NULL;
  struct CFitfp *cfitfp=NULL;
  FILE *sndfp=NULL;
  int tlen=0;
  int syncflg=0;

  char *expr=NULL;

  int channel=0;
  int rng=0;

  unsigned nsflg=0;

  int step=1;
  int frmcnt=0;

  int repeat=2;

  float xbox;
  float ybox;
  float wbox;
  float hbox;

  char tsfx[16];

  float box[3];
  char txt[256];

  int stid=0;
  int cpnum=0;
  int cptab[256];
  char revtxt[256];

  int cpid=-1;
  int scan=-8000;
  int sflg=0;

  int chisham=0;
  int old_aacgm=0;

  /* function pointer for MLT */
  double (*MLTCnv)(int, int, double);
  //double dec;
  double eqt,LsoT,LT,Hangle;
  
  prm=RadarParmMake();
  fit=FitMake();
  cfit=CFitMake();
  snd=SndMake();
  scn=RadarScanMake();


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

  fovcol=PlotColor(0x00,0x00,0x00,0xff);
  ffovcol=PlotColor(0xc0,0xc0,0xc0,0xff);

  fancol=PlotColor(0x80,0x80,0x80,0xff);
  ffancol=PlotColor(0xe0,0xe0,0xe0,0xff);

  gscol=PlotColor(0xa0,0xa0,0xa0,0xff);

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

  OptionAdd(&opt,"xp",'f',&xpoff);
  OptionAdd(&opt,"yp",'f',&ypoff);
  OptionAdd(&opt,"wdt",'f',&wdt);
  OptionAdd(&opt,"hgt",'f',&hgt);
  OptionAdd(&opt,"pad",'f',&pad);

  OptionAdd(&opt,"st",'t',&stmestr);
  OptionAdd(&opt,"et",'t',&etmestr);
  OptionAdd(&opt,"sd",'t',&sdtestr);
  OptionAdd(&opt,"ed",'t',&edtestr);
  OptionAdd(&opt,"ex",'t',&exstr);

  OptionAdd(&opt,"bgcol",'t',&bgcol_txt);
  OptionAdd(&opt,"txtcol",'t',&txtcol_txt);
  OptionAdd(&opt,"key",'t',&key_fname);
  OptionAdd(&opt,"key_path",'t',&key_path);
  OptionAdd(&opt,"vkey",'t',&vkey_fname);
  OptionAdd(&opt,"vkey_path",'t',&vkey_path);

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
  OptionAdd(&opt,"fovcol",'t',&fovcol_txt);
  OptionAdd(&opt,"ffovcol",'t',&ffovcol_txt);

  OptionAdd(&opt,"fov",'x',&fovflg);
  OptionAdd(&opt,"ffov",'x',&ffovflg);

  OptionAdd(&opt,"fancol",'t',&fancol_txt);
  OptionAdd(&opt,"ffancol",'t',&ffancol_txt);

  OptionAdd(&opt,"fan",'x',&fanflg);
  OptionAdd(&opt,"ffan",'x',&ffanflg);

  OptionAdd(&opt,"minbeam",'i',&minbeam);
  OptionAdd(&opt,"maxbeam",'i',&maxbeam);
  OptionAdd(&opt,"databeam",'x',&databeam);

  OptionAdd(&opt,"gscol",'t',&gscol_txt);

  OptionAdd(&opt,"v",'x',&velflg); /* plot velocity */
  OptionAdd(&opt,"p",'x',&pwrflg); /* plot power */
  OptionAdd(&opt,"w",'x',&wdtflg); /* plot spectral width */
  OptionAdd(&opt,"phi",'x',&phiflg); /* plot phi0 */
  OptionAdd(&opt,"e",'x',&elvflg); /* plot elevation angle */
  OptionAdd(&opt,"ve",'x',&verrflg); /* plot velocity error */
  OptionAdd(&opt,"we",'x',&werrflg); /* plot spectral width error */
  OptionAdd(&opt,"p0",'x',&pwr0flg); /* plot lag0 power */
  OptionAdd(&opt,"vec",'x',&vecflg);

  OptionAdd(&opt,"pmin",'d',&pmin); /* power minimum */
  OptionAdd(&opt,"pmax",'d',&pmax); /* power maximum */

  OptionAdd(&opt,"vmin",'d',&vmin); /* velocity minimum */
  OptionAdd(&opt,"vmax",'d',&vmax); /* velocity maximum */

  OptionAdd(&opt,"wmin",'d',&wmin); /* spectral width minimum */
  OptionAdd(&opt,"wmax",'d',&wmax); /* spectral width maximum */

  OptionAdd(&opt,"phimin",'d',&phimin); /* phi0 minimum */
  OptionAdd(&opt,"phimax",'d',&phimax); /* phi0 maximum */

  OptionAdd(&opt,"emin",'d',&emin); /* elevation angle minimum */
  OptionAdd(&opt,"emax",'d',&emax); /* elevation angle maximum */

  OptionAdd(&opt,"vemin",'d',&vemin); /* velocity error minimum */
  OptionAdd(&opt,"vemax",'d',&vemax); /* velocity error maximum */

  OptionAdd(&opt,"wemin",'d',&wemin); /* spectral width error minimum */
  OptionAdd(&opt,"wemax",'d',&wemax); /* spectral width error maximum */

  OptionAdd(&opt,"p0min",'d',&p0min); /* lag0 power minimum */
  OptionAdd(&opt,"p0max",'d',&p0max); /* lag0 power maximum */

  OptionAdd(&opt,"gs",'x',&gsflg); /* shade ground scatter */
  OptionAdd(&opt,"gm",'x',&gmflg); /* mask ground scatter */

  OptionAdd(&opt,"keyp",'x',&keyflg);
  OptionAdd(&opt,"vkeyp",'x',&vkeyflg);
  OptionAdd(&opt,"vecp",'x',&refvecflg);
  OptionAdd(&opt,"vsf",'f',&vsf);
  OptionAdd(&opt,"vecr",'f',&vecr);

  OptionAdd(&opt,"tmlbl",'x',&tlblflg);
  OptionAdd(&opt,"logo",'x',&logoflg);
  OptionAdd(&opt,"time",'x',&tmeflg);
  OptionAdd(&opt,"frame",'x',&frmflg);

  OptionAdd(&opt,"def",'x',&defflg);

  OptionAdd(&opt,"over",'x',&ovrflg);

  OptionAdd(&opt,"fit",'x',&fitflg);
  OptionAdd(&opt,"cfit",'x',&cfitflg);
  OptionAdd(&opt,"snd",'x',&sndflg);
  OptionAdd(&opt,"tl",'i',&tlen);
  OptionAdd(&opt,"cn",'t',&chnstr);
  OptionAdd(&opt,"ns",'x',&nsflg);

  OptionAdd(&opt,"cp",'i',&cpid); /* CP id */
  OptionAdd(&opt,"scan",'i',&scan); /* scan flag */

  OptionAdd(&opt,"fn",'t',&expr); /* apply function */

  OptionAdd(&opt,"s",'i',&step);

  OptionAdd(&opt,"r",'i',&repeat);

  OptionAdd(&opt,"chisham",'x',&chisham); /* use Chisham virtual height model */  

  farg=OptionProcess(1,argc,argv,&opt,rst_opterr);

  if (farg==-1) {
    exit(-1);
  }

  if (cfname !=NULL) { /* load the configuration file */
    int ffarg;
    do {
      fp=fopen(cfname,"r");
      if (fp==NULL) break;
      free(cfname);
      cfname=NULL;
      optf=OptionProcessFile(fp);
      if (optf !=NULL) {
        ffarg=OptionProcess(0,optf->argc,optf->argv,&opt,rst_opterr);
        if (ffarg==-1) {
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

  if (farg==argc) {
    OptionPrintInfo(stderr,errstr);
    exit(-1);
  }

  if ((cfitflg==0) && (sndflg==0)) fitflg=1;

  if (argc-farg>1) {
    dname=argv[argc-2];
    iname=argv[argc-1];
  } else dname=argv[argc-1];

  if (defflg) { /* set default plot */
    keyflg=1;   /* plot the color key (-keyp) */
    tmeflg=1;   /* plot the time of the plotted data (-time) */
    sqflg=1;    /* force the use of a square bounding box (-square) */
    mapflg=1;   /* plot coastlines (-coast) */
    fmapflg=1;  /* plot filled coastlines (-fcoast) */
    fanflg=1;   /* plot the radar field of view (-fan) */
    sf=1.5;     /* increase the scale factor (-sf 1.5) */
  }

  if (repeat==0) {
    xbox=0;
    ybox=0;
    wbox=wdt;
    hbox=hgt;
  } else {
    tpad=20+100*wdt/320;
    if (tpad>120) tpad=120;   

    xbox=0;
    ybox=tpad;
    wbox=wdt/repeat;
    hbox=(hgt-tpad)/repeat;
    repeat=repeat*repeat;
    pad=0;

  }

  if (exstr !=NULL) exsec=strtime(exstr);
  if (stmestr !=NULL) ssec=strtime(stmestr);
  if (etmestr !=NULL) esec=strtime(etmestr);
  if (sdtestr !=NULL) sdte=strdate(sdtestr);
  if (edtestr !=NULL) edte=strdate(edtestr);

  if (scan !=-8000) sflg=1;

  if (chnstr !=NULL) {
    if (tolower(chnstr[0])=='a') channel=1;
    if (tolower(chnstr[0])=='b') channel=2;
  }

  if (fitflg) {

    if (old) {
      oldfitfp=OldFitOpen(dname,iname);
      if (oldfitfp==NULL) {
        fprintf(stderr,"File not found.\n");
        exit(-1);
      }

      s=OldFitReadRadarScan(oldfitfp,&state,scn,prm,fit,
                            tlen,syncflg,channel);
      if (s==-1) {
        fprintf(stderr,"Error reading file.\n");
        exit(-1);
      }
    } else {
      if (iname !=NULL) {
        fp=fopen(iname,"r");
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
      fitfp=fopen(dname,"r");
      if (fitfp==NULL) {
        fprintf(stderr,"File not found.\n");
        exit(-1);
      }
      s=FitFreadRadarScan(fitfp,&state,scn,prm,fit,
                          tlen,syncflg,channel);
      if (s==-1) {
        fprintf(stderr,"Error reading file.\n");
        exit(-1);
      }
    }
  } else if (cfitflg) {
    cfitfp=CFitOpen(dname);
    if (cfitfp==NULL) {
      fprintf(stderr,"File not found.\n");
      exit(-1);
    }

    s=CFitReadRadarScan(cfitfp,&state,scn,cfit,tlen,syncflg,channel);

    if (s==-1) {
      fprintf(stderr,"Error reading file.\n");
      exit(-1);
    }
  } else if (sndflg) {
    sndfp=fopen(dname,"r");
    if (sndfp==NULL) {
      fprintf(stderr,"File not found.\n");
      exit(-1);
    }

    s=SndFreadRadarScan(sndfp,&state,scn,snd,tlen,syncflg);

    if (s==-1) {
      fprintf(stderr,"Error reading file.\n");
      exit(-1);
    }
  }

  if (delay == 0){
      fprintf(stderr, "Click to view next plot. Ctrl+c to exit. Use -delay option to cycle through all frames sequentially.\n");
  }

  if ((ssec !=-1) || (sdte !=-1)) {
    /* we must skip the start of the files */

    if (ssec==-1) ssec= ( (int) scn->st_time % (24*3600));
    if (sdte==-1) ssec+=scn->st_time - ( (int) scn->st_time % (24*3600));
    else ssec+=sdte;

    TimeEpochToYMDHMS(ssec,&yr,&mo,&dy,&hr,&mt,&sc);
    if (fitflg) {
      if (old) {
        s=OldFitSeek(oldfitfp,yr,mo,dy,hr,mt,sc,NULL);
        if (s ==-1) {
          fprintf(stderr,"File does not contain the requested interval.\n");
          exit(-1);
        }
        if (tlen==0) {
          while ((s=OldFitRead(oldfitfp,prm,fit)) !=-1) {
            if (abs(prm->scan)==1) break;
          }
        } else state=0;
        s=OldFitReadRadarScan(oldfitfp,&state,scn,prm,
                              fit,tlen,syncflg,channel);
      } else {
        s=FitFseek(fitfp,yr,mo,dy,hr,mt,sc,NULL,inx);
        if (s==-1) {
          fprintf(stderr,"File does not contain the requested interval.\n");
          exit(-1);
        }
        if (tlen==0) {
          while ((s=FitFread(fitfp,prm,fit)) !=-1) {
            if (abs(prm->scan)==1) break;
          }
        } else state=0;
        s=FitFreadRadarScan(fitfp,&state,scn,prm,
                            fit,tlen,syncflg,channel);
      }
    } else if (cfitflg) {
      s=CFitSeek(cfitfp,yr,mo,dy,hr,mt,sc,NULL,NULL);

      if (s ==-1) {
        fprintf(stderr,"File does not contain the requested interval.\n");
        exit(-1);
      }
      if (tlen==0) {
        while ((s=CFitRead(cfitfp,cfit)) !=-1) {
          if (abs(cfit->scan)==1) break;
        }
      } else state=0;
      s=CFitReadRadarScan(cfitfp,&state,scn,cfit,tlen,syncflg,channel);
    } else if (sndflg) {
      s=SndFseek(sndfp,yr,mo,dy,hr,mt,sc,NULL);

      if (s ==-1) {
        fprintf(stderr,"File does not contain the requested interval.\n");
        exit(-1);
      }
      if (tlen==0) {
        while ((s=SndFread(sndfp,snd)) !=-1) {
          if (abs(snd->scan)==1) break;
        }
      } else state=0;
      s=SndFreadRadarScan(sndfp,&state,scn,snd,tlen,syncflg);
    }
  } else ssec=scn->st_time;

  if (esec !=-1) {
    if (edte==-1) esec+=scn->st_time - ( (int) scn->st_time % (24*3600));
    else esec+=edte;
  }

  if (exsec !=0) esec=ssec+exsec;

  if (fitflg) {
    stid=prm->stid;
    cptab[0]=prm->cp;
    cpnum=1;
    if (fit->algorithm !=NULL) {
      sprintf(revtxt,"%s",fit->algorithm);
    } else {
      sprintf(revtxt,"Revision:%d.%d",fit->revision.major,
              fit->revision.minor);
    }
  }
  if (cfitflg) {
    stid=cfit->stid;
    cptab[0]=cfit->cp;
    cpnum=1;
    sprintf(revtxt,"Revision:%d.%d",cfit->version.major,
            cfit->version.minor);
  }
  if (sndflg) {
    stid=snd->stid;
    cptab[0]=snd->cp;
    cpnum=1;
    sprintf(revtxt,"Revision:%d.%d",snd->snd_revision.major,
            snd->snd_revision.minor);
  }

  if (magflg && old_aacgm) magflg = 2; /* set to 2 for old AACGM */

  /* set function pointer to compute MLT or MLT_v2 */
  if (old_aacgm) MLTCnv = &MLTConvertYrsec;
  else           MLTCnv = &MLTConvertYrsec_v2;

  TimeEpochToYMDHMS(scn->st_time,&yr,&mo,&dy,&hr,&mt,&sc);
  radar=RadarGetRadar(network,scn->stid);
  site=RadarYMDHMSGetSite(radar,yr,mo,dy,hr,mt,(int) sc);

  if (!old_aacgm) {
    AACGM_v2_SetDateTime(yr,mo,dy,hr,mt,(int)sc); /* required */
    if (magflg) AACGM_v2_Lock();
  }

  if (site->geolat>0) hemisphere=1;
  else hemisphere=-1;

  if (!sqflg) clip=MapCircleClip(10);
  else clip=MapSquareClip();

  if (lat>90) GeoLocCenter(site,magflg,&lat,&lon,chisham,old_aacgm);

  if ((lat<0) && (latmin>0)) latmin=-latmin;
  if ((lat>0) && (latmin<0)) latmin=-latmin;

  if (fovflg || ffovflg) fov=make_field_fov(scn->st_time,network,scn->stid,chisham);
  if ((fovflg || ffovflg) && magflg) {
    if (old_aacgm) MapModify(fov,AACGMtransform,NULL);
    else           MapModify(fov,AACGM_v2_transform,NULL);
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

  tfunc=MapStereographic;
  if (ortho) tfunc=MapOrthographic;

  strcpy(tsfx,"LT");
  if (magflg) strcpy(tsfx,"MLT");
  else if (lstflg) strcpy(tsfx,"LST");

  if (lat==90) poleflg=1;
  if (lat==-90) poleflg=1;

  if (poleflg) {
    if (mapflg || fmapflg) {
      nmap=MapTransform(map,2*sizeof(float),PolygonXYbbox,
                        tfunc,marg);
      pmap=PolygonClip(clip,nmap);
      PolygonFree(map);
      PolygonFree(nmap);
    }
    if (bndflg) {
      nbnd=MapTransform(bnd,2*sizeof(float),PolygonXYbbox,
                        tfunc,marg);
      pbnd=PolygonClip(clip,nbnd);
      PolygonFree(bnd);
      PolygonFree(nbnd);
    }
    if (grdflg) {
      ngrd=MapTransform(grd,2*sizeof(float),PolygonXYbbox,
                        tfunc,marg);
      pgrd=PolygonClip(clip,ngrd);
      PolygonFree(grd);
      PolygonFree(ngrd);
    }

    if (igrdflg) {
      nigrd=MapTransform(igrd,2*sizeof(float),PolygonXYbbox,
                         tfunc,marg);
      pigrd=PolygonClip(clip,nigrd);
      PolygonFree(igrd);
      PolygonFree(nigrd);
    }
    if (fovflg || ffovflg) {
      nfov=MapTransform(fov,2*sizeof(float),PolygonXYbbox,
                        tfunc,marg);
      pfov=PolygonClip(clip,nfov);
      PolygonFree(fov);
      PolygonFree(nfov);
    }

    if (tmkflg) {
      ntmk=MapTransform(tmk,2*sizeof(float),PolygonXYbbox,
                        tfunc,marg);
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
  if (fovcol_txt !=NULL) fovcol=PlotColorStringRGBA(fovcol_txt);
  if (ffovcol_txt !=NULL) ffovcol=PlotColorStringRGBA(ffovcol_txt);

  if (fancol_txt !=NULL) fancol=PlotColorStringRGBA(fancol_txt);
  if (ffancol_txt !=NULL) ffancol=PlotColorStringRGBA(ffancol_txt);

  if (gscol_txt !=NULL) gscol=PlotColorStringRGBA(gscol_txt);

  if (key_fname !=NULL) {
    if (key_path == NULL) key_path = getenv("COLOR_TABLE_PATH");
    if (key_path != NULL) {
      strcpy(kname, key_path);
      len = strlen(key_path);
      if (key_path[len-1] != '/') strcat(kname, "/");
      strcat(kname, key_fname);
    } else {
      fprintf(stderr, "No COLOR_TABLE_PATH set\n");
    }
    keyfp=fopen(kname,"r");
    if (keyfp !=NULL) {
      load_key(keyfp,&key);
      fclose(keyfp);
    } else {
      fprintf(stderr, "Color table %s not found\n", kname);
    }
  }

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

  if (pwrflg) pprm=1;
  if (velflg) pprm=2;
  if (wdtflg) pprm=3;
  if (phiflg) pprm=4;
  if (elvflg) pprm=5;
  if (verrflg) pprm=6;
  if (werrflg) pprm=7;
  if (pwr0flg) pprm=8;

  if (pprm==1) key.max=pmax;
  else if (pprm==2) key.max=vmax;
  else if (pprm==3) key.max=wmax;
  else if (pprm==4) key.max=phimax;
  else if (pprm==5) key.max=emax;
  else if (pprm==6) key.max=vemax;
  else if (pprm==7) key.max=wemax;
  else if (pprm==8) key.max=p0max;

  if (pprm==1) key.min=pmin;
  else if (pprm==2) key.min=vmin;
  else if (pprm==3) key.min=wmin;
  else if (pprm==4) key.min=phimin;
  else if (pprm==5) key.min=emin;
  else if (pprm==6) key.min=vemin;
  else if (pprm==7) key.min=wemin;
  else if (pprm==8) key.min=p0min;

  vkey.min=0;
  vkey.max=vmax;

  if (vkey.num==0) {
    vkey.num=KeyLinearMax;
    vkey.a=KeyLinearA[0];
    vkey.r=KeyLinearR[0];
    vkey.g=KeyLinearG[0];
    vkey.b=KeyLinearB[0];
  }

  if (key.num==0) {
    if (pprm==2) {
      key.num=KeyLinearMax;
      key.a=KeyLinearA[1];
      key.r=KeyLinearR[1];
      key.g=KeyLinearG[1];
      key.b=KeyLinearB[1];
    } else {
      key.num=KeyLinearMax;
      key.a=KeyLinearA[0];
      key.r=KeyLinearR[0];
      key.g=KeyLinearG[0];
      key.b=KeyLinearB[0];
    }
  }

  if ((wdt==0) || (hgt==0)) {
    fprintf(stderr,"invalid plot size.\n");
    exit(-1);
  }

  if (wdt>400) {
    apad=30;
  } else {
    khgt=80*wdt/320;
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

    win=XwinMakeWindow(xdoff,ydoff,wdt,hgt,0,
                       dp,wname,
                       wname,argv[0],wname,argc,argv,&xdf);
    if (win==NULL) {
      fprintf(stderr,"Could not create window.\n");
      exit(-1);
    }
    XwinShowWindow(win);
  }
  #endif

  if (repeat !=0) {
    if (pflg) PostScriptSetText(psdata,stream,stdout);
    PlotDocumentStart(plot,"field_plot",NULL,wdt,hgt,24);
    PlotPlotStart(plot,"field_plot",wdt,hgt,24);
    if (!ovrflg) PlotRectangle(plot,NULL,0,0,wdt,tpad,
                               1,bgcol,0x0f,0,NULL);
  }

  do {

    bm=0;
    for (c=0;c<scn->num;c++) {
      for (i=0;i<cpnum;i++) if (cptab[i]==scn->bm[c].cpid) break;
      if (i==cpnum) {
        cptab[i]=scn->bm[c].cpid;
        cpnum++;
      }
      if ((sflg) && (scn->bm[c].scan !=scan)) continue;
      if ((nsflg) && (scn->bm[c].scan<0)) continue;
      if ((cpid !=-1) && (scn-> bm[c].cpid !=cpid)) continue;
      bm++;
    }
    if (bm>0) {

      tval=(scn->st_time);
      TimeEpochToYMDHMS(tval,&yr,&mo,&dy,&hr,&mt,&sc);
      yrsec=TimeYMDHMSToYrsec(yr,mo,dy,hr,mt,sc);
      fprintf(stderr,"%d-%d-%d %d:%d:%d\n",dy,mo,yr,hr,mt,(int) sc);
      if (repeat==0) {
        if (tnflg) sprintf(filename,"%.2d%.2d.%.2d.%s",hr,mt,(int) sc,sfx); 
        else if (dnflg) sprintf(filename,"%.4d%.2d%.2d.%.2d%.2d.%.2d.%s",
                                yr,mo,dy,hr,mt,(int) sc,sfx);
        else sprintf(filename,"%.4d.%s",frmcnt,sfx);
        frmcnt++;
        if (pflg) {
          outfp=fopen(filename,"w");
          PostScriptSetText(psdata,stream,outfp);
        }
      }

      if (trmflg || ftrmflg) {
        if (lat>0) trm=SZATerminator(yr,mo,dy,hr,mt,sc,1,magflg,
                                     1.0,90.0);
        if (lat<0) trm=SZATerminator(yr,mo,dy,hr,mt,sc,-1,magflg,
                                     1.0,90.0);
      }

      if (magflg) tme_shft=-(*MLTCnv)(yr,yrsec,0.0)*15.0;
      else {
        if (lstflg) {
          //dec=SZASolarDec(yr,mo,dy,hr,mt,sc);
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
        if (pmap !=NULL) 
         rmap=MapTransform(pmap,2*sizeof(float),PolygonXYbbox,rotate,marg);
        if (pbnd !=NULL) 
        rbnd=MapTransform(pbnd,2*sizeof(float),PolygonXYbbox,rotate,marg);
        if (pgrd !=NULL) 
        rgrd=MapTransform(pgrd,2*sizeof(float),PolygonXYbbox,rotate,marg);
        if (pigrd !=NULL) 
        rigrd=MapTransform(pigrd,2*sizeof(float),PolygonXYbbox,rotate,marg);
        if (pfov !=NULL) 
        rfov=MapTransform(pfov,2*sizeof(float),PolygonXYbbox,rotate,marg);
        if (ptmk !=NULL) {
          if (rotflg) marg[1]=0;
          else marg[1]=lon-tme_shft;
          rtmk=MapTransform(ptmk,2*sizeof(float),PolygonXYbbox,rotate,marg);
          if (rotflg) marg[1]=lon+tme_shft;
          else marg[1]=lon;
        }
      } else {
        if (mapflg || fmapflg) {
          nmap=MapTransform(map,2*sizeof(float),PolygonXYbbox,tfunc,marg);
          rmap=PolygonClip(clip,nmap);
          PolygonFree(nmap);
        }
        if (bndflg) {
          nbnd=MapTransform(bnd,2*sizeof(float),PolygonXYbbox,tfunc,marg);
          rbnd=PolygonClip(clip,nbnd);
          PolygonFree(nbnd);
        }
        if (grdflg) {
          ngrd=MapTransform(grd,2*sizeof(float),PolygonXYbbox,tfunc,marg);
          rgrd=PolygonClip(clip,ngrd);
          PolygonFree(ngrd);
        }
        if (igrdflg) {
          nigrd=MapTransform(igrd,2*sizeof(float),PolygonXYbbox,tfunc,marg);
          rigrd=PolygonClip(clip,nigrd);
          PolygonFree(nigrd);
        }
        if (fovflg || ffovflg) {
          nfov=MapTransform(fov,2*sizeof(float),PolygonXYbbox,tfunc,marg);
          rfov=PolygonClip(clip,nfov);
          PolygonFree(nfov);
        }

        if (tmkflg) {
          if (rotflg) marg[1]=0;
          else marg[1]=lon-tme_shft;
          ntmk=MapTransform(tmk,2*sizeof(float),PolygonXYbbox,tfunc,marg);
          rtmk=PolygonClip(clip,ntmk);
          PolygonFree(ntmk);
          if (rotflg) marg[1]=lon+tme_shft;
          else marg[1]=lon;
        }
      }
      if (trmflg || ftrmflg) {
         ntrm=MapTransform(trm,2*sizeof(float),PolygonXYbbox,tfunc,marg);
         ptrm=PolygonClip(clip,ntrm);
         PolygonFree(ntrm);
         PolygonFree(trm);
         trm=NULL;
         ntrm=NULL;
      }
      if (repeat==0) {
        PlotDocumentStart(plot,filename,NULL,wdt,hgt,24);
        PlotPlotStart(plot,filename,wdt,hgt,24);
      }

      if (!ovrflg) PlotRectangle(plot,NULL,xbox,ybox,wbox,hbox,
                                 1,bgcol,0x0f,0,NULL);
      if (!sqflg) circle_clip(plot,xbox+wbox/2,ybox+hbox/2,
                              wbox/2-pad,hbox/2-pad);
      else square_clip(plot,xbox+pad,ybox+pad,wbox-2*pad,hbox-2*pad);

      if (fmapflg) {
        if (!sqflg) PlotEllipse(plot,NULL,xbox+wbox/2,ybox+hbox/2,
                                wbox/2-pad,hbox/2-pad,
                                1,seacol,0x0f,0,NULL);
        else PlotRectangle(plot,NULL,xbox+pad,ybox+pad,wbox-2*pad,
                           hbox-2*pad,
                           1,seacol,0x0f,0,NULL);
      }

      if ((grdflg) && (grdtop==0)) {
        MapPlotPolygon(plot,NULL,xbox+pad,ybox+pad,wbox-2*pad,hbox-2*pad,0,
                       grdcol,0x0f,0.5,NULL,
                       rgrd,1);
      }

      if ((igrdflg) && (igrdtop==0)) {
        MapPlotPolygon(plot,NULL,xbox+pad,ybox+pad,wbox-2*pad,hbox-2*pad,0,
                       igrdcol,0x0f,0.5,NULL,
                       rigrd,1);
      }

      if (fmapflg) {
        MapPlotPolygon(plot,NULL,xbox+pad,ybox+pad,wbox-2*pad,hbox-2*pad,
                       1,lndcol,0x0f,0,NULL,rmap,1);
        MapPlotPolygon(plot,NULL,xbox+pad,ybox+pad,wbox-2*pad,hbox-2*pad,
                       1,lndcol,0x0f,0,NULL,rmap,3);
        MapPlotPolygon(plot,NULL,xbox+pad,ybox+pad,wbox-2*pad,hbox-2*pad,
                       1,seacol,0x0f,0,NULL,rmap,0);
      }

      if (ftrmflg) 
         MapPlotPolygon(plot,NULL,xbox+pad,ybox+pad,wbox-2*pad,hbox-2*pad,1,
                        ftrmcol,0x0f,0.5,NULL,
                        ptrm,1);

      if (ffovflg) MapPlotPolygon(plot,NULL,xbox+pad,ybox+pad,wbox-2*pad,hbox-2*pad,1,
                                 ffovcol,0x0f,0.5,NULL,
                                 rfov,1);

      if (databeam) {
        minbeam=999;
        maxbeam=-1;
        for (c=0;c<scn->num;c++) {
          if (scn->bm[c].bm < minbeam) minbeam=scn->bm[c].bm;
          if (scn->bm[c].bm > maxbeam) maxbeam=scn->bm[c].bm;
        }
        maxbeam=maxbeam+1;
      }

      for (c=0;c<scn->num;c++) {
        if ((sflg) && (scn->bm[c].scan !=scan)) continue;
        if ((nsflg) && (scn->bm[c].scan<0)) continue;
        if ((cpid !=-1) && (scn-> bm[c].cpid !=cpid)) continue;
        n=GeoLocBeam(site,yr,&geol,&scn->bm[c],chisham,old_aacgm);

        for (rng=0;rng<scn->bm[c].nrang;rng++) {
          if ((expr !=NULL) && (eval_expr(expr,&scn->bm[c],rng)==0))
            scn->bm[c].sct[rng]=0;

          if ((gmflg) && (scn->bm[c].rng[rng].gsct !=0))
            scn->bm[c].sct[rng]=0;
        }

        if (ffanflg) plot_filled(plot,&scn->bm[c],&geol.bm[n],0,
                                 magflg,xbox+pad,ybox+pad,
                                 wbox-2*pad,hbox-2*pad,tfunc,marg,ffancol);
        if (pprm !=0) plot_field_cell(plot,&scn->bm[c],&geol.bm[n],0,magflg,xbox+pad,ybox+pad,
                                      wbox-2*pad,hbox-2*pad,tfunc,marg,find_color,
                                      &key,pprm,gscol,gsflg);

        if (vecflg) plot_field_vec(plot,&scn->bm[c],&geol.bm[n],0,magflg,xbox+pad,ybox+pad,
                                   wbox-2*pad,hbox-2*pad,vsf,tfunc,marg,find_color,
                                   &vkey,gscol,gsflg,0.5,vecr);

        if (maxbeam==-1) maxbeam=site->maxbeam;

        if (fanflg) plot_outline(plot,&scn->bm[c],&geol.bm[n],0,
                                 magflg,minbeam,maxbeam,xbox+pad,ybox+pad,
                                 wbox-2*pad,hbox-2*pad,tfunc,marg,fancol);
      }

      if (mapflg) {
        MapPlotPolygon(plot,NULL,xbox+pad,ybox+pad,wbox-2*pad,hbox-2*pad,0,cstcol,0x0f,
                       0.5,NULL,rmap,1);
        MapPlotOpenPolygon(plot,NULL,xbox+pad,ybox+pad,wbox-2*pad,hbox-2*pad,cstcol,0x0f,
                           0.5,NULL,rmap,2);
        MapPlotPolygon(plot,NULL,xbox+pad,ybox+pad,wbox-2*pad,hbox-2*pad,0,cstcol,0x0f,
                       0.5,NULL,rmap,0);
      }

      if (bndflg) MapPlotOpenPolygon(plot,NULL,xbox+pad,ybox+pad,wbox-2*pad,hbox-2*pad,
                                     bndcol,0x0f,0.5,NULL,rbnd,-1);

      if (fovflg) MapPlotPolygon(plot,NULL,xbox+pad,ybox+pad,wbox-2*pad,hbox-2*pad,0,
                                 fovcol,0x0f,0.5,NULL,rfov,1);

      if (trmflg)
        MapPlotPolygon(plot,NULL,xbox+pad,ybox+pad,wbox-2*pad,hbox-2*pad,0,
                       trmcol,0x0f,0.5,NULL,ptrm,1);

      if ((grdflg) && (grdtop==1)) {
        MapPlotPolygon(plot,NULL,xbox+pad,ybox+pad,wbox-2*pad,hbox-2*pad,0,
                       grdcol,0x0f,0.5,NULL,rgrd,1);
      }

      if ((igrdflg) && (igrdtop==1)) {
        MapPlotPolygon(plot,NULL,xbox+pad,ybox+pad,wbox-2*pad,hbox-2*pad,0,
                       igrdcol,0x0f,0.5,NULL,rigrd,1);
      }

      if (tmkflg) MapPlotPolygon(plot,NULL,xbox+pad,ybox+pad,wbox-2*pad,hbox-2*pad,0,
                                 tmkcol,0x0f,0.5,NULL,rtmk,1);

      PlotClip(plot,0,NULL,NULL,NULL);
      if (frmflg) {
        if (! sqflg) PlotEllipse(plot,NULL,xbox+wbox/2,ybox+hbox/2,
                                 wbox/2-pad,hbox/2-pad,
                                 0,grdcol,0x0f,0.5,NULL);
        else PlotRectangle(plot,NULL,xbox+pad,ybox+pad,wbox-2*pad-1,
                           hbox-2*pad-1,
                           0,grdcol,0x0f,0.5,NULL);
      }
      if (tmeflg) {
        if (repeat==0) plot_field_time(plot,xbox+5,ybox+5,wbox-10,hbox-10,0,
                                       scn->st_time,scn->ed_time,
                                       txtcol,0x0f,"Helvetica",12.0,fontdb);
        else plot_field_time(plot,xbox+2,ybox+2,wbox-4,hbox-4,1,
                             scn->st_time,scn->ed_time,
                             txtcol,0x0f,"Helvetica",10.0,fontdb);
      }
      px=2;
      if ((repeat==0) || ((repeat!=0) && (cnt==0))) {
        if ((keyflg) && (key.num !=0)) {
          double kstp,max=0,min=0;
          if (pprm==1) max=pmax;
          else if (pprm==2) max=vmax;
          else if (pprm==3) max=wmax;
          else if (pprm==4) max=phimax;
          else if (pprm==5) max=emax;
          else if (pprm==6) max=vemax;
          else if (pprm==7) max=wemax;
          else if (pprm==8) max=p0max;
          if (pprm==1) min=pmin;
          else if (pprm==2) min=vmin;
          else if (pprm==3) min=wmin;
          else if (pprm==4) min=phimin;
          else if (pprm==5) min=emin;
          else if (pprm==6) min=vemin;
          else if (pprm==7) min=wemin;
          else if (pprm==8) min=p0min;

          if (khgt<80) kstp=(max-min)/5.0;
          else kstp=(max-min)/10.0;

          if (pprm==1) GrplotStdKey(plot,px,apad,8,khgt,
                                    pmin,pmax,kstp,
                                    0,0,2,
                                    0,NULL,
                                    txtbox,fontdb,label_pwr,NULL,
                                    "Helvetica",10.0,txtcol,0x0f,0.5,
                                    key.num,key.a,key.r,key.g,key.b);
          else if (pprm==2) GrplotStdKey(plot,px,apad,8,khgt,
                                         vmin,vmax,kstp,
                                         0,0,2,
                                         0,NULL,
                                         txtbox,fontdb,label_vel,NULL,
                                         "Helvetica",10.0,txtcol,0x0f,0.5,
                                         key.num,key.a,key.r,key.g,key.b);
          else if (pprm==3) GrplotStdKey(plot,px,apad,8,khgt,
                                         wmin,wmax,kstp,
                                         0,0,2,
                                         0,NULL,
                                         txtbox,fontdb,label_wdt,NULL,
                                         "Helvetica",10.0,txtcol,0x0f,0.5,
                                         key.num,key.a,key.r,key.g,key.b);
          else if (pprm==4) GrplotStdKey(plot,px,apad,8,khgt,
                                         phimin,phimax,kstp,
                                         0,0,2,
                                         0,NULL,
                                         txtbox,fontdb,label_phi,NULL,
                                         "Helvetica",10.0,txtcol,0x0f,0.5,
                                         key.num,key.a,key.r,key.g,key.b);
          else if (pprm==5) GrplotStdKey(plot,px,apad,8,khgt,
                                         emin,emax,kstp,
                                         0,0,2,
                                         0,NULL,
                                         txtbox,fontdb,label_elv,NULL,
                                         "Helvetica",10.0,txtcol,0x0f,0.5,
                                         key.num,key.a,key.r,key.g,key.b);
          else if (pprm==6) GrplotStdKey(plot,px,apad,8,khgt,
                                         vemin,vemax,kstp,
                                         0,0,2,
                                         0,NULL,
                                         txtbox,fontdb,label_vel,NULL,
                                         "Helvetica",10.0,txtcol,0x0f,0.5,
                                         key.num,key.a,key.r,key.g,key.b);
          else if (pprm==7) GrplotStdKey(plot,px,apad,8,khgt,
                                         wemin,wemax,kstp,
                                         0,0,2,
                                         0,NULL,
                                         txtbox,fontdb,label_wdt,NULL,
                                         "Helvetica",10.0,txtcol,0x0f,0.5,
                                         key.num,key.a,key.r,key.g,key.b);
          else if (pprm==8) GrplotStdKey(plot,px,apad,8,khgt,
                                         p0min,p0max,kstp,
                                         0,0,2,
                                         0,NULL,
                                         txtbox,fontdb,label_pwr,NULL,
                                         "Helvetica",10.0,txtcol,0x0f,0.5,
                                         key.num,key.a,key.r,key.g,key.b);
          px+=kwdt;
        }
        if ((vkeyflg) && (vkey.num !=0)) {
          double kstp,max=0,min=0;
          max=vmax;
          min=0;
          if (khgt<80) kstp=(max-min)/5.0;
          else kstp=(max-min)/10.0;
          GrplotStdKey(plot,px,apad,8,khgt,
                       0,vmax,kstp,
                       0,0,2,
                       0,NULL,
                       txtbox,fontdb,label_vec,NULL,
                       "Helvetica",10.0,txtcol,0x0f,0.5,
                       vkey.num,vkey.a,vkey.r,vkey.g,vkey.b);
          px+=kwdt;
        }

        if (refvecflg) {
          if (px==2) px+=10;
          if (ortho) plot_refvec(plot,px,1.8*apad,0,vmax,magflg,
                                 xbox+pad,ybox+pad,wdt-2*pad,hgt-2*pad,
                                 vsf,tfunc,marg,txtcol,0x0f,0.5,
                                 "Helvetica",10.0,fontdb,vecr,old_aacgm);
          else plot_refvec(plot,px,1.8*apad,0,vmax,magflg,
                           xbox+pad,ybox+pad,wdt-2*pad,hgt-2*pad,
                           vsf,MapStereographic,marg,txtcol,0x0f,0.5,
                           "Helvetica",10.0,fontdb,vecr,old_aacgm);
        }
      }

     if (logoflg==1) {
       if (repeat==0) {
         plot_logo(plot,4,4,wdt-8,hgt-18,txtcol,0x0f,"Helvetica",10.0,fontdb);
         plot_web(plot,4,4,wdt-8,hgt-8,txtcol,0x0f,"Helvetica",10.0,fontdb);
         plot_credit(plot,4,4,wdt-8,hgt-8,txtcol,0x0f,"Helvetica",8.0,fontdb);
       } else {
         plot_logo(plot,4,4,wdt-8,8,txtcol,0x0f,"Helvetica",10.0,fontdb);
         plot_web(plot,4,4,wdt-8,8,txtcol,0x0f,"Helvetica",10.0,fontdb);
         plot_credit(plot,4,4,wdt-8,18,txtcol,0x0f,"Helvetica",8.0,fontdb);
       }

     }

       if (tlblflg) plot_time_label(plot,xbox+pad,ybox+pad,wbox-2*pad,
                                    hbox-2*pad,
                                    90*hemisphere,flip,tsfx,
                                    lon-tme_shft*(! rotflg),
                                    (wbox/2)-pad,6,
                                    txtcol,0x0f,"Helvetica",10.0,fontdb);
       if (repeat==0) {
         PlotPlotEnd(plot);
         PlotDocumentEnd(plot);

         if (pflg) fclose(outfp);

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
             outfp=fopen(filename,"w");
             if (xmlflg) FrameBufferSaveXML(img,stream,outfp);
             else if (ppmflg) FrameBufferSavePPM(img,outfp);
             else if (ppmxflg) FrameBufferSavePPMX(img,outfp);
             else FrameBufferSavePNG(img,outfp);
             fclose(outfp);
           }
#else
           outfp=fopen(filename,"w");
           if (xmlflg) FrameBufferSaveXML(img,stream,outfp);
           else if (ppmflg) FrameBufferSavePPM(img,outfp);
           else if (ppmxflg) FrameBufferSavePPMX(img,outfp);
           else FrameBufferSavePNG(img,outfp);
           fclose(outfp);
#endif
           FrameBufferFree(img);
           img=NULL;
         }
      } else {
        xbox+=wbox;
        if (xbox>=wdt) {
          xbox=0;
          ybox+=hbox;
        }
      }

      if (rmap !=NULL) PolygonFree(rmap);
      if (rbnd !=NULL) PolygonFree(rbnd);
      if (rgrd !=NULL) PolygonFree(rgrd);
      if (rfov !=NULL) PolygonFree(rfov);
      if (rtmk !=NULL) PolygonFree(rtmk);
      if (ptrm !=NULL) PolygonFree(ptrm);
      ptrm=NULL;
    }

    for (c=0;c<step;c++) {

      if (fitflg) {
        if (old)
          s=OldFitReadRadarScan(oldfitfp,&state,scn,prm,fit,tlen,
                                syncflg,channel);
        else s=FitFreadRadarScan(fitfp,&state,scn,prm,fit,tlen,
                                 syncflg,channel);
      } else if (cfitflg)
        s=CFitReadRadarScan(cfitfp,&state,scn,cfit,tlen,
                            syncflg,channel);
      else if (sndflg)
        s=SndFreadRadarScan(sndfp,&state,scn,snd,tlen,syncflg);
    }
    if ((esec !=-1) && (scn->ed_time>esec)) break;
    cnt++;
    if (cnt==repeat) break;
  } while (s !=-1);

  if (repeat !=0) {
    float lnehgt;
    char tmp[32];

    sprintf(txt,"Xy");
    txtbox("Helvetica",12.0,strlen(txt),txt,box,fontdb);
    lnehgt=1.5*(box[2]-box[1]);

    plot_field_time(plot,0,24,wdt,tpad-18,2, 
                    ssec,scn->ed_time,
                    txtcol,0x0f,"Helvetica",12.0,fontdb);

    sprintf(txt,"Station:%s (%s)",RadarGetName(network,stid),
            RadarGetCode(network,stid,0));
    txtbox("Helvetica",12.0,strlen(txt),txt,box,fontdb);

    PlotText(plot,NULL,"Helvetica",12.0,wdt-box[0],24+lnehgt,
             strlen(txt),txt,txtcol,0x0f,0);

    sprintf(txt,"Operated by:%s",RadarGetOperator(network,stid));
    txtbox("Helvetica",12.0,strlen(txt),txt,box,fontdb);

    PlotText(plot,NULL,"Helvetica",12.0,wdt-box[0],24+2*lnehgt,
             strlen(txt),txt,txtcol,0x0f,0);

    strcpy(txt,"Program ID");
    if (cpnum>1) strcat(txt,"s");
    strcat(txt,":");
    for (i=0;i<cpnum;i++) {
      if (i !=0) strcat(txt,",");
      sprintf(tmp,"%d",cptab[i]);
      strcat(txt,tmp);
    }
    txtbox("Helvetica",12.0,strlen(txt),txt,box,fontdb);

    PlotText(plot,NULL,"Helvetica",12.0,wdt-box[0],24+3*lnehgt,
             strlen(txt),txt,txtcol,0x0f,0);

    if (pprm==1) sprintf(txt,"Parameter:Power");
    if (pprm==2) sprintf(txt,"Parameter:Velocity");
    if (pprm==3) sprintf(txt,"Parameter:Spectral Width");
    if (pprm==4) sprintf(txt,"Parameter:Phi0");
    if (pprm==5) sprintf(txt,"Parameter:Elevation Angle");
    if (pprm==6) sprintf(txt,"Parameter:Velocity Error");
    if (pprm==7) sprintf(txt,"Parameter:Spectral Width Error");
    if (pprm==8) sprintf(txt,"Parameter:Lag0 Power");
    txtbox("Helvetica",12.0,strlen(txt),txt,box,fontdb);

    PlotText(plot,NULL,"Helvetica",12.0,wdt-box[0],24+4*lnehgt,
             strlen(txt),txt,txtcol,0x0f,0);

    txtbox("Helvetica",12.0,strlen(revtxt),revtxt,box,fontdb);

    PlotText(plot,NULL,"Helvetica",12.0,wdt-box[0],24+5*lnehgt,
             strlen(revtxt),revtxt,txtcol,0x0f,0);
    if (expr !=NULL) {
      sprintf(txt,"Expression:%s",expr);
      txtbox("Helvetica",12.0,strlen(txt),txt,box,fontdb);
      PlotText(plot,NULL,"Helvetica",12.0,0.75*wdt-box[0]/2,24+5*lnehgt,
               strlen(txt),txt,txtcol,0x0f,0);

    }

    if (magflg) sprintf(txt,"Geomagnetic");
    else sprintf(txt,"Geographic");
    txtbox("Helvetica",12.0,strlen(txt),txt,box,fontdb);
    PlotText(plot,NULL,"Helvetica",12.0,(wdt-box[0])/2,24+1*lnehgt,
             strlen(txt),txt,txtcol,0x0f,0);

    if (sflg) {
      if (scan !=0) sprintf(txt,"Scan Type:%d",scan);
      else sprintf(txt,"Scan Type:Regular");
      txtbox("Helvetica",12.0,strlen(txt),txt,box,fontdb);
      PlotText(plot,NULL,"Helvetica",12.0,(wdt-box[0])/2,24+3*lnehgt,
               strlen(txt),txt,txtcol,0x0f,0);

    }

    if (cpid !=-1) {
      sprintf(txt,"Program ID Plotted:%d",cpid);
      txtbox("Helvetica",12.0,strlen(txt),txt,box,fontdb);
      PlotText(plot,NULL,"Helvetica",12.0,(wdt-box[0])/2,24+4*lnehgt,
               strlen(txt),txt,txtcol,0x0f,0);
    }

    if (channel !=0) {
      if (channel==1) sprintf(txt,"Channel:A");
      else sprintf(txt,"Channel:B");
      txtbox("Helvetica",12.0,strlen(txt),txt,box,fontdb);
      PlotText(plot,NULL,"Helvetica",12.0,(wdt-box[0])/2,24+5*lnehgt,
               strlen(txt),txt,txtcol,0x0f,0);
    }

    PlotPlotEnd(plot);
    PlotDocumentEnd(plot);
    if (gflg) { /* image file */
#ifdef _XLIB_
      if (xd !=0) {
        XwinFrameBufferWindow(img,win);
        xstat=XwinDisplayEvent(dp,1,&win,1,NULL);
      } else {
        if (xmlflg) FrameBufferSaveXML(img,stream,stdout);
        else if (ppmflg) FrameBufferSavePPM(img,stdout);
        else if (ppmxflg) FrameBufferSavePPMX(img,stdout);
        else FrameBufferSavePNG(img,stdout);
      }
#else
      if (xmlflg) FrameBufferSaveXML(img,stream,stdout);
      else if (ppmflg) FrameBufferSavePPM(img,stdout);
      else if (ppmxflg) FrameBufferSavePPMX(img,stdout);
      else FrameBufferSavePNG(img,stdout);
#endif
      FrameBufferFree(img);
      img=NULL;
    }
  }

#ifdef _XLIB_
  if (xd !=0) {
    XwinFreeWindow(win);
    XwinCloseDisplay(dp);
  }
#endif
  return 0;

}

