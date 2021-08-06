/* padppmx.c
   ========= */

/*
 LICENSE AND DISCLAIMER
 
 Copyright (c) 2012 The Johns Hopkins University/Applied Physics Laboratory
 
 This file is part of the Radar Software Toolkit (RST).
 
 RST is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with RST.  If not, see <http://www.gnu.org/licenses/>.
 
 
 
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
  fclose(fp);
  return 0;
}

int rst_opterr(char *txt) {
  fprintf(stderr,"Option not recognized: %s\n",txt);
  fprintf(stderr,"Please try: padxml --help\n");
  return(-1);
}

int main(int argc,char *argv[]) {
  
  
  int arg=0,s;
  FILE *fp;
  unsigned int bgcolor=0;
  char *bgtxt=NULL; 
  struct FrameBuffer *src=NULL;
  struct FrameBuffer *dst=NULL;
 
 
  char lbuf[256];
  struct XMLdata *xmldata=NULL;
  struct FrameBufferXML xmlimg;



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


  arg=OptionProcess(1,argc,argv,&opt,rst_opterr);

  if (arg==-1) {
    exit(-1);
  }

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


  if (dst==NULL) {
    if (wdt==-1) wdt=src->wdt;
    if (hgt==-1) hgt=src->hgt;
    dst=FrameBufferMake(NULL,wdt,hgt,src->depth);
  }

  FrameBufferClear(dst,bgcolor,0x0f);  
  FrameBufferImage(dst,NULL,src,0x0f,xpad,ypad,0,NULL);  
  FrameBufferSaveXML(dst,stream,stdout); 
 
  return 0;
}




