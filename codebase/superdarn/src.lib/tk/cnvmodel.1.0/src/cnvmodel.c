/* cnvmodel.c
   ==========
   Author: R.J.Barnes and others

       RST is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.

    Modifications
    =============
    2020-11-12 Marina Schmidt converted RST complex -> C library complex
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <complex.h>
#include "rtypes.h"
#include "rmath.h"

#include "shfconst.h" /* use the same constants as in fitting procedure */
#include "cnvmodel.h"

//TODO: address these notes?
/*-----------------------------------------------------------------------------
 * Notes:
 *
 * - Why are we loading all models instead of just the one that we need?
 *   Check first that the desired model is loaded (!=NULL) if not then load it.
 *
 * - Need better error checking on model selection so no NULL model is returned.
 *
 * - Is there a correction for most extreme Bz+ PSR10 model?
 *
 */

/*-----------------------------------------------------------------------------
 *
 * global variables
 *
 *
 */
char *mod_hemi[6] = {"north","south",0};
char *mod_tilt[] = {"DP-","DP0","DP+",0};
char *mod_tilts[] = {"negative","neutral","positive"};
int   mod_tlti[]  = {-10,10,-1};
struct model *model[2][3][6][8];
/*
 * RG96 Model bins
 * ---------------
 */
int   RG96_nang = 8;
char *RG96_mod_ang[]   = {"315t45", "0t90"   , "45t135", "90t180" , "135t225",
                          "180t270", "225t315", "270t360", 0};
char *RG96_mod_angs[]  = {"Bz+"   , "Bz+/By+", "By+"   , "Bz-/By+", "Bz-"    ,
                          "Bz-/By-", "By-"    , "Bz+/By-", 0};
float RG96_mod_angil[] = {-22.5, 22.5, 67.5, 112.5, 157.5, 202.5, 247.5, 292.5};
float RG96_mod_angih[] = {22.5, 67.5, 112.5, 157.5, 202.5, 247.5, 292.5, 337.5};

int   RG96_nlev = 5;
char *RG96_mod_lev[]   = {"2t3"   ,"0t4"   ,"4t6"   ,"6t12"   ,"7t20"   , 0};
char *RG96_mod_levs[]  = {"2<Kp<3","0<BT<4","4<BT<6","6<BT<12","7<BT<20", 0};
float RG96_mod_levi[]  = {0,4,6,12,20,-1};

/*
 * PSR10 Model bins
 * ----------------
 */
int   PSR10_nang = 8;
      /* bins are same as RG96 but filenames are same as CS10 */

int   PSR10_nlev = 3;
char *PSR10_mod_lev[]   = {"0t3"   ,"3t5"   ,"5t10"   ,0};
char *PSR10_mod_levs[]  = {"0<BT<3","3<BT<5","5<BT<10",0};
float PSR10_mod_levi[]  = {3,5,10,-1};

/*
 * CS10 Model bins
 * ---------------
 */
int   CS10_nang = 8;
char *CS10_mod_ang[]   = {"Bz+", "Bz+_By+", "By+", "Bz-_By+", "Bz-",
                          "Bz-_By-", "By-", "Bz+_By-",0};
char *CS10_mod_angs[]  = {"Bz+", "Bz+/By+", "By+", "Bz-/By+", "Bz-",
                          "Bz-/By-", "By-", "Bz+/By-",0};
float CS10_mod_angil[] = {-25, 25, 70, 110, 155, 205, 250, 290};
float CS10_mod_angih[] = {25, 70, 110, 155, 205, 250, 290, 335};

int   CS10_nlev = 6;
char *CS10_mod_lev[]   = {"0.00t1.20","1.20t1.70","1.70t2.20","2.20t2.90",
                          "2.90t4.10","4.10t20.00",0};
char *CS10_mod_levs[]  = {"0<Esw<1.2","1.2<Esw<1.7","1.7<Esw<2.2","2.2<Esw<2.9",
                          "2.9<Esw<4.1","4.1<Esw<20",0};
float CS10_mod_levi[]  = {1.2, 1.7, 2.2, 2.9, 4.1, 20, -1};

/*
 * TS18 Model bins
 * ---------------
 */
int   TS18_nang = 8;
char *TS18_mod_ang[]   = {"Bz+", "Bz+_By+", "By+", "Bz-_By+", "Bz-",
                          "Bz-_By-", "By-", "Bz+_By-",0};
char *TS18_mod_angs[]  = {"Bz+", "Bz+/By+", "By+", "Bz-/By+", "Bz-",
                          "Bz-/By-", "By-", "Bz+/By-",0};
float TS18_mod_angil[] = {-25, 25, 70, 110, 155, 205, 250, 290};
float TS18_mod_angih[] = {25, 70, 110, 155, 205, 250, 290, 335};

int   TS18_nlev = 5;
char *TS18_mod_lev[]   = {"0.00t1.20","1.20t1.60","1.60t2.10","2.10t3.00",
                          "3.00t20.00",0};
char *TS18_mod_levs[]  = {"0<Esw<1.2","1.2<Esw<1.6","1.6<Esw<2.1","2.1<Esw<3.0",
                          "3.0<Esw<20",0};
float TS18_mod_levi[]  = {1.2, 1.6, 2.1, 3.0, 20, -1};

/*
 * TS18-Kp Model bins
 * ---------------
 */
int   TS18_Kp_nang = 8;
char *TS18_Kp_mod_ang[]   = {"Bz+", "Bz+_By+", "By+", "Bz-_By+", "Bz-",
                             "Bz-_By-", "By-", "Bz+_By-",0};
char *TS18_Kp_mod_angs[]  = {"Bz+", "Bz+/By+", "By+", "Bz-/By+", "Bz-",
                             "Bz-/By-", "By-", "Bz+/By-",0};
float TS18_Kp_mod_angil[] = {-25, 25, 70, 110, 155, 205, 250, 290};
float TS18_Kp_mod_angih[] = {25, 70, 110, 155, 205, 250, 290, 335};

int   TS18_Kp_nlev = 6;
char *TS18_Kp_mod_lev[]   = {"0t1","1t2","2t3","3t4",
                             "4t6","6t8",0};
char *TS18_Kp_mod_levs[]  = {"0<Kp<1","1<Kp<2","2<Kp<3","3<Kp<4",
                             "4<Kp<6","6<Kp<8",0};
float TS18_Kp_mod_levi[]  = {1, 2, 3, 4, 6, 8, -1};


int mnum = 0;


//TODO:
/*-----------------------------------------------------------------------------
 *
 * function definitions
 *
 *
 */

struct model *load_model(FILE *fp, int ihem, int ilev, int iang,
                                   int itlt, int imod)
{
  struct model *ptr = NULL;
  int i,k,l,m,lx,mx;
  float cr,ci;

  ptr = malloc(sizeof(struct model));
  if (ptr==NULL) return NULL;
  ptr->ihem = ihem;
  ptr->ilev = ilev;
  ptr->iang = iang;
  ptr->itlt = itlt;

  switch (imod) {
    case RG96:
      strcpy(ptr->hemi,"Null");
      strcpy(ptr->tilt,"Null");
      strcpy(ptr->level,RG96_mod_levs[ilev]);
      strcpy(ptr->angle,RG96_mod_angs[iang]);
      break;
    case PSR10:
      strcpy(ptr->hemi,mod_hemi[ihem]);
      strcpy(ptr->tilt,mod_tilts[itlt]);
      strcpy(ptr->level,PSR10_mod_levs[ilev]);
      strcpy(ptr->angle,RG96_mod_angs[iang]); /* same as RG96 */
      break;
    case CS10:
      strcpy(ptr->hemi,mod_hemi[ihem]);
      strcpy(ptr->tilt,mod_tilts[itlt]);
      strcpy(ptr->level,CS10_mod_levs[ilev]);
      strcpy(ptr->angle,CS10_mod_angs[iang]);
      break;
    case TS18:
      strcpy(ptr->hemi,"Null");
      strcpy(ptr->tilt,mod_tilts[itlt]);
      strcpy(ptr->level,TS18_mod_levs[ilev]);
      strcpy(ptr->angle,TS18_mod_angs[iang]); /* same as CS10 */
      break;
    case TS18_Kp:
      strcpy(ptr->hemi,"Null");
      strcpy(ptr->tilt,"Null");
      strcpy(ptr->level,TS18_Kp_mod_levs[ilev]);
      strcpy(ptr->angle,TS18_Kp_mod_angs[iang]); /* same as CS10 */
      break;
  }

  for (i=0; i<3; i++) 
    if (fgets(ptr->str[i],128,fp) == NULL) break;
  if (i < 3) {
    free(ptr);
    return NULL;
  }

  if (fscanf(fp,"%g",&ptr->latref) !=1) {
    free(ptr);
    return NULL;
  }

  if (fscanf(fp,"%d %d",&ptr->ltop,&ptr->mtop) !=2) {
    free(ptr);
    return NULL;
  }

  ptr->aoeff_p=malloc(sizeof(double complex)*(ptr->ltop+1)*(ptr->ltop+1));
  if (ptr->aoeff_p==NULL) {
    free(ptr);
    return NULL;
  }
  ptr->aoeff_n=malloc(sizeof(double complex)*(ptr->ltop+1)*(ptr->ltop+1));
  if (ptr->aoeff_n==NULL) {
    free(ptr->aoeff_p);
    free(ptr);
    return NULL;
  }
  memset(ptr->aoeff_p,0,sizeof(double complex)*(ptr->ltop+1)*(ptr->ltop+1));
  memset(ptr->aoeff_n,0,sizeof(double complex)*(ptr->ltop+1)*(ptr->ltop+1));


  for (l=0; l<=ptr->ltop; l++) {
    for (m=-l; m<=l; m++) {
      if (fscanf(fp,"%d %d %g %g",&lx,&mx,&cr,&ci) != 4) break;

      if (m < 0) {
        k = l*(ptr->ltop+1)-m;
        ptr->aoeff_n[k] = CMPLX(cr, ci);
      } else {
        k = l*(ptr->ltop+1)+m;
        ptr->aoeff_p[k] = CMPLX(cr, ci);
      }

    }
    if (m <= l) break;
  }

  if (l <= ptr->ltop) {
    free(ptr->aoeff_n);
    free(ptr->aoeff_p);
    free(ptr);
  }

  return ptr;
}


int load_all_models(char *path, int imod, int ecdip)
{
  char fname[256];
  FILE *fp;
  int h,i,j,k;

  switch (imod) {
    case RG96:  /***********************************************************/
      for (i=0; i<RG96_nlev; i++) {
        for (j=0; j<RG96_nang; j++) {
          sprintf(fname,"%s/rg96/mod_%s_%s.spx",path,RG96_mod_lev[i],
                         RG96_mod_ang[j]);
          fp = fopen(fname,"r");
          if (fp == NULL) continue;
          model[0][0][i][j] = load_model(fp,-1,i,j,-1,imod);
          fclose(fp);
          if (model[0][0][i][j] == NULL) continue;
          mnum++;
        }
      }
      break;
    case PSR10:  /***********************************************************/
      for (h=0; mod_hemi[h] != NULL; h++) {
        for (i=0; i<PSR10_nlev; i++) {
          for (j=0; j<PSR10_nang; j++) {
            for (k=0; mod_tilt[k] != NULL; k++) {
              sprintf(fname,"%s/psr10/mod_%s_%s_%s_%s.spx",path,mod_hemi[h],
                             PSR10_mod_lev[i],CS10_mod_ang[j],mod_tilt[k]);
              fp = fopen(fname,"r");
              if (fp == NULL) continue;
              model[h][k][i][j] = load_model(fp,h,i,j,k,imod);
              fclose(fp);
              if (model[h][k][i][j] == NULL) continue;
              mnum++;
            }
          }
        }
      }
      break;
    case CS10:  /***********************************************************/
      for (h=0; mod_hemi[h] != NULL; h++) {
        for (i=0; i<CS10_nlev; i++) {
          for (j=0; j<CS10_nang; j++) {
            for (k=0; mod_tilt[k] != NULL; k++) {
              if ((i==5) && (j>2) && (j<6)) continue;   /* skip extreme Bz- */

              sprintf(fname,"%s/cs10/mod_%s_%s_%s_%s.spx",path,mod_hemi[h],
                             CS10_mod_lev[i],CS10_mod_ang[j],mod_tilt[k]);
              fp = fopen(fname,"r");
              if (fp == NULL) continue;
              model[h][k][i][j] = load_model(fp,h,i,j,k,imod);
              fclose(fp);
              if (model[h][k][i][j] == NULL) continue;
              mnum++;
            }
          }
        }
      }
      break;
    case TS18:  /***********************************************************/
      for (i=0; i<TS18_nlev; i++) {
        for (j=0; j<TS18_nang; j++) {
          for (k=0; mod_tilt[k] != NULL; k++) {
            if (ecdip) {
              sprintf(fname,"%s/ecdip/ts18/mod_%s_%s_%s.spx",path,TS18_mod_lev[i],
                             TS18_mod_ang[j],mod_tilt[k]);
            } else {
              sprintf(fname,"%s/ts18/mod_%s_%s_%s.spx",path,TS18_mod_lev[i],
                             TS18_mod_ang[j],mod_tilt[k]);
            }
            fp = fopen(fname,"r");
            if (fp == NULL) continue;
            model[0][k][i][j] = load_model(fp,-1,i,j,k,imod);
            fclose(fp);
            if (model[0][k][i][j] == NULL) continue;
            mnum++;
          }
        }
      }
      break;
    case TS18_Kp:  /********************************************************/
      for (i=0; i<TS18_Kp_nlev; i++) {
        for (j=0; j<TS18_Kp_nang; j++) {
          sprintf(fname,"%s/ts18_kp/mod_%s_%s.spx",path,TS18_Kp_mod_lev[i],
                         TS18_Kp_mod_ang[j]);
          fp = fopen(fname,"r");
          if (fp == NULL) continue;
          model[0][0][i][j] = load_model(fp,-1,i,j,-1,imod);
          fclose(fp);
          if (model[0][0][i][j] == NULL) continue;
          mnum++;
        }
      }
      break;
    default:
      fprintf(stderr, "Statistical Model %d not defined.\n", imod);
      return (-1);
  }

  return (0);
}  

/* Ideally this should be a more generalized function that allows other models
   to also be interpolated. Each model may have a different number of
   parameters to consider.
 */
struct model *interp_coeffs(int ih, float tilt, float mag, float cang, int imod)
{
  struct model *ptr=NULL;
  float tlow[2] = {-20, 0};
  float thgh[2] = {  0,20};
  int i,it1,it2,im1,im2,ia1,ia2,l,m,k;
  double afac,afac_l,afac_h,denom;
  float Al,Bl,Cl,Dl,El,Fl,Gl,Hl;
  double dtp,dtn,mgp,mgn,afp,afn;
  double complex *Ap,*Bp,*Cp,*Dp,*Ep,*Fp,*Gp,*Hp;
  double complex *An,*Bn,*Cn,*Dn,*En,*Fn,*Gn,*Hn;

  /* These are hardcoded to the largest possible number of
   * model bins (currently CS10) - EGT */
  int nang=0,nlev=0;
  float alow[8],ahgh[8];
  float mlow[6],mhgh[6];
  float mod_angil[8],mod_angih[8];
  // TODO: maybe initialize these variables as good practice and removing warnings 
  float mod_levi[6];

  switch (imod) {
    case CS10:
      nang = CS10_nang;
      for (i=0; i<nang; i++) {
        mod_angil[i] = CS10_mod_angil[i];
        mod_angih[i] = CS10_mod_angih[i];
      }
      nlev = CS10_nlev;
      for (i=0; i<nlev-1; i++)
        mod_levi[i] = CS10_mod_levi[i];
      break;

    case TS18:
      nang = TS18_nang;
      for (i=0; i<nang; i++) {
        mod_angil[i] = TS18_mod_angil[i];
        mod_angih[i] = TS18_mod_angih[i];
      }
      nlev = TS18_nlev;
      for (i=0; i<nlev-1; i++)
        mod_levi[i] = TS18_mod_levi[i];
      break;
  }

  /* setup reference point arrays */
  mlow[0] = .5*mod_levi[0];
  mhgh[0] = .5*(mod_levi[0]+mod_levi[1]);
  for (i=1; i<nlev-1; i++) {
    mlow[i] = .5*(mod_levi[i-1]+mod_levi[i]);
    mhgh[i] = .5*(mod_levi[i]+mod_levi[i+1]);
  }
  if (imod == TS18) {
    mhgh[nlev-2] = 5.0;
  } else {
    mhgh[nlev-2] = 7.5;
  }

  for (i=0; i<nang-1; i++) {
    alow[i] = .5*(mod_angil[i]+mod_angih[i]);
    ahgh[i] = .5*(mod_angil[i+1]+mod_angih[i+1]);
  }
  alow[i] = .5*(mod_angil[i]+mod_angih[i]);
  ahgh[i] = alow[0]+360;

  /* restrict parameter values to within valid range */
  if (cang >= ahgh[nang-1]) cang -= 360.;
  if (cang < alow[0])       cang += 360.;

  if (mag > mhgh[nlev-2]) mag = mhgh[nlev-2];
  if (mag < mlow[0])      mag = mlow[0];

  if (tilt > thgh[1]) tilt = thgh[1];
  if (tilt < tlow[0]) tilt = tlow[0];

  /* check for Bz<0 saturation */
  if (imod == CS10) {
    if ((mag>mhgh[CS10_nlev-3]) && (cang>alow[2]) && (cang<ahgh[5]))
      mag = mhgh[CS10_nlev-3];
  }

  /* find nearest neighbors */
  it1 = (tilt < 0) ? 0 : 1;  /* dipole tilt */
  it2 = it1+1;

  for (im1=0; (mag > mhgh[im1]); im1++);  /* Magnitude */
  im2 = im1+1;

  for (ia1=0; ia1 < nang; ia1++)   /* Angle */
    if ((cang >= alow[ia1]) && (cang < ahgh[ia1])) break;

  /* set up new model structure */
  ptr = malloc(sizeof(struct model));
  if (ptr==NULL) return NULL;

  for (i=0; i<3; i++) strcpy(ptr->str[i], model[0][0][0][0]->str[i]);
  ptr->ltop = model[0][0][0][0]->ltop;
  ptr->mtop = model[0][0][0][0]->mtop;

  strcpy(ptr->hemi,mod_hemi[ih]);
  sprintf(ptr->tilt, "tilt %5.1f deg.",tilt);
  sprintf(ptr->level,"Esw  %5.1f mV/m",mag);
  sprintf(ptr->angle,"Bang %5.0f deg.",cang);

  ptr->aoeff_p=malloc(sizeof(double complex)*(ptr->ltop+1)*(ptr->ltop+1));
  ptr->aoeff_n=malloc(sizeof(double complex)*(ptr->ltop+1)*(ptr->ltop+1));
  memset(ptr->aoeff_p,0,sizeof(double complex)*(ptr->ltop+1)*(ptr->ltop+1));
  memset(ptr->aoeff_n,0,sizeof(double complex)*(ptr->ltop+1)*(ptr->ltop+1));

  ptr->ihem = ih;
  ptr->ilev = im1;
  ptr->iang = ia1;
  ptr->itlt = it1;

  /* do tri-linear interpolation of coeffs */
  if (ia1 == nang-1) ia2 = 0;
  else ia2 = ia1+1;

  /* check for Bz- saturation */
  if ((imod == CS10) && (im2 == 5) && ((ia2>2) && (ia2<6))) {
    ia2 = ia1;
  }

  afac_h = fabs(sin(.5*ahgh[ia1]*PI/180.));
  afac_l = fabs(sin(.5*alow[ia1]*PI/180.));
  afac   = fabs(sin(.5*cang*PI/180.));
  denom = (afac_h-afac_l)*(mhgh[im1]-mlow[im1])*(thgh[it1]-tlow[it1]);

  /* deltas for interpolation */
  dtp = thgh[it1] - tilt;
  dtn = tilt - tlow[it1];
  afp = afac_h - afac;
  afn = afac - afac_l;
  mgp = mhgh[im1] - mag;
  mgn = mag - mlow[im1];

  Ap = malloc(sizeof(double complex)*(ptr->ltop+1)*(ptr->ltop+1));
  Bp = malloc(sizeof(double complex)*(ptr->ltop+1)*(ptr->ltop+1));
  Cp = malloc(sizeof(double complex)*(ptr->ltop+1)*(ptr->ltop+1));
  Dp = malloc(sizeof(double complex)*(ptr->ltop+1)*(ptr->ltop+1));
  Ep = malloc(sizeof(double complex)*(ptr->ltop+1)*(ptr->ltop+1));
  Fp = malloc(sizeof(double complex)*(ptr->ltop+1)*(ptr->ltop+1));
  Gp = malloc(sizeof(double complex)*(ptr->ltop+1)*(ptr->ltop+1));
  Hp = malloc(sizeof(double complex)*(ptr->ltop+1)*(ptr->ltop+1));

  An = malloc(sizeof(double complex)*(ptr->ltop+1)*(ptr->ltop+1));
  Bn = malloc(sizeof(double complex)*(ptr->ltop+1)*(ptr->ltop+1));
  Cn = malloc(sizeof(double complex)*(ptr->ltop+1)*(ptr->ltop+1));
  Dn = malloc(sizeof(double complex)*(ptr->ltop+1)*(ptr->ltop+1));
  En = malloc(sizeof(double complex)*(ptr->ltop+1)*(ptr->ltop+1));
  Fn = malloc(sizeof(double complex)*(ptr->ltop+1)*(ptr->ltop+1));
  Gn = malloc(sizeof(double complex)*(ptr->ltop+1)*(ptr->ltop+1));
  Hn = malloc(sizeof(double complex)*(ptr->ltop+1)*(ptr->ltop+1));

  for (l=0; l<=ptr->ltop; l++) {
    for (m=-l; m<=l; m++) {
      if (m < 0) {
        k = l*(ptr->ltop+1)-m;
        An[k] = CMPLX(creal(model[ih][it1][im1][ia1]->aoeff_n[k])/denom, 
                cimag(model[ih][it1][im1][ia1]->aoeff_n[k])/denom);
        Bn[k] = CMPLX(creal(model[ih][it1][im1][ia2]->aoeff_n[k])/denom, 
                cimag(model[ih][it1][im1][ia2]->aoeff_n[k])/denom);
        Cn[k] = CMPLX(creal(model[ih][it1][im2][ia1]->aoeff_n[k])/denom, 
                cimag(model[ih][it1][im2][ia1]->aoeff_n[k])/denom);
        Dn[k] = CMPLX(creal(model[ih][it1][im2][ia2]->aoeff_n[k])/denom, 
                cimag(model[ih][it1][im2][ia2]->aoeff_n[k])/denom);
        En[k] = CMPLX(creal(model[ih][it2][im1][ia1]->aoeff_n[k])/denom, 
                cimag(model[ih][it2][im1][ia1]->aoeff_n[k])/denom);
        Fn[k] = CMPLX(creal(model[ih][it2][im1][ia2]->aoeff_n[k])/denom, 
                cimag(model[ih][it2][im1][ia2]->aoeff_n[k])/denom);
        Gn[k] = CMPLX(creal(model[ih][it2][im2][ia1]->aoeff_n[k])/denom, 
                cimag(model[ih][it2][im2][ia1]->aoeff_n[k])/denom);
        Hn[k] = CMPLX(creal(model[ih][it2][im2][ia2]->aoeff_n[k])/denom, 
                cimag(model[ih][it2][im2][ia2]->aoeff_n[k])/denom);
      } else {
        k = l*(ptr->ltop+1)+m;
        Ap[k] = CMPLX(creal(model[ih][it1][im1][ia1]->aoeff_p[k])/denom,
                cimag(model[ih][it1][im1][ia1]->aoeff_p[k])/denom);
        Bp[k] = CMPLX(creal(model[ih][it1][im1][ia2]->aoeff_p[k])/denom,
                cimag(model[ih][it1][im1][ia2]->aoeff_p[k])/denom);
        Cp[k] = CMPLX(creal(model[ih][it1][im2][ia1]->aoeff_p[k])/denom,
                cimag(model[ih][it1][im2][ia1]->aoeff_p[k])/denom);
        Dp[k] = CMPLX(creal(model[ih][it1][im2][ia2]->aoeff_p[k])/denom,
                cimag(model[ih][it1][im2][ia2]->aoeff_p[k])/denom);
        Ep[k] = CMPLX(creal(model[ih][it2][im1][ia1]->aoeff_p[k])/denom,
                cimag(model[ih][it2][im1][ia1]->aoeff_p[k])/denom);
        Fp[k] = CMPLX(creal(model[ih][it2][im1][ia2]->aoeff_p[k])/denom,
                cimag(model[ih][it2][im1][ia2]->aoeff_p[k])/denom);
        Gp[k] = CMPLX(creal(model[ih][it2][im2][ia1]->aoeff_p[k])/denom,
                cimag(model[ih][it2][im2][ia1]->aoeff_p[k])/denom);
        Hp[k] = CMPLX(creal(model[ih][it2][im2][ia2]->aoeff_p[k])/denom,
                cimag(model[ih][it2][im2][ia2]->aoeff_p[k])/denom);
      }
    }
  }

  for (l=0; l<=ptr->ltop; l++) {
    for (m=-l; m<=l; m++) {
      if (m < 0) {
        k = l*(ptr->ltop+1)-m;
        // note: aoeff_n[k]
        // TODO: rest of this should probably be a function
        ptr->aoeff_n[k] = CMPLX(creal(An[k])*afp*mgp*dtp + 
                creal(Bn[k])*afn*mgp*dtp + creal(Cn[k])*afp*mgn*dtp +
                creal(Dn[k])*afn*mgn*dtp + creal(En[k])*afp*mgp*dtn + 
                creal(Fn[k])*afn*mgp*dtn + creal(Gn[k])*afp*mgn*dtn + 
                creal(Hn[k])*afn*mgn*dtn, cimag(An[k])*afp*mgp*dtp +
                cimag(Bn[k])*afn*mgp*dtp + cimag(Cn[k])*afp*mgn*dtp +
                cimag(Dn[k])*afn*mgn*dtp + cimag(En[k])*afp*mgp*dtn +
                cimag(Fn[k])*afn*mgp*dtn + cimag(Gn[k])*afp*mgn*dtn +
                cimag(Hn[k])*afn*mgn*dtn);
      } else {
        k = l*(ptr->ltop+1)+m;
        // Note: aoeff_p[k]
        ptr->aoeff_p[k] = CMPLX(creal(Ap[k])*afp*mgp*dtp +
                creal(Bp[k])*afn*mgp*dtp + creal(Cp[k])*afp*mgn*dtp +
                creal(Dp[k])*afn*mgn*dtp + creal(Ep[k])*afp*mgp*dtn +
                creal(Fp[k])*afn*mgp*dtn + creal(Gp[k])*afp*mgn*dtn +
                creal(Hp[k])*afn*mgn*dtn, cimag(Ap[k])*afp*mgp*dtp +
                cimag(Bp[k])*afn*mgp*dtp + cimag(Cp[k])*afp*mgn*dtp +
                cimag(Dp[k])*afn*mgn*dtp + cimag(Ep[k])*afp*mgp*dtn +
                cimag(Fp[k])*afn*mgp*dtn + cimag(Gp[k])*afp*mgn*dtn +
                cimag(Hp[k])*afn*mgn*dtn);

      }
    }
  }

  /* interpolate boundary also */
  Al = model[ih][it1][im1][ia1]->latref/denom;
  Bl = model[ih][it1][im1][ia2]->latref/denom;
  Cl = model[ih][it1][im2][ia1]->latref/denom;
  Dl = model[ih][it1][im2][ia2]->latref/denom;
  El = model[ih][it2][im1][ia1]->latref/denom;
  Fl = model[ih][it2][im1][ia2]->latref/denom;
  Gl = model[ih][it2][im2][ia1]->latref/denom;
  Hl = model[ih][it2][im2][ia2]->latref/denom;

  ptr->latref = roundf( Al*afp*mgp*dtp + Bl*afn*mgp*dtp +
                        Cl*afp*mgn*dtp + Dl*afn*mgn*dtp +
                        El*afp*mgp*dtn + Fl*afn*mgp*dtn +
                        Gl*afp*mgn*dtn + Hl*afn*mgn*dtn );

  free(Ap);
  free(Bp);
  free(Cp);
  free(Dp);
  free(Ep);
  free(Fp);
  free(Gp);
  free(Hp);
  free(An);
  free(Bn);
  free(Cn);
  free(Dn);
  free(En);
  free(Fn);
  free(Gn);
  free(Hn);
  return (ptr);
}


struct model *determine_model(float Vsw, float Bx, float By, float Bz, int hemi,
                              float tilt, float kp, int imod, int nointerp)
{
  int ihem,itlt, ilev,iang,i;
  float esw,bt,bazm;
  struct model *imodel = NULL;

  bt   = sqrt(By*By + Bz*Bz);

  /* flip sign of By for shemi in models w/o shemi patterns */
  if (hemi < 0 && (imod == RG96 || imod == TS18 || imod == TS18_Kp)) By = -By;
  bazm = atan2(By,Bz)*180/PI;

  switch (imod) {
    case RG96:
      ihem = 0; /* no bins for these in RG96 */
      itlt = 0;

      if (bazm >= RG96_mod_angih[RG96_nang-1]) bazm -= 360;
      if (bazm <  RG96_mod_angil[0])           bazm += 360;

      /* magnitude */
      for (i=0; (RG96_mod_levi[i] !=-1) && (bt > RG96_mod_levi[i]); i++);
      if (RG96_mod_levi[i] == -1) i--;
      ilev = i;

      /* angle */
      for (i=0; i < RG96_nang; i++)
        if ( (bazm > RG96_mod_angil[i]) && (bazm < RG96_mod_angih[i]) ) break;
      if (i == RG96_nang) i--;
      iang = i;

      /* correct for extreme Bz+ */
      if ((ilev == 4) && (iang != 0)) ilev--;

      imodel = model[ihem][itlt][ilev][iang];
      break;

    case PSR10:
      if (hemi < 0) tilt = -tilt;

      /* hemisphere */ 
      ihem = (hemi < 0) ? 1 : 0;

      if (bazm >= RG96_mod_angih[RG96_nang-1]) bazm -= 360.;
      if (bazm <  RG96_mod_angil[0])           bazm += 360.;

      /* tilt */
      for (i=0; (mod_tlti[i] !=-1) && (tilt > mod_tlti[i]); i++);
      itlt = i;

      /* angle */
      for (i=0; i < PSR10_nang; i++)
        if ((bazm >= RG96_mod_angil[i]) && (bazm < RG96_mod_angih[i])) break;
      if (i == RG96_nang) i--;
      iang = i;

      /* magnitude */
      for (i=0; (PSR10_mod_levi[i] !=-1) && (bt >= PSR10_mod_levi[i]); i++);
      if (PSR10_mod_levi[i] == -1) i--;
      ilev = i;

      /* correct for extreme Bz- */
/*    if ((ilev==5) && (iang>2) && (iang<6)) ilev--;*/
/* SGS: is there a correction for PSR10??? */

      imodel = model[ihem][itlt][ilev][iang];

      break;

    case CS10:
      if (Vsw == 0) Vsw = 450.; /* not sure if this should be here: SGS */
                                /* Default solar wind velocity */
      esw = 1e-3*abs(Vsw*bt);
      if (hemi < 0) tilt = -tilt;

      /* hemisphere */
      ihem = (hemi < 0) ? 1 : 0;

      if (nointerp) {

        if (bazm >= CS10_mod_angih[CS10_nang-1]) bazm -= 360.;
        if (bazm <  CS10_mod_angil[0])           bazm += 360.;

        /* tilt */
        for (i=0; (mod_tlti[i] !=-1) && (tilt > mod_tlti[i]); i++);
        itlt = i;

        /* angle */
        for (i=0; i < CS10_nang; i++)
          if ((bazm >= CS10_mod_angil[i]) && (bazm < CS10_mod_angih[i])) break;
        if (i == CS10_nang) i--;
        iang = i;

        /* magnitude */
        for (i=0; (CS10_mod_levi[i] !=-1) && (esw >= CS10_mod_levi[i]); i++);
        if (CS10_mod_levi[i] == -1) i--;
        ilev = i;

        /* correct for extreme Bz- */
        if ((ilev==5) && (iang>2) && (iang<6)) ilev--;

        imodel = model[ihem][itlt][ilev][iang];

      } else imodel = interp_coeffs(ihem,tilt,esw,bazm,imod);

      break;

    case TS18:
      if (Vsw == 0) Vsw = 450.; /* not sure if this should be here: SGS */
                                /* Default solar wind velocity */
      esw = 1e-3*abs(Vsw*bt);
      if (hemi < 0) tilt = -tilt;

      ihem = 0; /* no hemisphere bins for TS18 */

      if (nointerp) {

        if (bazm >= TS18_mod_angih[TS18_nang-1]) bazm -= 360.;
        if (bazm <  TS18_mod_angil[0])           bazm += 360.;

        /* tilt */
        for (i=0; (mod_tlti[i] !=-1) && (tilt > mod_tlti[i]); i++);
        itlt = i;

        /* angle */
        for (i=0; i < TS18_nang; i++)
          if ((bazm >= TS18_mod_angil[i]) && (bazm < TS18_mod_angih[i])) break;
        if (i == TS18_nang) i--;
        iang = i;

        /* magnitude */
        for (i=0; (TS18_mod_levi[i] !=-1) && (esw >= TS18_mod_levi[i]); i++);
        if (TS18_mod_levi[i] == -1) i--;
        ilev = i;

        imodel = model[ihem][itlt][ilev][iang];

      } else imodel = interp_coeffs(ihem,tilt,esw,bazm,imod);

      break;

    case TS18_Kp:
      ihem = 0; /* no bins for these in TS18-Kp */
      itlt = 0;

      if (bazm >= TS18_Kp_mod_angih[TS18_Kp_nang-1]) bazm -= 360.;
      if (bazm <  TS18_Kp_mod_angil[0])              bazm += 360.;

      /* magnitude */
      for (i=0; (TS18_Kp_mod_levi[i] !=-1) && (kp >= TS18_Kp_mod_levi[i]); i++);
      if (TS18_Kp_mod_levi[i] == -1) i--;
      ilev = i;

      /* angle */
      for (i=0; i < TS18_Kp_nang; i++)
        if ((bazm >= TS18_Kp_mod_angil[i]) && (bazm < TS18_Kp_mod_angih[i])) break;
      if (i == TS18_Kp_nang) i--;
      iang = i;
  
      /* correct for extreme Kp */
      if ((ilev==5) && (iang!=4)) ilev--;

      imodel = model[ihem][itlt][ilev][iang];
      break;
  }

  return (imodel); 
}     


double factorial(double n)
{
  double nfac=1;
  int m;
  for (m=n;m>0;m--) nfac=nfac*m;
  return nfac;
}

/*
 * complex multiplication b * c
 * 
 * returns the product of b * c
 *
 * Modified: 2020-11-12 Marina Schmidt - made it into a pure function
 */
double complex cmult(double complex b,double complex c)
{
  double complex a; 
  a = CMPLX(creal(b)*creal(c) - cimag(b)*cimag(c),
          creal(b)*cimag(c) + cimag(b)*creal(c));
  return a;
}


void slv_ylm_mod(float theta, float phi, int order, double complex *ylm_p,
                 double complex *ylm_n, double *anorm, double *plm_p,
                 double *apcnv)
{
  int l,m,i;
  double x;
  double Pmm;
  double num,den;
  double numf,denf;
  for (l=0;l<=order;l++) {
    for (m=0;m<=l;m++) {
       num=l-m;
       den=l+m;

       numf=factorial(num);
       denf=factorial(den);

       anorm[l*(order+1)+m]=sqrt((2*l+1)/(4*PI)*numf/denf);
       apcnv[l*(order+1)+m]=pow(-1,m)*numf/denf;
    }
  }

  for (l=0;l<=order;l++) {
    for (m=0;m<=l;m++) {
      x=cos(theta);

      Pmm=1.0;

      if (m>0) {
        double fct;
        double sx2;
        sx2=sqrt((1-x)*(1+x));
        fct=1;
        for (i=1;i<=m;i++) {
          Pmm=-Pmm*fct*sx2;
          fct=fct+2;
        }
      }
      if (l !=m) {
         double pnmp1;
         pnmp1=x*(2*m+1)*Pmm;
         if (l != (m+1)) {
           double Pll=0;
           int ll;
           for (ll=m+2;ll<=l;ll++) {
             Pll=(x*(2*ll-1)*pnmp1-(ll+m-1)*Pmm)/(ll-m);
             Pmm=pnmp1;
             pnmp1=Pll;
           }
           Pmm=Pll;
         } else Pmm=pnmp1;
      }
      plm_p[l*(order+1)+m]=Pmm;

      ylm_p[l*(order+1)+m] = CMPLX(Pmm*anorm[l*(order+1)+m]*cos(m*phi), 
              Pmm*anorm[l*(order+1)+m]*sin(m*phi));
      ylm_n[l*(order+1)+m] = CMPLX(pow(-1,m)*creal(ylm_p[l*(order+1)+m]),
              -pow(-1,m)*cimag(ylm_p[l*(order+1)+m]));

    }
  }
}


void slv_sph_kset(float latmin, int num, float *phi, float *the,
                  float *the_col, double *ele_phi, double *ele_the,
                  struct model *mod, double *pot)
{
  int i,m,l,n;
  int ltop,mtop;
  double complex *ylm_px=NULL;
  double complex *ylm_nx=NULL;
  double *plm_px=NULL;
  double complex *xot_arr=NULL;

  double *pot_arr=NULL;
  double complex Ix;
  double complex T1,T2;
  double complex t;
/*  float Re=6362.0+300.0; */
  float Rd = Radial_Dist/1000.;  /* using values in shfconst.h */

  int mlow,mhgh;

  double *anorm,*apcnv;

  ltop=mod->ltop;
  mtop=mod->mtop;
  ylm_px=malloc(sizeof(double complex)*(ltop+1)*(ltop+1)*num);
  ylm_nx=malloc(sizeof(double complex)*(ltop+1)*(ltop+1)*num);
  plm_px=malloc(sizeof(double complex)*(ltop+1)*(ltop+1)*num);
  pot_arr=malloc(sizeof(double)*num);
  xot_arr=malloc(sizeof(double complex)*num);
  anorm=malloc(sizeof(double)*(ltop+1)*(ltop+1));
  apcnv=malloc(sizeof(double)*(ltop+1)*(ltop+1));


  if ((ylm_px==NULL) || (ylm_nx==NULL) || (plm_px==NULL) ||
      (pot_arr==NULL) || (xot_arr==NULL) || (anorm==NULL) ||
      (apcnv==NULL)) {
    if (ylm_px !=NULL) free(ylm_px);
    if (ylm_nx !=NULL) free(ylm_nx);
    if (plm_px !=NULL) free(plm_px);
    if (pot_arr !=NULL) free(pot_arr);
    if (xot_arr !=NULL) free(xot_arr);
    if (anorm !=NULL) free(anorm);
    if (apcnv !=NULL) free(apcnv);

  }

  for (i=0;i<num;i++) {
    slv_ylm_mod(the[i],phi[i],ltop, &ylm_px[(ltop+1)*(ltop+1)*i],
                                    &ylm_nx[(ltop+1)*(ltop+1)*i], anorm,
                                    &plm_px[(ltop+1)*(ltop+1)*i], apcnv);
  }

  for (i=0;i<num;i++) {

    Ix = CMPLX(0, 0);
    for (l=0;l<=ltop;l++) {
      mlow=-l;
      if (mtop<l) mlow=-mtop;
      mhgh=-mlow;

      for (m=mlow;m<0;m++) {

        t = cmult(mod->aoeff_n[l*(ltop+1)-m],
              ylm_nx[i*(ltop+1)*(ltop+1)+l*(ltop+1)-m]);

        Ix += CMPLX(creal(t), cimag(t));
      }

      for (m=0;m<=mhgh;m++) {

        t = cmult(mod->aoeff_p[l*(ltop+1)+m],
              ylm_px[i*(ltop+1)*(ltop+1)+l*(ltop+1)+m]);
        Ix += CMPLX(creal(t), cimag(t));
       }
    }

    pot[i] = creal(Ix);
    pot_arr[i] = creal(Ix);
    xot_arr[i] = CMPLX(creal(Ix), cimag(Ix));
  }

  for (i=0;i<num;i++) {
    Ix = CMPLX(0, 0);

    for (l=0;l<=ltop;l++) {

      mlow=-l;
      if (mtop<l) mlow=-mtop;
      mhgh=-mlow;

      for (m=mlow;m<0;m++) {
        t = cmult(mod->aoeff_n[l*(ltop+1)-m],
              ylm_nx[i*(ltop+1)*(ltop+1)+l*(ltop+1)-m]);
        Ix += CMPLX(m*creal(t), m*cimag(t));

      }

      for (m=0;m<=mhgh;m++) {

         t = cmult(mod->aoeff_p[l*(ltop+1)+m],
               ylm_px[i*(ltop+1)*(ltop+1)+l*(ltop+1)+m]);
         Ix += CMPLX(m*creal(t), m*cimag(t));
       }
    }

    ele_phi[i] = (1000.0/(Rd*sin(the_col[i])))*cimag(Ix);

    Ix = CMPLX(0, 0);
    for (l=0;l<=ltop;l++) {
      mlow=-l;
      if (mtop<l) mlow=-mtop;
      mhgh=-mlow;

      for (m=mlow;m<0;m++) {
        n=-m;
        T1 = CMPLX(n*cos(the[i])/sin(the[i])*
                plm_px[i*(ltop+1)*(ltop+1)+l*(ltop+1)-m],
                n*cos(the[i])/sin(the[i])*
                plm_px[i*(ltop+1)*(ltop+1)+l*(ltop+1)-m]);
        if ((n+1) <=l) {
           T2 = CMPLX(plm_px[i*(ltop+1)*(ltop+1)+l*(ltop+1)+n+1], 
                   plm_px[i*(ltop+1)*(ltop+1)+l*(ltop+1)+n+1]);
        } else {
          T2 = CMPLX(0, 0);
        }

        T1= CMPLX((creal(T1)+creal(T2))*pow(-1,m)*
                cos(m*phi[i])*anorm[l*(ltop+1)-m], 
                (cimag(T1)+cimag(T2))*pow(-1,m)*
                sin(m*phi[i])*anorm[l*(ltop+1)-m]);
        t = cmult(T1,mod->aoeff_n[l*(ltop+1)-m]);
        Ix += CMPLX(creal(t), cimag(t));
      }

      for (m=0;m<=mhgh;m++) {
        T1 = CMPLX(m*cos(the[i])/sin(the[i])*
                plm_px[i*(ltop+1)*(ltop+1)+l*(ltop+1)+m], 
                m*cos(the[i])/sin(the[i])*
                plm_px[i*(ltop+1)*(ltop+1)+l*(ltop+1)+m]);
        if ((m+1) <=l) {
          T2 = CMPLX(plm_px[i*(ltop+1)*(ltop+1)+l*(ltop+1)+m+1], 
                  plm_px[i*(ltop+1)*(ltop+1)+l*(ltop+1)+m+1]);
        } else {
          T2 = CMPLX(0, 0);
        }
        T1 = CMPLX((creal(T1)+creal(T2))*cos(m*phi[i])*anorm[l*(ltop+1)+m],
                (cimag(T1)+cimag(T2))*sin(m*phi[i])*anorm[l*(ltop+1)+m]);
        t = cmult(T1,mod->aoeff_p[l*(ltop+1)+m]);

        Ix += CMPLX(creal(t), cimag(t));
      }
    }
    if (latmin > 0)
      ele_the[i]=-1000.0*creal(Ix)*(180.0/(90.0-latmin))/Rd;
    else
      ele_the[i]=-1000.0*creal(Ix)*(180.0/(90.0+latmin))/Rd;
  }
}

