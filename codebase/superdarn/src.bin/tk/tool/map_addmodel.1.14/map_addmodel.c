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
  OptionAdd(&opt,"-version",'x',&version);

  OptionAdd(&opt,"old",'x',&old);
  OptionAdd(&opt,"old_aacgm",'x',&old_aacgm);
  OptionAdd(&opt,"rg96",'x',&rg96);
  OptionAdd(&opt,"psr10",'x',&psr10);
  OptionAdd(&opt,"cs10",'x',&cs10);
  OptionAdd(&opt,"ts18",'x',&ts18);
  OptionAdd(&opt,"ts18_kp",'x',&ts18_kp);
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
    fprintf(stderr,"Grid file not found.\n");
    exit(-1);
  }

  if (!ts18 && !ts18_kp && !cs10 && !psr10 && !rg96) ts18 = 1;
  if (rg96)    imod = RG96;
  if (psr10)   imod = PSR10;
  if (cs10)    imod = CS10;
  if (ts18_kp) imod = TS18_Kp;
  if (ts18)    imod = TS18;

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
                          map->hemisphere, tilt, map->Kp, imod, nointerp);

    if (mod == NULL) {
      fprintf(stderr,"PSR10 model not defined for input conditions.\n");
      exit(-1);
    }

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

