/* DataMapAddScalar.c
   ==================
   Author: R.J.Barnes */


#include <stdio.h>
#include <stdlib.h>

#include "rtypes.h"
#include "dmap.h"



int main(int argc,char *argv[]) {

  int t;

  int value=0;

  struct DataMap *dmap;

  dmap=DataMapMake();

  DataMapAddScalar(dmap,"value",DATAINT,&value);

  for (t=0;t<100;t++) {
    value=value+1;
    DataMapFwrite(stdout,dmap);
  }
  DataMapFree(dmap);
  return 0;

}
