/* DataMapWrite.c
   ==============
   Author: R.J.Barnes */


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


#include "rtypes.h"
#include "dmap.h"



int main(int argc,char *argv[]) {

  int t,n;

  float set[3]={0.1,0.2,10.2};
  int value;

  int32 dim=1;
  int32 rng[1]={3};

  struct DataMap *dmap;

  int fid=0;
  int mask=S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;

  dmap=DataMapMake();

  DataMapAddArray(dmap,"set",DATAFLOAT,dim,rng,&set);
  DataMapAddScalar(dmap,"value",DATAINT,&value);

  fid=open("test.dmap",O_WRONLY | O_TRUNC | O_CREAT,mask);
  

  for (t=0;t<100;t++) {
    for (n=0;n<3;n++) set[n]=set[n]+0.1;
    DataMapWrite(fid,dmap);
  }
  DataMapFree(dmap);
  close(fid);
  return 0;

}
