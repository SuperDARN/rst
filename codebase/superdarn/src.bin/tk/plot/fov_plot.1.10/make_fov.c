/* make_fov.c
   ========== 
   Author: R.J.Barnes
*/


/*
   See license.txt
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
#include "polygon.h"




struct PolygonData *make_fov(double tval,struct RadarNetwork *network,
                             float alt) {
  double rho,lat,lon;
  int i,rn,bm;
  float pnt[2];
  int yr,mo,dy,hr,mt;
  double sc;
  int frang=180;
  int rsep=45;
  struct PolygonData *ptr=NULL;
  struct RadarSite *site=NULL;

  TimeEpochToYMDHMS(tval,&yr,&mo,&dy,&hr,&mt,&sc);

  ptr=PolygonMake(sizeof(float)*2,NULL);
         
  for (i=0;i<network->rnum;i++) {
 
    site=RadarYMDHMSGetSite(&(network->radar[i]),yr,mo,dy,hr,mt,(int) sc);
    if (site==NULL) continue;
    PolygonAddPolygon(ptr,i);
    
    for (rn=0;rn<=site->maxrange;rn++) {
      RPosGeo(0,0,rn,site,frang,rsep,
                          site->recrise,alt,&rho,&lat,&lon);
      pnt[0]=lat;
      pnt[1]=lon;
      PolygonAdd(ptr,pnt);
    }
    for (bm=1;bm<=site->maxbeam;bm++) {
      RPosGeo(0,bm,site->maxrange,site,frang,rsep,
                          site->recrise,alt,&rho,&lat,&lon);
      pnt[0]=lat;
      pnt[1]=lon;
      PolygonAdd(ptr,pnt);
    }
    for (rn=site->maxrange-1;rn>=0;rn--) {
      RPosGeo(0,site->maxbeam,rn,site,frang,rsep,
                          site->recrise,alt,&rho,&lat,&lon);
      pnt[0]=lat;
      pnt[1]=lon;
      PolygonAdd(ptr,pnt);
    }
    for (bm=site->maxbeam-1;bm>0;bm--) {
      RPosGeo(0,bm,0,site,frang,rsep,
                          site->recrise,alt,&rho,&lat,&lon);
      pnt[0]=lat;
      pnt[1]=lon;
      PolygonAdd(ptr,pnt);
    }
  }
  return ptr;
}




