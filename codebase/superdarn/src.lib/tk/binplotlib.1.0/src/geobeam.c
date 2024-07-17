/* geobeam.c
   =========
 Copyright (c) 2012 The Johns Hopkins University/Applied Physics Laboratory
 
 This file is part of the Radar Software Toolkit (RST).
 
 RST is free software: you can redistribute it and/or modify
 it under the terms of the GNU Lesser General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 GNU Lesser General Public License for more details.
 
 You should have received a copy of the GNU Lesser General Public License
 along with RST.  If not, see <http://www.gnu.org/licenses/>.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "rmath.h"
#include "radar.h" 
#include "rpos.h"
#include "invmag.h"
#include "scandata.h"
#include "geobeam.h"
#include "aacgm.h"
#include "aacgmlib_v2.h"



int GeoLocCenter(struct RadarSite *site,int mag,float *lat,float *lon,
                 int chisham,int old_aacgm) {

    int s;
    double glat,glon,mlat,mlon,rho,srng;
    RPosGeo(0,8,35,site,180,45,100,
            300.0,&rho,&glat,&glon,&srng,chisham);

    if (mag) { 
        if (old_aacgm) 
        {
            s=AACGMConvert(glat,glon,300,&mlat,&mlon,&rho,0);
            if (s == -1)
            {
                fprintf(stderr, "Warning: AACGMConvert returned a -1\n");
            }
        }
        else
        {
            s=AACGM_v2_Convert(glat,glon,300,&mlat,&mlon,&rho,0);
            if (s == -1)
            {
                fprintf(stderr, "Warning: AACGMConvert returned a -1\n");
            }
        }
        *lat=mlat;
        *lon=mlon;
    } else {
        *lat=glat;
        *lon=glon;
    }
    return 0;
}



int GeoLocBeam(struct RadarSite *site,int year,
               struct GeoLocScan *geol,struct RadarBeam *bm,
               int chisham,int old_aacgm) {

    int n,s,x;
    int rng;
    double rho,lat,lon,glat,glon,mlat,mlon;
    double geoazm,elv,magazm,srng;

    for (n=0;n<geol->num;n++) {
        if (geol->bm[n].bm !=bm->bm) continue;
        if (geol->bm[n].frang !=bm->frang) continue;
        if (geol->bm[n].rsep !=bm->rsep) continue;
        if (geol->bm[n].rxrise !=bm->rxrise) continue;
        if (geol->bm[n].nrang !=bm->nrang) continue;
        break;
    }

    if (n<geol->num) return n;

    /* we've got a new beam */

    if (geol->bm==NULL) geol->bm=malloc(sizeof(struct GeoLocBeam));
    else {
        struct GeoLocBeam *tmp=NULL;
        tmp=realloc(geol->bm,(geol->num+1)*sizeof(struct GeoLocBeam));
        if (tmp==NULL) return -1;
        geol->bm=tmp;
    }
    geol->bm[n].bm=bm->bm;
    geol->bm[n].frang=bm->frang;
    geol->bm[n].rsep=bm->rsep;
    geol->bm[n].rxrise=bm->rxrise;
    geol->bm[n].nrang=bm->nrang;

    for (x=0;x<3;x++) {
        geol->bm[n].glat[x]=malloc(sizeof(float)*(bm->nrang+1));
        geol->bm[n].glon[x]=malloc(sizeof(float)*(bm->nrang+1));
        geol->bm[n].mlat[x]=malloc(sizeof(float)*(bm->nrang+1));
        geol->bm[n].mlon[x]=malloc(sizeof(float)*(bm->nrang+1));
    }
    geol->bm[n].gazm=malloc(sizeof(float)*bm->nrang);
    geol->bm[n].mazm=malloc(sizeof(float)*bm->nrang);

    for (rng=0;rng<=bm->nrang;rng++) {
        RPosGeo(0,bm->bm,rng,site,bm->frang,bm->rsep,bm->rxrise,
                300.0,&rho,&lat,&lon,&srng,chisham);
        geol->bm[n].glat[0][rng]=lat;
        geol->bm[n].glon[0][rng]=lon;
        RPosGeo(0,bm->bm+1,rng,site,bm->frang,bm->rsep,bm->rxrise,
                300.0,&rho,&lat,&lon,&srng,chisham);
        geol->bm[n].glat[2][rng]=lat;
        geol->bm[n].glon[2][rng]=lon;
        if (rng<bm->nrang) {
            RPosGeo(1,bm->bm,rng,site,bm->frang,bm->rsep,bm->rxrise,
                    300.0,&rho,&lat,&lon,&srng,chisham);
            geol->bm[n].glat[1][rng]=lat;
            geol->bm[n].glon[1][rng]=lon;
            RPosRngBmAzmElv(bm->bm,rng,year,site,
                            bm->frang,bm->rsep,bm->rxrise,
                            300.0,&geoazm,&elv,chisham);
            geol->bm[n].gazm[rng]=geoazm;
        }

        glat=geol->bm[n].glat[0][rng];
        glon=geol->bm[n].glon[0][rng];
        if (old_aacgm) 
        {
            s=AACGMConvert(glat,glon,300,&mlat,&mlon,&rho,0);
            if (s == -1)
            {
                fprintf(stderr, "Warning: AACGMConvert returned a -1\n");
            }
        }
        else           
        {
            s=AACGM_v2_Convert(glat,glon,300,&mlat,&mlon,&rho,0);
            if (s == -1)
            {
                fprintf(stderr, "Warning: AACGMConvert returned a -1\n");
            }
        }
        geol->bm[n].mlat[0][rng]=mlat;
        geol->bm[n].mlon[0][rng]=mlon;

        glat=geol->bm[n].glat[2][rng];
        glon=geol->bm[n].glon[2][rng];
        if (old_aacgm) s=AACGMConvert(glat,glon,300,&mlat,&mlon,&rho,0);
        else           s=AACGM_v2_Convert(glat,glon,300,&mlat,&mlon,&rho,0);
        geol->bm[n].mlat[2][rng]=mlat;
        geol->bm[n].mlon[2][rng]=mlon;
        if (rng<bm->nrang) {
            RPosInvMag(bm->bm,rng,year,site,bm->frang,bm->rsep,bm->rxrise,
                       300.0,&mlat,&mlon,&magazm,&srng,chisham,old_aacgm);
            geol->bm[n].mlat[1][rng]=mlat;
            geol->bm[n].mlon[1][rng]=mlon;
            geol->bm[n].mazm[rng]=magazm;
        }
    }
    geol->num++;

    return n;
}
