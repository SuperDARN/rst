/* scalexml.c
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

int stream(char *buf,int sze,void *data) {
  FILE *fp;
  fp=(FILE *) data;
  fwrite(buf,sze,1,stdout);
  return 0;
}


int main(int argc,char *argv[]) {
  
  int s=0;
  int arg=0;
  FILE *fp;
  char lbuf[256];
  struct FrameBuffer *src=NULL;
  struct FrameBuffer *dst=NULL;
  struct XMLdata *xmldata=NULL;
  struct FrameBufferXML xmlimg;
 

  struct FrameBufferMatrix m;
 
  unsigned char sflg=0;
  float wdt=-1;
  float hgt=-1;
  float sf=1.0;

  unsigned char help=0;
  unsigned char option=0;
 

  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"-option",'x',&option);

  OptionAdd(&opt,"smooth",'x',&sflg);
  OptionAdd(&opt,"wdt",'f',&wdt);
  OptionAdd(&opt,"hgt",'f',&hgt);
  OptionAdd(&opt,"sf",'f',&sf);
 

  arg=OptionProcess(1,argc,argv,&opt,NULL);   

  if (help==1) {
    OptionPrintInfo(stdout,hlpstr);
    exit(0);
  }

  if (option==1) {
    OptionDump(stdout,&opt);
    exit(0);
  }

  xmldata=XMLMake();
  xmlimg.xml=xmldata;
  xmlimg.img=&src;

  XMLSetStart(xmldata,FrameBufferXMLStart,&xmlimg);
  XMLSetEnd(xmldata,FrameBufferXMLEnd,&xmlimg);

  if (arg<argc) {
    fp=fopen(argv[arg],"r");
    if (fp==NULL) {
      fprintf(stderr,"Error opening file.\n");
      exit(-1);
    }
  } else fp=stdin;

  while(fgets(lbuf,255,fp) !=NULL) {
    s=XMLDecode(xmldata,lbuf,strlen(lbuf));
    if (s !=0) break;
  }
  if (s !=0) {
    fprintf(stderr,"Error decoding image.\n");
    exit(-1);
  }

  if (src==NULL) {
    fprintf(stderr,"Error decoding image.\n");
    exit(-1);
  }
  if (arg<argc) fclose(fp);

  if (wdt==-1) wdt=src->wdt;
  if (hgt==-1) hgt=src->hgt;

  if ((wdt==0) || (hgt==0)) {
    fprintf(stderr,"Invalid image size.\n");
    exit(-1);
  }

  dst=FrameBufferMake(NULL,wdt,hgt,src->depth);
 
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
 
  FrameBufferSaveXML(dst,stream,stdout); 

  return 0;
}




