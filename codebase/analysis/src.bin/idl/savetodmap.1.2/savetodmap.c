/* savetodmap.c
   ============ 
   Author R.J.Barnes
*/

/*
   See license.txt
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <zlib.h>
#include <stdint.h>
#include "rtypes.h"
#include "rconvert.h"
#include "option.h"
#include "dmap.h"
#include "idlsave.h"

#include "hlpstr.h"

struct OptionData opt;
int arg=0;

int main(int argc,char *argv[]) {

  unsigned char zflg=0;
  unsigned char help=0;
  unsigned char option=0;

  struct DataMap *dmap=NULL;
  struct IDLFile *fp=NULL;

  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"-option",'x',&option);
  OptionAdd(&opt,"z",'x',&zflg);

  arg=OptionProcess(1,argc,argv,&opt,NULL);
  if (help==1) {
    OptionPrintInfo(stdout,hlpstr);
    exit(0);
  }

  if (option==1) {
    OptionDump(stdout,&opt);
    exit(0);
  }

  if (arg !=argc) fp=IDLOpen(argv[arg],zflg);
  else fp=IDLFdopen(fileno(stdin),zflg);

  dmap=IDLReadSave(fp);

  if (dmap==NULL) return 0;

  if (zflg) {
    gzFile gz=0;
    gz=gzdopen(fileno(stdout),"w");
    DataMapWriteZ(gz,dmap);
    gzclose(gz);
  } else DataMapFwrite(stdout,dmap);
  
  return 0;


}
