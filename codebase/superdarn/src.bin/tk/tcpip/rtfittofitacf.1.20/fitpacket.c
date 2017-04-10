/* fitpacket.c
   =========== 
   Author: R.J.Barnes
*/


/*
   See license.txt
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <zlib.h>
#include "rtypes.h"
#include "dmap.h"
#include "rtime.h"
#include "dmap.h"
#include "rprm.h"
#include "fitdata.h"
#include "fitwrite.h"



unsigned char *fitpacket(struct RadarParm *prm,
              struct FitData *fit,int *size) {

  int sze=0;
  unsigned char *buf;
  struct DataMap *data;
 
  data=DataMapMake();
  RadarParmEncode(data,prm);
  FitEncode(data,prm,fit);
  buf=DataMapEncodeBuffer(data,&sze);
  if (size !=NULL) *size=sze;
  DataMapFree(data);
  return buf;
}
