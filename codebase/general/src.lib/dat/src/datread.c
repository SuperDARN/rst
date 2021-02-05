/* raw_read.c
   ==========
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
#include <zlib.h>
#include "rtypes.h"
#include "dmap.h"
#include "lmt.h"
#include "dat.h"
#include "datread.h"



int DatRead(struct Datfp *fp,struct DatData *datdata) { 
  if (fp==NULL) return -1;
  return (fp->datread)(fp, datdata);
}
 
int DatReadData(struct Datfp *fp,struct DatData *datdata) {

 struct DatData *dat_tmp;

 fprintf(stderr, "read  data\n");
 /* reads only the power and ACFs without altering the 
    radar parameters */
  if (fp==NULL) return -1;
  dat_tmp=malloc(sizeof(struct DatData));
  if (dat_tmp==NULL) return -1;
  if ( (fp->datread)(fp,dat_tmp) !=0) return -1;

  datdata->PARMS.NOISE=dat_tmp->PARMS.NOISE;
  datdata->PARMS.TFREQ=dat_tmp->PARMS.TFREQ;

  memcpy(datdata->pwr0,dat_tmp->pwr0,
         sizeof(int32)*ORIG_MAX_RANGE);

  memcpy(datdata->acfd,dat_tmp->acfd,
		 sizeof(int32)*2*LAG_TAB_LEN*ORIG_MAX_RANGE);
  memcpy(datdata->xcfd,dat_tmp->xcfd,
		 sizeof(int32)*2*LAG_TAB_LEN*ORIG_MAX_RANGE);
  free(dat_tmp); 
  return 0;
}




