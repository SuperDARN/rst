/* gtablewrite.h
   =============
   Author: R.J.Barnes
*/

/*
  Copyright (c) 2012 The Johns Hopkins University/Applied Physics Laboratory

This file is part of the Radar Software Toolkit (RST).

RST is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see <https://www.gnu.org/licenses/>.

Modifications:
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <ctype.h>
#include <zlib.h>
#include "rtypes.h"
#include "rtime.h"
#include "dmap.h"
#include "radar.h"
#include "scandata.h"
#include "gtable.h"



int GridTableWrite(int fid,struct GridTable *ptr,char *logbuf,int xtd) {

    struct DataMap *data;

    int s;
    int32 stnum=1;
    int32 npnt=0;

    int16 syr,smo,sdy,shr,smt;
    double ssc;
    int16 eyr,emo,edy,ehr,emt;
    double esc;

    int yr,mo,dy,hr,mt;
    double sc;

    int16 stid[1];
    int16 chn[1];
    int16 nvec[1];
    float freq[1];
    int16 major_rev[1];
    int16 minor_rev[1];
    int16 progid[1];
    float noise_mean[1];
    float noise_sd[1];
    int16 gsct[1];
    float v_min[1];
    float v_max[1];
    float p_min[1];
    float p_max[1];
    float w_min[1];
    float w_max[1];
    float ve_min[1];
    float ve_max[1];

    int n,p;
    int size=0;
    unsigned char *buf=NULL;

    float *gmlon=NULL;
    float *gmlat=NULL;
    float *kvect=NULL;
    float *srng=NULL;
    int16 *vstid=NULL;
    int16 *vchn=NULL;
    int32 *index=NULL;
    float *vlos=NULL;
    float *vlos_sd=NULL;
    float *pwr=NULL;
    float *pwr_sd=NULL;
    float *wdt=NULL;
    float *wdt_sd=NULL;

    npnt=0;
    for (p=0;p<ptr->pnum;p++) if (ptr->pnt[p].cnt!=0) npnt++;

    stid[0]=ptr->st_id;
    chn[0]=ptr->chn;
    nvec[0]=npnt;
    freq[0]=ptr->freq;
    major_rev[0]=MAJOR_REVISION;
    minor_rev[0]=MINOR_REVISION;
    progid[0]=ptr->prog_id;
    noise_mean[0]=ptr->noise.mean;
    noise_sd[0]=ptr->noise.sd;
    gsct[0]=ptr->gsct;
    v_min[0]=ptr->min[0];
    v_max[0]=ptr->max[0];
    p_min[0]=ptr->min[1];
    p_max[0]=ptr->max[1];
    w_min[0]=ptr->min[2];
    w_max[0]=ptr->max[2];
    ve_min[0]=ptr->min[3];
    ve_max[0]=ptr->max[3];

    if (npnt !=0) {
        size=npnt*(4*sizeof(float)+sizeof(int32)+2*sizeof(int16)+
                    (2+4*xtd)*sizeof(float));
        buf=malloc(size);
        if (buf==NULL) return -1;
        gmlon=(float *) (buf);
        gmlat=(float *) (buf+sizeof(float)*npnt);
        kvect=(float *) (buf+2*sizeof(float)*npnt);
        srng=(float *)  (buf+3*sizeof(float)*npnt);
        vstid=(int16 *) (buf+4*sizeof(float)*npnt);
        vchn=(int16 *)  (buf+4*sizeof(float)*npnt+sizeof(int16)*npnt);
        index=(int32 *) (buf+4*sizeof(float)*npnt+2*sizeof(int16)*npnt);
        vlos=(float *)  (buf+4*sizeof(float)*npnt+
                               sizeof(int32)*npnt+
                             2*sizeof(int16)*npnt);
        vlos_sd=(float *) (buf+4*sizeof(float)*npnt+
                                 sizeof(int32)*npnt+
                               2*sizeof(int16)*npnt+
                                 sizeof(float)*npnt);

        if (xtd) {
            pwr=(float *) (buf+4*sizeof(float)*npnt+
                                 sizeof(int32)*npnt+
                               2*sizeof(int16)*npnt+
                               2*sizeof(float)*npnt);
            pwr_sd=(float *) (buf+4*sizeof(float)*npnt+
                                    sizeof(int32)*npnt+
                                  2*sizeof(int16)*npnt+
                                  3*sizeof(float)*npnt);
            wdt=(float *) (buf+4*sizeof(float)*npnt+
                                 sizeof(int32)*npnt+
                               2*sizeof(int16)*npnt+
                               4*sizeof(float)*npnt);
            wdt_sd=(float *) (buf+4*sizeof(float)*npnt+
                                    sizeof(int32)*npnt+
                                  2*sizeof(int16)*npnt+
                                  5*sizeof(float)*npnt);
        }
        n=0;
        for (p=0;p<ptr->pnum;p++) {
            if (ptr->pnt[p].cnt==0) continue;
            gmlat[n]=ptr->pnt[p].mlat;
            gmlon[n]=ptr->pnt[p].mlon;
            kvect[n]=ptr->pnt[p].azm;
            srng[n]=ptr->pnt[p].srng;
            vstid[n]=ptr->st_id;
            vchn[n]=ptr->chn;
            index[n]=ptr->pnt[p].ref;
            vlos[n]=ptr->pnt[p].vel.median;
            vlos_sd[n]=ptr->pnt[p].vel.sd;
            if (xtd) {
                pwr[n]=ptr->pnt[p].pwr.median;
                pwr_sd[n]=ptr->pnt[p].pwr.sd;
                wdt[n]=ptr->pnt[p].wdt.median;
                wdt_sd[n]=ptr->pnt[p].wdt.sd;
            }
            n++;
        }
    }

    TimeEpochToYMDHMS(ptr->ed_time,&yr,&mo,&dy,&hr,&mt,&sc);
    eyr=yr;
    emo=mo;
    edy=dy;
    ehr=hr;
    emt=mt;
    esc=sc;
    TimeEpochToYMDHMS(ptr->st_time,&yr,&mo,&dy,&hr,&mt,&sc);
    syr=yr;
    smo=mo;
    sdy=dy;
    shr=hr;
    smt=mt;
    ssc=sc;

    if (logbuf !=NULL) 
        sprintf(logbuf,"%d-%d-%d %d:%d:%d %d:%d:%d pnts=%d (%d)",
                syr,smo,sdy,shr,smt,(int) ssc,
                ehr,emt,(int) esc,ptr->npnt,ptr->st_id);

    data=DataMapMake();

    DataMapAddScalar(data,"start.year",DATASHORT,&syr);
    DataMapAddScalar(data,"start.month",DATASHORT,&smo);
    DataMapAddScalar(data,"start.day",DATASHORT,&sdy);
    DataMapAddScalar(data,"start.hour",DATASHORT,&shr);
    DataMapAddScalar(data,"start.minute",DATASHORT,&smt);
    DataMapAddScalar(data,"start.second",DATADOUBLE,&ssc);

    DataMapAddScalar(data,"end.year",DATASHORT,&eyr);
    DataMapAddScalar(data,"end.month",DATASHORT,&emo);
    DataMapAddScalar(data,"end.day",DATASHORT,&edy);
    DataMapAddScalar(data,"end.hour",DATASHORT,&ehr);
    DataMapAddScalar(data,"end.minute",DATASHORT,&emt);
    DataMapAddScalar(data,"end.second",DATADOUBLE,&esc);

    DataMapAddArray(data,"stid",DATASHORT,1,&stnum,stid);
    DataMapAddArray(data,"channel",DATASHORT,1,&stnum,chn);
    DataMapAddArray(data,"nvec",DATASHORT,1,&stnum,nvec);
    DataMapAddArray(data,"freq",DATAFLOAT,1,&stnum,freq);
    DataMapAddArray(data,"major.revision",DATASHORT,1,&stnum,major_rev);
    DataMapAddArray(data,"minor.revision",DATASHORT,1,&stnum,minor_rev);
    DataMapAddArray(data,"program.id",DATASHORT,1,&stnum,progid);
    DataMapAddArray(data,"noise.mean",DATAFLOAT,1,&stnum,noise_mean);
    DataMapAddArray(data,"noise.sd",DATAFLOAT,1,&stnum,noise_sd);
    DataMapAddArray(data,"gsct",DATASHORT,1,&stnum ,gsct);
    DataMapAddArray(data,"v.min",DATAFLOAT,1,&stnum,v_min);
    DataMapAddArray(data,"v.max",DATAFLOAT,1,&stnum,v_max);
    DataMapAddArray(data,"p.min",DATAFLOAT,1,&stnum,p_min);
    DataMapAddArray(data,"p.max",DATAFLOAT,1,&stnum,p_max);
    DataMapAddArray(data,"w.min",DATAFLOAT,1,&stnum,w_min);
    DataMapAddArray(data,"w.max",DATAFLOAT,1,&stnum,w_max);
    DataMapAddArray(data,"ve.min",DATAFLOAT,1,&stnum,ve_min);
    DataMapAddArray(data,"ve.max",DATAFLOAT,1,&stnum,ve_max);

    if (npnt !=0) {
        DataMapAddArray(data,"vector.mlat",DATAFLOAT,1,&npnt,gmlat);
        DataMapAddArray(data,"vector.mlon",DATAFLOAT,1,&npnt,gmlon);
        DataMapAddArray(data,"vector.kvect",DATAFLOAT,1,&npnt,kvect);
        DataMapAddArray(data,"vector.srng",DATAFLOAT,1,&npnt,srng);
        DataMapAddArray(data,"vector.stid",DATASHORT,1,&npnt,vstid);
        DataMapAddArray(data,"vector.channel",DATASHORT,1,&npnt,vchn);
        DataMapAddArray(data,"vector.index",DATAINT,1,&npnt,index);
        DataMapAddArray(data,"vector.vel.median",DATAFLOAT,1,&npnt,vlos);
        DataMapAddArray(data,"vector.vel.sd",DATAFLOAT,1,&npnt,vlos_sd);
        if (xtd) {
            DataMapAddArray(data,"vector.pwr.median",DATAFLOAT,1,&npnt,pwr);
            DataMapAddArray(data,"vector.pwr.sd",DATAFLOAT,1,&npnt,pwr_sd);
            DataMapAddArray(data,"vector.wdt.median",DATAFLOAT,1,&npnt,wdt);
            DataMapAddArray(data,"vector.wdt.sd",DATAFLOAT,1,&npnt,wdt_sd);
        }
    }

    if (fid !=-1) s=DataMapWrite(fid,data);
    else s=DataMapSize(data);
    DataMapFree(data);
    free(buf);
    return s;

}



int GridTableFwrite(FILE *fp,struct GridTable *ptr,char *logbuf,int xtd) {

    return GridTableWrite(fileno(fp),ptr,logbuf,xtd);

}
