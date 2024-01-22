/* solve_model.c
   =============
   Author: E.G.Thomas

Copyright (C) <year>  <name of author>

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
#include <math.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include "rtypes.h"
#include "rmath.h"
#include "option.h"
#include "rtime.h"

#include "aacgm.h"
#include "mlt.h"
#include "aacgmlib_v2.h"
#include "mlt_v2.h"
#include "shfconst.h" /* use the same constants as in fitting procedure */
#include "igrfcall.h"
#include "igrflib.h"
#include "cnvmodel.h"
#include "calc_bmag.h"
#include "hlpstr.h"

/*-----------------------------------------------------------------------------
 *
 * global variables
 *
 *
 */
struct OptionData opt;

struct mdata {
  double mlat,mlon;
  double azm,vel;
  double pot;
};


/*-----------------------------------------------------------------------------
 *
 * prototypes
 *
 *
 */
int solve_model(int num, struct mdata *ptr, float latmin, struct model *mod,
                int hemi, float decyear, int igrf_flag, int magflg);
struct mdata *get_model_pos(float latmin, int hemi, int *num,
                            float lat_step, float lon_step, int equal);
double strdate(char *txt);
double strtime(char *txt);

/*-----------------------------------------------------------------------------
 *
 * function definitions
 *
 *
 */

int rst_opterr(char *txt) {
  fprintf(stderr,"Option not recognized: %s\n",txt);
  fprintf(stderr,"Please try: solve_model --help\n");
  return(-1);
}

int main(int argc,char *argv[]) {

  int old_aacgm=0;
  int ecdip=0;

  int arg;
  unsigned char help=0;
  unsigned char option=0;
  unsigned char version=0;

  char *fmt="%#10g %#10g %#10g %#10g %#10g\n";

  char *envstr;
  int num=0;
  int status;
  int yrsec;
  int i;

  char *tmetxt=NULL;
  char *dtetxt=NULL;
  double dval=-1;
  double tval=-1;

  unsigned char sh=0;
  int hemisphere=1;

  int yr,mo,dy,hr,mt,sec,dno;
  double sc;

  struct model *mod = NULL;

  float dBx=0;
  float dBy=0;
  float dBz=0;
  float dVx=0;
  float dtilt=-99;
  float dKp=0;

  float decyear = 0.;
  float tilt = 0.;
  int noigrf = 0;
  int nointerp = 0;
  int rg96 = 0;
  int psr10 = 0;
  int cs10 = 0;
  int ts18 = 0;
  int ts18_kp = 0;
  int imod = 0;

  int magflg = 0;

  float lat_step=1.0;
  float lon_step=2.0;
  int equal=0;

  struct mdata *mdata=NULL;

  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"-option",'x',&option);
  OptionAdd(&opt,"-version",'x',&version);

  OptionAdd(&opt,"d",'t',&dtetxt);
  OptionAdd(&opt,"t",'t',&tmetxt);

  OptionAdd(&opt,"sh",'x',&sh);

  OptionAdd(&opt,"by",'f',&dBy);
  OptionAdd(&opt,"bz",'f',&dBz);
  OptionAdd(&opt,"vx",'f',&dVx);
  OptionAdd(&opt,"tilt",'f',&dtilt);
  OptionAdd(&opt,"kp",'f',&dKp);

  OptionAdd(&opt,"rg96",'x',&rg96);
  OptionAdd(&opt,"psr10",'x',&psr10);
  OptionAdd(&opt,"cs10",'x',&cs10);
  OptionAdd(&opt,"ts18",'x',&ts18);
  OptionAdd(&opt,"ts18_kp",'x',&ts18_kp);
  OptionAdd(&opt,"nointerp",'x',&nointerp);
  OptionAdd(&opt,"old_aacgm",'x',&old_aacgm);
  OptionAdd(&opt,"ecdip",'x',&ecdip);
  OptionAdd(&opt,"noigrf",'x',&noigrf);        /* SGS: default is to use IGRF
                                                       to compute model vecs  */

  OptionAdd(&opt,"lat_step",'f',&lat_step);
  OptionAdd(&opt,"lon_step",'f',&lon_step);
  OptionAdd(&opt,"equal",'x',&equal);

  arg=OptionProcess(1,argc,argv,&opt,rst_opterr);

  if (arg==-1) {
    exit(-1);
  }

  if (help==1) {
    OptionPrintInfo(stdout,hlpstr);
    exit(0);
  }

  if (option==1) {
    OptionDump(stdout,&opt);
    exit(0);
  }

  if (version==1) {
    OptionVersion(stdout);
    exit(0);
  }

  if (!ts18 && !ts18_kp && !cs10 && !psr10 && !rg96) ts18 = 1;
  if (rg96)    imod = RG96;
  if (psr10)   imod = PSR10;
  if (cs10)    imod = CS10;
  if (ts18_kp) imod = TS18_Kp;
  if (ts18)    imod = TS18;

  if (ecdip && imod != TS18) {
    fprintf(stderr,"Eccentric dipole coordinates are only valid for TS18 model.\n");
    exit(-1);
  }

  if (ecdip) magflg = 2;
  else if (old_aacgm) magflg = 1;
  else magflg = 0;

  envstr=getenv("SD_MODEL_TABLE");
  if (envstr==NULL) {
    fprintf(stderr,"Environment variable SD_MODEL_TABLE must be defined.\n");
    exit(-1);
  }

  status = load_all_models(envstr,imod,ecdip);
  if (status != 0) {
    fprintf(stderr,"Failed to load statistical model.\n");
    exit(-1);
  }

  if (tmetxt !=NULL) tval=strtime(tmetxt);
  if (dtetxt !=NULL) dval=strdate(dtetxt);

  if (dval == -1) {
    fprintf(stderr,"\nDate not set, using today's date\n\n");
    AACGM_v2_SetNow();
    AACGM_v2_GetDateTime(&yr,&mo,&dy,&hr,&mt,&sec,&dno);
    dval=TimeYMDHMSToEpoch(yr,mo,dy,0,0,0);
  }

  if (tval !=-1) tval+=dval;
  else tval=dval;

  TimeEpochToYMDHMS(tval,&yr,&mo,&dy,&hr,&mt,&sc);
  yrsec = TimeYMDHMSToYrsec(yr,mo,dy,hr,mt,(int) sc);
  decyear = yr + (float)yrsec/TimeYMDHMSToYrsec(yr,12,31,23,59,59);

  if (!noigrf || ecdip) IGRF_SetDateTime(yr,mo,dy,hr,mt,(int)sc);
  if (!old_aacgm) AACGM_v2_SetDateTime(yr,mo,dy,hr,mt,(int)sc);

  if ((dtilt == -99) && (imod == TS18 || imod == CS10 || imod == PSR10))
    tilt = IGRF_Tilt(yr,mo,dy,hr,mt,(int)sc);
  else tilt = dtilt;

  if (sh==1) hemisphere = -1;

  if (lat_step < 0.5) lat_step=0.5;
  if (lon_step < 0.5) lon_step=0.5;

  /* determine the model */
  mod = determine_model(dVx, dBx, dBy, dBz, hemisphere, tilt, dKp, imod, nointerp);

  if (mod == NULL) {
    fprintf(stderr,"PSR10 model not defined for input conditions.\n");
    exit(-1);
  }

  /* create model grid */
  mdata = get_model_pos(mod->latref,hemisphere,&num,lat_step,lon_step,equal);

  /* solve for the model */
  status = solve_model(num, mdata, mod->latref, mod, hemisphere,
                       decyear, noigrf, magflg);

  if (status != 0) {
    fprintf(stderr,"Failed to solve statistical model.\n");
    exit(-1);
  }

  fprintf(stdout,"Date:  %4d-%02d-%02d %02d:%02d\n",yr,mo,dy,hr,mt);
  switch (imod) {
    case RG96:
      fprintf(stdout,"Model: RG96\n");
      fprintf(stdout,"Bin:   %s, %s\n",mod->level,mod->angle);
      break;
    case PSR10:
      fprintf(stdout,"Model: PSR10\n");
      fprintf(stdout,"Bin:   %s, %s, %s tilt\n",mod->level,mod->angle,mod->tilt);
      break;
    case CS10:
      fprintf(stdout,"Model: CS10\n");
      fprintf(stdout,"Bin:   %s, %s, %s",mod->level,mod->angle,mod->tilt);
      if (nointerp) fprintf(stdout," tilt\n");
      else          fprintf(stdout,"\n");
      break;
    case TS18:
      if (ecdip) fprintf(stdout,"Model: TS18 (Eccentric Dipole)\n");
      else       fprintf(stdout,"Model: TS18\n");
      fprintf(stdout,"Bin:   %s, %s, %s",mod->level,mod->angle,mod->tilt);
      if (nointerp) fprintf(stdout," tilt\n");
      else          fprintf(stdout,"\n");
      break;
    case TS18_Kp:
      fprintf(stdout,"Model: TS18-Kp\n");
      fprintf(stdout,"Bin:   %s, %s\n",mod->level,mod->angle);
      break;
  }
  if (equal) fprintf(stdout,"Grid:  Equal-area (lat_step: %4.2f [deg])\n",lat_step);
  else       fprintf(stdout,"Grid:  Uniform (lat_step: %4.2f, lon_step: %4.2f [deg])\n",lat_step,lon_step);
  fprintf(stdout,"\n");
  fprintf(stdout,"MLAT [deg]   MLT [hr]   Pot [kV] Vazm [deg] Vmag [m/s]\n");
  fprintf(stdout,"---------- ---------- ---------- ---------- ----------\n");
  for (i=0;i<num;i++) {
    fprintf(stdout,fmt,mdata[i].mlat,mdata[i].mlon,mdata[i].pot,mdata[i].azm,mdata[i].vel);
  }

  free(mdata);

  return 0;
}


struct mdata *get_model_pos(float latmin,int hemi,int *num,
                            float lat_step,float lon_step,int equal)
{
  struct mdata *ptr=NULL;
  float nlat, nlon;
  float grdlat;
  double lspc;
  int i, j;
  int cnt=0;

  nlat=(int)((90.0-latmin)/lat_step);

  if (equal) {

    for (i=0;i<nlat;i++) {
      grdlat=(float)i*lat_step + latmin + lat_step/2.0;
      lspc=(double)((int)(cos(fabs(grdlat)*PI/180.0)*360.0/lat_step+0.5))/360.0;
      nlon=(float)lspc * 360.0;

      for (j=0;j<nlon;j++) {
        if (ptr == NULL) ptr = malloc(sizeof(struct mdata));
        else             ptr = realloc(ptr,sizeof(struct mdata)*(cnt+1));

        ptr[cnt].mlat=(double)grdlat;
        ptr[cnt].mlon=(double)((j*360.0/nlon)+((360.0/nlon)/2.0));
        ptr[cnt].pot=0.0;
        ptr[cnt].azm=0.0;
        ptr[cnt].vel=0.0;
        cnt++;
      }
    }

  } else {

    nlon=(float)((int)(360.0/lon_step));

    if (ptr == NULL) ptr = malloc(sizeof(struct mdata)*nlat*nlon);

    for (i=0;i<nlat;i++) {
      for (j=0;j<nlon;j++) {
        ptr[cnt].mlat=(double)(i*lat_step+latmin+lat_step/2.0);
        ptr[cnt].mlon=(double)(j*lon_step);
        ptr[cnt].pot=0.0;
        ptr[cnt].azm=0.0;
        ptr[cnt].vel=0.0;
        cnt++;
      }
    }

  }

  *num = cnt;

  return ptr;
}


int solve_model(int num, struct mdata *ptr, float latmin, struct model *mod,
                int hemi, float decyear, int noigrf, int magflg)
{
  int i;
  double *ele_phi=NULL,*ele_the=NULL,*pot=NULL;
  float *phi=NULL,*the=NULL,*the_col=NULL; 
  double bpolar;
  double bmag = -0.5e-4;

  if (hemi == 1) bpolar = BNorth;
  else           bpolar = BSouth;

  if (mod==NULL) return -1; 

  phi     = malloc(sizeof(float)*num);
  the     = malloc(sizeof(float)*num);
  the_col = malloc(sizeof(float)*num);
  ele_the = malloc(sizeof(double)*num);
  ele_phi = malloc(sizeof(double)*num);
  pot     = malloc(sizeof(double)*num);

  if ( (phi==NULL) || (the==NULL) || (the_col==NULL) ||
       (ele_the==NULL) || (ele_phi==NULL) || (pot==NULL) ) {
    if (phi !=NULL) free(phi);
    if (the !=NULL) free(the);
    if (the_col !=NULL) free(the_col);
    if (ele_the !=NULL) free(ele_the);
    if (ele_phi !=NULL) free(ele_phi);
    if (pot !=NULL) free(pot);

    return -1;
  }

  for (i=0; i<num; i++) {
    phi[i]     = ptr[i].mlon*PI/180.0;
    the[i]     = (90.0-ptr[i].mlat)*(1.0/(90.0-latmin))*PI;
    the_col[i] = (90.0-ptr[i].mlat)*PI/180.0;
    ele_phi[i] = 0;
    ele_the[i] = 0;
    pot[i]     = 0;
  }

  slv_sph_kset(latmin,num,phi,the,the_col,ele_phi,ele_the,mod,pot);

  for (i=0; i<num; i++) {
    ele_phi[i] = ele_phi[i]*hemi;
    ele_the[i] = ele_the[i]*hemi;

    if (noigrf) {
      bmag = -1.0e3*bpolar*(1.0 - 3.0*Altitude/Re)*
              sqrt(3.0*(cos(the_col[i])*cos(the_col[i]))+1.0)/2.0;
    } else {
      bmag = 1e3*calc_bmag(hemi*ptr[i].mlat,ptr[i].mlon,decyear,magflg);
    }

    ptr[i].azm        = atan2(ele_the[i]/bmag,ele_phi[i]/bmag)*180./PI;
    ptr[i].vel        = sqrt( ele_the[i]*ele_the[i] +
                              ele_phi[i]*ele_phi[i] )/bmag;

    if (fabs(pot[i]) > 0.01) ptr[i].pot = pot[i];
    if (hemi==-1) ptr[i].mlat = -ptr[i].mlat;
    ptr[i].mlon = ptr[i].mlon*24.0/360.0;
  }

  free(the);
  free(the_col);
  free(phi);
  free(ele_phi);
  free(ele_the);
  free(pot);

  return 0;
}


double strdate(char *text) {

  double tme;
  int val;
  int yr,mo,dy;

  val=atoi(text);
  dy=val % 100;
  mo=(val / 100) % 100;
  yr=(val / 10000);

  if (yr<1970) yr+=1900;

  tme=TimeYMDHMSToEpoch(yr,mo,dy,0,0,0);

  return tme;
}


double strtime(char *text) {

  int hr,mn;
  int i;

  for (i=0;(text[i] !=':') && (text[i] !=0);i++);
  if (text[i]==0) {
    fprintf(stderr,"Warning: must include ':' in '-t hr:mn' input - your date/time is probably incorrect!\n");
    return atoi(text)*3600L;
  }
  text[i]=0;
  hr=atoi(text);
  mn=atoi(text+i+1);

  return hr*3600L+mn*60L;
}
