/* map_addmodel.c
   ==============
   Author: R.J.Barnes and others
*/

/*
   See license.txt
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
#include "map_addmodel.h"
#include "map_addhmb.h"
#include "calc_bmag.h"
#include "hlpstr.h"

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
char *mod_tilts[] = {"negative","neutral","positive",0};
int   mod_tlti[]  = {0,10,20,-1};

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


struct CnvMapData *map;
struct GridData *grd;

struct OptionData opt;

struct model {
  char hemi[64];
  char tilt[64];
  char angle[64];
  char level[64];
  int ihem,ilev,itlt,iang;
  char str[3][128];
  float latref;
  int ltop,mtop;
  struct complex *aoeff_n;
  struct complex *aoeff_p;
};

int mnum = 0;
struct model *model[2][3][6][8]; /* [hemi][tilt][lev][ang] */


/*-----------------------------------------------------------------------------
 *
 * prototypes
 *
 *
 */
void add_model(struct CnvMapData *map,int num,struct GridGVec *ptr);
int solve_model(int num, struct GridGVec *ptr, float latmin, struct model *mod,
                int hemi, float decyear, int igrf_flag, int old_aacgm);
double factorial(double n);
void cmult(struct complex *a, struct complex *b, struct complex *c);
void slv_ylm_mod(float theta, float phi, int order, struct complex *ylm_p,
                 struct complex *ylm_n, double *anorm, double *plm_p,
                 double *apcnv);
void slv_sph_kset(float latmin, int num, float *phi, float *the,
                  float *the_col, double *ele_phi, double *ele_the,
                  struct model *mod);
struct GridGVec *get_model_pos(int Lmax, float latmin, int hemi,
                               int level, int *num);
struct model *determine_model(float Vsw, float Bx, float By, float Bz, int hemi,
                              float tilt, int imod, int nointerp);
struct model *interp_coeffs(int ih, float tilt, float mag, float cang, int imod);
struct model *load_model(FILE *fp, int ihem, int ilev, int iang, int itlt,
                                   int imod);
int load_all_models(char *path, int imod);


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

  int arg;
  unsigned char help=0;
  unsigned char option=0;
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
  int cs10 = 0;
  int rg96 = 0;
  int ts18 = 0;
  int psr10 = 0;
  int imod = 0;

  float bndstep = 5.; /* HMB parameters */
  float latref = 59;
  int bndnp;

  struct GridGVec *mdata=NULL;

  /* function pointers for file reading/writing (old and new) */
  int (*Map_Read)(FILE *, struct CnvMapData *, struct GridData *);
  int (*Map_Write)(FILE *, struct CnvMapData *, struct GridData *);

  map = CnvMapMake();
  grd = GridMake();

  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"-option",'x',&option);

  OptionAdd(&opt,"old",'x',&old);
  OptionAdd(&opt,"old_aacgm",'x',&old_aacgm);
  OptionAdd(&opt,"rg96",'x',&rg96);
  OptionAdd(&opt,"psr10",'x',&psr10);
  OptionAdd(&opt,"cs10",'x',&cs10);
  OptionAdd(&opt,"ts18",'x',&ts18);
  OptionAdd(&opt,"nointerp",'x',&nointerp);
  OptionAdd(&opt,"noigrf",'x',&noigrf);        /* SGS: default is to use IGRF
                                                       to compute model vecs  */
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

  if (arg !=argc) fname=argv[arg];

  if (dpstr !=NULL) {
    for (i=0;(dstr[i] !=0) && (dstr[i] !=tolower(dpstr[0]));i++);
    doping=i+1;
  }

  if (fname==NULL) fp=stdin;
  else fp=fopen(fname,"r");
  if (fp==NULL) {
    fprintf(stderr,"Grid file not found.\n");
    exit(-1);
  }

  /* SGS: 201703 default is CS10 */
  if (!cs10 && !psr10 && !rg96 && !ts18) cs10 = 1;
  if (rg96)  imod = RG96;
  if (psr10) imod = PSR10;
  if (cs10)  imod = CS10;
  if (ts18)  imod = TS18;

  envstr=getenv("SD_MODEL_TABLE");
  if (envstr==NULL) {
    fprintf(stderr,"Environment variable SD_MODEL_TABLE must be defined.\n");
    exit(-1);
  }

  status = load_all_models(envstr,imod);
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
      if (!noigrf)    IGRF_SetDateTime(yr,mo,dy,hr,mt,(int)sc);
      if (!old_aacgm) AACGM_v2_SetDateTime(yr,mo,dy,hr,mt,(int)sc);
      first = 0;
    }

    /* SGS: does this overide the tilt set with flag in map_addimf()? */
    if (imod == TS18 || imod == CS10 || imod == PSR10)
      tilt = IGRF_Tilt(yr,mo,dy,hr,mt,(int)sc);
    map->tilt = tilt;

    /* determine the model */
    mod = determine_model(map->Vx, map->Bx, map->By, map->Bz,
                          map->hemisphere, tilt, imod, nointerp);

    /* Add lower latitude limit (HMB) from model if not found from data */
    if (map->latmin == -1) {
      bndnp = 360/bndstep + 1;
      map_addhmb(yr,yrsec,map,bndnp,bndstep,latref,mod->latref,old_aacgm);
    }

    if (order != 0)   map->fit_order    = order;
    if (doping != -1) map->doping_level = doping;

    /* get the position of the model vectors */
    if ((mod != oldmod) || (map->latmin != oldlatmin)) {

      if (mdata != NULL) free(mdata);

      mdata = get_model_pos(map->fit_order,fabs(map->latmin),map->hemisphere,
                            map->doping_level,&num);

      /* solve for the model */
      status = solve_model(num, mdata,fabs(map->latmin), mod, map->hemisphere,
                           decyear, noigrf, old_aacgm);
      if (status != 0) {
        fprintf(stderr,"Failed to solve statistical model.\n");
        exit(-1);
      }
      oldmod = mod;
      oldlatmin = map->latmin;
    }

    /* now transform the model vectors and add them to the map file */
    add_model(map,num,mdata);

    strcpy(map->imf_model[0],mod->angle);
    strcpy(map->imf_model[1],mod->level);
    strcpy(map->imf_model[2],mod->tilt);
    switch (imod) {
      case RG96:  strcpy(map->imf_model[3],"RG96"); break;
      case PSR10: strcpy(map->imf_model[3],"PSR10"); break;
      case CS10:  strcpy(map->imf_model[3],"CS10"); break;
      case TS18:  strcpy(map->imf_model[3],"TS18"); break;
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

  ptr->aoeff_p=malloc(sizeof(struct complex)*(ptr->ltop+1)*(ptr->ltop+1));
  if (ptr->aoeff_p==NULL) {
    free(ptr);
    return NULL;
  }
  ptr->aoeff_n=malloc(sizeof(struct complex)*(ptr->ltop+1)*(ptr->ltop+1));
  if (ptr->aoeff_n==NULL) {
    free(ptr->aoeff_p);
    free(ptr);
    return NULL;
  }
  memset(ptr->aoeff_p,0,sizeof(struct complex)*(ptr->ltop+1)*(ptr->ltop+1));
  memset(ptr->aoeff_n,0,sizeof(struct complex)*(ptr->ltop+1)*(ptr->ltop+1));


  for (l=0; l<=ptr->ltop; l++) {
    for (m=-l; m<=l; m++) {
      if (fscanf(fp,"%d %d %g %g",&lx,&mx,&cr,&ci) != 4) break;

      if (m < 0) {
        k = l*(ptr->ltop+1)-m;
        ptr->aoeff_n[k].x = cr;
        ptr->aoeff_n[k].y = ci;
      } else {
        k = l*(ptr->ltop+1)+m;
        ptr->aoeff_p[k].x = cr;
        ptr->aoeff_p[k].y = ci;
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


int load_all_models(char *path, int imod)
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
      fprintf(stderr, "TS18 Statistical Model not yet implemented\n");
      return (-1);
      for (i=0; i<TS18_nlev; i++) {
        for (j=0; j<TS18_nang; j++) {
          for (k=0; mod_tilt[k] != NULL; k++) {
            sprintf(fname,"%s/ts18/mod_%s_%s_%s.spx",path,TS18_mod_lev[i],
                           TS18_mod_ang[j],mod_tilt[k]);
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
  struct complex *Ap,*Bp,*Cp,*Dp,*Ep,*Fp,*Gp,*Hp;
  struct complex *An,*Bn,*Cn,*Dn,*En,*Fn,*Gn,*Hn;

  /* These are hardcoded to the largest possible number of
   * model bins (currently CS10) - EGT */
  int nang=0,nlev=0;
  float alow[8],ahgh[8];
  float mlow[6],mhgh[6];
  float mod_angil[8],mod_angih[8];
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

  for (i=0; i<3; i++) strcpy(ptr->str[i],model[0][0][0][0]->str[i]);
  ptr->ltop = model[0][0][0][0]->ltop;
  ptr->mtop = model[0][0][0][0]->mtop;

  strcpy(ptr->hemi,mod_hemi[ih]);
  sprintf(ptr->tilt, "tilt %5.1f deg.",tilt);
  sprintf(ptr->level,"Esw  %5.1f mV/m",mag);
  sprintf(ptr->angle,"Bang %5.0f deg.",cang);

  ptr->aoeff_p=malloc(sizeof(struct complex)*(ptr->ltop+1)*(ptr->ltop+1));
  ptr->aoeff_n=malloc(sizeof(struct complex)*(ptr->ltop+1)*(ptr->ltop+1));
  memset(ptr->aoeff_p,0,sizeof(struct complex)*(ptr->ltop+1)*(ptr->ltop+1));
  memset(ptr->aoeff_n,0,sizeof(struct complex)*(ptr->ltop+1)*(ptr->ltop+1));

  ptr->ihem = ih;
  ptr->ilev = im1;
  ptr->iang = ia1;
  ptr->itlt = it1;

  /* do tri-linear interpolation of coeffs */
  if (ia1 == nang-1) ia2 = 0;
  else ia2 = ia1+1;

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

  Ap = malloc(sizeof(struct complex)*(ptr->ltop+1)*(ptr->ltop+1));
  Bp = malloc(sizeof(struct complex)*(ptr->ltop+1)*(ptr->ltop+1));
  Cp = malloc(sizeof(struct complex)*(ptr->ltop+1)*(ptr->ltop+1));
  Dp = malloc(sizeof(struct complex)*(ptr->ltop+1)*(ptr->ltop+1));
  Ep = malloc(sizeof(struct complex)*(ptr->ltop+1)*(ptr->ltop+1));
  Fp = malloc(sizeof(struct complex)*(ptr->ltop+1)*(ptr->ltop+1));
  Gp = malloc(sizeof(struct complex)*(ptr->ltop+1)*(ptr->ltop+1));
  Hp = malloc(sizeof(struct complex)*(ptr->ltop+1)*(ptr->ltop+1));

  An = malloc(sizeof(struct complex)*(ptr->ltop+1)*(ptr->ltop+1));
  Bn = malloc(sizeof(struct complex)*(ptr->ltop+1)*(ptr->ltop+1));
  Cn = malloc(sizeof(struct complex)*(ptr->ltop+1)*(ptr->ltop+1));
  Dn = malloc(sizeof(struct complex)*(ptr->ltop+1)*(ptr->ltop+1));
  En = malloc(sizeof(struct complex)*(ptr->ltop+1)*(ptr->ltop+1));
  Fn = malloc(sizeof(struct complex)*(ptr->ltop+1)*(ptr->ltop+1));
  Gn = malloc(sizeof(struct complex)*(ptr->ltop+1)*(ptr->ltop+1));
  Hn = malloc(sizeof(struct complex)*(ptr->ltop+1)*(ptr->ltop+1));

  for (l=0; l<=ptr->ltop; l++) {
    for (m=-l; m<=l; m++) {
      if (m < 0) {
        k = l*(ptr->ltop+1)-m;
        An[k].x = model[ih][it1][im1][ia1]->aoeff_n[k].x/denom;
        An[k].y = model[ih][it1][im1][ia1]->aoeff_n[k].y/denom;
        Bn[k].x = model[ih][it1][im1][ia2]->aoeff_n[k].x/denom;
        Bn[k].y = model[ih][it1][im1][ia2]->aoeff_n[k].y/denom;
        Cn[k].x = model[ih][it1][im2][ia1]->aoeff_n[k].x/denom;
        Cn[k].y = model[ih][it1][im2][ia1]->aoeff_n[k].y/denom;
        Dn[k].x = model[ih][it1][im2][ia2]->aoeff_n[k].x/denom;
        Dn[k].y = model[ih][it1][im2][ia2]->aoeff_n[k].y/denom;
        En[k].x = model[ih][it2][im1][ia1]->aoeff_n[k].x/denom;
        En[k].y = model[ih][it2][im1][ia1]->aoeff_n[k].y/denom;
        Fn[k].x = model[ih][it2][im1][ia2]->aoeff_n[k].x/denom;
        Fn[k].y = model[ih][it2][im1][ia2]->aoeff_n[k].y/denom;
        Gn[k].x = model[ih][it2][im2][ia1]->aoeff_n[k].x/denom;
        Gn[k].y = model[ih][it2][im2][ia1]->aoeff_n[k].y/denom;
        Hn[k].x = model[ih][it2][im2][ia2]->aoeff_n[k].x/denom;
        Hn[k].y = model[ih][it2][im2][ia2]->aoeff_n[k].y/denom;
      } else {
        k = l*(ptr->ltop+1)+m;
        Ap[k].x = model[ih][it1][im1][ia1]->aoeff_p[k].x/denom;
        Ap[k].y = model[ih][it1][im1][ia1]->aoeff_p[k].y/denom;
        Bp[k].x = model[ih][it1][im1][ia2]->aoeff_p[k].x/denom;
        Bp[k].y = model[ih][it1][im1][ia2]->aoeff_p[k].y/denom;
        Cp[k].x = model[ih][it1][im2][ia1]->aoeff_p[k].x/denom;
        Cp[k].y = model[ih][it1][im2][ia1]->aoeff_p[k].y/denom;
        Dp[k].x = model[ih][it1][im2][ia2]->aoeff_p[k].x/denom;
        Dp[k].y = model[ih][it1][im2][ia2]->aoeff_p[k].y/denom;
        Ep[k].x = model[ih][it2][im1][ia1]->aoeff_p[k].x/denom;
        Ep[k].y = model[ih][it2][im1][ia1]->aoeff_p[k].y/denom;
        Fp[k].x = model[ih][it2][im1][ia2]->aoeff_p[k].x/denom;
        Fp[k].y = model[ih][it2][im1][ia2]->aoeff_p[k].y/denom;
        Gp[k].x = model[ih][it2][im2][ia1]->aoeff_p[k].x/denom;
        Gp[k].y = model[ih][it2][im2][ia1]->aoeff_p[k].y/denom;
        Hp[k].x = model[ih][it2][im2][ia2]->aoeff_p[k].x/denom;
        Hp[k].y = model[ih][it2][im2][ia2]->aoeff_p[k].y/denom;
      }
    }
  }

  for (l=0; l<=ptr->ltop; l++) {
    for (m=-l; m<=l; m++) {
      if (m < 0) {
        k = l*(ptr->ltop+1)-m;

        ptr->aoeff_n[k].x = An[k].x*afp*mgp*dtp + Bn[k].x*afn*mgp*dtp +
                            Cn[k].x*afp*mgn*dtp + Dn[k].x*afn*mgn*dtp +
                            En[k].x*afp*mgp*dtn + Fn[k].x*afn*mgp*dtn +
                            Gn[k].x*afp*mgn*dtn + Hn[k].x*afn*mgn*dtn;

        ptr->aoeff_n[k].y = An[k].y*afp*mgp*dtp + Bn[k].y*afn*mgp*dtp +
                            Cn[k].y*afp*mgn*dtp + Dn[k].y*afn*mgn*dtp +
                            En[k].y*afp*mgp*dtn + Fn[k].y*afn*mgp*dtn +
                            Gn[k].y*afp*mgn*dtn + Hn[k].y*afn*mgn*dtn;
      } else {
        k = l*(ptr->ltop+1)+m;

        ptr->aoeff_p[k].x = Ap[k].x*afp*mgp*dtp + Bp[k].x*afn*mgp*dtp +
                            Cp[k].x*afp*mgn*dtp + Dp[k].x*afn*mgn*dtp +
                            Ep[k].x*afp*mgp*dtn + Fp[k].x*afn*mgp*dtn +
                            Gp[k].x*afp*mgn*dtn + Hp[k].x*afn*mgn*dtn;

        ptr->aoeff_p[k].y = Ap[k].y*afp*mgp*dtp + Bp[k].y*afn*mgp*dtp +
                            Cp[k].y*afp*mgn*dtp + Dp[k].y*afn*mgn*dtp +
                            Ep[k].y*afp*mgp*dtn + Fp[k].y*afn*mgp*dtn +
                            Gp[k].y*afp*mgn*dtn + Hp[k].y*afn*mgn*dtn;

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

  return (ptr);
}


struct model *determine_model(float Vsw, float Bx, float By, float Bz, int hemi,
                              float tilt, int imod, int nointerp)
{
  int ihem,itlt, ilev,iang,i;
  float esw,bt,bazm;
  struct model *imodel = NULL;

  bt   = sqrt(By*By + Bz*Bz);

  /* flip sign of By for shemi in models w/o shemi patterns */
  if (hemi < 0 && (imod == RG96 || imod == TS18)) By = -By;
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
      if (mod_tlti[i] == -1) i--;
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
        if (mod_tlti[i] == -1) i--;
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
        if (mod_tlti[i] == -1) i--;
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
  }

  return (imodel); 
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

double factorial(double n)
{
  double nfac=1;
  int m;
  for (m=n;m>0;m--) nfac=nfac*m;
  return nfac;
}

void cmult(struct complex *a,struct complex *b,struct complex *c)
{
  a->x = b->x*c->x - b->y*c->y;
  a->y = b->x*c->y + b->y*c->x;
}

void slv_ylm_mod(float theta, float phi, int order, struct complex *ylm_p,
                 struct complex *ylm_n, double *anorm, double *plm_p,
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

      ylm_p[l*(order+1)+m].x=Pmm*anorm[l*(order+1)+m]*cos(m*phi);
      ylm_p[l*(order+1)+m].y=Pmm*anorm[l*(order+1)+m]*sin(m*phi);
      ylm_n[l*(order+1)+m].x=pow(-1,m)*ylm_p[l*(order+1)+m].x;
      ylm_n[l*(order+1)+m].y=-pow(-1,m)*ylm_p[l*(order+1)+m].y;

    }
  }
}

void slv_sph_kset(float latmin, int num, float *phi, float *the,
                  float *the_col, double *ele_phi, double *ele_the,
                  struct model *mod)
{
  int i,m,l,n;
  int ltop,mtop;
  struct complex *ylm_px=NULL;
  struct complex *ylm_nx=NULL;
  double *plm_px=NULL;
  struct complex *xot_arr=NULL;

  double *pot_arr=NULL;
  struct complex Ix;
  struct complex T1,T2;
  struct complex t;
/*  float Re=6362.0+300.0; */
  float Rd = Radial_Dist/1000.;  /* using values in shfconst.h */

  int mlow,mhgh;

  double *anorm,*apcnv;

  ltop=mod->ltop;
  mtop=mod->mtop;
  ylm_px=malloc(sizeof(struct complex)*(ltop+1)*(ltop+1)*num);
  ylm_nx=malloc(sizeof(struct complex)*(ltop+1)*(ltop+1)*num);
  plm_px=malloc(sizeof(struct complex)*(ltop+1)*(ltop+1)*num);
  pot_arr=malloc(sizeof(double)*num);
  xot_arr=malloc(sizeof(struct complex)*num);
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

    Ix.x=0;
    Ix.y=0;
    for (l=0;l<=ltop;l++) {
      mlow=-l;
      if (mtop<l) mlow=-mtop;
      mhgh=-mlow;

      for (m=mlow;m<0;m++) {

        cmult(&t,&mod->aoeff_n[l*(ltop+1)-m],
              &ylm_nx[i*(ltop+1)*(ltop+1)+l*(ltop+1)-m]);

        Ix.x += t.x;
        Ix.y += t.y;
      }

      for (m=0;m<=mhgh;m++) {

        cmult(&t,&mod->aoeff_p[l*(ltop+1)+m],
              &ylm_px[i*(ltop+1)*(ltop+1)+l*(ltop+1)+m]);
         Ix.x += t.x;
         Ix.y += t.y;
       }
    }

    pot_arr[i]   = Ix.x;
    xot_arr[i].x = Ix.x;
    xot_arr[i].y = Ix.y;
  }

  for (i=0;i<num;i++) {
    Ix.x=0;
    Ix.y=0;

    for (l=0;l<=ltop;l++) {

      mlow=-l;
      if (mtop<l) mlow=-mtop;
      mhgh=-mlow;

      for (m=mlow;m<0;m++) {
        cmult(&t,&mod->aoeff_n[l*(ltop+1)-m],
              &ylm_nx[i*(ltop+1)*(ltop+1)+l*(ltop+1)-m]);
        Ix.x += m*t.x;
        Ix.y += m*t.y;

      }

      for (m=0;m<=mhgh;m++) {

         cmult(&t,&mod->aoeff_p[l*(ltop+1)+m],
               &ylm_px[i*(ltop+1)*(ltop+1)+l*(ltop+1)+m]);
         Ix.x += m*t.x;
         Ix.y += m*t.y; 
       }
    }

    ele_phi[i] = (1000.0/(Rd*sin(the_col[i])))*Ix.y;

    Ix.x=0;
    Ix.y=0;
    for (l=0;l<=ltop;l++) {
      mlow=-l;
      if (mtop<l) mlow=-mtop;
      mhgh=-mlow;

      for (m=mlow;m<0;m++) {
        n=-m;
        T1.x=n*cos(the[i])/sin(the[i])*
            plm_px[i*(ltop+1)*(ltop+1)+l*(ltop+1)-m];
        T1.y=n*cos(the[i])/sin(the[i])*
            plm_px[i*(ltop+1)*(ltop+1)+l*(ltop+1)-m];
        if ((n+1) <=l) {
           T2.x=plm_px[i*(ltop+1)*(ltop+1)+l*(ltop+1)+n+1];
           T2.y=plm_px[i*(ltop+1)*(ltop+1)+l*(ltop+1)+n+1];
        } else {
          T2.x=0;
          T2.y=0;
        }

        T1.x=(T1.x+T2.x)*pow(-1,m)*cos(m*phi[i])*anorm[l*(ltop+1)-m];
        T1.y=(T1.y+T2.y)*pow(-1,m)*sin(m*phi[i])*anorm[l*(ltop+1)-m];
        cmult(&t,&T1,&mod->aoeff_n[l*(ltop+1)-m]);
        Ix.x += t.x; 
        Ix.y += t.y;
      }

      for (m=0;m<=mhgh;m++) {
        T1.x=m*cos(the[i])/sin(the[i])*
             plm_px[i*(ltop+1)*(ltop+1)+l*(ltop+1)+m];
        T1.y=m*cos(the[i])/sin(the[i])*
             plm_px[i*(ltop+1)*(ltop+1)+l*(ltop+1)+m];
        if ((m+1) <=l) {
          T2.x=plm_px[i*(ltop+1)*(ltop+1)+l*(ltop+1)+m+1];
          T2.y=plm_px[i*(ltop+1)*(ltop+1)+l*(ltop+1)+m+1];
        } else {
          T2.x=0;
          T2.y=0;
        }
        T1.x=(T1.x+T2.x)*cos(m*phi[i])*anorm[l*(ltop+1)+m];
        T1.y=(T1.y+T2.y)*sin(m*phi[i])*anorm[l*(ltop+1)+m];
        cmult(&t,&T1,&mod->aoeff_p[l*(ltop+1)+m]);

        Ix.x += t.x; 
        Ix.y += t.y;
      }
    }
    if (latmin > 0)
      ele_the[i]=-1000.0*(180.0/(90.0-latmin))/Rd*Ix.x;
    else 
      ele_the[i]=-1000.0*(180.0/(90.0+latmin))/Rd*Ix.x;
  }
}


int solve_model(int num, struct GridGVec *ptr, float latmin, struct model *mod,
                int hemi, float decyear, int noigrf, int old_aacgm)
{
  int i;
  double *ele_phi=NULL,*ele_the=NULL;
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

  if ( (phi==NULL) || (the==NULL) || (the_col==NULL) ||
       (ele_the==NULL) ||(ele_phi==NULL) ) {
    if (phi !=NULL) free(phi);
    if (the !=NULL) free(the);
    if (the_col !=NULL) free(the_col);
    if (ele_the !=NULL) free(ele_the);
    if (ele_phi !=NULL) free(ele_phi);

    return -1;
  }

  for (i=0; i<num; i++) {
    phi[i]     = ptr[i].mlon*PI/180.0;
    the[i]     = (90-ptr[i].mlat)*(1.0/(90-latmin))*PI;
    the_col[i] = (90-ptr[i].mlat)*PI/180.0;
    ele_phi[i] = 0;
    ele_the[i] = 0;
  }

  slv_sph_kset(latmin,num,phi,the,the_col,ele_phi,ele_the,mod);

  for (i=0; i<num; i++) {
    ele_phi[i] = ele_phi[i]*hemi;
    ele_the[i] = ele_the[i]*hemi;

    if (noigrf) { /* use dipole value for B */
      bmag = -1e3*bpolar*(1 - 3*Altitude/Re)*
              sqrt(3.*(cos(the_col[i])*cos(the_col[i]))+1.)/2.;
    } else {
      bmag = 1e3*calc_bmag(hemi*ptr[i].mlat,ptr[i].mlon,decyear,old_aacgm);
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

    if (map->hemisphere==-1) map->model[i].mlat=-map->model[i].mlat;
    if (map->hemisphere==-1) map->model[i].azm=-map->model[i].azm;

     map->model[i].mlon-=map->mlt.av*15.0;
     if (map->model[i].mlon<-180) map->model[i].mlon+=360;
     if (map->model[i].mlon>180) map->model[i].mlon-=360;
  }
}

