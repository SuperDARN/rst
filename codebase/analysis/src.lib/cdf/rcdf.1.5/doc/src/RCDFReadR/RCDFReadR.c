/* RCDFReadR.c
   ===========
   Author: R.J.Barnes
*/


#include <stdio.h>
#include <stdlib.h>
#include <cdf.h>
#include "rcdf.h"


int main(int argc,char *argv[]) {
  char *varname[256];
  struct RCDFData vardata[256];

  int n,recno;
  CDFid id;
  CDFstatus status;
  int max;

  fprintf(stdout,"RCDFReadR\n");
  if (argc<4) {
    fprintf(stderr,"Filename, record number and variable must be given.\n");
    exit(-1);
  } 

  recno=atoi(argv[2]);
  for (n=0;n<argc-3;n++) varname[n]=argv[n+3];
  varname[argc-3]=0;

 
  status=CDFopen(argv[1],&id);
  if (status !=CDF_OK) {
     fprintf(stderr,"Could not open cdf file.\n");
     exit(-1);
  }

  max=RCDFReadR(id,recno,varname,vardata);

  CDFclose(id);
  
  for (n=0;n<argc-3;n++)
    fprintf(stdout,"%s %d %d %d %d\n",vardata[n].name,(int) vardata[n].status,
	    vardata[n].num,vardata[n].type,vardata[n].numdim);
  
  return 0;
}
