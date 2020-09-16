/* fitpacket.c
   =========== 
   Author: R.J.Barnes
*/


/*
 LICENSE AND DISCLAIMER
 
 Copyright (c) 2012 The Johns Hopkins University/Applied Physics Laboratory
 
 This file is part of the Radar Software Toolkit (RST).
 
 RST is free software: you can redistribute it and/or modify
 it under the terms of the GNU Lesser General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 any later version.
 
 RST is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU Lesser General Public License for more details.
 
 You should have received a copy of the GNU Lesser General Public License
 along with RST.  If not, see <http://www.gnu.org/licenses/>.
 
 
 
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
