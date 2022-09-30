/* vec_plot.c
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

#ifdef _XLIB_
#include "xwin.h"
#endif
#include "rtime.h"
#include "rmap.h"
#include "aacgm.h"
#include "mlt.h"
#include "aacgmlib_v2.h"
#include "mlt_v2.h"

#include "radar.h"
#include "calcvector.h"

#include "text_box.h"
#include "plot_circle.h"
#include "plot_radial.h"
#include "plot_time.h"

#include "errstr.h"
#include "hlpstr.h"
#include "version.h"



struct OptionData opt;
struct OptionFile *optf=NULL;

struct vec {
  float mlon,mlat;
  int type,qflg;
  float Veast,Vpole,VEeast,VEpole;
};

struct vecdata {
  char name[64];
  double st_time;
  double ed_time;
  int npnt;
  struct vec *data;
};
  

double mlon_av_val=0;
int mlon_av_cnt=0;
struct vecdata vec;
int gcnt[360/5][90-60];

struct vec *grid[360/5][90-60];

char *dte_fnt_nme={"Helvetica"};
float dte_fnt_sze=8.0;

char *tme_fnt_nme={"Helvetica"};
float tme_fnt_sze=8.0;

char *lgo_fnt_nme={"Helvetica"};
float lgo_fnt_sze=14.0;

void label_logo(struct Plot *plot,
	        float xoff,float yoff,float wdt,float hgt,
                char *fontname,float fontsize,unsigned int color,
                unsigned char mask,
                void *txtdata) {
  float txbox[3];
  char txt[256]; 
  sprintf(txt,"SuperDARN");

  txtbox(fontname,fontsize,strlen(txt),txt,txbox,txtdata);
  PlotText(plot,NULL,fontname,fontsize,xoff,yoff+txbox[2],strlen(txt),txt,
               color,mask,1);

}

void label_web(struct Plot *plot,
	        float xoff,float yoff,float wdt,float hgt,
                char *fontname,float fontsize,unsigned int color,
	        unsigned char mask,
                void *txtdata) {
  float txbox[3];
  char txt[256]; 
  sprintf(txt,"http://superdarn.jhuapl.edu");
  txtbox(fontname,fontsize,strlen(txt),txt,txbox,txtdata);
  PlotText(plot,NULL,fontname,fontsize,xoff+wdt-txbox[0],
            yoff+txbox[2],strlen(txt),txt,
            color,mask,1);
}


int vec_sort_east(const void *a,const void *b) {
  struct vec *x;
  struct vec *y;
  x=(struct vec *) a;
  y=(struct vec *) b;

  if (x->Veast < y->Veast) return -1;
  if (x->Veast > y->Veast) return 1;
  return 0;

}

int vec_sort_pole(const void *a,const void *b) {
  struct vec *x;
  struct vec *y;
  x=(struct vec *) a;
  y=(struct vec *) b;

  if (x->Vpole < y->Vpole) return -1;
  if (x->Vpole > y->Vpole) return 1;
  return 0;

}

int read_record(FILE *fp,struct vecdata *ptr) {
  char line[1024];
  int yr,mo,dy,hr,mt,sc,syrsec,eyrsec;
  int mlat,mlon,type,qflg,Veast,Vpole,VEeast,VEpole;

  if (fgets(line,256,fp)==0) return -1;  

  if (strncmp(line,"vlptm",5)==0) {
    /* header record */
    char *tkn;
    tkn=strtok(line," ");
    tkn=strtok(NULL," ");
    tkn=strtok(NULL," ");
    tkn=strtok(NULL,"\n");
    if (tkn !=NULL) {
      int i;
      for (i=0;(tkn[i]==' ') && (tkn[i] !=0);i++);
      if (tkn[i] !=0) strcpy(ptr->name,tkn+i); 
    }
    if (fgets(line,256,fp)==0) return -1;
    if (fgets(line,256,fp)==0) return -1;
    if (fgets(line,256,fp)==0) return -1;
    if (fgets(line,256,fp)==0) return -1;
    return 1;
  }
  
  if (sscanf(line,"%d %d %d %d %d %d %d %d",&yr,&mo,&dy,&hr,&mt,&sc,
                    &syrsec,&eyrsec) !=8) return -1;

   
  TimeYrsecToYMDHMS(syrsec,yr,&mo,&dy,&hr,&mt,&sc);
  ptr->st_time=TimeYMDHMSToEpoch(yr,mo,dy,hr,mt,sc);
  TimeYrsecToYMDHMS(eyrsec,yr,&mo,&dy,&hr,&mt,&sc);
  ptr->ed_time=TimeYMDHMSToEpoch(yr,mo,dy,hr,mt,sc);
  

  if (fgets(line,256,fp)==0) return -1;
  if (fgets(line,256,fp)==0) return -1;
  if (fgets(line,256,fp)==0) return -1;
 
  if (sscanf(line,"%d %d %d %d %d %d %d %d",&mlat,&mlon,&type,&qflg,
	      &Veast,&Vpole,&VEeast,&VEpole) !=8) return -1;
  
  ptr->npnt=0; 
  if (ptr->data !=NULL) {
    free(ptr->data);
    ptr->data=NULL;
  }
  
  while ((Veast !=0) || (Vpole !=0) || (type !=0) || (qflg !=0) || 
	 (mlat !=0) || (mlon !=0)) {
    
   if (ptr->data==NULL) ptr->data=malloc(sizeof(struct vec));
   else ptr->data=realloc(ptr->data,(ptr->npnt+1)*sizeof(struct vec));

   ptr->data[ptr->npnt].mlat=mlat/100.0;
   ptr->data[ptr->npnt].mlon=mlon/100.0;
   ptr->data[ptr->npnt].type=type;
   ptr->data[ptr->npnt].qflg=qflg;
   ptr->data[ptr->npnt].Veast=Veast/10.0;
   ptr->data[ptr->npnt].Vpole=Vpole/10.0;
   ptr->data[ptr->npnt].VEeast=VEeast/10.0;
   ptr->data[ptr->npnt].VEpole=VEpole/10.0;
   ptr->npnt++;
    
   if (fgets(line,256,fp)==0) return -1;

   if (sscanf(line,"%d %d %d %d %d %d %d %d",&mlat,&mlon,&type,&qflg,
	      &Veast,&Vpole,&VEeast,&VEpole) !=8) return -1;
   
  } 
 
  return 0;
}


void filter(struct vecdata *vec,int qlevel) {
  int i,j,k,c;
  float lon,lat;
  struct vec *ptr=NULL;
  lon=(360.0*((int) vec->st_time % (24*3600)))/(24*3600);
  j=lon/5;
  if (j<0) return;
  if (j>=360/5) return;

  for (k=0;k<vec->npnt;k++) {
    if (vec->data[k].qflg>qlevel) continue;
    lat=vec->data[k].mlat;
    i=lat-60;
    if (i<0) continue;
    if (i>=40) continue;
    ptr=grid[j][i];
    mlon_av_val+=vec->data[k].mlon;
    mlon_av_cnt++;
    if (ptr==NULL) grid[j][i]=malloc(sizeof(struct vec));
    else grid[j][i]=realloc(grid[j][i],(gcnt[j][i]+1)*sizeof(struct vec));
    ptr=grid[j][i];
    c=gcnt[j][i];
    memcpy(&ptr[c],&(vec->data[k]),sizeof(struct vec));
    gcnt[j][i]++;
  }
}

void free_filter() {
  int i,j;
  for (i=0;i<(360/5);i++) {
    for (j=0;j<(90-60);j++) {
      gcnt[i][j]=0;
      if (grid[i][j] !=NULL) {
        free(grid[i][j]);
        grid[i][j]=NULL;
      }
    }
  }
}
  
int stream(char *buf,int sze,void *data) {
  FILE *fp;
  fp=(FILE *) data;
  fwrite(buf,sze,1,fp);
  return 0;
} 

int rst_opterr(char *txt) {
  fprintf(stderr,"Option not recognized: %s\n",txt);
  fprintf(stderr,"Please try: vec_plot --help\n");
  return(-1);
}

int main(int argc,char *argv[]) {


#ifdef _XLIB_
  int xdf=0;
  int xd=0;
  struct XwinDisplay *dp;
  struct XwinWindow *win;
  char *display_name=NULL;
  char *wname="vec_plot";
  int xdoff=-1;
  int ydoff=-1;
#endif

  struct FrameBuffer *img=NULL;
  struct PostScript *psdata=NULL;
  FILE *fontfp=NULL;
  char *fntdbfname=NULL;
  struct FrameBufferFontDB *fontdb=NULL;
  unsigned int bgcol;
  unsigned int grdcol;
  unsigned int txtcol;
  float lne=0.5;

  int arg;
  int fc;
  char *fname=NULL;
  char *cfname=NULL;
  FILE *fp;
  int status=0;
 
  float xoff=0,yoff=0;
  float wdt=600,hgt=600;
  float pad=15;
  float tpad=0;
  float pwdt=0;
  int npanel=0;

  float txbox[3];

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
  unsigned char logo=0;

  char *bgcol_txt=NULL;
  char *txtcol_txt=NULL;
  char *grdcol_txt=NULL;

  float marg[8]={0,0,0,0};
  int c=0;

  int old_aacgm=0;

  /* function pointer for MLT */
  double (*MLTCnv)(int, int, double);

  int yr,mo,dy,hr,mt;
  double sc;
  double mlt;
  int yrsec;
  double stime=0;


  int cnt=0;
  char str[256];
  char station[256];
  struct vec *ptr=NULL;
  double lat,lon,mag,azm;
  float Vpole,Veast;
  float scale=2.0; 
  int i,j;
  
  float map[2];
  float pnt[2];
  float ax,ay,bx,by;

  bgcol=PlotColor(255,255,255,255);
  txtcol=PlotColor(0,0,0,255);
  grdcol=PlotColor(128,128,128,255);
 
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
 
  OptionAdd(&opt,"logo",'x',&logo);

  OptionAdd(&opt,"p",'i',&npanel);

  OptionAdd(&opt,"bgcol",'t',&bgcol_txt);
  OptionAdd(&opt,"txtcol",'t',&txtcol_txt);
  OptionAdd(&opt,"grdcol",'t',&grdcol_txt);

  OptionAdd(&opt,"old_aacgm",'x',&old_aacgm);

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
    OptionPrintInfo(stdout,errstr);
    exit(0);
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
 

 
  if (bgcol_txt !=NULL) bgcol=PlotColorStringRGBA(bgcol_txt);
  if (txtcol_txt !=NULL) txtcol=PlotColorStringRGBA(txtcol_txt);
  if (grdcol_txt !=NULL) grdcol=PlotColorStringRGBA(grdcol_txt);

  /* set function pointer to compute MLT or MLT_v2 */
  if (old_aacgm) MLTCnv = &MLTConvertYrsec;
  else           MLTCnv = &MLTConvertYrsec_v2;

  if (npanel==0) npanel=argc-arg; 
  if (logo) tpad=20;

  if (npanel>1) pwdt=wdt/2;
  else pwdt=wdt;
  if (npanel>1) hgt=pwdt*(1+(npanel-1)/2)+tpad;
  else hgt=pwdt;

  if (pwdt>400) {
    pad=30;
    tme_fnt_sze=14.0;
    dte_fnt_sze=20.0;
   } else if (wdt>200) {
    tme_fnt_sze=8.0;
    dte_fnt_sze=10.0;
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

  PlotDocumentStart(plot,"vec.plot",NULL,wdt,hgt,24);
  PlotPlotStart(plot,"vec.plot",wdt,hgt,24);
  PlotRectangle(plot,NULL,0,0,wdt,hgt,1,bgcol,0x0f,0,NULL);
 
  if (logo==1) {
    label_web(plot,0,0,wdt,tpad,tme_fnt_nme,tme_fnt_sze,txtcol,0x0f,fontdb);
    label_logo(plot,0,0,wdt,tpad,lgo_fnt_nme,lgo_fnt_sze,txtcol,0x0f,fontdb);
  }

  marg[0]=90.0;
  marg[1]=0;
  marg[2]=0.8*90.0/(90-50.0);

  c=0;
  for (fc=arg;fc<argc;fc++) {
    fname=argv[fc];
    if (strcmp(fname,"stdin")==0) fp=stdin;
    else {
      fp=fopen(fname,"r");
      if (fp==NULL) continue;
      fprintf(stderr,"Plotting:%s\n",fname);
    }

    
    status=read_record(fp,&vec);
    if (status !=1) {
      fprintf(stderr,"Error reading file.\n");
      continue;
    }

    do {

      xoff=(c % 2)*pwdt;
      yoff=(c / 2)*pwdt+tpad;
     
      cnt=0;    
      
      strcpy(station,vec.name);
     
      while ( (status=read_record(fp,&vec))==0) {
        if (cnt==0) stime=vec.st_time;
		
        filter(&vec,5);
	
        cnt++;
      }
      

      TimeEpochToYMDHMS(stime,&yr,&mo,&dy,&hr,&mt,&sc);
      
      plot_circle(plot,marg,60.0,90.0,10.0,
                  xoff+pad,yoff+pad,pwdt-2*pad,pwdt-2*pad,
                  grdcol,0x0f,lne,NULL);
      plot_radial(plot,marg,60,0,359,45,xoff+pad,yoff+pad,pwdt-2*pad,
                  pwdt-2*pad,grdcol,0x0f,lne,NULL);
      
      for (i=0;i<(360/5);i++) {
        for (j=0;j<(90-60);j++) {
          if (gcnt[i][j]==0) continue;
        
          lon=i*5.0;
          lat=j+60;
          ptr=grid[i][j];
          if (gcnt[i][j]>1) { 
            qsort(ptr,gcnt[i][j],sizeof(struct vec),vec_sort_pole);
            Vpole=ptr[gcnt[i][j]/2].Vpole;
            qsort(ptr,gcnt[i][j],sizeof(struct vec),vec_sort_east);
            Veast=ptr[gcnt[i][j]/2].Veast;
          } else {
            Vpole=ptr[0].Vpole;
            Veast=ptr[0].Veast;
         }

	  
          mag=sqrt(Veast*Veast+Vpole*Vpole);
          if (mag>4000) continue;
          if (Vpole !=0) azm=atan2d(Veast,Vpole);
          else azm=0;
	  map[0]=lat;
          map[1]=lon;
	  MapStereographic(2*sizeof(float),map,2*sizeof(float),pnt,marg);
          ax=xoff+pad+pnt[0]*(pwdt-2*pad);
          ay=yoff+pad+pnt[1]*(pwdt-2*pad);

	  PlotEllipse(plot,NULL,ax,ay,
                       2,2,1,txtcol,0x0f,lne,NULL);

          RPosCalcVector(lat,lon,scale*mag,azm,&lat,&lon);
	  map[0]=lat;
          map[1]=lon;
          MapStereographic(2*sizeof(float),map,2*sizeof(float),pnt,marg);
          bx=xoff+pad+pnt[0]*(pwdt-2*pad);
          by=yoff+pad+pnt[1]*(pwdt-2*pad);
	  PlotLine(plot,ax,ay,bx,by,txtcol,0x0f,lne,NULL);
        }
      }

      yrsec=TimeYMDHMSToYrsec(yr,mo,dy,hr,mt,sc);

      if (!old_aacgm) AACGM_v2_SetDateTime(yr,mo,dy,hr,mt,(int)sc); /* required */

      if (mlon_av_cnt !=0) {
        mlon_av_val=mlon_av_val/mlon_av_cnt;

        mlt=12.0-(*MLTCnv)(yr,yrsec,mlon_av_val);
        if (mlt<0) mlt=24+mlt;
        if (mlt>24) mlt=mlt-24;

	map[0]=90.0;
	map[1]=0.0;
	MapStereographic(2*sizeof(float),map,2*sizeof(float),pnt,marg);
	ax=xoff+pad+pnt[0]*(pwdt-2*pad);
	ay=yoff+pad+pnt[1]*(pwdt-2*pad);

	map[0]=60.0;
        map[1]=mlt*360.0/24;
	MapStereographic(2*sizeof(float),map,2*sizeof(float),pnt,marg);
        bx=xoff+pad+pnt[0]*(pwdt-2*pad);
        by=yoff+pad+pnt[1]*(pwdt-2*pad);
        PlotLine(plot,ax,ay,bx,by,txtcol,0x0f,lne,NULL);
      }

      map[0]=90.0;
      map[1]=0.0;
      MapStereographic(2*sizeof(float),map,2*sizeof(float),pnt,marg);
      ax=pnt[0];
      ay=pnt[1];

      RPosCalcVector(90.0,0.0,scale*1000.0,0.0,&lat,&lon);
      map[0]=lat;
      map[1]=lon;

      MapStereographic(2*sizeof(float),map,2*sizeof(float),pnt,marg);
      bx=pnt[0];
      by=pnt[1];

      mag=sqrt((bx-ax)*(bx-ax)+(by-ay)*(by-ay));

      PlotEllipse(plot,NULL,xoff+pwdt-pwdt*0.2,yoff+pwdt*0.05,
		   2,2,1,txtcol,0x0f,lne,NULL);

      PlotLine(plot,
		xoff+pwdt-pwdt*0.2,yoff+pwdt*0.05,
	        xoff+pwdt-pwdt*0.2+mag*(pwdt-2*pad),yoff+pwdt*0.05,
                txtcol,0x0f,lne,NULL);

      txtbox(dte_fnt_nme,dte_fnt_sze,strlen("1000m/s"),"1000m/s",txbox,fontdb);
      PlotText(plot,NULL,dte_fnt_nme,dte_fnt_sze,
                xoff+pwdt-pwdt*0.2,yoff+pwdt*0.05+4+txbox[2],
                strlen("1000m/s"),"1000m/s",
                txtcol,0x0f,1);

      plot_vec_time(plot,xoff+pad,yoff+pad,pwdt-2*pad,pwdt-2*pad,
                    txtcol,0x0f,tme_fnt_nme,tme_fnt_sze,fontdb);



      txtbox(dte_fnt_nme,dte_fnt_sze,strlen("Xy"),"Xy",txbox,fontdb);


      PlotText(plot,NULL,dte_fnt_nme,dte_fnt_sze,
                xoff+pwdt*0.05,yoff+pwdt*0.05,strlen(station),station,
		txtcol,0x0f,1);
      sprintf(str,"%.2d/%.2d/%.4d",mo,dy,yr);
      PlotText(plot,NULL,dte_fnt_nme,dte_fnt_sze,
		xoff+pwdt*0.05,yoff+pwdt*0.05+4+(txbox[2]-txbox[1]),
                strlen(str),str,txtcol,0x0f,1);
      free_filter();
      c++;
      if (c>npanel) break;
    } while (status !=-1);
    fclose(fp);
    if (c>npanel) break;
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
























