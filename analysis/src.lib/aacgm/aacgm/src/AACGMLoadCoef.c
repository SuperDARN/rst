/* AACGMLoadCoef.c
   ===============
   Author: R.J.Barnes
*/


#include <stdio.h>
#include <stdlib.h>
#include "aacgm.h"


int main(int argc,char *argv[]) {
  char *fname=NULL;
  double ilat,ilon,hgt;
  double olat,olon,r;
  int flg;
  int s;

  ilat=85.0;
  ilon=45.0;
  hgt=150.0;

  flg=0;
  
  if (argc<2) {
    fprintf(stderr,"Require coefficient filename.\n");
    exit(-1);
  }
  fname=argv[1];

  s=AACGMLoadCoef(fname);
  if (s !=0) {
    fprintf(stderr,"Error loading coefficients.\n");
    exit(-1);
  }
  s=AACGMConvert(ilat,ilon,hgt,&olat,&olon,&r,flg);
  if (s !=0) {
    fprintf(stderr,"Error in AACGM conversion.\n");
    exit(-1);
  }
  fprintf(stdout,"AACGMLoadCoef\n");
  fprintf(stdout,"Coefficient file:%s\n",fname);
  fprintf(stdout,"Input: lat=%g, lon=%g, height=%g\n",ilat,ilon,hgt);
  fprintf(stdout,"Output: lat=%g, lon=%g\n",olat,olon);
  return 0;
}
