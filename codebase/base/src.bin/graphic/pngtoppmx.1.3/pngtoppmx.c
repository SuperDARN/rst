/* pngtoppmx.c
   =========== */

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
  struct FrameBuffer *img=NULL;
  char *name=NULL;  

  unsigned char help=0;
  unsigned char option=0;
 
  OptionAdd(&opt,"-help",'x',&help);  
  OptionAdd(&opt,"-option",'x',&option);
  OptionAdd(&opt,"name",'t',&name);


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

  img=FrameBufferLoadPNG(fp,name);
 
  if (img==NULL) {
    fprintf(stderr,"Error decoding image.\n");
    exit(-1);
  }
  if (arg<argc) fclose(fp);
  if (img !=NULL) FrameBufferSavePPMX(img,stdout);
  return 0;
}




