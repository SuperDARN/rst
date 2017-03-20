/* extract_grid.c
   ==============
   Author: R.J.Barnes
*/

/*
   See license.txt
*/


#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/types.h>
#include "rtypes.h"
#include "option.h"
#include "rtime.h"
#include "rfile.h"
#include "griddata.h"
#include "gridread.h"
#include "oldgridread.h"

#include "hlpstr.h"

struct GridData *rcd;
struct OptionData opt;




int main(int argc,char *argv[]) {
  
 /* File format transistion
   * ------------------------
   * 
   * When we switch to the new file format remove any reference
   * to "new". Change the command line option "new" to "old" and
   * remove "old=!new".
   */


  int old=0;
  int new=0;

  int arg;

  unsigned char help=0;
  unsigned char option=0;

  unsigned char rflg=0;


  FILE *fp;   
  char *fname=NULL;
  double tme;
  int yr,mo,dy,hr,mt,eyr,emo,edy;
  double sc;
  int i;

  rcd=GridMake();

  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"-option",'x',&option);

  OptionAdd(&opt,"new",'x',&new); 
  OptionAdd(&opt,"mid",'x',&rflg);

  arg=OptionProcess(1,argc,argv,&opt,NULL);

  old=!new;

  if (help==1) {
    OptionPrintInfo(stdout,hlpstr);
    exit(0);
  }
  if (option==1) {
    OptionDump(stdout,&opt);
    exit(0);
  }



  rflg=!rflg;

  if (arg<argc) fname=argv[arg];
  if (fname==NULL) fp=stdin;
  else fp=fopen(fname,"r");
  if (fp==NULL) {
    fprintf(stderr,"File not found.\n");
    exit(-1);
  }
  if (old) {
    while (OldGridFread(fp,rcd) !=-1) {
      if (rflg==0) {
        tme=(rcd->st_time+rcd->ed_time)/2.0;
        TimeEpochToYMDHMS(tme,&yr,&mo,&dy,&hr,&mt,&sc);
        fprintf(stdout,"%.4d %.2d %.2d %.2d %.2d %.2d ",
		       yr,mo,dy,hr,mt,(int) sc);
      } else {
        TimeEpochToYMDHMS(rcd->st_time,&yr,&mo,&dy,&hr,&mt,&sc);
        fprintf(stdout,"%.4d %.2d %.2d %.2d %.2d %.2d ",
	        yr,mo,dy,hr,mt,(int) sc);
        TimeEpochToYMDHMS(rcd->ed_time,&eyr,&emo,&edy,&hr,&mt,&sc);
        if ((eyr !=yr) || (emo !=mo) || (edy !=dy)) hr+=24;
        fprintf(stdout,"%.2d %.2d %.2d ",hr,mt,(int) sc);
      }
    
      fprintf(stdout,"%d %d ",rcd->vcnum,rcd->stnum);
    
      for (i=0;i<rcd->stnum;i++) fprintf(stdout,"%d ",rcd->sdata[i].npnt);
      for (i=0;i<rcd->stnum;i++) fprintf(stdout,"%d ",rcd->sdata[i].st_id);
      fprintf(stdout,"\n");
    

    }
    if (fp !=stdout) fclose(fp);
  } else {
    while (GridFread(fp,rcd) !=-1) {
      if (rflg==0) {
        tme=(rcd->st_time+rcd->ed_time)/2.0;
        TimeEpochToYMDHMS(tme,&yr,&mo,&dy,&hr,&mt,&sc);
        fprintf(stdout,"%.4d %.2d %.2d %.2d %.2d %.2d ",
		       yr,mo,dy,hr,mt,(int) sc);
      } else {
        TimeEpochToYMDHMS(rcd->st_time,&yr,&mo,&dy,&hr,&mt,&sc);
        fprintf(stdout,"%.4d %.2d %.2d %.2d %.2d %.2d ",
	        yr,mo,dy,hr,mt,(int) sc);
        TimeEpochToYMDHMS(rcd->ed_time,&eyr,&emo,&edy,&hr,&mt,&sc);
        if ((eyr !=yr) || (emo !=mo) || (edy !=dy)) hr+=24;
        fprintf(stdout,"%.2d %.2d %.2d ",hr,mt,(int) sc);
      }
    
      fprintf(stdout,"%d %d ",rcd->vcnum,rcd->stnum);
    
      for (i=0;i<rcd->stnum;i++) fprintf(stdout,"%d ",rcd->sdata[i].npnt);
      for (i=0;i<rcd->stnum;i++) fprintf(stdout,"%d ",rcd->sdata[i].st_id);
      fprintf(stdout,"\n");
    

    }
    if (fp !=stdout) fclose(fp);
  }

  return 0;
}






















