/* ppmtopng.c
   ========== */

/*
   See license.txt
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "option.h"
#include "rxml.h"
#include "rfbuffer.h"

#include "hlpstr.h"

struct OptionData opt;

int main(int argc,char *argv[]) {
  unsigned int color=0;
  unsigned int alpha=255;
  unsigned char nonzero=0;
  int arg=0;
  int c=0,v=0;
  FILE *fp=NULL;
  FILE *afp=NULL;
  char *name=NULL;
  char *atxt=NULL;
  char *afname=NULL;
  struct FrameBuffer *img=NULL;
  struct FrameBuffer *msk=NULL;  


  unsigned char help=0;
  unsigned char option=0;

 
  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"-option",'x',&option);

  OptionAdd(&opt,"name",'t',&name);
  OptionAdd(&opt,"alpha",'t',&atxt);
  OptionAdd(&opt,"afname",'t',&afname);

  OptionAdd(&opt,"nz",'x',&nonzero);
  
  arg=OptionProcess(1,argc,argv,&opt,NULL);   

  if (help==1) {
    OptionPrintInfo(stdout,hlpstr);
    exit(0);
  }

  if (option==1) {
    OptionDump(stdout,&opt);
    exit(0);
  }

  if (arg<argc) {
    fp=fopen(argv[arg],"r");
    if (fp==NULL) {
      fprintf(stderr,"Error opening file.\n");
      exit(-1);
    }
  } else fp=stdin;

  if (afname !=NULL) {
    afp=fopen(afname,"r");
     if (afp==NULL) {
      fprintf(stderr,"Error opening alpha file.\n");
      exit(-1);
    }   
  }


  if (atxt !=NULL) {
    int alen;
    alen=strlen(atxt);

    if ((alen>2) && (atxt[0]=='0') && (atxt[1]=='x'))
       sscanf(atxt+2,"%x",&alpha);
    else if (atxt[alen-1]=='%') {
      float aflt;
      atxt[alen-1]=0;
      sscanf(atxt,"%g",&aflt);
      if (aflt<0) aflt=0;
      if (aflt>100) aflt=100;
      alpha=255*aflt/100.0;
    } else alpha=atoi(atxt);
    if (alpha>255) alpha=255;
  }
  color=FrameBufferColor(0,0,0,alpha);

  while ((img=FrameBufferLoadPPM(fp,name)) !=NULL) {

    if (afp !=NULL) msk=FrameBufferLoadPPM(afp,name);

    FrameBufferClear(img,color,0x08);
    if (nonzero) {
      for (c=0;c<img->wdt*img->hgt;c++) {
        v=img->img[c]+img->img[c+img->wdt*img->wdt]+
          img->img[c+2*img->wdt*img->wdt];
        if (v==0) img->msk[c]=0;
      }
    }
    if (msk !=NULL) memcpy(img->msk,msk->img,img->wdt*img->hgt);

    FrameBufferSavePNG(img,stdout);
    FrameBufferFree(img);
    if (msk !=NULL) FrameBufferFree(msk);
    img=NULL;
    msk=NULL;
  }


  return 0;
}




