/* map_addmodel.c
   =============== 
   Author: R.J.Barnes 
*/


/*
 LICENSE AND DISCLAIMER
 
 Copyright (c) 2012 The Johns Hopkins University/Applied Physics Laboratory
 
 This file is part of the Radar Software Toolkit (RST).
 
 RST is free software: you can redistribute it and/or modify
 it under the terms of the GNU Lesser General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 any later version.
 
 RST is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU Lesser General Public License for more details.
 
 You should have received a copy of the GNU Lesser General Public License
 along with RST.  If not, see <http://www.gnu.org/licenses/>.
 
 
 
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

#include "hlpstr.h"





char *mod_lev[]={"2t3","0t4","4t6","6t12","7t20",0};
char *mod_ang[]={"315t45", "0t90", "45t135", "90t180", 
		 "135t225", "180t270", "225t315", "270t360",0};

char *mod_levn[]={"2<Kp<3", "0<BT<4", "4<BT<6", "6<BT<12", "7<BT<20",0};
char *mod_angn[]={"Bz+", "Bz+/By+", "By+", 
                  "Bz-/By+", "Bz-", "Bz-/By-", "By-", "Bz+/By-",0};

int mod_levi[8]={0,4,6,12,20,-1};
float mod_angil[8];
float mod_angih[8];

struct CnvMapData *map;
struct GridData *grd;

struct OptionData opt;


struct model {
  char angle[64];
  char level[64];
  int ilev,iang;
  char str[3][128];
  float latref;
  int ltop,mtop;
  struct complex *aoeff_n;
  struct complex *aoeff_p;
};

int mnum=0;
struct model *model[60];

struct model *load_model(FILE *fp,char *level,char *angle,int ilev,int iang) {
  struct model *ptr=NULL;
  int i,l,m,lx,mx;
  float cr,ci;

  ptr=malloc(sizeof(struct model));
  if (ptr==NULL) return NULL;
  strcpy(ptr->level,level);
  strcpy(ptr->angle,angle);
  ptr->ilev=ilev;
  ptr->iang=iang;
  
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


int load_all_models(char *path) {
  char fname[256];
  FILE *fp;
  int i,j;

  for (i=0;mod_lev[i] !=NULL;i++) {
    for (j=0;mod_ang[j] !=NULL;j++) {
      sprintf(fname,"%s/mod_%s_%s.spx",path,mod_lev[i],mod_ang[j]);
      fp=fopen(fname,"r");
      if (fp==NULL) continue;
      model[mnum]=load_model(fp,mod_levn[i],mod_angn[j],i,j);     
      fclose(fp);  
      if (model[mnum]==NULL) continue;
      mnum++;
    }
  }
  return 0;
}  
  
int determine_model(float Bx,float By,float Bz) {
  int ilev,iang,i;
  float bt,bazm;

  bt=sqrt(By*By+Bz*Bz);
  
  bazm=atan2(By,Bz)*360/(2*PI);
  
  if (bazm>=(360-22.5)) bazm=bazm-360;
  if (bazm<-22.5) bazm=bazm+360;
  
  /* okay now work out the limits */

  for (ilev=0;(mod_levi[ilev] !=-1) && (bt>mod_levi[ilev]);ilev++);  
  if (mod_levi[ilev]==-1) ilev--;

  for (iang=0;iang<8;iang++) {
     if ((bazm>mod_angil[iang]) && (bazm<mod_angih[iang])) break;
  }
  if (iang==8) iang--;

  /* correct for extreme Bz+ */

  if ((ilev==4) && (iang !=0)) ilev--;
  
  for (i=0;i<mnum;i++) if ((model[i] !=NULL) && 
         (model[i]->ilev==ilev) && 
         (model[i]->iang==iang)) break;
  if (i<mnum) return i;
  return 0; 
  
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
  float Re=6362.0+300.0;

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
    ele_phi[i]=(1000.0/(Re*sin(the_col[i])))*Ix.y;
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
     ele_the[i]=-1000.0*(180.0/(90.0-latmin))/Re*Ix.x;
    else 
     ele_the[i]=-1000.0*(180.0/(90.0+latmin))/Re*Ix.x;

  }
	
 
}


int solve_model(int num,struct GridGVec *ptr,float latmin,struct model *mod,
                int hemisphere) {
  int i;
  double *ele_phi=NULL,*ele_the=NULL;
  double vmag,vaz;
  float *phi=NULL,*the=NULL,*the_col=NULL; 

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

    vmag= sqrt( (ele_the[i]/0.05)*(ele_the[i]/0.05)+
                (ele_phi[i]/0.05)*(ele_phi[i]/0.05));
    vaz=atan2(ele_the[i]/0.05,ele_phi[i]/0.05)*360/(2.0*PI);
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
  int modnum;

  int yr,mo,dy,hr,mt;
  double sc;

  int order=0;
  int doping=-1;

  char *dstr="lmh";
 
  int oldmodnum=-1;
  float oldlatmin=-1;

  struct GridGVec *mdata=NULL;


  map=CnvMapMake();
  grd=GridMake();

  /* setup clock angles */  

  for (i=0;i<8;i++) {
    mod_angil[i]=i*45-22.5;
    mod_angih[i]=i*45+22.5;
  }
 
  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"-option",'x',&option);

  OptionAdd(&opt,"new",'x',&new);
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

  envstr=getenv("SD_MODEL_TABLE");
  if (envstr==NULL) {
    fprintf(stderr,"Environment variable SD_MODEL_TABLE must be defined.\n");
    exit(-1);
  }

  load_all_models(envstr);
  
  if (old) {
    while (OldCnvMapFread(fp,map,grd)!=-1) {  
   
      tme=(grd->st_time+grd->ed_time)/2.0;
      TimeEpochToYMDHMS(tme,&yr,&mo,&dy,&hr,&mt,&sc);
      yrsec=TimeYMDHMSToYrsec(yr,mo,dy,hr,mt,(int) sc);
    
    
      /* determine the model */
      if (map->hemisphere==-1) modnum=determine_model(map->Bx,-map->By,map->Bz);
      else modnum=determine_model(map->Bx,map->By,map->Bz);
 
      /* get the position of the model vectors */



      if (order !=0) map->fit_order=order;
      if (doping !=-1) map->doping_level=doping;

      if ((modnum !=oldmodnum) || (map->latmin !=oldlatmin)) {

       if (mdata !=NULL)  free(mdata);

        mdata=get_model_pos(map->fit_order,fabs(map->latmin),map->hemisphere,
                          map->doping_level,&num);    
    

        /* solve for the model */
     
    
        status=solve_model(num,mdata,fabs(map->latmin),model[modnum],
                         map->hemisphere);
        oldmodnum=modnum;
        oldlatmin=map->latmin;
      }

      /* now transform the model vectors and add them to the map file */
        
    
      add_model(map,num,mdata);
    
      strcpy(map->imf_model[1],model[modnum]->level);
      strcpy(map->imf_model[0],model[modnum]->angle);


      OldCnvMapFwrite(stdout,map,grd);
      TimeEpochToYMDHMS(grd->st_time,&yr,&mo,&dy,&hr,&mt,&sc);
      if (vb==1) 
        fprintf(stderr,"%d-%d-%d %d:%d:%d %s %s\n",yr,mo,dy,
	        hr,mt,(int) sc,model[modnum]->level,
                model[modnum]->angle);  

      cnt++;
    }
  } else {
    while (CnvMapFread(fp,map,grd)!=-1) {  
   
      tme=(grd->st_time+grd->ed_time)/2.0;
      TimeEpochToYMDHMS(tme,&yr,&mo,&dy,&hr,&mt,&sc);
      yrsec=TimeYMDHMSToYrsec(yr,mo,dy,hr,mt,(int) sc);
    
      /* determine the model */
      if (map->hemisphere==-1) modnum=determine_model(map->Bx,-map->By,map->Bz);
      else modnum=determine_model(map->Bx,map->By,map->Bz);
 
      /* get the position of the model vectors */

      if (order !=0) map->fit_order=order;
      if (doping !=-1) map->doping_level=doping;

      if ((modnum !=oldmodnum) || (map->latmin !=oldlatmin)) {

       if (mdata !=NULL)  free(mdata);

        mdata=get_model_pos(map->fit_order,fabs(map->latmin),map->hemisphere,
                          map->doping_level,&num);    
    

        /* solve for the model */
     
    
        status=solve_model(num,mdata,fabs(map->latmin),model[modnum],
                         map->hemisphere);
        oldmodnum=modnum;
        oldlatmin=map->latmin;
      }

      /* now transform the model vectors and add them to the map file */
        
    
      add_model(map,num,mdata);
    
      strcpy(map->imf_model[1],model[modnum]->level);
      strcpy(map->imf_model[0],model[modnum]->angle);


      CnvMapFwrite(stdout,map,grd);
      TimeEpochToYMDHMS(grd->st_time,&yr,&mo,&dy,&hr,&mt,&sc);
      if (vb==1) 
        fprintf(stderr,"%d-%d-%d %d:%d:%d %s %s\n",yr,mo,dy,
	        hr,mt,(int) sc,model[modnum]->level,
                model[modnum]->angle);  

      cnt++;
    }
  }

  return 0;
}






















