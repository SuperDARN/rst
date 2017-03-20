/* padpng.c
   ======== */

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

int main(int argc,char *argv[]) {
  
  
  int arg=0;
  FILE *fp;
  unsigned int bgcolor=0;
  char *bgtxt=NULL; 
  struct FrameBuffer *src=NULL;
  struct FrameBuffer *dst=NULL;
 
 

  int wdt=-1;
  int hgt=-1;
  int xpad=0;
  int ypad=0;

  unsigned char help=0;
  unsigned char option=0;


  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"-option",'x',&option);


  OptionAdd(&opt,"wdt",'i',&wdt);
  OptionAdd(&opt,"hgt",'i',&hgt);
  OptionAdd(&opt,"xpad",'i',&xpad);
  OptionAdd(&opt,"ypad",'i',&ypad);

  OptionAdd(&opt,"bgcol",'t',&bgtxt);


  arg=OptionProcess(1,argc,argv,&opt,NULL);   

  if (help==1) {
    OptionPrintInfo(stdout,hlpstr);
    exit(0);
  }

  if (option==1) {
    OptionDump(stdout,&opt);
    exit(0);
  }

  bgcolor=FrameBufferColor(0x00,0x00,0x00,0xff);

  if (bgtxt !=NULL) {
     sscanf(bgtxt,"%x",&bgcolor);
     bgcolor=bgcolor | 0xff000000;
  }

  if (arg<argc) {
    fp=fopen(argv[arg],"r");
    if (fp==NULL) {
      fprintf(stderr,"Error opening file.\n");
      exit(-1);
    }
  } else fp=stdin;

  while ((src=FrameBufferLoadPNG(fp,NULL)) !=NULL) {

    if (dst==NULL) {
      if (wdt==-1) wdt=src->wdt;
      if (hgt==-1) hgt=src->hgt;
      dst=FrameBufferMake(NULL,wdt,hgt,src->depth);
    }

    FrameBufferClear(dst,bgcolor,0x0f);  
    FrameBufferImage(dst,NULL,src,0x0f,xpad,ypad,0,NULL);  
    FrameBufferSavePNG(dst,stdout); 
    FrameBufferFree(src);
    src=NULL;
  }

  if (arg<argc) fclose(fp);


  return 0;
}




