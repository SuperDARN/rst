/* fitwrite.c
   ========== 
   Author R.J.Barnes

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
    2021-11-12 Emma Bland (UNIS): Added "elv_error" and "elv_fitted" fields for FitACF v3
                                  Only write XCF fitted parameters to file for FitACF v2 and earlier
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <zlib.h>
#include "rtypes.h"
#include "dmap.h"
#include "rprm.h"
#include "fitblk.h"
#include "fitdata.h"



int FitEncode(struct DataMap *ptr,struct RadarParm *prm, struct FitData *fit) {

  int c,x;
  int32 snum,xnum;
  int32 p0num;

  int16 *slist=NULL;
  float *pwr0=NULL;

  int16 *nlag=NULL;

  char *qflg=NULL;
  char *gflg=NULL;

  float *p_l=NULL;
  float *p_l_e=NULL;
  float *p_s=NULL;
  float *p_s_e=NULL;

  float *v=NULL;
  float *v_e=NULL;

  float *w_l=NULL;
  float *w_l_e=NULL;
  float *w_s=NULL;
  float *w_s_e=NULL;

  float *sd_l=NULL;
  float *sd_s=NULL;
  float *sd_phi=NULL;

  char *x_qflg=NULL;
  char *x_gflg=NULL;

  float *x_p_l=NULL;
  float *x_p_l_e=NULL;
  float *x_p_s=NULL;
  float *x_p_s_e=NULL;

  float *x_v=NULL;
  float *x_v_e=NULL;

  float *x_w_l=NULL;
  float *x_w_l_e=NULL;
  float *x_w_s=NULL;
  float *x_w_s_e=NULL;

  float *phi0=NULL;
  float *phi0_e=NULL;
  float *elv=NULL;
  float *elv_low=NULL;    // fitacf 1-2
  float *elv_high=NULL;   // fitacf 1-2
  float *elv_fitted=NULL; // fitacf 3
  float *elv_error=NULL;  // fitacf 3

  float *x_sd_l=NULL;
  float *x_sd_s=NULL;
  float *x_sd_phi=NULL;

  float sky_noise=fit->noise.skynoise;
  float lag0_noise=fit->noise.lag0;
  float vel_noise=fit->noise.vel;

  DataMapAddScalar(ptr,"algorithm",DATASTRING,&fit->algorithm);

  DataMapAddScalar(ptr,"fitacf.revision.major",DATAINT,
		    &fit->revision.major);
  DataMapAddScalar(ptr,"fitacf.revision.minor",DATAINT,
		    &fit->revision.minor);

  DataMapStoreScalar(ptr,"noise.sky",DATAFLOAT,&sky_noise);
  DataMapStoreScalar(ptr,"noise.lag0",DATAFLOAT,&lag0_noise);
  DataMapStoreScalar(ptr,"noise.vel",DATAFLOAT,&vel_noise);

  DataMapStoreScalar(ptr,"tdiff",DATAFLOAT,&fit->tdiff);

  p0num=prm->nrang;
  pwr0=DataMapStoreArray(ptr,"pwr0",DATAFLOAT,1,&p0num,NULL);
  for (c=0;c<p0num;c++) pwr0[c]=fit->rng[c].p_0;

  snum=0;
  for (c=0;c<prm->nrang;c++) {
      if ( (fit->rng[c].qflg==1) ||
              ((fit->xrng !=NULL) && (fit->xrng[c].qflg==1)))
          snum++;
  }

  if (prm->xcf !=0) 
      xnum=snum;
  else 
      xnum=0;

  if (snum==0){
      return 0;
  }

  slist=DataMapStoreArray(ptr,"slist",DATASHORT,1,&snum,NULL);
  nlag=DataMapStoreArray(ptr,"nlag",DATASHORT,1,&snum,NULL);

  qflg=DataMapStoreArray(ptr,"qflg",DATACHAR,1,&snum,NULL);
  gflg=DataMapStoreArray(ptr,"gflg",DATACHAR,1,&snum,NULL);
  
  p_l=DataMapStoreArray(ptr,"p_l",DATAFLOAT,1,&snum,NULL);
  p_l_e=DataMapStoreArray(ptr,"p_l_e",DATAFLOAT,1,&snum,NULL);

  p_s=DataMapStoreArray(ptr,"p_s",DATAFLOAT,1,&snum,NULL);
  p_s_e=DataMapStoreArray(ptr,"p_s_e",DATAFLOAT,1,&snum,NULL);
  v=DataMapStoreArray(ptr,"v",DATAFLOAT,1,&snum,NULL);
  v_e=DataMapStoreArray(ptr,"v_e",DATAFLOAT,1,&snum,NULL);

  w_l=DataMapStoreArray(ptr,"w_l",DATAFLOAT,1,&snum,NULL);
  w_l_e=DataMapStoreArray(ptr,"w_l_e",DATAFLOAT,1,&snum,NULL);
  w_s=DataMapStoreArray(ptr,"w_s",DATAFLOAT,1,&snum,NULL);
  w_s_e=DataMapStoreArray(ptr,"w_s_e",DATAFLOAT,1,&snum,NULL);

  sd_l=DataMapStoreArray(ptr,"sd_l",DATAFLOAT,1,&snum,NULL);
  sd_s=DataMapStoreArray(ptr,"sd_s",DATAFLOAT,1,&snum,NULL);
  sd_phi=DataMapStoreArray(ptr,"sd_phi",DATAFLOAT,1,&snum,NULL);

  if (prm->xcf !=0) {
  
    /* fit.revision.major has values of 4 and 5 in some historical data. 
       The logic of the if statements below should be changed if a new major
       version of FitACF is created in the future */
    
    if (fit->revision.major==3) {
      //XCF fitted parameters for FitACF 3
      phi0=DataMapStoreArray(ptr,"phi0",DATAFLOAT,1,&xnum,NULL);
      phi0_e=DataMapStoreArray(ptr,"phi0_e",DATAFLOAT,1,&xnum,NULL);
      elv=DataMapStoreArray(ptr,"elv",DATAFLOAT,1,&xnum,NULL);
      elv_fitted=DataMapStoreArray(ptr,"elv_fitted",DATAFLOAT,1,&xnum,NULL);
      elv_error=DataMapStoreArray(ptr,"elv_error",DATAFLOAT,1,&xnum,NULL);
      
      x_sd_phi=DataMapStoreArray(ptr,"x_sd_phi",DATAFLOAT,1,&xnum,NULL);
    } else {
      //XCF fitted parameters for FitACF 1-2
      x_qflg=DataMapStoreArray(ptr,"x_qflg",DATACHAR,1,&xnum,NULL);
      x_gflg=DataMapStoreArray(ptr,"x_gflg",DATACHAR,1,&xnum,NULL);
  
      x_p_l=DataMapStoreArray(ptr,"x_p_l",DATAFLOAT,1,&xnum,NULL);
      x_p_l_e=DataMapStoreArray(ptr,"x_p_l_e",DATAFLOAT,1,&xnum,NULL);
      x_p_s=DataMapStoreArray(ptr,"x_p_s",DATAFLOAT,1,&xnum,NULL);
      x_p_s_e=DataMapStoreArray(ptr,"x_p_s_e",DATAFLOAT,1,&xnum,NULL);
  
      x_v=DataMapStoreArray(ptr,"x_v",DATAFLOAT,1,&xnum,NULL); 
      x_v_e=DataMapStoreArray(ptr,"x_v_e",DATAFLOAT,1,&xnum,NULL);
 
      x_w_l=DataMapStoreArray(ptr,"x_w_l",DATAFLOAT,1,&xnum,NULL);
      x_w_l_e=DataMapStoreArray(ptr,"x_w_l_e",DATAFLOAT,1,&xnum,NULL);
      x_w_s=DataMapStoreArray(ptr,"x_w_s",DATAFLOAT,1,&xnum,NULL);
      x_w_s_e=DataMapStoreArray(ptr,"x_w_s_e",DATAFLOAT,1,&xnum,NULL);
  
      phi0=DataMapStoreArray(ptr,"phi0",DATAFLOAT,1,&xnum,NULL);
      phi0_e=DataMapStoreArray(ptr,"phi0_e",DATAFLOAT,1,&xnum,NULL);
      elv=DataMapStoreArray(ptr,"elv",DATAFLOAT,1,&xnum,NULL);
      elv_low=DataMapStoreArray(ptr,"elv_low",DATAFLOAT,1,&xnum,NULL);
      elv_high=DataMapStoreArray(ptr,"elv_high",DATAFLOAT,1,&xnum,NULL);

      x_sd_l=DataMapStoreArray(ptr,"x_sd_l",DATAFLOAT,1,&xnum,NULL);
      x_sd_s=DataMapStoreArray(ptr,"x_sd_s",DATAFLOAT,1,&xnum,NULL);
      x_sd_phi=DataMapStoreArray(ptr,"x_sd_phi",DATAFLOAT,1,&xnum,NULL);

    }
  }
  
  x=0;
  for (c=0;c<prm->nrang;c++) {
    if ( (fit->rng[c].qflg==1) ||
         ((fit->xrng !=NULL) && (fit->xrng[c].qflg==1))) {
      slist[x]=c;
      nlag[x]=fit->rng[c].nump;
      
      qflg[x]=fit->rng[c].qflg;
      gflg[x]=fit->rng[c].gsct;
        
      p_l[x]=fit->rng[c].p_l;
      p_l_e[x]=fit->rng[c].p_l_err;
      p_s[x]=fit->rng[c].p_s;
      p_s_e[x]=fit->rng[c].p_s_err;
        
      v[x]=fit->rng[c].v;
      v_e[x]=fit->rng[c].v_err;

      w_l[x]=fit->rng[c].w_l;
      w_l_e[x]=fit->rng[c].w_l_err;
      w_s[x]=fit->rng[c].w_s;
      w_s_e[x]=fit->rng[c].w_s_err;

      sd_l[x]=fit->rng[c].sdev_l;
      sd_s[x]=fit->rng[c].sdev_s;
      sd_phi[x]=fit->rng[c].sdev_phi;

      if (xnum !=0) {
        
      /* FitACF v3 does not determine XCF fitted parameters, so only 
         write these data to file for FitACF v1-2. The elevation field 
         names have also changed. 
         NB: update if statement logic if new major revision of FitACF
             is created in the future*/
        if (fit->revision.major==3) {
          phi0[x]=fit->xrng[c].phi0;
          phi0_e[x]=fit->xrng[c].phi0_err;
          elv[x]=fit->elv[c].normal;
          elv_fitted[x]=fit->elv[c].fitted;
          elv_error[x]=fit->elv[c].error;

          x_sd_phi[x]=fit->xrng[c].sdev_phi;
        } else {
          x_qflg[x]=fit->xrng[c].qflg;
          x_gflg[x]=fit->xrng[c].gsct;

          x_p_l[x]=fit->xrng[c].p_l;
          x_p_l_e[x]=fit->xrng[c].p_l_err;
          x_p_s[x]=fit->xrng[c].p_s;
          x_p_s_e[x]=fit->xrng[c].p_s_err;

          x_v[x]=fit->xrng[c].v;
          x_v_e[x]=fit->xrng[c].v_err;

          x_w_l[x]=fit->xrng[c].w_l;
          x_w_l_e[x]=fit->xrng[c].w_l_err;
          x_w_s[x]=fit->xrng[c].w_s;
          x_w_s_e[x]=fit->xrng[c].w_s_err;

          phi0[x]=fit->xrng[c].phi0;
          phi0_e[x]=fit->xrng[c].phi0_err;
          elv[x]=fit->elv[c].normal;
          elv_low[x]=fit->elv[c].low;
          elv_high[x]=fit->elv[c].high;

          x_sd_l[x]=fit->xrng[c].sdev_l;
          x_sd_s[x]=fit->xrng[c].sdev_s;
          x_sd_phi[x]=fit->xrng[c].sdev_phi;
        }
      }
      x++;
    }
  }
  return 0;
}



int FitWrite(int fid,struct RadarParm *prm,
            struct FitData *fit) {

  int s;
  struct DataMap *ptr=NULL;

  ptr=DataMapMake();
  if (ptr==NULL) return -1;

  s=RadarParmEncode(ptr,prm);
  
  if (s==0) s=FitEncode(ptr,prm,fit);
  
  if (s==0) {
    if (fid !=-1) s=DataMapWrite(fid,ptr);
    else s=DataMapSize(ptr);
  }

  DataMapFree(ptr);
  return s;

}


int FitFwrite(FILE *fp,struct RadarParm *prm,
              struct FitData *fit) {
  return FitWrite(fileno(fp),prm,fit);
}


