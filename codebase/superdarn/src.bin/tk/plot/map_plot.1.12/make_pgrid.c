/* make_pgrid.c
   ============ 
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
#include <string.h>
#include <math.h>
#include "cnvgrid.h"



int make_pgrid(double latmin,struct CnvGrid *ptr) {
  
  int i,j;

  double lat_step;
  double lon_step;
 
  int nlat,nlon;
  int num;
  int poly; 

  ptr->type=0;
  ptr->num=0;
 
  lat_step=1.0;
  lon_step=2.0;
  num=0;  
  poly=0;


  if (latmin>0)  nlat=(int) (90.0-latmin)/lat_step;
  else  nlat=(int) (90.0+latmin)/lat_step;
  nlon=(int) (360.0/lon_step); 

  if (ptr->vertex !=NULL) free(ptr->vertex);
  if (ptr->lat !=NULL) free(ptr->lat);
  if (ptr->lon !=NULL) free(ptr->lon);
  if (ptr->mag !=NULL) free(ptr->mag);

  ptr->vertex=malloc(sizeof(int)*nlat*nlon*4);
  ptr->lat=malloc(sizeof(double)*nlat*nlon);
  ptr->lon=malloc(sizeof(double)*nlat*nlon);
  ptr->mag=malloc(sizeof(double)*nlat*nlon);

  if ((ptr->lat==NULL) || (ptr->lon==NULL) || 
      (ptr->mag==NULL) || (ptr->vertex==NULL)) return -1;

 if (latmin>0) {
    for (i=0;i<nlat;i++) { 
      for (j=0;j<nlon;j++) {
         if (i != nlat-1) {
           ptr->vertex[4*poly]=num;
           if (j !=nlon-1) ptr->vertex[4*poly+1]=num+1;
           else ptr->vertex[4*num+1]=num-nlon+1;
           ptr->vertex[4*poly+2]=ptr->vertex[4*poly+1]+nlon;
           ptr->vertex[4*poly+3]=num+nlon;     
           poly++;  
         } 
         ptr->lat[num]=i*lat_step+latmin;
         ptr->lon[num]=j*lon_step;
         ptr->mag[num]=0;
         num++;
      }
    }
  } else {
    for (i=0;i<nlat;i++) { 
      for (j=0;j<nlon;j++) {
         if (i != nlat-1) {
           ptr->vertex[4*poly]=num;
           if (j !=nlon-1) ptr->vertex[4*poly+1]=num+1;
           else ptr->vertex[4*num+1]=num-nlon+1;
           ptr->vertex[4*poly+2]=ptr->vertex[4*poly+1]+nlon;
           ptr->vertex[4*poly+3]=num+nlon;     
           poly++;  
         } 
         ptr->lat[num]=-i*lat_step+latmin;
         ptr->lon[num]=j*lon_step;
         ptr->mag[num]=0;
         num++;
      }
    }
  }

  ptr->num=num;
  ptr->nlat=nlat;
  ptr->nlon=nlon;
  ptr->poly=poly;
  return 0;
}
 






