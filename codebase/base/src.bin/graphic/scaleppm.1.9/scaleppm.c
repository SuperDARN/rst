/* scaleppm.c
   ========== */

/*
   See license.txt
*/


#include <stdio.h>
#include <stdlib.h>
#include "option.h"
#include "rxml.h"
#include "rfbuffer.h"

#include "hlpstr.h"





struct OptionData opt;


unsigned int ColorStringRGB(char *txt) {
  unsigned int colval;
  if (txt==NULL) return 0;
  sscanf(txt,"%x",&colval);
  return colval;
}

int main(int argc,char *argv[]) {
  
  int arg=0;
  FILE *fp;
  struct FrameBuffer *src=NULL;
  struct FrameBuffer *dst=NULL;
  struct FrameBufferMatrix m;
 
  unsigned char sflg=0;
  float wdt=-1;
  float hgt=-1;
  float sf=1.0;

  char *bgcol_txt=NULL;
  int bgcol=0x000000;

  unsigned char help=0;
  unsigned char option=0;

 
  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"-option",'x',&option);


  OptionAdd(&opt,"smooth",'x',&sflg);
  OptionAdd(&opt,"wdt",'f',&wdt);
  OptionAdd(&opt,"hgt",'f',&hgt);
  OptionAdd(&opt,"sf",'f',&sf);
  OptionAdd(&opt,"bgcol",'t',&bgcol_txt);
 
  
  arg=OptionProcess(1,argc,argv,&opt,NULL);   

  if (help==1) {
    OptionPrintInfo(stdout,hlpstr);
    exit(0);
  }

  if (option==1) {
    OptionDump(stdout,&opt);
    exit(0);
  }


  if (bgcol_txt !=NULL) bgcol=ColorStringRGB(bgcol_txt);

  if (arg<argc) {
    fp=fopen(argv[arg],"r");
    if (fp==NULL) {
      fprintf(stderr,"Error opening file.\n");
      exit(-1);
    }
  } else fp=stdin;

  while ((src=FrameBufferLoadPPM(fp,NULL)) !=NULL) {

    if (dst==NULL) {
      if ((wdt==-1) && (hgt==-1)) {
         wdt=src->wdt*sf;
         hgt=src->hgt*sf;
      } else if (wdt==-1) {
         wdt=src->wdt*hgt/src->hgt;
      } else if (hgt==-1) {
         hgt=src->hgt*wdt/src->wdt;
      }
      if ((wdt==0) || (hgt==0)) {
        fprintf(stderr,"Invalid image size.\n");
        exit(-1);
      }
      dst=FrameBufferMake(NULL,wdt,hgt,src->depth);
    } 

    FrameBufferClear(dst,bgcol,0x0f);


    if ((wdt==src->wdt) && (hgt==src->hgt))
      FrameBufferImage(dst,NULL,src,0xf0,0,0,0,NULL);  
    else {
      float xsc=wdt/src->wdt;
      float ysc=hgt/src->hgt;
      m.a=xsc;
      m.b=0;
      m.c=0;
      m.d=ysc;
      FrameBufferImage(dst,&m,src,0xf0,0,0,sflg,NULL); 
    }
    FrameBufferSavePPM(dst,stdout); 
    FrameBufferFree(src);
    src=NULL;
  }

  if (arg<argc) fclose(fp);



  return 0;
}




