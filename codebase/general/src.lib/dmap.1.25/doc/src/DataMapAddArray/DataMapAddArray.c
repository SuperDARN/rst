/* DataMapAddArray.c
   =================
   Author: R.J.Barnes */


#include <stdio.h>
#include <stdlib.h>

#include "rtypes.h"
#include "dmap.h"



int main(int argc,char *argv[]) {

  int t,n;

  float set[3]={0.1,0.2,10.2};

  int32 dim=1;
  int32 rng[1]={3};

  struct DataMap *dmap;

  dmap=DataMapMake();

  DataMapAddArray(dmap,"set",DATAFLOAT,dim,rng,&set);

  for (t=0;t<100;t++) {
    for (n=0;n<3;n++) set[n]=set[n]+0.1;
    DataMapFwrite(stdout,dmap);
  }
  DataMapFree(dmap);
  return 0;

}
