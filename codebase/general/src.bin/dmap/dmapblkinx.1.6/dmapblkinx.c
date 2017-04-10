/* dmapblkinx.c
   ============
   Author: R.J.Barnes
*/


/*
   See license.txt
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <zlib.h>
#include "rtypes.h"
#include "rconvert.h"
#include "option.h"
#include "dmap.h"
#include "hlpstr.h"




struct OptionData opt;
int arg=0;

int main(int argc,char *argv[]) {
  struct DataMap *ptr;

  unsigned char help=0;
  unsigned char option=0;
  unsigned char zflg=0;


  FILE *fp=NULL;
  gzFile zfp=0;

  int sze=0;

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

  if (arg !=argc) {
    if (zflg) {
      zfp=gzopen(argv[arg],"r");
      if (zfp==0) {
        fprintf(stderr,"File not found.\n");
        exit(-1);
      }
    } else {
      fp=fopen(argv[arg],"r");
      if (fp==NULL) {
        fprintf(stderr,"File not found.\n");
        exit(-1);
      }
    }  
  } else {
    if (zflg) zfp=gzdopen(fileno(stdin),"r");
    else fp=stdin; 
  }


  sze=0;   
  while (1) {

    if (zflg) ptr=DataMapReadZ(zfp);
    else ptr=DataMapFread(fp);

    if (ptr==NULL) break;

    ConvertFwriteInt(stdout,sze);
    sze+=DataMapSize(ptr);
    DataMapFree(ptr);

  }
  if (zflg) gzclose(zfp);
  else fclose(fp);
  return 0;  


}
