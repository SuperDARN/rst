/* AACGMConvert.c
   ==============
   Author: R.J.Barnes
*/


#include <stdio.h>
#include <stdlib.h>
#include "aacgm.h"


int main(int argc,char *argv[]) {

  double ilat,ilon,hgt;
  double olat,olon,r;
  int flg;
  int s;

  ilat=85.0;
  ilon=45.0;
  hgt=150.0;

  flg=0;
  
  s=AACGMConvert(ilat,ilon,hgt,&olat,&olon,&r,flg);
  if (s !=0) {
    fprintf(stderr,"Error in AACGM conversion.\n");
    exit(-1);
  }
  fprintf(stdout,"AACGMConvert\n");
  fprintf(stdout,"Input: lat=%g, lon=%g, height=%g\n",ilat,ilon,hgt);
  fprintf(stdout,"Output: lat=%g, lon=%g\n",olat,olon);
  return 0;
}
