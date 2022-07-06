/* Copyright (C)  2021 SuperDARN Canada, University of Saskatchwan 
 * Author: Marina Schmidt
 *
 * Modifications:
 *      2022-03-01 Marina Schmidt (USASK), switching intt to int 16  
 * Disclaimer:
 * 
 * This file is part of the Radar Software Toolkit (RST).
 * 
 * RST is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */ 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <zlib.h>
#include "rtypes.h"
#include "rconvert.h"
#include "dmap.h"
#include "lmt.h"
#include "dat.h"

/* DatToDmap
 * converts dat structure to DataMap structure
 * Params:
 *  DataMap ptr - Dmap structure found in dmap.h
 *  DatData dat - Dat structure for storing all fields found in dat.h
 */
int DatToDmap(struct DataMap *ptr, struct DatData *dat) {
    int tx;
    int c,d,x;
    int32 p0num,snum;
    int32 anum[3],xnum[3];

    float *acfd=NULL;
    float *xcfd=NULL;

    int n;
    int16 *pulse=NULL,*lag=NULL;
    int32 pnum;
    int32 lnum[2];

    pnum=dat->PARMS.MPPUL;
    lnum[0]=2;
    lnum[1]=dat->PARMS.MPLGS+1;
    
    // Dat Scalar parameters
    DataMapAddScalar(ptr,"radar.revision.major",DATACHAR,&dat->PARMS.REV.MAJOR);
    DataMapAddScalar(ptr,"radar.revision.minor",DATACHAR,&dat->PARMS.REV.MINOR);
    DataMapAddScalar(ptr,"nparm",DATACHAR,&dat->PARMS.NPARM); 
    DataMapAddScalar(ptr,"cp",DATASHORT,&dat->PARMS.CP);
    DataMapAddScalar(ptr,"stid",DATASHORT,&dat->PARMS.ST_ID);
    DataMapAddScalar(ptr,"time.yr",DATASHORT,&dat->PARMS.YEAR);
    DataMapAddScalar(ptr,"time.mo",DATASHORT,&dat->PARMS.MONTH);
    DataMapAddScalar(ptr,"time.dy",DATASHORT,&dat->PARMS.DAY);
    DataMapAddScalar(ptr,"time.hr",DATASHORT,&dat->PARMS.HOUR);
    DataMapAddScalar(ptr,"time.mt",DATASHORT,&dat->PARMS.MINUT);
    DataMapAddScalar(ptr,"time.sc",DATASHORT,&dat->PARMS.SEC);
    DataMapAddScalar(ptr,"txpow",DATASHORT,&dat->PARMS.TXPOW);
    DataMapAddScalar(ptr,"nave",DATASHORT,&dat->PARMS.NAVE);
    DataMapAddScalar(ptr,"atten",DATASHORT,&dat->PARMS.ATTEN);
    DataMapAddScalar(ptr,"lagfr",DATASHORT,&dat->PARMS.LAGFR);
    DataMapAddScalar(ptr,"smsep",DATASHORT,&dat->PARMS.SMSEP);
    DataMapAddScalar(ptr,"ercod",DATASHORT,&dat->PARMS.ERCOD);
    DataMapAddScalar(ptr,"stat.agc",DATASHORT,&dat->PARMS.AGC_STAT);
    DataMapAddScalar(ptr,"stat.lopwr",DATASHORT,&dat->PARMS.LOPWR_STAT);
    DataMapAddScalar(ptr,"nbaud",DATASHORT,&dat->PARMS.NBAUD);
    DataMapAddScalar(ptr,"noise",DATAINT,&dat->PARMS.NOISE);
    DataMapAddScalar(ptr,"noise.mean",DATAINT,&dat->PARMS.NOISE_MEAN);

    DataMapAddScalar(ptr,"channel",DATASHORT,&dat->PARMS.CHANNEL);
    DataMapAddScalar(ptr,"rxrise",DATASHORT,&dat->PARMS.RXRISE);
    DataMapAddScalar(ptr,"bmnum",DATASHORT,&dat->PARMS.BMNUM);

    DataMapAddScalar(ptr,"usr_resL1",DATAINT,&dat->PARMS.usr_resL1);
    DataMapAddScalar(ptr,"usr_resL2",DATAINT,&dat->PARMS.usr_resL2);
    DataMapAddScalar(ptr,"intt",DATASHORT,&dat->PARMS.INTT);

    DataMapAddScalar(ptr,"usr_resS1",DATASHORT,&dat->PARMS.usr_resS1);
    DataMapAddScalar(ptr,"usr_resS2",DATASHORT,&dat->PARMS.usr_resS2);
    DataMapAddScalar(ptr,"usr_resS3",DATASHORT,&dat->PARMS.usr_resS3);

    DataMapAddScalar(ptr,"txpl",DATASHORT,&dat->PARMS.TXPL);
    DataMapAddScalar(ptr,"mpinc",DATASHORT,&dat->PARMS.MPINC);
    DataMapAddScalar(ptr,"mppul",DATASHORT,&dat->PARMS.MPPUL);
    DataMapAddScalar(ptr,"mplgs",DATASHORT,&dat->PARMS.MPLGS);

    DataMapAddScalar(ptr,"nrang",DATASHORT,&dat->PARMS.NRANG);
    DataMapAddScalar(ptr,"frang",DATASHORT,&dat->PARMS.FRANG);
    DataMapAddScalar(ptr,"rsep",DATASHORT,&dat->PARMS.RSEP);
    DataMapAddScalar(ptr,"xcf",DATASHORT,&dat->PARMS.XCF);
    DataMapAddScalar(ptr,"tfreq",DATASHORT,&dat->PARMS.TFREQ);
    DataMapAddScalar(ptr,"scan",DATASHORT,&dat->PARMS.SCAN);

    DataMapAddScalar(ptr,"mxpwr",DATAINT,&dat->PARMS.MXPWR);
    DataMapAddScalar(ptr,"lvmax",DATAINT,&dat->PARMS.LVMAX);
    DataMapAddScalar(ptr,"combf",DATASTRING,dat->COMBF);
    
    // Adding Dat array parameters
    pulse=(int16 *) DataMapStoreArray(ptr,"ptab",DATASHORT,1,&pnum,NULL);
    if (pulse==NULL) 
        return -1;
    
    lag = (int16 *) DataMapStoreArray(ptr,"ltab",DATASHORT,2,lnum,NULL);
    if (lag==NULL) 
        return -1;

    for (n=0;n<dat->PARMS.MPPUL;n++) 
        pulse[n]=dat->PULSE_PATTERN[n];
    
    for (n=0;n<2;n++){
        for (x=0;x<=dat->PARMS.MPLGS;x++){
            lag[2*x+n]=dat->LAG_TABLE[n][x];
        }
    }

    // number of scalars
    snum=0;
    tx=0;

    for (c=0;c<dat->PARMS.NRANG;c++) {
        if (dat->pwr0[c]<tx) 
            continue;
        snum++;
    }

    p0num=dat->PARMS.NRANG;
    // number of array
    anum[0]=2;
    anum[1]=dat->PARMS.MPLGS;
    anum[2]=snum;

    if (dat->PARMS.XCF !=0) {
        xnum[0]=2;
        xnum[1]=dat->PARMS.MPLGS;
        xnum[2]=snum;
    } else {
        xnum[0]=0;
        xnum[1]=0;
        xnum[2]=0;
    }

    if (dat->acfd[0] == NULL) {
        DataMapStoreArray(ptr,"pwr0",DATAFLOAT,1,&p0num,dat->pwr0);
        fprintf(stderr,"Warning: acfd array missing\n");
        return 0;
    }
    
    DataMapStoreArray(ptr,"pwr0",DATAFLOAT,1,&p0num,dat->pwr0);
    if (snum !=0) {
        acfd=DataMapStoreArray(ptr,"acfd",DATAFLOAT,3,anum,NULL);
        if (dat->PARMS.XCF !=0) 
            xcfd=DataMapStoreArray(ptr,"xcfd",DATAFLOAT,3,xnum,NULL);
        for (c=0;c<dat->PARMS.NRANG;c++) {
            if (dat->pwr0[c]<tx) 
                continue;
            for (d=0; d<dat->PARMS.MPLGS; d++) { 
                // Converting a 3 dimensional array to 1 dimensional by flattening
                acfd[2*(c*dat->PARMS.MPLGS+d)] = dat->acfd[c][d][0];
                acfd[2*(c*dat->PARMS.MPLGS+d)+1] = dat->acfd[c][d][1];
            }
            if (dat->PARMS.XCF !=0) {
                // Converting a 3 dimensional array to 1 dimensional by flattening
                for (d=0; d<dat->PARMS.MPLGS; d++) {
                    xcfd[2*(c*dat->PARMS.MPLGS+d)] = dat->xcfd[c][d][0];
                    xcfd[2*(c*dat->PARMS.MPLGS+d)+1] = dat->xcfd[c][d][1];
                }
            }
        }
    }
    return 0;
}
