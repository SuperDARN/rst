/* 
 Copyright (c) 2021 University of Saskatchewan
 Author: Marina Schmidt
 
 Copyright (c) 2012 The Johns Hopkins University/Applied Physics Laboratory
 Copied code from raw_read.c in cmpraw modified for dat files

 This file is part of the Radar Software Toolkit (RST).

 RST is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
  
 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <https://www.gnu.org/licenses/>.
  
 Modifications:
 
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <zlib.h>
#include <errno.h>
#include "rtypes.h"
#include "dmap.h"
#include "lmt.h"
#include "dat.h"
#include "datread.h"


/*DatRead
 * helper function that passes onto DatReadCurrent
 * Params: 
 *  Datfp fp - file pointer
 *  DatData datdata - Dat data structure found in dat.h
 *  returns:
 *      integer, -1 Fail, 0 Success
 */
int DatRead(struct Datfp *fp,struct DatData *datdata) { 
  if (fp==NULL){ 
      fprintf(stderr, "Error: File pointer empty\n");
      return -1;
  }
  return (fp->datread)(fp, datdata);
}
 
/* DatReadData
 * reads only the power and ACFs without altering the radar parameters
 * Params: 
 *  Datfp fp - file pointer
 *  DatData datdata - Dat data structure found in dat.h
 *  returns:
 *      integer, -1 Fail, 0 Success
 */
int DatReadData(struct Datfp *fp,struct DatData *datdata) {
    struct DatData *dat_tmp;
    /* reads only the power and ACFs without altering the 
      radar parameters */
    if (fp==NULL){
        fprintf(stderr, "Error: File pointer empty\n");
        return -1;
    }
    dat_tmp=malloc(sizeof(struct DatData));
    if (dat_tmp==NULL) 
    {
        fprintf(stderr, "Error: Malloc returned %d on allocating memory for dat_tmp, %s\n", errno, strerror(errno));
        return -1;
    }
    if ( (fp->datread)(fp,dat_tmp) !=0) 
    {
            fprintf(stderr, "Error: Reading dat file returned %d, %s\n", errno, strerror(errno));
        return -1;
    }

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
