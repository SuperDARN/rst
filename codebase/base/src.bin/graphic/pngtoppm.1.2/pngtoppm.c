/* pngtoppm.c
   =========== */

/*
   See license.txt
*/


#include <stdio.h>
#include <stdlib.h>
#include "option.h"
#include "rfbuffer.h"

#include "hlpstr.h"





struct OptionData opt;

int main(int argc,char *argv[]) {
  
  int arg=0;
  FILE *fp;
  struct FrameBuffer *src=NULL;
  struct FrameBuffer *dst=NULL;

  unsigned int bgcolor=0;
  char *bgtxt=NULL;

  unsigned alpha=0;

  unsigned char help=0;
  unsigned char option=0;

 
  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"-option",'x',&option);

  OptionAdd(&opt,"bgcol",'t',&bgtxt);
  OptionAdd(&opt,"alpha",'x',&alpha);
   
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

  bgcolor=FrameBufferColor(0x00,0x00,0x00,0x00);

  if (bgtxt !=NULL) {
     sscanf(bgtxt,"%x",&bgcolor);
  }

  while ((src=FrameBufferLoadPNG(fp,"image")) !=NULL) {
 
    if (bgcolor !=0) {
      if (dst==NULL) {
        dst=FrameBufferMake(NULL,src->wdt,src->hgt,24);
         if (dst==NULL) {
          fprintf(stderr,"Error creating image.\n");
          exit(-1);
         }
      }
      FrameBufferClear(dst,bgcolor,0x0f);
      FrameBufferImage(dst,NULL,src,0x0f,0,0,0,NULL); 
      if (alpha==0) FrameBufferSavePPM(dst,stdout);
      else  FrameBufferSavePPMAlpha(dst,stdout);
    } else {
      if (alpha==0) FrameBufferSavePPM(src,stdout);
      else FrameBufferSavePPMAlpha(src,stdout);
    }
    FrameBufferFree(src);
    src=NULL;
  }


  if (arg<argc) fclose(fp);



  return 0;
}




