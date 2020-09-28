/* make_fov.c
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
#include <math.h>
#include <sys/types.h>
#include "rtypes.h"
#include "rtime.h"
#include "rfile.h"
#include "radar.h"
#include "rpos.h"
#include "griddata.h"
#include "cnvmap.h"
#include "polygon.h"


/* This function returns FOVs for all operational radars */
struct PolygonData *make_fov(double tval,struct RadarNetwork *network,
                             int chisham,int old_aacgm) {

    double rho,lat,lon;
    int i,rn,bm;
    float pnt[2];
    int yr,mo,dy,hr,mt;
    double sc;
    int frang=180;
    int rsep=45;
    struct PolygonData *ptr=NULL;
    struct RadarSite *site=NULL;
    int maxrange=75;

    TimeEpochToYMDHMS(tval,&yr,&mo,&dy,&hr,&mt,&sc);

    ptr=PolygonMake(sizeof(float)*2,NULL);

    for (i=0;i<network->rnum;i++) {

        if (network->radar[i].status !=1) continue;
        site=RadarYMDHMSGetSite(&(network->radar[i]),yr,mo,dy,hr,mt,(int) sc);
        if (site==NULL) continue;
        PolygonAddPolygon(ptr,1);

        for (rn=0;rn<=maxrange;rn++) {
            RPosMag(0,0,rn,site,frang,rsep,
                    site->recrise,0,&rho,&lat,&lon,
                    chisham,old_aacgm);
            pnt[0]=lat;
            pnt[1]=lon;
            PolygonAdd(ptr,pnt);
        }

        for (bm=1;bm<=site->maxbeam;bm++) {
            RPosMag(0,bm,maxrange,site,frang,rsep,
                    site->recrise,0,&rho,&lat,&lon,
                    chisham,old_aacgm);
            pnt[0]=lat;
            pnt[1]=lon;
            PolygonAdd(ptr,pnt);
        }

        for (rn=maxrange-1;rn>=0;rn--) {
            RPosMag(0,site->maxbeam,rn,site,frang,rsep,
                    site->recrise,0,&rho,&lat,&lon,
                    chisham,old_aacgm);
            pnt[0]=lat;
            pnt[1]=lon;
            PolygonAdd(ptr,pnt);
        }

        for (bm=site->maxbeam-1;bm>0;bm--) {
            RPosMag(0,bm,0,site,frang,rsep,
                    site->recrise,0,&rho,&lat,&lon,
                    chisham,old_aacgm);
            pnt[0]=lat;
            pnt[1]=lon;
            PolygonAdd(ptr,pnt);
        }
    }

    return ptr;
}


/* This function returns only the radar FoVs that are contributing grid vectors */
struct PolygonData *make_fov_data(struct GridData *gptr,struct RadarNetwork *network,
                                  int chisham,int old_aacgm) {

    double rho,lat,lon;
    int i,j,rn,bm;
    float pnt[2];
    int yr,mo,dy,hr,mt;
    double sc;
    int frang=180;
    int rsep=45;
    struct PolygonData *ptr=NULL;
    struct RadarSite *site=NULL;
    int st_id;
    int maxrange=75;

    TimeEpochToYMDHMS(gptr->st_time,&yr,&mo,&dy,&hr,&mt,&sc);

    ptr=PolygonMake(sizeof(float)*2,NULL);

    for (j=0;j<gptr->stnum;j++) {

        if (gptr->sdata[j].npnt==0) continue;

        st_id=gptr->sdata[j].st_id;

        for (i=0;i<network->rnum;i++) {

            if (network->radar[i].id !=st_id) continue;
            site=RadarYMDHMSGetSite(&(network->radar[i]),yr,mo,dy,hr,mt,(int) sc);
            if (site==NULL) continue;
            PolygonAddPolygon(ptr,1);

            for (rn=0;rn<=maxrange;rn++) {
                RPosMag(0,0,rn,site,frang,rsep,
                        site->recrise,0,&rho,&lat,&lon,
                        chisham,old_aacgm);
                pnt[0]=lat;
                pnt[1]=lon;
                PolygonAdd(ptr,pnt);
            }

            for (bm=1;bm<=site->maxbeam;bm++) {
                RPosMag(0,bm,maxrange,site,frang,rsep,
                        site->recrise,0,&rho,&lat,&lon,
                        chisham,old_aacgm);
                pnt[0]=lat;
                pnt[1]=lon;
                PolygonAdd(ptr,pnt);
            }

            for (rn=maxrange-1;rn>=0;rn--) {
                RPosMag(0,site->maxbeam,rn,site,frang,rsep,
                        site->recrise,0,&rho,&lat,&lon,
                        chisham,old_aacgm);
                pnt[0]=lat;
                pnt[1]=lon;
                PolygonAdd(ptr,pnt);
            }

            for (bm=site->maxbeam-1;bm>0;bm--) {
                RPosMag(0,bm,0,site,frang,rsep,
                        site->recrise,0,&rho,&lat,&lon,
                        chisham,old_aacgm);
                pnt[0]=lat;
                pnt[1]=lon;
                PolygonAdd(ptr,pnt);
            }
        }
    }

    return ptr;
}

