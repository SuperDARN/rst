/* map_addmodel.c
   =============== 
   Author: R.J.Barnes 
*/


/*
 (c) 2010 JHU/APL & Others - Please Consult LICENSE.superdarn-rst.3.2-beta-4-g32f7302.txt for more information.
 
 
 
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
#include "geopack.h"
#include "mlt.h"
#include "igrfcall.h"
#include "shfconst.h" /* use the same constants as in fitting procedure */

#include "hlpstr.h"


char *mod_hemi[6] = {"north","south",0};
char *mod_tilt[4] = {"DP-","DP0","DP+",0};
int   mod_tlti[4] = {0,10,20,-1};

char *CS10_mod_ang[16]  = {"Bz+", "Bz+_By+", "By+", 
			   "Bz-_By+", "Bz-", "Bz-_By-", "By-", "Bz+_By-",0};
char *CS10_mod_lev[16]  = {"0.00t1.20","1.20t1.70","1.70t2.20","2.20t2.90",
			   "2.90t4.10","4.10t20.00",0};
float CS10_mod_angil[8] = {-25, 25, 70, 110, 155, 205, 250, 290};
float CS10_mod_angih[8] = {25, 70, 110, 155, 205, 250, 290, 335};
float CS10_mod_levi[8]  = {1.2, 1.7, 2.2, 2.9, 4.1, 20, -1};
int   CS10_nlev = 6;
int   CS10_nang = 8;

/* RG96 model clock angle and magnitude bins */
char *RG96_mod_ang[16] = {"315t45", "0t90", "45t135", "90t180", 
			  "135t225", "180t270", "225t315", "270t360",0};
char *RG96_mod_lev[16] = {"2t3","0t4","4t6","6t12","7t20",0};
float RG96_mod_angil[8];
float RG96_mod_angih[8];
float RG96_mod_levi[8] = {0,4,6,12,20,-1};
int   RG96_nlev = 5;
int   RG96_nang = 8;

float latref=59;

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

int mnum=0;
struct model *model[2][3][6][8]; /* [hemi,tilt,lev,ang] */

struct model *load_model(FILE *fp,int ihem,int ilev,int iang,int itlt) {
  struct model *ptr=NULL;
  int i,l,m,lx,mx;
  float cr,ci;

  ptr=malloc(sizeof(struct model));
  if (ptr==NULL) return NULL;
  ptr->ihem=ihem;
  ptr->ilev=ilev;
  ptr->iang=iang;
  ptr->itlt=itlt;


  if (ihem != -1) { /* Using CS10 model */
    strcpy(ptr->hemi,mod_hemi[ihem]); 
    strcpy(ptr->tilt,mod_tilt[itlt]); 
    strcpy(ptr->level,CS10_mod_lev[ilev]);
    strcpy(ptr->angle,CS10_mod_ang[iang]);
  } else {
    strcpy(ptr->hemi,"Null");
    strcpy(ptr->tilt,"Null");
    strcpy(ptr->level,RG96_mod_lev[ilev]);
    strcpy(ptr->angle,RG96_mod_ang[iang]);
  }
  
  for (i=0;i<3;i++) 
    if (fgets(ptr->str[i],128,fp)==NULL) break;
  if (i<3) {
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


  for (l=0;l<=ptr->ltop;l++) {
    for (m=-l;m<=l;m++) {
      if (fscanf(fp,"%d %d %g %g",&lx,&mx,&cr,&ci) !=4) break;
     
      if (m<0) {
        ptr->aoeff_n[l*(ptr->ltop+1)-m].x=cr;
        ptr->aoeff_n[l*(ptr->ltop+1)-m].y=ci;
      }
      if (m>=0) {
        ptr->aoeff_p[l*(ptr->ltop+1)+m].x=cr;
        ptr->aoeff_p[l*(ptr->ltop+1)+m].y=ci;
      }

    }
    if (m<=l) break;
  }
  if (l<=ptr->ltop) {
    free(ptr->aoeff_n);
    free(ptr->aoeff_p);
    free(ptr);
  }
  return ptr;
}


int load_all_models(char *path, int cs10) {
  char fname[256];
  FILE *fp;
  int h,i,j,k;

  if (cs10 == 1) {
    for (h=0;mod_hemi[h] !=NULL;h++) {    
      for (i=0;i<CS10_nlev;i++) {
	for (j=0;j<CS10_nang;j++) {
	  for (k=0;mod_tilt[k] !=NULL;k++) {
	    if ((i==5) && (j>2) && (j<6)) continue;   /* skip extreme Bz- */
    
	    sprintf(fname,"%s/mod_%s_%s_%s_%s.spx",path,mod_hemi[h],
		    CS10_mod_lev[i],CS10_mod_ang[j],mod_tilt[k]);
	    fp=fopen(fname,"r");
	    if (fp==NULL) continue;
	    model[h][k][i][j]=load_model(fp,h,i,j,k); 
	    fclose(fp);  
	    if (model[h][k][i][j]==NULL) continue;
	    mnum++;
	  }
	}
      } 
    }
  } else {
    h = -1; 
    k = -1;
    for (i=0;i<RG96_nlev;i++) {
      for (j=0;j<RG96_nang;j++) {
	sprintf(fname,"%s/mod_%s_%s.spx",path,RG96_mod_lev[i],RG96_mod_ang[j]);
	fp=fopen(fname,"r");
	if (fp==NULL) continue;
	model[0][0][i][j]=load_model(fp,h,i,j,k);     
	fclose(fp);  
	if (model[0][0][i][j]==NULL) continue;
	mnum++;
      }
    }
  }
  return 0;
}  

struct model *interp_CS10_coeffs(int ih,float tilt, float mag,float cang) {
  struct model *ptr=NULL;
  float alow[8], ahgh[8];
  float mlow[5], mhgh[5];
  float tlow[2] = {-20,0};
  float thgh[2] = {0,20};
  int i,it,im,ia,ia2,l,m;
  double afac,afac_l,afac_h,denom;
  float Al,Bl,Cl,Dl,El,Fl,Gl,Hl;
  struct complex *Ap,*Bp,*Cp,*Dp,*Ep,*Fp,*Gp,*Hp;
  struct complex *An,*Bn,*Cn,*Dn,*En,*Fn,*Gn,*Hn;

  /* setup reference point arrays */

  mlow[0] = .5*CS10_mod_levi[0];
  mhgh[0] = .5*(CS10_mod_levi[0]+CS10_mod_levi[1]);
  for (i=1;i<CS10_nlev-1;i++) {
    mlow[i] = .5*(CS10_mod_levi[i-1]+CS10_mod_levi[i]);
    mhgh[i] = .5*(CS10_mod_levi[i]+CS10_mod_levi[i+1]);
  }
  mhgh[CS10_nlev-2] = 7.5;

  for (i=0;i<CS10_nang-1;i++) {
    alow[i] = .5*(CS10_mod_angil[i]+CS10_mod_angih[i]);
    ahgh[i] = .5*(CS10_mod_angil[i+1]+CS10_mod_angih[i+1]);
  }
  alow[i] = .5*(CS10_mod_angil[i]+CS10_mod_angih[i]);
  ahgh[i] = alow[0]+360;

  /* restrict parameter values to within valid range */
  if (cang >=ahgh[CS10_nang-1]) cang = cang - 360.;
  if (cang < alow[0])           cang = cang + 360.;

  if (mag > mhgh[CS10_nlev-2]) mag = mhgh[CS10_nlev-2];
  if (mag < mlow[0])           mag = mlow[0];

  if (tilt > thgh[1]) tilt = thgh[1];
  if (tilt < tlow[0]) tilt = tlow[0];

  /* check for Bz<0 saturation */
  if ((mag>mhgh[CS10_nlev-3]) && (cang>alow[2]) && (cang<ahgh[5])) mag = mhgh[CS10_nlev-3];

  /* find nearest neighbors */   
  if (tilt<0) it = 0; else it = 1; /* Tilt */ 

  for (im=0;(mag>mhgh[im]);im++);  /* Magnitude */
  
  /* Angle */
  for (ia=0;ia<8;ia++) {
    if ((cang>=alow[ia]) && (cang<ahgh[ia])) break;
  }

  /* set up new model structure */
  ptr=malloc(sizeof(struct model));
  if (ptr==NULL) return NULL;

  for (i=0;i<3;i++) strcpy(ptr->str[i],model[0][0][0][0]->str[i]);
  ptr->ltop = model[0][0][0][0]->ltop;
  ptr->mtop = model[0][0][0][0]->mtop;

  strcpy(ptr->hemi,mod_hemi[ih]); 
  sprintf(ptr->tilt, "%5.1f",tilt); 
  sprintf(ptr->level,"%5.1f",mag);
  sprintf(ptr->angle,"%5.0f",cang);

  ptr->aoeff_p=malloc(sizeof(struct complex)*(ptr->ltop+1)*(ptr->ltop+1));
  ptr->aoeff_n=malloc(sizeof(struct complex)*(ptr->ltop+1)*(ptr->ltop+1));
  memset(ptr->aoeff_p,0,sizeof(struct complex)*(ptr->ltop+1)*(ptr->ltop+1));
  memset(ptr->aoeff_n,0,sizeof(struct complex)*(ptr->ltop+1)*(ptr->ltop+1));

  ptr->ihem=ih;
  ptr->ilev=im;
  ptr->iang=ia;
  ptr->itlt=it;

  /* do tri-linear interpolation of coeffs */

  if (ia == CS10_nang-1) ia2 = 0;
  else ia2 = ia+1;
  
  afac_h = fabs(sin(.5*ahgh[ia]*PI/180.));
  afac_l = fabs(sin(.5*alow[ia]*PI/180.));
  afac   = fabs(sin(.5*cang*PI/180.));
  denom = (afac_h-afac_l)*(mhgh[im]-mlow[im])*(thgh[it]-tlow[it]);

  Ap=malloc(sizeof(struct complex)*(ptr->ltop+1)*(ptr->ltop+1));
  Bp=malloc(sizeof(struct complex)*(ptr->ltop+1)*(ptr->ltop+1));
  Cp=malloc(sizeof(struct complex)*(ptr->ltop+1)*(ptr->ltop+1));
  Dp=malloc(sizeof(struct complex)*(ptr->ltop+1)*(ptr->ltop+1));
  Ep=malloc(sizeof(struct complex)*(ptr->ltop+1)*(ptr->ltop+1));
  Fp=malloc(sizeof(struct complex)*(ptr->ltop+1)*(ptr->ltop+1));
  Gp=malloc(sizeof(struct complex)*(ptr->ltop+1)*(ptr->ltop+1));
  Hp=malloc(sizeof(struct complex)*(ptr->ltop+1)*(ptr->ltop+1));

  An=malloc(sizeof(struct complex)*(ptr->ltop+1)*(ptr->ltop+1));
  Bn=malloc(sizeof(struct complex)*(ptr->ltop+1)*(ptr->ltop+1));
  Cn=malloc(sizeof(struct complex)*(ptr->ltop+1)*(ptr->ltop+1));
  Dn=malloc(sizeof(struct complex)*(ptr->ltop+1)*(ptr->ltop+1));
  En=malloc(sizeof(struct complex)*(ptr->ltop+1)*(ptr->ltop+1));
  Fn=malloc(sizeof(struct complex)*(ptr->ltop+1)*(ptr->ltop+1));
  Gn=malloc(sizeof(struct complex)*(ptr->ltop+1)*(ptr->ltop+1));
  Hn=malloc(sizeof(struct complex)*(ptr->ltop+1)*(ptr->ltop+1));

  for (l=0;l<=ptr->ltop;l++) {
    for (m=-l;m<=l;m++) {     
      if (m<0) {
        An[l*(ptr->ltop+1)-m].x=model[ih][it][im][ia]->aoeff_n[l*(ptr->ltop+1)-m].x/denom;
        An[l*(ptr->ltop+1)-m].y=model[ih][it][im][ia]->aoeff_n[l*(ptr->ltop+1)-m].y/denom;
        Bn[l*(ptr->ltop+1)-m].x=model[ih][it][im][ia2]->aoeff_n[l*(ptr->ltop+1)-m].x/denom;
        Bn[l*(ptr->ltop+1)-m].y=model[ih][it][im][ia2]->aoeff_n[l*(ptr->ltop+1)-m].y/denom;
        Cn[l*(ptr->ltop+1)-m].x=model[ih][it][im+1][ia]->aoeff_n[l*(ptr->ltop+1)-m].x/denom;
        Cn[l*(ptr->ltop+1)-m].y=model[ih][it][im+1][ia]->aoeff_n[l*(ptr->ltop+1)-m].y/denom;
        Dn[l*(ptr->ltop+1)-m].x=model[ih][it][im+1][ia2]->aoeff_n[l*(ptr->ltop+1)-m].x/denom;
        Dn[l*(ptr->ltop+1)-m].y=model[ih][it][im+1][ia2]->aoeff_n[l*(ptr->ltop+1)-m].y/denom;
        En[l*(ptr->ltop+1)-m].x=model[ih][it+1][im][ia]->aoeff_n[l*(ptr->ltop+1)-m].x/denom;
        En[l*(ptr->ltop+1)-m].y=model[ih][it+1][im][ia]->aoeff_n[l*(ptr->ltop+1)-m].y/denom;
        Fn[l*(ptr->ltop+1)-m].x=model[ih][it+1][im][ia2]->aoeff_n[l*(ptr->ltop+1)-m].x/denom;
        Fn[l*(ptr->ltop+1)-m].y=model[ih][it+1][im][ia2]->aoeff_n[l*(ptr->ltop+1)-m].y/denom;
        Gn[l*(ptr->ltop+1)-m].x=model[ih][it+1][im+1][ia]->aoeff_n[l*(ptr->ltop+1)-m].x/denom;
        Gn[l*(ptr->ltop+1)-m].y=model[ih][it+1][im+1][ia]->aoeff_n[l*(ptr->ltop+1)-m].y/denom;
        Hn[l*(ptr->ltop+1)-m].x=model[ih][it+1][im+1][ia2]->aoeff_n[l*(ptr->ltop+1)-m].x/denom;
        Hn[l*(ptr->ltop+1)-m].y=model[ih][it+1][im+1][ia2]->aoeff_n[l*(ptr->ltop+1)-m].y/denom;
      }
      if (m>=0) {
        Ap[l*(ptr->ltop+1)+m].x=model[ih][it][im][ia]->aoeff_p[l*(ptr->ltop+1)+m].x/denom;
        Ap[l*(ptr->ltop+1)+m].y=model[ih][it][im][ia]->aoeff_p[l*(ptr->ltop+1)+m].y/denom;
        Bp[l*(ptr->ltop+1)+m].x=model[ih][it][im][ia2]->aoeff_p[l*(ptr->ltop+1)+m].x/denom;
        Bp[l*(ptr->ltop+1)+m].y=model[ih][it][im][ia2]->aoeff_p[l*(ptr->ltop+1)+m].y/denom;
        Cp[l*(ptr->ltop+1)+m].x=model[ih][it][im+1][ia]->aoeff_p[l*(ptr->ltop+1)+m].x/denom;
        Cp[l*(ptr->ltop+1)+m].y=model[ih][it][im+1][ia]->aoeff_p[l*(ptr->ltop+1)+m].y/denom;
        Dp[l*(ptr->ltop+1)+m].x=model[ih][it][im+1][ia2]->aoeff_p[l*(ptr->ltop+1)+m].x/denom;
        Dp[l*(ptr->ltop+1)+m].y=model[ih][it][im+1][ia2]->aoeff_p[l*(ptr->ltop+1)+m].y/denom;
        Ep[l*(ptr->ltop+1)+m].x=model[ih][it+1][im][ia]->aoeff_p[l*(ptr->ltop+1)+m].x/denom;
        Ep[l*(ptr->ltop+1)+m].y=model[ih][it+1][im][ia]->aoeff_p[l*(ptr->ltop+1)+m].y/denom;
        Fp[l*(ptr->ltop+1)+m].x=model[ih][it+1][im][ia2]->aoeff_p[l*(ptr->ltop+1)+m].x/denom;
        Fp[l*(ptr->ltop+1)+m].y=model[ih][it+1][im][ia2]->aoeff_p[l*(ptr->ltop+1)+m].y/denom;
        Gp[l*(ptr->ltop+1)+m].x=model[ih][it+1][im+1][ia]->aoeff_p[l*(ptr->ltop+1)+m].x/denom;
        Gp[l*(ptr->ltop+1)+m].y=model[ih][it+1][im+1][ia]->aoeff_p[l*(ptr->ltop+1)+m].y/denom;
        Hp[l*(ptr->ltop+1)+m].x=model[ih][it+1][im+1][ia2]->aoeff_p[l*(ptr->ltop+1)+m].x/denom;
        Hp[l*(ptr->ltop+1)+m].y=model[ih][it+1][im+1][ia2]->aoeff_p[l*(ptr->ltop+1)+m].y/denom;
      }
    }
  }
  for (l=0;l<=ptr->ltop;l++) {
    for (m=-l;m<=l;m++) {
      if (m<0) {
        ptr->aoeff_n[l*(ptr->ltop+1)-m].x= An[l*(ptr->ltop+1)-m].x*(afac_h-afac)*(mhgh[im]-mag)*(thgh[it]-tilt)
                                         + Bn[l*(ptr->ltop+1)-m].x*(afac-afac_l)*(mhgh[im]-mag)*(thgh[it]-tilt)
                                         + Cn[l*(ptr->ltop+1)-m].x*(afac_h-afac)*(mag-mlow[im])*(thgh[it]-tilt)
                                         + Dn[l*(ptr->ltop+1)-m].x*(afac-afac_l)*(mag-mlow[im])*(thgh[it]-tilt)
                                         + En[l*(ptr->ltop+1)-m].x*(afac_h-afac)*(mhgh[im]-mag)*(tilt-tlow[it])
                                         + Fn[l*(ptr->ltop+1)-m].x*(afac-afac_l)*(mhgh[im]-mag)*(tilt-tlow[it])
	                                 + Gn[l*(ptr->ltop+1)-m].x*(afac_h-afac)*(mag-mlow[im])*(tilt-tlow[it])
                                	 + Hn[l*(ptr->ltop+1)-m].x*(afac-afac_l)*(mag-mlow[im])*(tilt-tlow[it]);

        ptr->aoeff_n[l*(ptr->ltop+1)-m].y= An[l*(ptr->ltop+1)-m].y*(afac_h-afac)*(mhgh[im]-mag)*(thgh[it]-tilt)
                                         + Bn[l*(ptr->ltop+1)-m].y*(afac-afac_l)*(mhgh[im]-mag)*(thgh[it]-tilt)
                                         + Cn[l*(ptr->ltop+1)-m].y*(afac_h-afac)*(mag-mlow[im])*(thgh[it]-tilt)
                                         + Dn[l*(ptr->ltop+1)-m].y*(afac-afac_l)*(mag-mlow[im])*(thgh[it]-tilt)
                                         + En[l*(ptr->ltop+1)-m].y*(afac_h-afac)*(mhgh[im]-mag)*(tilt-tlow[it])
                                         + Fn[l*(ptr->ltop+1)-m].y*(afac-afac_l)*(mhgh[im]-mag)*(tilt-tlow[it])
	                                 + Gn[l*(ptr->ltop+1)-m].y*(afac_h-afac)*(mag-mlow[im])*(tilt-tlow[it])
                                	 + Hn[l*(ptr->ltop+1)-m].y*(afac-afac_l)*(mag-mlow[im])*(tilt-tlow[it]);
      }
      if (m>=0) {
        ptr->aoeff_p[l*(ptr->ltop+1)+m].x= Ap[l*(ptr->ltop+1)+m].x*(afac_h-afac)*(mhgh[im]-mag)*(thgh[it]-tilt)
                                         + Bp[l*(ptr->ltop+1)+m].x*(afac-afac_l)*(mhgh[im]-mag)*(thgh[it]-tilt)
                                         + Cp[l*(ptr->ltop+1)+m].x*(afac_h-afac)*(mag-mlow[im])*(thgh[it]-tilt)
                                         + Dp[l*(ptr->ltop+1)+m].x*(afac-afac_l)*(mag-mlow[im])*(thgh[it]-tilt)
                                         + Ep[l*(ptr->ltop+1)+m].x*(afac_h-afac)*(mhgh[im]-mag)*(tilt-tlow[it])
                                         + Fp[l*(ptr->ltop+1)+m].x*(afac-afac_l)*(mhgh[im]-mag)*(tilt-tlow[it])
	                                 + Gp[l*(ptr->ltop+1)+m].x*(afac_h-afac)*(mag-mlow[im])*(tilt-tlow[it])
                                	 + Hp[l*(ptr->ltop+1)+m].x*(afac-afac_l)*(mag-mlow[im])*(tilt-tlow[it]);

        ptr->aoeff_p[l*(ptr->ltop+1)+m].y= Ap[l*(ptr->ltop+1)+m].y*(afac_h-afac)*(mhgh[im]-mag)*(thgh[it]-tilt)
                                         + Bp[l*(ptr->ltop+1)+m].y*(afac-afac_l)*(mhgh[im]-mag)*(thgh[it]-tilt)
                                         + Cp[l*(ptr->ltop+1)+m].y*(afac_h-afac)*(mag-mlow[im])*(thgh[it]-tilt)
                                         + Dp[l*(ptr->ltop+1)+m].y*(afac-afac_l)*(mag-mlow[im])*(thgh[it]-tilt)
                                         + Ep[l*(ptr->ltop+1)+m].y*(afac_h-afac)*(mhgh[im]-mag)*(tilt-tlow[it])
                                         + Fp[l*(ptr->ltop+1)+m].y*(afac-afac_l)*(mhgh[im]-mag)*(tilt-tlow[it])
	                                 + Gp[l*(ptr->ltop+1)+m].y*(afac_h-afac)*(mag-mlow[im])*(tilt-tlow[it])
                                	 + Hp[l*(ptr->ltop+1)+m].y*(afac-afac_l)*(mag-mlow[im])*(tilt-tlow[it]);
      }
    }
  }

  /* interpolate boundary also */
  Al = model[ih][it][im][ia]->latref/denom;
  Bl = model[ih][it][im][ia2]->latref/denom;
  Cl = model[ih][it][im+1][ia]->latref/denom;
  Dl = model[ih][it][im+1][ia2]->latref/denom;
  El = model[ih][it+1][im][ia]->latref/denom;
  Fl = model[ih][it+1][im][ia2]->latref/denom;
  Gl = model[ih][it+1][im+1][ia]->latref/denom;
  Hl = model[ih][it+1][im+1][ia2]->latref/denom;

  ptr->latref = roundf(Al*(afac_h-afac)*(mhgh[im]-mag)*(thgh[it]-tilt)
		     + Bl*(afac-afac_l)*(mhgh[im]-mag)*(thgh[it]-tilt)
		     + Cl*(afac_h-afac)*(mag-mlow[im])*(thgh[it]-tilt)
		     + Dl*(afac-afac_l)*(mag-mlow[im])*(thgh[it]-tilt)
		     + El*(afac_h-afac)*(mhgh[im]-mag)*(tilt-tlow[it])
		     + Fl*(afac-afac_l)*(mhgh[im]-mag)*(tilt-tlow[it])
		     + Gl*(afac_h-afac)*(mag-mlow[im])*(tilt-tlow[it])
		     + Hl*(afac-afac_l)*(mag-mlow[im])*(tilt-tlow[it]));

  return ptr;
}

struct model *determine_model(float Vsw,float By,float Bz,int hemisphere,float tilt,int cs10,int nointerp) {
  int ilev,iang,itlt,ihem;
  float esw,bt,bazm;
  struct model *imod;

  if ((cs10 != 1) && (hemisphere < 0)) By = -By; /* Flip sign of By for South */

  bt=sqrt(By*By+Bz*Bz);
  
  bazm=atan2(By,Bz)*360/(2*PI);
    
  /* okay now work out the limits */

  if (cs10==1) {
    if (Vsw == 0) Vsw = 400.; /* Default solar wind velocity */
    esw = 1e-3*Vsw*bt;
    if (hemisphere < 0) tilt = -tilt;

    /* Hemisphere */
    if (hemisphere < 0) ihem = 1; else ihem = 0;
    
    if (nointerp != 1) {
      imod = interp_CS10_coeffs(ihem,tilt,esw,bazm);
    } else {

      if (bazm >=CS10_mod_angih[7]) bazm=bazm-360;
      if (bazm < CS10_mod_angil[0]) bazm=bazm+360;
  
      /* Tilt */ 
      for (itlt=0;(mod_tlti[itlt] !=-1) && (tilt>mod_tlti[itlt]);itlt++);  
      if (mod_tlti[itlt]==-1) itlt--;

      /* Angle */
      for (iang=0;iang<8;iang++) {
	if ((bazm>=CS10_mod_angil[iang]) && (bazm<CS10_mod_angih[iang])) break;
      }
      if (iang==8) iang--;

      /* Magnitude */
      for (ilev=0;(CS10_mod_levi[ilev] !=-1) && (esw>=CS10_mod_levi[ilev]);ilev++);  
      if (CS10_mod_levi[ilev]==-1) ilev--;

      /* correct for extreme Bz- */
      if ((ilev==5) && (iang>2) && (iang<6)) ilev--;

      imod = model[ihem][itlt][ilev][iang]; 
    }
  } else {
    ihem = 0;
    itlt = 0;

    if (bazm >=RG96_mod_angih[7]) bazm=bazm-360;
    if (bazm < RG96_mod_angil[0]) bazm=bazm+360;

    /* Magnitude */
    for (ilev=0;(RG96_mod_levi[ilev] !=-1) && (bt>RG96_mod_levi[ilev]);ilev++);  
    if (RG96_mod_levi[ilev]==-1) ilev--;

    /* Angle */
    for (iang=0;iang<8;iang++) {
      if ((bazm>RG96_mod_angil[iang]) && (bazm<RG96_mod_angih[iang])) break;
    }
    if (iang==8) iang--;

    /* correct for extreme Bz+ */

    if ((ilev==4) && (iang !=0)) ilev--;
    
    imod = model[ihem][itlt][ilev][iang]; 
  }

  return imod; 
  
}     


struct GridGVec *get_model_pos(int Lmax,float latmin,int hemisphere,
                           int level,int *num) {
  struct GridGVec *ptr=NULL;
  int cnt=0;
  int LL,i,n;
  float dt,fact;
  int mpmax,mp,imax;
  float alpha;
  float lat1,lat2,lon,phi,dphi;

  if ((Lmax % 2) !=0) LL=Lmax+1; 
  else LL=Lmax;

  dt=PI/(LL+2);
  fact=(level-1)/2+1;
  
  mpmax=Lmax*2*fact;
  imax=LL/2+1;

  alpha=180.0/(90.0-latmin);

  for (i=imax;i>0;i--) {
     mp=mpmax-2*(imax-i);
     lat1=90.0-(dt*i)/alpha*360/(2*PI);
     dphi=2*PI/mp;
     lat2=90.0-(PI-dt*i)/alpha*360/(2*PI);

 

     for (n=0;n<mp;n++) {
       phi=n*dphi;
       lon=phi*360/(2*PI);
    
       if (ptr==NULL) ptr=malloc(sizeof(struct GridGVec));
       else ptr=realloc(ptr,sizeof(struct GridGVec)*(cnt+1));
       ptr[cnt].mlon=lon;
       ptr[cnt].mlat=lat1;
       ptr[cnt].azm=0;
       ptr[cnt].vel.median=0;

       cnt++;
       if (i !=imax) {
         if (ptr==NULL) ptr=malloc(sizeof(struct GridGVec));
         else ptr=realloc(ptr,sizeof(struct GridGVec)*(cnt+1));
         ptr[cnt].mlon=lon;
         ptr[cnt].mlat=lat2;
         ptr[cnt].azm=0;
         ptr[cnt].vel.median=0;
         cnt++;
       }
     }
  }

 
  *num=cnt;

  return ptr;

}

double factorial(double n) {
  double nfac=1;
  int m;
  for (m=n;m>0;m--) nfac=nfac*m;
  return nfac;
}

void cmult(struct complex *a,struct complex *b,struct complex *c) {
  a->x=b->x*c->x-b->y*c->y;
  a->y=b->x*c->y+b->y*c->x;
}



void slv_ylm_mod(float theta,float phi,int order,struct complex *ylm_p,
                 struct complex *ylm_n,double *anorm,double *plm_p,
                 double *apcnv) {

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

void slv_sph_kset(float latmin,int num,float *phi,float *the,
                  float *the_col,double *ele_phi,double *ele_the,
                  struct model *mod) {
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
  float Rd = Radial_Dist/1000;

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
   
    slv_ylm_mod(the[i],phi[i],ltop,
                &ylm_px[(ltop+1)*(ltop+1)*i],
                &ylm_nx[(ltop+1)*(ltop+1)*i],
                anorm,
                &plm_px[(ltop+1)*(ltop+1)*i],
                apcnv);

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
        
        Ix.x+=t.x;
        Ix.y+=t.y;
      }

      for (m=0;m<=mhgh;m++) {
   
         cmult(&t,&mod->aoeff_p[l*(ltop+1)+m],
              &ylm_px[i*(ltop+1)*(ltop+1)+l*(ltop+1)+m]);
         Ix.x+=t.x;
         Ix.y+=t.y;      
       }
    }
       pot_arr[i]=Ix.x;
       xot_arr[i].x=Ix.x;
       xot_arr[i].y=Ix.y;

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
        Ix.x+=m*t.x;
        Ix.y+=m*t.y;   

      }

      for (m=0;m<=mhgh;m++) {

         cmult(&t,&mod->aoeff_p[l*(ltop+1)+m],
               &ylm_px[i*(ltop+1)*(ltop+1)+l*(ltop+1)+m]);
         Ix.x+=m*t.x;
         Ix.y+=m*t.y; 
       }
    }
    ele_phi[i]=(1000.0/(Rd*sin(the_col[i])))*Ix.y;
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
        Ix.x+=t.x; 
        Ix.y+=t.y;
 
      
        

    
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

        Ix.x+=t.x; 
        Ix.y+=t.y;
 

      }
    }
    if (latmin>0)
     ele_the[i]=-1000.0*(180.0/(90.0-latmin))/Rd*Ix.x;
    else 
     ele_the[i]=-1000.0*(180.0/(90.0+latmin))/Rd*Ix.x;

  }
	
 
}

double calc_bmag(float mlat, float mlon, float date) {
  double bmag;
  double glat, glon, r;
  double x,y,z;

  
  AACGMConvert((double)mlat,(double)mlon,1.,&glat,&glon,&r,1);
  IGRFCall(date,glat,glon,Altitude/1000.,&x,&y,&z);
  bmag = 1e-9*sqrt(x*x + y*y + z*z);
  /* fprintf(stderr,"mlat= %f, bmag=%g\n",mlat, bmag); */

  return bmag;
}

int solve_model(int num,struct GridGVec *ptr,float latmin,struct model *mod,
                int hemisphere, float decyear, int igrf_flag) {
  int i;
  double *ele_phi=NULL,*ele_the=NULL;
  double vmag,vaz;
  float *phi=NULL,*the=NULL,*the_col=NULL; 
  double bpolar,bmag=-0.5e-4;

  if (hemisphere == 1) bpolar = BNorth; 
  else bpolar = BSouth;

  if (mod==NULL) return -1; 

  phi=malloc(sizeof(float)*num);
  the=malloc(sizeof(float)*num);
  the_col=malloc(sizeof(float)*num);
  ele_the=malloc(sizeof(double)*num);
  ele_phi=malloc(sizeof(double)*num);

  if ( (phi==NULL) || (the==NULL) || (the_col==NULL) || 
       (ele_the==NULL) ||(ele_phi==NULL)) {
    if (phi !=NULL) free(phi);
    if (the !=NULL) free(the);
    if (the_col !=NULL) free(the_col);
    if (ele_the !=NULL) free(ele_the);
    if (ele_phi !=NULL) free(ele_phi);
    return -1;
  }

  
  for (i=0;i<num;i++) {
    phi[i]=ptr[i].mlon*2*PI/360.0;
    the[i]=(90-ptr[i].mlat)*(1.0/(90-latmin))*PI;
    the_col[i]=(90-ptr[i].mlat)*PI/180.0;
    ele_phi[i]=0;
    ele_the[i]=0;   
  }

  slv_sph_kset(latmin,num,phi,the,the_col,ele_phi,ele_the,mod);

  for (i=0;i<num;i++) {
    ele_phi[i]=ele_phi[i]*hemisphere;
    ele_the[i]=ele_the[i]*hemisphere;

    /* bmag = .05; */
    if (igrf_flag == 0) {
      bmag = -1000*bpolar*(1.0 - 3.0 * Altitude/Re)*
	      sqrt(3.0*(cos(the_col[i])*cos(the_col[i]))+ 1.0)/2.0;
    } else bmag = 1000*calc_bmag(hemisphere*ptr[i].mlat,ptr[i].mlon,decyear);

    vmag= sqrt( (ele_the[i]/bmag)*(ele_the[i]/bmag)+
                (ele_phi[i]/bmag)*(ele_phi[i]/bmag));
    vaz=atan2(ele_the[i]/bmag,ele_phi[i]/bmag)*360/(2.0*PI);
    ptr[i].azm=vaz;
    ptr[i].vel.median=vmag;
  }     

  free(the);
  free(the_col);
  free(phi);
  free(ele_phi);
  free(ele_the);
  return 0;
}


void add_model(struct CnvMapData *map,int num,struct GridGVec *ptr) {

  int off,i;
  off=map->num_model;

  map->num_model+=num;

  if (map->model==NULL) map->model=malloc(sizeof(struct GridGVec)*
					  map->num_model);
  else map->model=realloc(map->model,sizeof(struct GridGVec)*map->num_model);


  /* move the existing vectors down - the model always comes first
     in the model vector list */

 
  memmove(&map->model[num],map->model,
          off*sizeof(struct GridGVec));

  


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

/* Copied from map_addhmb so that HMB can be added here in the case that
   a low lat limit wasn't identified in the data by map_addhmb  */
void add_hmb_grd(float latmin,int yr,int yrsec,struct CnvMapData *map) {
  int i;
  float lat;
  float lstp;
  int nlon;
  float mlt,latx,lon;
  float bfac,del_L;

  char *lflg=NULL;
  int c=0;
  int off;

  /* We should do something about the hemisphere here */

  for (lat=latmin+0.5;lat<90;lat++) {  
    nlon=(int) (360*cos(lat*PI/180)+0.5);
    lstp=360.0/nlon;

    if (lflg==NULL) lflg=malloc(nlon);
    else lflg=realloc(lflg,nlon);
    memset(lflg,0,nlon);

    /* walk round in longitude locating cells that should be filled */

    c=0;
    lon=0.5*lstp;
    for (i=0;i<nlon;i++) {
            

      mlt=MLTConvertYrsec(yr,yrsec,lon);
    
      bfac=(90-latmin)/(90-latref);
      del_L=bfac*5.5;
      latx=latmin;
      if ((mlt>=11) && (mlt<=19)) 
        latx=latmin+del_L*(1+cos((PI/8)*(mlt-11)));      
      else if ((mlt<11) && (mlt>=5)) 
        latx=latmin+del_L*(1+cos((PI/6)*(11-mlt)));
   
      if (lat<=latx) {
        lflg[i]=1;
        c++;
      }      
      lon+=lstp;
    }
    if (c==0) break; /* no vectors at this latitude */

    off=map->num_model;
    map->num_model+=c;
    if (map->model==NULL) 
      map->model=malloc(sizeof(struct GridGVec)*map->num_model);
    else map->model=realloc(map->model,sizeof(struct GridGVec)*map->num_model);
     
    lon=0.5*lstp;
    
    for (i=0;i<nlon;i++) {
      if (lflg[i] !=0) {     
        if (map->hemisphere==1) map->model[off].mlat=lat;
        else map->model[off].mlat=-lat; 
        map->model[off].mlon=lon;  
        map->model[off].azm=45.0;
        map->model[off].vel.median=1.0;
        map->model[off].vel.sd=0.0;
        off++;
      }
      lon+=lstp;
    }
  }



}

/* Copied from map_addhmb so that HMB can be added here in the case that
   a low lat limit wasn't identified in the data by map_addhmb  */
void map_addhmb(int yr,int yrsec,struct CnvMapData *map,
		int bndnp,float bndstep,
                float latref,float latmin) {
  int i;
  float bfac,del_L;
  float mlt;

  map->num_bnd=bndnp;

  if (map->bnd_lat==NULL) map->bnd_lat=malloc(sizeof(double)*map->num_bnd);
  else map->bnd_lat=realloc(map->bnd_lat,sizeof(double)*map->num_bnd);

  if (map->bnd_lon==NULL) map->bnd_lon=malloc(sizeof(double)*map->num_bnd);
  else map->bnd_lon=realloc(map->bnd_lon,sizeof(double)*map->num_bnd);

  for (i=0;i<map->num_bnd;i++) {
    map->bnd_lon[i]=i*bndstep;
    mlt=MLTConvertYrsec(yr,yrsec,map->bnd_lon[i]);
     
    bfac=(90-latmin)/(90-latref);
    del_L=bfac*5.5;
  
    if (map->hemisphere==1) map->bnd_lat[i]=latmin;
    else  map->bnd_lat[i]=-latmin;
    if (map->hemisphere==1) {
      if ((mlt>=11) && (mlt<=19)) 
        map->bnd_lat[i]=latmin+del_L*(1+cos((PI/8)*(mlt-11)));      
      else if ((mlt<11) && (mlt>=5)) 
        map->bnd_lat[i]=latmin+del_L*(1+cos((PI/6)*(11-mlt)));
    } else {
      if ((mlt>=11) && (mlt<=19)) 
        map->bnd_lat[i]=-latmin-del_L*(1+cos((PI/8)*(mlt-11)));      
      else if ((mlt<11) && (mlt>=5)) 
        map->bnd_lat[i]=-latmin-del_L*(1+cos((PI/6)*(11-mlt)));
    }

  }

  add_hmb_grd(latmin,yr,yrsec,map); 
  if (map->hemisphere==1) map->latmin=latmin;  
  else map->latmin=-latmin; 
}


int main(int argc,char *argv[]) {


 /* File format transistion
   * ------------------------
   * 
   * When we switch to the new file format remove any reference
   * to "new". Change the command line option "new" to "old" and
   * remove "old=!new".
   */

  int old=0;
  int new=0;
  
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
  int i;

  int cnt=0;

  int yr,mo,dy,hr,mt;
  double sc;

  int order=0;
  int doping=-1;

  char *dstr="lmh";
 
  float oldlatmin=-1;

  float tilt=0;
  int cs10=0;
  int nointerp=0;
  struct model *imod=NULL;
  struct model *oldimod=NULL;
  float decyear = 0;
  int igrf_flag = 0;

  /* HMB parameters */
  float bndstep=5;
  int bndnp;

  struct GridGVec *mdata=NULL;


  map=CnvMapMake();
  grd=GridMake();
 
  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"-option",'x',&option);

  OptionAdd(&opt,"new",'x',&new);
  OptionAdd(&opt,"CS10",'x',&cs10);
  OptionAdd(&opt,"nointerp",'x',&nointerp);
  OptionAdd(&opt,"igrf",'x',&igrf_flag);
  OptionAdd(&opt,"vb",'x',&vb);
  OptionAdd(&opt,"o",'i',&order);
  OptionAdd(&opt,"d",'t',&dpstr);

  arg=OptionProcess(1,argc,argv,&opt,NULL);

  old=!new;

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

  if (cs10==0) {                       /* Using RG96 model patterns */
    /* setup clock angle bins */  
    for (i=0;i<8;i++) {
      RG96_mod_angil[i]=i*45-22.5;
      RG96_mod_angih[i]=i*45+22.5;
    }
  }

  envstr=getenv("SD_MODEL_TABLE");
  if (envstr==NULL) {
    fprintf(stderr,"Environment variable SD_MODEL_TABLE must be defined.\n");
    exit(-1);
  }
  load_all_models(envstr,cs10);

  /* fprintf(stderr,"Done loading models\n"); */

  if (old) {
    while (OldCnvMapFread(fp,map,grd)!=-1) {  
   
      map->imf_flag = igrf_flag;

      tme=(grd->st_time+grd->ed_time)/2.0;
      TimeEpochToYMDHMS(tme,&yr,&mo,&dy,&hr,&mt,&sc);
      yrsec=TimeYMDHMSToYrsec(yr,mo,dy,hr,mt,(int) sc);
      decyear = yr + (float)yrsec/TimeYMDHMSToYrsec(yr,12,31,23,59,59);
    
      /* determine the model */
      if (cs10==1) status = GeoPackRecalc(yr,mo,dy,hr,mt,sc,&tilt);
      map->tilt = tilt;
      imod=determine_model(map->Bx,map->By,map->Bz,map->hemisphere,tilt,cs10,nointerp);

      /* No HMB found from data */
      if (map->latmin == -1) { 
	bndnp=360/bndstep+1;
	map_addhmb(yr,yrsec,map,bndnp,bndstep,latref,imod->latref);
      }

      if (order !=0) map->fit_order=order;
      if (doping !=-1) map->doping_level=doping;

      /* get the position of the model vectors */

      if ((imod !=oldimod) || (map->latmin !=oldlatmin)) {
       if (mdata !=NULL)  free(mdata);

        mdata=get_model_pos(map->fit_order,fabs(map->latmin),map->hemisphere,
                          map->doping_level,&num);    
    

        /* solve for the model */
     
    
        status=solve_model(num,mdata,fabs(map->latmin),imod,
			   map->hemisphere,decyear,igrf_flag);
	
	oldimod = imod;
        oldlatmin=map->latmin;
      }

      /* now transform the model vectors and add them to the map file */
        
      
      add_model(map,num,mdata);
      
      if (cs10==1) strcpy(map->imf_model[3],"CS10");
      else         strcpy(map->imf_model[3],"RG96");
      strcpy(map->imf_model[0],imod->angle);
      strcpy(map->imf_model[1],imod->level);
      strcpy(map->imf_model[2],imod->tilt);


      OldCnvMapFwrite(stdout,map,grd);
      TimeEpochToYMDHMS(grd->st_time,&yr,&mo,&dy,&hr,&mt,&sc);
      if (vb==1) 
        fprintf(stderr,"%d-%d-%d %d:%d:%d %s %s %s %s\n",yr,mo,dy,
	        hr,mt,(int) sc,map->imf_model[0],imod->level,
                imod->angle,imod->tilt);  

      cnt++;
    }
  } else {
    while (CnvMapFread(fp,map,grd)!=-1) {     
      map->igrf_flag = igrf_flag;

      tme=(grd->st_time+grd->ed_time)/2.0;
      TimeEpochToYMDHMS(tme,&yr,&mo,&dy,&hr,&mt,&sc);
      yrsec=TimeYMDHMSToYrsec(yr,mo,dy,hr,mt,(int) sc);
      decyear = yr + (float)yrsec/TimeYMDHMSToYrsec(yr,12,31,23,59,59);
      
      /* determine the model */
      if (cs10==1) status = GeoPackRecalc(yr,mo,dy,hr,mt,sc,&tilt);
      map->tilt = tilt;
      imod=determine_model(map->Vx,map->By,map->Bz,map->hemisphere,tilt,cs10,nointerp);

      /* No HMB found from data */
      if (map->latmin == -1) { 
	bndnp=360/bndstep+1;
	map_addhmb(yr,yrsec,map,bndnp,bndstep,latref,imod->latref);
      }

      /* get the position of the model vectors */

      if (order !=0) map->fit_order=order;
      if (doping !=-1) map->doping_level=doping;

      if ((imod !=oldimod) || (map->latmin !=oldlatmin)) {

	if (mdata !=NULL)  free(mdata);
       
        mdata=get_model_pos(map->fit_order,fabs(map->latmin),map->hemisphere,
                          map->doping_level,&num);    
    

        /* solve for the model */
	
    
        status=solve_model(num,mdata,fabs(map->latmin),imod,
			   map->hemisphere,decyear,igrf_flag);

	
        oldimod=imod;
        oldlatmin=map->latmin;
      }

      /* now transform the model vectors and add them to the map file */
        
      add_model(map,num,mdata);
          
      if (cs10==1) strcpy(map->imf_model[3],"CS10");
      else         strcpy(map->imf_model[3],"RG96");
      strcpy(map->imf_model[0],imod->angle);
      strcpy(map->imf_model[1],imod->level);
      strcpy(map->imf_model[2],imod->tilt);

      status = CnvMapFwrite(stdout,map,grd);
      TimeEpochToYMDHMS(grd->st_time,&yr,&mo,&dy,&hr,&mt,&sc);
      if (vb==1) 
        fprintf(stderr,"%d-%d-%d %d:%d:%d %s %s %s %s\n",yr,mo,dy,
	        hr,mt,(int) sc,map->imf_model[0],map->imf_model[1],
		map->imf_model[2],map->imf_model[3]);
      cnt++;
    }
  }

  return 0;
}






















