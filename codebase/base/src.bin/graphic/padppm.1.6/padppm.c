/* padppm.c
   ======== */

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
#include "option.h"
#include "rxml.h"
#include "rfbuffer.h"

#include "hlpstr.h"





struct OptionData opt;

int rst_opterr(char *txt) {
  fprintf(stderr,"Option not recognized: %s\n",txt);
  fprintf(stderr,"Please try: padppm --help\n");
  return(-1);
}

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

  while ((src=FrameBufferLoadPPM(fp,NULL)) !=NULL) {

    if (dst==NULL) {
      if (wdt==-1) wdt=src->wdt;
      if (hgt==-1) hgt=src->hgt;
      dst=FrameBufferMake(NULL,wdt,hgt,src->depth);
    }

    FrameBufferClear(dst,bgcolor,0x0f);  
    FrameBufferImage(dst,NULL,src,0x0f,xpad,ypad,0,NULL);  
    FrameBufferSavePPM(dst,stdout); 
    FrameBufferFree(src);
    src=NULL;
  }

  if (arg<argc) fclose(fp);


  return 0;
}




