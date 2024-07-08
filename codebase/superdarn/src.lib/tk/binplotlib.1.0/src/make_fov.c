/* make_fov.c
   ========== 
   Author: R.J.Barnes

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
#include <string.h>
#include <math.h>
#include <sys/types.h>
#include "rtypes.h"
#include "rtime.h"
#include "rfile.h"
#include "radar.h"
#include "rpos.h"
#include "griddata.h"
#include "polygon.h"


struct PolygonData *make_fov(double tval,struct RadarNetwork *network,
                             float alt,int chisham) {

    double rho,lat,lon,srng;
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

        site=RadarYMDHMSGetSite(&(network->radar[i]),yr,mo,dy,hr,mt,(int) sc);
        if (site==NULL) continue;
        PolygonAddPolygon(ptr,i);

        for (rn=0;rn<=maxrange;rn++) {
            RPosGeo(0,0,rn,site,frang,rsep,
                    site->recrise,alt,&rho,&lat,&lon,&srng,chisham);
            pnt[0]=lat;
            pnt[1]=lon;
            PolygonAdd(ptr,pnt);
        }

        for (bm=1;bm<=site->maxbeam;bm++) {
            RPosGeo(0,bm,maxrange,site,frang,rsep,
                    site->recrise,alt,&rho,&lat,&lon,&srng,chisham);
            pnt[0]=lat;
            pnt[1]=lon;
            PolygonAdd(ptr,pnt);
        }

        for (rn=maxrange-1;rn>=0;rn--) {
            RPosGeo(0,site->maxbeam,rn,site,frang,rsep,
                    site->recrise,alt,&rho,&lat,&lon,&srng,chisham);
            pnt[0]=lat;
            pnt[1]=lon;
            PolygonAdd(ptr,pnt);
        }

        for (bm=site->maxbeam-1;bm>0;bm--) {
            RPosGeo(0,bm,0,site,frang,rsep,
                    site->recrise,alt,&rho,&lat,&lon,&srng,chisham);
            pnt[0]=lat;
            pnt[1]=lon;
            PolygonAdd(ptr,pnt);
        }
    }
    return ptr;
}


struct PolygonData *make_field_fov(double tval,struct RadarNetwork *network,
                                   int id,int chisham) {

    double rho,lat,lon,srng;
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
        if (network->radar[i].id==id) continue;
        site=RadarYMDHMSGetSite(&(network->radar[i]),yr,mo,dy,hr,mt,(int) sc);
        if (site==NULL) continue;
        PolygonAddPolygon(ptr,1);

        for (rn=0;rn<=maxrange;rn++) {
            RPosGeo(0,0,rn,site,frang,rsep,
                    site->recrise,0,&rho,&lat,&lon,&srng,chisham);
            pnt[0]=lat;
            pnt[1]=lon;
            PolygonAdd(ptr,pnt);
        }

        for (bm=1;bm<=site->maxbeam;bm++) {
            RPosGeo(0,bm,maxrange,site,frang,rsep,
                    site->recrise,0,&rho,&lat,&lon,&srng,chisham);
            pnt[0]=lat;
            pnt[1]=lon;
            PolygonAdd(ptr,pnt);
        }

        for (rn=maxrange-1;rn>=0;rn--) {
            RPosGeo(0,site->maxbeam,rn,site,frang,rsep,
                    site->recrise,0,&rho,&lat,&lon,&srng,chisham);
            pnt[0]=lat;
            pnt[1]=lon;
            PolygonAdd(ptr,pnt);
        }

        for (bm=site->maxbeam-1;bm>0;bm--) {
            RPosGeo(0,bm,0,site,frang,rsep,
                    site->recrise,0,&rho,&lat,&lon,&srng,chisham);
            pnt[0]=lat;
            pnt[1]=lon;
            PolygonAdd(ptr,pnt);
        }
    }
    return ptr;
}


/* This function returns FOVs for all operational radars */
struct PolygonData *make_grid_fov(double tval,struct RadarNetwork *network,
                                  int chisham,int old_aacgm) {

    double rho,lat,lon,srng;
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
                    site->recrise,0,&rho,&lat,&lon,&srng,
                    chisham,old_aacgm);
            pnt[0]=lat;
            pnt[1]=lon;
            PolygonAdd(ptr,pnt);
        }

        for (bm=1;bm<=site->maxbeam;bm++) {
            RPosMag(0,bm,maxrange,site,frang,rsep,
                    site->recrise,0,&rho,&lat,&lon,&srng,
                    chisham,old_aacgm);
            pnt[0]=lat;
            pnt[1]=lon;
            PolygonAdd(ptr,pnt);
        }

        for (rn=maxrange-1;rn>=0;rn--) {
            RPosMag(0,site->maxbeam,rn,site,frang,rsep,
                    site->recrise,0,&rho,&lat,&lon,&srng,
                    chisham,old_aacgm);
            pnt[0]=lat;
            pnt[1]=lon;
            PolygonAdd(ptr,pnt);
        }

        for (bm=site->maxbeam-1;bm>0;bm--) {
            RPosMag(0,bm,0,site,frang,rsep,
                    site->recrise,0,&rho,&lat,&lon,&srng,
                    chisham,old_aacgm);
            pnt[0]=lat;
            pnt[1]=lon;
            PolygonAdd(ptr,pnt);
        }
    }

    return ptr;
}


/* This function returns only the radar FOVs that are contributing grid vectors */
struct PolygonData *make_grid_fov_data(struct GridData *gptr,struct RadarNetwork *network,
                                       int chisham,int old_aacgm) {

    double rho,lat,lon,srng;
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
                        site->recrise,0,&rho,&lat,&lon,&srng,
                        chisham,old_aacgm);
                pnt[0]=lat;
                pnt[1]=lon;
                PolygonAdd(ptr,pnt);
            }

            for (bm=1;bm<=site->maxbeam;bm++) {
                RPosMag(0,bm,maxrange,site,frang,rsep,
                        site->recrise,0,&rho,&lat,&lon,&srng,
                        chisham,old_aacgm);
                pnt[0]=lat;
                pnt[1]=lon;
                PolygonAdd(ptr,pnt);
            }

            for (rn=maxrange-1;rn>=0;rn--) {
                RPosMag(0,site->maxbeam,rn,site,frang,rsep,
                        site->recrise,0,&rho,&lat,&lon,&srng,
                        chisham,old_aacgm);
                pnt[0]=lat;
                pnt[1]=lon;
                PolygonAdd(ptr,pnt);
            }

            for (bm=site->maxbeam-1;bm>0;bm--) {
                RPosMag(0,bm,0,site,frang,rsep,
                        site->recrise,0,&rho,&lat,&lon,&srng,
                        chisham,old_aacgm);
                pnt[0]=lat;
                pnt[1]=lon;
                PolygonAdd(ptr,pnt);
            }
        }
    }

    return ptr;
}

