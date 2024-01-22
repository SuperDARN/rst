/* map_addmodel.c
   ==============
   Author: R.J.Barnes and others

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
#include <math.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include "rtypes.h"
#include "rmath.h"
#include "option.h"
#include "rfile.h"
#include "rtime.h"
#include "griddata.h"

#include "cnvgrid.h"
#include "cnvmap.h"
#include "cnvmapread.h"
#include "oldcnvmapread.h"
#include "cnvmapwrite.h"
#include "oldcnvmapwrite.h"
#include "aacgm.h"
#include "mlt.h"
#include "aacgmlib_v2.h"
#include "mlt_v2.h"
#include "shfconst.h" /* use the same constants as in fitting procedure */
#include "igrfcall.h"
#include "igrflib.h"
#include "cnvmodel.h"
#include "map_addhmb.h"
#include "calc_bmag.h"
#include "hlpstr.h"

/*-----------------------------------------------------------------------------
 *
 * global variables
 *
 *
 */
struct CnvMapData *map;
struct GridData *grd;

struct OptionData opt;


/*-----------------------------------------------------------------------------
 *
 * prototypes
 *
 *
 */
void add_model(struct CnvMapData *map, int num, struct GridGVec *ptr);
int solve_model(int num, struct GridGVec *ptr, float latmin, struct model *mod,
                int hemi, float decyear, int igrf_flag, int magflg);
struct GridGVec *get_model_pos(int Lmax, float latmin, int hemi,
                               int level, int *num);
struct GridGVec *get_grid_pos(struct GridData *grd, int *num, float mlt);
struct GridGVec *get_model_pos_all(float latmin, int *num, float mlt);


/*-----------------------------------------------------------------------------
 *
 * function definitions
 *
 *
 */

int rst_opterr(char *txt) {
  fprintf(stderr,"Option not recognized: %s\n",txt);
  fprintf(stderr,"Please try: map_addmodel --help\n");
  return(-1);
}

int main(int argc,char *argv[]) {

  int old=0;
  int old_aacgm=0;
  int ecdip=0;

  int arg;
  unsigned char help=0;
  unsigned char option=0;
  unsigned char version=0;
  unsigned char vb=0;
  unsigned char *dpstr=NULL;

  FILE *fp;
  char *envstr;
  int num=0;
  int status;
  char *fname=NULL;
  int tme;
  int yrsec;
  int i,first;

  int cnt=0;

  int yr,mo,dy,hr,mt;
  double sc;

  int order=0;
  int doping=-1;

  char *dstr="lmh";

  struct model *mod = NULL;
  struct model *oldmod = NULL;
  float oldlatmin=-1;

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

  int residuals = 0;
  float dvel,dazm;
  float mvel,mazm;

  int all_model = 0;
  int data_model = 0;

  float bndstep = 5.; /* HMB parameters */
  float latref = 59;
  int bndnp;
  float bfac,del_L,mlt,latmin;

  struct GridGVec *mdata=NULL;

  /* function pointers for file reading/writing (old and new) */
  int (*Map_Read)(FILE *, struct CnvMapData *, struct GridData *);
  int (*Map_Write)(FILE *, struct CnvMapData *, struct GridData *);

  map = CnvMapMake();
  grd = GridMake();

  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"-option",'x',&option);
  OptionAdd(&opt,"-version",'x',&version);

  OptionAdd(&opt,"old",'x',&old);
  OptionAdd(&opt,"old_aacgm",'x',&old_aacgm);
  OptionAdd(&opt,"ecdip",'x',&ecdip);
  OptionAdd(&opt,"rg96",'x',&rg96);
  OptionAdd(&opt,"psr10",'x',&psr10);
  OptionAdd(&opt,"cs10",'x',&cs10);
  OptionAdd(&opt,"ts18",'x',&ts18);
  OptionAdd(&opt,"ts18_kp",'x',&ts18_kp);
  OptionAdd(&opt,"nointerp",'x',&nointerp);
  OptionAdd(&opt,"noigrf",'x',&noigrf);        /* SGS: default is to use IGRF
                                                       to compute model vecs  */
  OptionAdd(&opt,"residuals",'x',&residuals);
  OptionAdd(&opt,"all_model",'x',&all_model);
  OptionAdd(&opt,"data_model",'x',&data_model);

  OptionAdd(&opt,"vb",'x',&vb);
  OptionAdd(&opt,"o",'i',&order);
  OptionAdd(&opt,"d",'t',&dpstr);

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

  if (arg !=argc) fname=argv[arg];

  if (dpstr !=NULL) {
    for (i=0;(dstr[i] !=0) && (dstr[i] !=tolower(dpstr[0]));i++);
    doping=i+1;
  }

  if (fname==NULL) fp=stdin;
  else fp=fopen(fname,"r");
  if (fp==NULL) {
    fprintf(stderr,"File not found.\n");
    exit(-1);
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

  /* set function pointer to read/write old or new */
  if (old) {
    Map_Read  = &OldCnvMapFread;
    Map_Write = &OldCnvMapFwrite;
  } else {
    Map_Read  = &CnvMapFread;
    Map_Write = &CnvMapFwrite;
  }

  first = 1;
  while ((*Map_Read)(fp,map,grd) != -1) {

    tme = (grd->st_time + grd->ed_time)/2.0;
    TimeEpochToYMDHMS(tme,&yr,&mo,&dy,&hr,&mt,&sc);
    yrsec = TimeYMDHMSToYrsec(yr,mo,dy,hr,mt,(int) sc);
    decyear = yr + (float)yrsec/TimeYMDHMSToYrsec(yr,12,31,23,59,59);

    /* SGS: imf_flag set to 9 in map_addimf(), so not sure about this... */
    if (old) map->imf_flag = !noigrf;
    else     map->noigrf   = noigrf;

    if (first) {
      if (!noigrf || ecdip) IGRF_SetDateTime(yr,mo,dy,hr,mt,(int)sc);
      if (!old_aacgm) AACGM_v2_SetDateTime(yr,mo,dy,hr,mt,(int)sc);
      first = 0;
    }

    /* SGS: does this overide the tilt set with flag in map_addimf()? */
    if (imod == TS18 || imod == CS10 || imod == PSR10)
      tilt = IGRF_Tilt(yr,mo,dy,hr,mt,(int)sc);
    map->tilt = tilt;

    /* determine the model */
    mod = determine_model(map->Vx, map->Bx, map->By, map->Bz,
                          map->hemisphere, tilt, map->Kp, imod, nointerp);

    if (mod == NULL) {
      fprintf(stderr,"PSR10 model not defined for input conditions.\n");
      exit(-1);
    }

    /* Add lower latitude limit (HMB) from model if not found from data */
    if (map->latmin == -1 && !residuals && !all_model && !data_model) {
      bndnp = 360/bndstep + 1;
      map_addhmb(yr,yrsec,map,bndnp,bndstep,latref,mod->latref,magflg);
    }

    if (order != 0)   map->fit_order    = order;
    if (doping != -1) map->doping_level = doping;

    /* get the position of the model vectors */
    if ((mod != oldmod) || (map->latmin != oldlatmin) || (residuals)) {

      if (mdata != NULL) free(mdata);

      if (residuals || data_model) {
        if (map->hemisphere == 1) map->latmin = mod->latref;
        else                      map->latmin = -mod->latref;
        mdata = get_grid_pos(grd,&num,map->mlt.av); /* want model at grid data locations */
      } else if (all_model) {
        if (map->hemisphere == 1) map->latmin = mod->latref;
        else                      map->latmin = -mod->latref;
        mdata = get_model_pos_all(mod->latref,&num,map->mlt.av); /* want model at all possible locations */
      } else {
        mdata = get_model_pos(map->fit_order,fabs(map->latmin),map->hemisphere,
                              map->doping_level,&num);
      }

      /* solve for the model */
      status = solve_model(num, mdata, fabs(map->latmin), mod, map->hemisphere,
                           decyear, noigrf, magflg);
      if (status != 0) {
        fprintf(stderr,"Failed to solve statistical model.\n");
        exit(-1);
      }
      oldmod = mod;
      oldlatmin = map->latmin;
    }

    if (residuals) {
      /* model HMB parameters */
      bfac = (90-map->latmin)/(90-latref);
      del_L = bfac*5.5;

      /* Calculate residuals by subtracting projections of model vectors onto
         LOS data, from the LOS data */
      for (i=0; i<num; i++) {
        dvel = grd->data[i].vel.median;
        dazm = grd->data[i].azm;

        /* set model vectors below lower latitude limit (HMB) to zero */
        latmin = map->latmin;
        mlt = mdata[i].mlon/15.0;
        if ((mlt>=11) && (mlt<=19))    latmin = map->latmin + del_L*(1+cos((PI/8)*(mlt-11)));
        else if ((mlt<11) && (mlt>=5)) latmin = map->latmin + del_L*(1+cos((PI/6)*(11-mlt)));

        if (mdata[i].mlat <= latmin) {
          mdata[i].vel.median = 0.0;
          mdata[i].azm = 0.0;
        }

        /* model velocity projected onto vLOS direction */
        mazm = mdata[i].azm;
        mvel = mdata[i].vel.median*cos((dazm-mazm)*PI/180.);

        mdata[i].vel.median = dvel - mvel; /* difference from model */
        mdata[i].azm = grd->data[i].azm;   /* same as LOS direction */

        if (mdata[i].vel.median < 0) {
          mdata[i].vel.median = -mdata[i].vel.median; /* convention is >0 */
          if (mdata[i].azm < 0) mdata[i].azm += 180.;
          else mdata[i].azm -= 180.;
        }
      }
    }

    /* now transform the model vectors and add them to the map file */
    add_model(map,num,mdata);

    strcpy(map->imf_model[0],mod->angle);
    strcpy(map->imf_model[1],mod->level);
    strcpy(map->imf_model[2],mod->tilt);
    switch (imod) {
      case RG96:    strcpy(map->imf_model[3],"RG96"); break;
      case PSR10:   strcpy(map->imf_model[3],"PSR10"); break;
      case CS10:    strcpy(map->imf_model[3],"CS10"); break;
      case TS18:    strcpy(map->imf_model[3],"TS18"); break;
      case TS18_Kp: strcpy(map->imf_model[3],"TS18-Kp"); break;
    }

    (*Map_Write)(stdout,map,grd);

    if (vb == 1) {
      TimeEpochToYMDHMS(grd->st_time,&yr,&mo,&dy,&hr,&mt,&sc);
      fprintf(stderr,"%d-%d-%d %d:%d:%d %s %s\n",yr,mo,dy, hr,mt,(int) sc,
                      mod->level, mod->angle);  
    }

    cnt++;
  }

  return 0;
}


struct GridGVec *get_model_pos(int Lmax,float latmin,int hemi,
                               int level,int *num)
{
  struct GridGVec *ptr=NULL;
  int cnt=0;
  int LL,i,n;
  float dt,fact;
  int mpmax,mp,imax;
  float alpha;
  float lat1,lat2,lon,phi,dphi;

  if ((Lmax % 2) != 0) LL = Lmax+1;
  else                 LL = Lmax;

  dt = PI/(LL+2);
  fact = (level-1)/2+1;

  mpmax = Lmax*2*fact;
  imax  = LL/2+1;

  alpha = 180.0/(90.0-latmin);

  for (i=imax; i>0; i--) {
     mp = mpmax-2*(imax-i);
     lat1 = 90.0-(dt*i)/alpha*360/(2*PI);
     dphi = 2*PI/mp;
     lat2 = 90.0-(PI-dt*i)/alpha*360/(2*PI);

     for (n=0; n<mp; n++) {
       phi = n*dphi;
       lon = phi*180/PI;

       if (ptr == NULL) ptr = malloc(sizeof(struct GridGVec));
       else             ptr = realloc(ptr,sizeof(struct GridGVec)*(cnt+1));
       ptr[cnt].mlon       = lon;
       ptr[cnt].mlat       = lat1;
       ptr[cnt].azm        = 0;
       ptr[cnt].vel.median = 0;
       cnt++;
       if (i != imax) {
         if (ptr == NULL) ptr = malloc(sizeof(struct GridGVec));
         else             ptr = realloc(ptr,sizeof(struct GridGVec)*(cnt+1));
         ptr[cnt].mlon       = lon;
         ptr[cnt].mlat       = lat2;
         ptr[cnt].azm        = 0;
         ptr[cnt].vel.median = 0;
         cnt++;
       }
     }
  }

  *num = cnt;

  return ptr;
}


struct GridGVec *get_grid_pos(struct GridData *grd,int *num,float mlt)
{
  struct GridGVec *ptr=NULL;
  int cnt=0;
  int i;

  for (i=0;i<grd->vcnum;i++) {
    if (ptr == NULL) ptr = malloc(sizeof(struct GridGVec));
    else             ptr = realloc(ptr,sizeof(struct GridGVec)*(cnt+1));
    ptr[cnt].mlat       = fabs(grd->data[i].mlat);
    ptr[cnt].mlon       = grd->data[i].mlon + mlt*15.0;
    if (ptr[cnt].mlon > 360) ptr[cnt].mlon -= 360.0;
    ptr[cnt].azm        = 0;
    ptr[cnt].vel.median = 0;
    cnt++;
  }

  *num = cnt;

  return ptr;
}


struct GridGVec *get_model_pos_all(float latmin,int *num,float mlt)
{
  struct GridGVec *ptr=NULL;
  float nlat, nlon;
  float grdlat;
  double lspc;
  int cnt=0;
  int i, j;

  nlat = (int)(90.0-latmin);
  for (i=0;i<nlat;i++) {
    grdlat = i + (int)latmin + 0.5;
    lspc = ((int)(360*cos(fabs(grdlat)*PI/180)+0.5))/360.;
    nlon = lspc*360.;

    for (j=0;j<nlon;j++) {
      if (ptr == NULL) ptr = malloc(sizeof(struct GridGVec));
      else             ptr = realloc(ptr,sizeof(struct GridGVec)*(cnt+1));

      ptr[cnt].mlat       = grdlat;
      ptr[cnt].mlon       = ((j*360./nlon)+((360./nlon)/2.0))+mlt*15.0;
      if (ptr[cnt].mlon > 360) ptr[cnt].mlon -= 360.0;
      ptr[cnt].azm        = 0;
      ptr[cnt].vel.median = 0;
      //ptr[cnt].index = 1000*( (int)ptr[cnt].mlat ) + (int)(ptr[cnt].mlon*lspc);
      cnt++;
    }
  }

  *num = cnt;

  return ptr;
}


int solve_model(int num, struct GridGVec *ptr, float latmin, struct model *mod,
                int hemi, float decyear, int noigrf, int magflg)
{
  int i;
  double *ele_phi=NULL,*ele_the=NULL,*pot=NULL;
  float *phi=NULL,*the=NULL,*the_col=NULL; 
  double bpolar;
  double bmag = -0.5e-4;

  /* SGS: Altitude is a constant defined in shfconst.h to be 300 km */

  if (hemi == 1) bpolar = BNorth;  /* SGS: defined in shfconst.h */
  else           bpolar = BSouth;

  if (mod==NULL) return -1; 

  phi     = malloc(sizeof(float)*num);
  the     = malloc(sizeof(float)*num);
  the_col = malloc(sizeof(float)*num);
  ele_the = malloc(sizeof(double)*num);
  ele_phi = malloc(sizeof(double)*num);
  pot     = malloc(sizeof(double)*num);

  if ( (phi==NULL) || (the==NULL) || (the_col==NULL) ||
       (ele_the==NULL) ||(ele_phi==NULL) || (pot==NULL) ) {
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
    the[i]     = (90-ptr[i].mlat)*(1.0/(90-latmin))*PI;
    the_col[i] = (90-ptr[i].mlat)*PI/180.0;
    ele_phi[i] = 0;
    ele_the[i] = 0;
    pot[i]     = 0;
  }

  slv_sph_kset(latmin,num,phi,the,the_col,ele_phi,ele_the,mod,pot);

  for (i=0; i<num; i++) {
    ele_phi[i] = ele_phi[i]*hemi;
    ele_the[i] = ele_the[i]*hemi;

    if (noigrf) { /* use dipole value for B */
      bmag = -1e3*bpolar*(1 - 3*Altitude/Re)*
              sqrt(3.*(cos(the_col[i])*cos(the_col[i]))+1.)/2.;
    } else {
      bmag = 1e3*calc_bmag(hemi*ptr[i].mlat,ptr[i].mlon,decyear,magflg);
    }

    ptr[i].azm        = atan2(ele_the[i]/bmag,ele_phi[i]/bmag)*180./PI;
    ptr[i].vel.median = sqrt( ele_the[i]*ele_the[i] +
                              ele_phi[i]*ele_phi[i] )/bmag;
  }

  free(the);
  free(the_col);
  free(phi);
  free(ele_phi);
  free(ele_the);
  free(pot);

  return 0;
}


void add_model(struct CnvMapData *map,int num,struct GridGVec *ptr)
{
  int off,i;
  off=map->num_model;

  map->num_model+=num;

  if (map->model==NULL) map->model=malloc(sizeof(struct GridGVec)*
                                            map->num_model);
  else map->model=realloc(map->model,sizeof(struct GridGVec)*map->num_model);

  /* move the existing vectors down - the model always comes first
     in the model vector list */
  memmove(&map->model[num],map->model, off*sizeof(struct GridGVec));

  /* copy vectors */
  memcpy(map->model,ptr,num*sizeof(struct GridGVec));

  /* now correct for MLT offset */
  for (i=0;i<num;i++) {

    if (map->hemisphere==-1) {
      map->model[i].mlat=-map->model[i].mlat;
      map->model[i].azm=-map->model[i].azm;
    }

    map->model[i].mlon-=map->mlt.av*15.0;
    if ((map->model[i].mlon<0) || (map->model[i].mlon>360)) {
      map->model[i].mlon -= floor(map->model[i].mlon/360.)*360.;
    }
  }
}
