/* isect.c
   =======
   Author: R.J.Barnes
 Copyright (c) 2012 The Johns Hopkins University/Applied Physics Laboratory

This file is part of the Radar Software Toolkit (RST).

RST is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with this program. If not, see <https://www.gnu.org/licenses/>.

Modifications:
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef _DARWIN
#include <malloc.h>
#endif
#include <math.h>
#include "polygon.h"



int PolygonXYbbox(void *a,void *b,int s) {

  float *pnt;
  float *box;

  pnt=(float *) a;
  box=(float *) b;

  if (pnt==NULL) {
    box[0]=1e16;
    box[1]=1e16;
    box[2]=-1e16;
    box[3]=-1e16;
    return 0;
  }  

  if (pnt[0]<box[0]) box[0]=pnt[0];
  if (pnt[1]<box[1]) box[1]=pnt[1];
  if (pnt[0]>box[2]) box[2]=pnt[0];
  if (pnt[1]>box[3]) box[3]=pnt[1];

  return 0;

}

int PolygonTestBox(float *a,float *b) {
 int s;
 s=((a[0]<b[0]) && (a[2]<b[0])) ||
   ((a[1]<b[1]) && (a[3]<b[1])) ||
   ((a[0]>b[2]) && (a[2]>b[2])) ||
   ((a[1]>b[3]) && (a[3]>b[3]));
 return s;

}

float *PolygonCoord(void *a,int s) {
  float *pnt;
  pnt=(float *) a;
  return pnt;
}


struct PolygonData *PolygonClip(struct PolygonData *apol,
                                struct PolygonData *bpol) {

  /* algorithm calculates intersection with each boundary */

  int an;
  int xn,n,j;
  float ax,ay,bx,by;
  float *ptr;
  char *pp,*qp;
  float ip[2];
  char *ipnt;
  int wp,wq,p,q;
  int m,r,s;
 
  float ma,mb,mc,md; /* matrix */
  float determ;      /* determinant */
  float ia,ib,ic,id; /* inverse */
  float dx,dy,dt;
  float sx,sy,tx,ty;
  float kl,jl;
 
  struct PolygonData *xpol=NULL;
  struct PolygonData *ypol=NULL;

  if ((apol==NULL) || (bpol==NULL)) return NULL;

  an=apol->num[0];

  ipnt=malloc(bpol->sze);
  xpol=bpol;
  ypol=PolygonMake(bpol->sze,PolygonXYbbox);

  for (m=1;m<=an;m++) {

    r=m-1;
    s=(m==an) ? 0 : m;
    ptr=PolygonCoord((char *) apol->data+(apol->off[0]+r)*apol->sze,
                     apol->sze);
    ax=ptr[0];
    ay=ptr[1];
    ptr=PolygonCoord((char *) apol->data+(apol->off[0]+s)*apol->sze,
                     apol->sze);
    bx=ptr[0];
    by=ptr[1];

  
    /* co-ordinate set of line segment */

    dx=bx-ax;
    dy=by-ay;

    dt=sqrt(dx*dx+dy*dy);
   
    ma=dx/dt;
    mb=-dy/dt;
    mc=dy/dt;
    md=dx/dt;

   /* find inverse of equation */

   determ=1/(ma*md-mb*mc);
   ia=md*determ;
   ib=-mb*determ;
   ic=-mc*determ;
   id=ma*determ;

  
   for (j=0;j<xpol->polnum;j++) {    
     /*
     if (PolygonTestBox( (float *) ((char *) xpol->box+xpol->sze*2*j), 
                         (float *) apol->box)==1) continue; 
     */

     PolygonAddPolygon(ypol,xpol->type[j]);
     xn=xpol->num[j];     
     for (n=1;n<=xn;n++) {
  
       p=n-1;
       q=(n==xn) ? 0 : n;
       
       pp=((char *) xpol->data+(xpol->off[j]+p)*xpol->sze);
       qp=((char *) xpol->data+(xpol->off[j]+q)*xpol->sze);

       ptr=PolygonCoord(pp,xpol->sze);


       ty=ic*(ptr[0]-ax)+id*(ptr[1]-ay);
       wp=(ty>0);
       ptr=PolygonCoord(qp,xpol->sze);
  
       ty=ic*(ptr[0]-ax)+id*(ptr[1]-ay);
       wq=(ty>0);
 
 
       if (wp && wq) {
         if (p==0) PolygonAdd(ypol,pp);      
         if (q !=0) PolygonAdd(ypol,qp); 
       } else if (wp != wq) {
         if ((wp) && (p==0)) PolygonAdd(ypol,pp); 
           
         /* add intersection point here */

         ptr=PolygonCoord(pp,xpol->sze);

         sx=ia*(ptr[0]-ax)+ib*(ptr[1]-ay);
         sy=ic*(ptr[0]-ax)+id*(ptr[1]-ay);

         ptr=PolygonCoord(qp,apol->sze);

         tx=ia*(ptr[0]-ax)+ib*(ptr[1]-ay);
         ty=ic*(ptr[0]-ax)+id*(ptr[1]-ay);

         if (tx !=sx) {
           kl=(ty-sy)/(tx-sx);
           jl=sy-sx*kl;
           ip[0]=ma*-jl/kl+ax;
           ip[1]=mc*-jl/kl+ay;
          } else {
           ip[0]=ma*sx+ax;
           ip[1]=mc*sx+ay;
          }    

	  memcpy(ipnt,qp,xpol->sze);
          ptr=PolygonCoord(ipnt,xpol->sze);
          ptr[0]=ip[0];
          ptr[1]=ip[1];
   
          PolygonAdd(ypol,ipnt);

          if ((wq) && (q !=0)) PolygonAdd(ypol,qp); 
         
        }
     }
  
     if (ypol->num[ypol->polnum-1]<3) {
        PolygonRemove(ypol);
     }
     
    } 

   if (xpol !=bpol) PolygonFree(xpol);
   xpol=ypol;
   ypol=PolygonMake(xpol->sze,PolygonXYbbox);
    
  }
 
  PolygonFree(ypol);
  
  if (xpol->pntnum==0) {
    PolygonFree(xpol);
    return NULL;
  } 
  return xpol;  
}














