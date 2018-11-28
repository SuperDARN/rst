/* meteor_proc.c
   =============
   Author: Kevin O'Rourke, adapted by R.J.Barnes
*/




/* 
 $Log: meteor_proc.c,v $
 Revision 1.11  2007/09/19 19:36:03  code
 Fixed bug in processing cfit files.

 Revision 1.10  2006/03/10 18:31:50  barnes
 Included Nigel Wade's modification to correct for infinite looping condition.

 Revision 1.9  2006/03/03 15:39:39  barnes
 Modification to support the older method of operation
 where the meridional beam is specified on the command line
 and only one hour is processed.

 Revision 1.8  2006/03/03 15:28:12  barnes
 Fixed uninitialized variable.

 Revision 1.7  2005/07/15 18:25:42  barnes
 Fixed minor bugs.

 Revision 1.6  2003/04/03 19:10:03  barnes
 Fixed bug in passing frang rather than rsep.

 Revision 1.5  2003/04/03 18:43:40  barnes
 Changed geographic to use frang and rsep.

 Revision 1.4  2003/02/25 23:34:33  barnes
 Modification to use the base libraries.

 Revision 1.3  2002/01/21 18:29:55  barnes
 Modification to make code backwards compatible with ROS 1.01.

 Revision 1.2  2002/01/21 16:14:54  barnes
 Removed debugging code

 Revision 1.1  2002/01/21 16:09:45  barnes
 Initial revision

 */


#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <math.h>
#include <zlib.h>
#include "rtypes.h"
#include "option.h"
#include "rtime.h"
#include "rmath.h"
#include "dmap.h"


#include "radar.h"
#include "rprm.h"
#include "fitdata.h"
#include "fitread.h"
#include "oldfitread.h"
#include "cfitdata.h"
#include "cfitread.h"
#include "rpos.h"

#include "errstr.h"
#include "hlpstr.h"

#include "meteor.h"
#include "math.h"

#include "nrutil.h"

struct RadarNetwork *network;
struct Radar *radar;
struct RadarSite *site;

struct CFitdata cfit;
struct RadarParm prm;
struct FitData fit;

struct OptionData opt;

int cnt=0;

int merbm[32]={6,11,1,9,0,15,0,11,13,3,5,2,0,4,4,0};
int merid[32]={1,3,5,6,7,8,9,10,16,4,11,12,13,14,15,32};

struct metdata {
  int yr,mo,dy,hr,mt,sc;
  int bmnum;
  int frang,rsep,rxrise;
  int max_gate;
  unsigned char flg[75];
  double vlos[75];
};

int num[24];
struct metdata *met[24];

int max_beam=0;
double bm_total[24];
double bm_sdtmp[24];
int bm_count[24];
int num_avgs=0;

int beams=0;
double vlos[24];
double sdev[24];

double vx,vy;
double vm;
double sdvx,sdvy;
double lat,lon,rho,vmlat,vmlon;

double *a;
double **u;
double **v;
double *w;
double *x;
double *y;
double *sig;

double coseps;
double chisq;

double **cvm;

double calc_coseps(double range) {
  double eps;
  eps = asin(METEOR_HEIGHT/range);
  return cos(eps);
};


double calc_azi(int bmnum) {
  double azi;
  azi = site->bmsep*(bmnum-7.5)+site->boresite;
  return (azi*PI/180.0);
};

void cosfunc(double x, double afunc[], int ma) {
  afunc[1] = -cos(x);
  afunc[2] = sin(x);
};

int rst_opterr(char *txt) {
  fprintf(stderr,"Option not recognized: %s\n",txt);
  fprintf(stderr,"Please try: meteor_proc --help\n");
  return(-1);
}


int main (int argc,char *argv[]) {

  int old;

  int arg;
  unsigned char vb=0;
  unsigned char help=0;
  unsigned char option=0;
  unsigned char version=0;

  int i,j;
  FILE *fitfp=NULL;
  struct OldFitFp *ffp=NULL;
  struct CFitfp *cfp=NULL;
  FILE *fp;
  int c;
  char *mz_str=NULL; 
  double max_vel = MAX_VEL;
  double min_sn = MIN_SN;
  double max_v_err = MAX_V_ERR;
  double max_w_l = MAX_W_L;
  int max_range = MAX_MET_RANGE;
  int vm_beam = -1;
  int min_beams = MIN_BEAMS;
  int bm_type = BM_TYPE;
  int max_gate=0;
  int st_id=-1;
  int req_hr=-1;
  int yr,mo,dy,hr,mt;
  double sc;
  int year,month,day;

  int frang,rsep,rxrise=0;

  unsigned char cfitflg=0;

  int chisham=0;

  int bc=0;

  char *envstr;

  envstr=getenv("SD_RADAR");
  if (envstr==NULL) {
    fprintf(stderr,"Environment variable 'SD_RADAR' must be defined.\n");
    exit(-1);
  }

  fp=fopen(envstr,"r");

  if (fp==NULL) {
    fprintf(stderr,"Could not locate radar information file.\n");
    exit(-1);
  }

  network=RadarLoad(fp);
  fclose(fp); 
  if (network==NULL) {
    fprintf(stderr,"Failed to read radar information.\n");
    exit(-1);
  }

  envstr=getenv("SD_HDWPATH");
  if (envstr==NULL) {
    fprintf(stderr,"Environment variable 'SD_HDWPATH' must be defined.\n");
    exit(-1);
  }

  RadarLoadHardware(envstr,network);

  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"-option",'x',&option);
  OptionAdd(&opt,"-version",'x',&version);
  OptionAdd(&opt,"vb",'x',&vb);

  OptionAdd(&opt,"old",'x',&old);

  OptionAdd(&opt,"mv",'d',&max_vel);
  OptionAdd(&opt,"ms",'d',&min_sn);
  OptionAdd(&opt,"me",'d',&max_v_err);
  OptionAdd(&opt,"mw",'d',&max_w_l);

  OptionAdd(&opt,"bm",'i',&vm_beam);

  OptionAdd(&opt,"mr",'i',&max_range);

  OptionAdd(&opt,"mb",'i',&min_beams);

  OptionAdd(&opt,"mz",'t',&mz_str);

  OptionAdd(&opt,"hr",'i',&req_hr);

  OptionAdd(&opt,"cfit",'x',&cfitflg);

  OptionAdd(&opt,"chisham",'x',&chisham);

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

  if (mz_str !=NULL) {
     if (tolower(mz_str[0])=='m') bm_type=0;
     else bm_type=1;
  }

  if (arg==argc) {
    OptionPrintInfo(stdout,errstr);
    exit(-1);
  }

  for (c=arg;c<argc;c++) {

    if (cfitflg==0) {
      if (old) {
        ffp=OldFitOpen(argv[c],NULL); 
        fprintf(stderr,"Opening file %s\n",argv[c]);
        if (ffp==NULL) {
          fprintf(stderr,"file %s not found\n",argv[c]);
          continue;
        }

        while (OldFitRead(ffp,&prm,&fit) !=-1) {

          if (site==NULL) {
            radar=RadarGetRadar(network,prm.stid);
            site=RadarYMDHMSGetSite(radar,prm.time.yr,prm.time.mo,
                                    prm.time.dy,prm.time.hr,
                                    prm.time.mt,(int) prm.time.sc);

            st_id=prm.stid;
            frang=prm.frang;
            rsep=prm.rsep;
            rxrise=prm.rxrise;
            if (rxrise==0) rxrise=site->recrise;
            rxrise=site->recrise;
            if (vm_beam==-1) {
              for (i=0;(merid[i] !=st_id) && (merid[i] !=0);i++);
              vm_beam=merbm[i];
            }
          }
          /* select the data */

          hr=prm.time.hr;
          cnt=num[hr];

          if ((req_hr !=-1) && (hr !=req_hr)) continue;
          if (prm.scan <0) continue;
          if (prm.frang==0) continue;
          if (prm.rsep==0) continue;
          if (met[hr]==NULL) met[hr]=malloc(sizeof(struct metdata));
          else met[hr]=realloc(met[hr],sizeof(struct metdata)*(cnt+1));
          if (prm.bmnum > max_beam) max_beam=prm.bmnum;

          met[hr][cnt].yr=prm.time.yr;
          met[hr][cnt].mo=prm.time.mo;
          met[hr][cnt].dy=prm.time.dy;
          met[hr][cnt].hr=prm.time.hr;
          met[hr][cnt].mt=prm.time.mt;
          met[hr][cnt].sc=prm.time.sc;
          met[hr][cnt].bmnum=prm.bmnum;
          met[hr][cnt].frang=prm.frang;
          met[hr][cnt].rsep=prm.rsep;
          met[hr][cnt].rxrise=rxrise;

          max_gate=(max_range-prm.frang)/prm.rsep;
          met[hr][cnt].max_gate=max_gate;
          for (i=0;i<max_gate;i++) {
            met[hr][cnt].flg[i]=0;
            if (fit.rng[i].qflg==0) continue;
            if (fabs(fit.rng[i].v) > max_vel) continue;
            if (fit.rng[i].p_l < min_sn) continue;
            if (fit.rng[i].v_err >= max_v_err) continue;
            if (fit.rng[i].w_l > max_w_l) continue;
            met[hr][cnt].flg[i]=1;
            met[hr][cnt].vlos[i]=fit.rng[i].v;
          }
          num[hr]++;
        }
        OldFitClose(ffp);
      } else {
        fitfp=fopen(argv[c],"r");
        fprintf(stderr,"Opening file %s\n",argv[c]);
        if (fitfp==NULL) {
          fprintf(stderr,"file %s not found\n",argv[c]);
          continue;
        }

        while (FitFread(fitfp,&prm,&fit) !=-1) {

          if (site==NULL) {
            radar=RadarGetRadar(network,prm.stid);
            site=RadarYMDHMSGetSite(radar,prm.time.yr,prm.time.mo,
                                    prm.time.dy,prm.time.hr,
                                    prm.time.mt,(int) prm.time.sc);

            st_id=prm.stid;
            frang=prm.frang;
            rsep=prm.rsep;
            rxrise=prm.rxrise;
            if (rxrise==0) rxrise=site->recrise;
            rxrise=site->recrise;
            if (vm_beam==-1) {
              for (i=0;(merid[i] !=st_id) && (merid[i] !=0);i++);
              vm_beam=merbm[i];
            }
          }
          /* select the data */

          hr=prm.time.hr;
          cnt=num[hr];

          if ((req_hr !=-1) && (hr !=req_hr)) continue;
          if (prm.scan <0) continue;
          if (prm.frang==0) continue;
          if (prm.rsep==0) continue;
          if (met[hr]==NULL) met[hr]=malloc(sizeof(struct metdata));
          else met[hr]=realloc(met[hr],sizeof(struct metdata)*(cnt+1));
          if (prm.bmnum > max_beam) max_beam=prm.bmnum;

          met[hr][cnt].yr=prm.time.yr;
          met[hr][cnt].mo=prm.time.mo;
          met[hr][cnt].dy=prm.time.dy;
          met[hr][cnt].hr=prm.time.hr;
          met[hr][cnt].mt=prm.time.mt;
          met[hr][cnt].sc=prm.time.sc;
          met[hr][cnt].bmnum=prm.bmnum;
          met[hr][cnt].frang=prm.frang;
          met[hr][cnt].rsep=prm.rsep;
          met[hr][cnt].rxrise=rxrise;

          max_gate=(max_range-prm.frang)/prm.rsep;
          met[hr][cnt].max_gate=max_gate;
          for (i=0;i<max_gate;i++) {
            met[hr][cnt].flg[i]=0;
            if (fit.rng[i].qflg==0) continue;
            if (fabs(fit.rng[i].v) > max_vel) continue;
            if (fit.rng[i].p_l < min_sn) continue;
            if (fit.rng[i].v_err >= max_v_err) continue;
            if (fit.rng[i].w_l > max_w_l) continue;
            met[hr][cnt].flg[i]=1;
            met[hr][cnt].vlos[i]=fit.rng[i].v;
          }
          num[hr]++;
        }
        fclose(fitfp);
      }
    } else {
      cfp=CFitOpen(argv[c]); 
      fprintf(stderr,"Opening file %s\n",argv[c]);
      if (cfp==NULL) {
        fprintf(stderr,"file %s not found\n",argv[c]);
        continue;
      }

      while (CFitRead(cfp,&cfit) !=-1) {
        TimeEpochToYMDHMS(cfit.time,&yr,&mo,&dy,&hr,&mt,&sc);
        if (site==NULL) {
          radar=RadarGetRadar(network,cfit.stid);
          site=RadarYMDHMSGetSite(radar,yr,mo,dy,hr,mt,
                                  (int) sc);

          st_id=cfit.stid;
          frang=cfit.frang;
          rsep=cfit.rsep;
          rxrise=cfit.rxrise;
          if (rxrise==0) rxrise=site->recrise;
          rxrise=site->recrise;
          if (vm_beam==-1) {
            for (i=0;(merid[i] !=st_id) && (merid[i] !=0);i++);
            vm_beam=merbm[i];
          }
        }
        /* select the data */

        cnt=num[hr];
        if ((req_hr !=-1) && (hr !=req_hr)) continue;
        if (cfit.scan <0) continue;
        if (cfit.frang==0) continue;
        if (cfit.rsep==0) continue;
        if (met[hr]==NULL) met[hr]=malloc(sizeof(struct metdata));
        else met[hr]=realloc(met[hr],sizeof(struct metdata)*(cnt+1));
        if (prm.bmnum > max_beam) max_beam=prm.bmnum;

        met[hr][cnt].yr=yr;
        met[hr][cnt].mo=mo;
        met[hr][cnt].dy=dy;
        met[hr][cnt].hr=hr;
        met[hr][cnt].mt=mt;
        met[hr][cnt].sc=sc;
        met[hr][cnt].bmnum=cfit.bmnum;
        met[hr][cnt].frang=cfit.frang;
        met[hr][cnt].rsep=cfit.rsep;
        met[hr][cnt].rxrise=rxrise;

        max_gate=(max_range-cfit.frang)/cfit.rsep;
        met[hr][cnt].max_gate=max_gate;
        for (i=0;i<max_gate;i++) met[hr][cnt].flg[i]=0;
        for (j=0;j<cfit.num;j++) {
          if (cfit.rng[j]>=max_gate) continue;
          i=cfit.rng[j];
          if (fabs(cfit.data[j].v) > max_vel) continue;
          if (cfit.data[j].p_l < min_sn) continue;
          if (cfit.data[j].v_e >= max_v_err) continue;
          if (cfit.data[j].w_l > max_w_l) continue;
          met[hr][cnt].flg[i]=1;
          met[hr][cnt].vlos[i]=cfit.data[j].v;
        }
        num[hr]++;
      }
      CFitClose(cfp);
    }
  }
  x = dvector(1,max_beam+1);
  y = dvector(1,max_beam+1);
  sig = dvector(1,max_beam+1);
  a = dvector(1,2);
  u = dmatrix(1,max_beam+1,1,2);
  v = dmatrix(1,2,1,2);
  w = dvector(1,2);

  coseps = calc_coseps(max_range/2.0);

  cvm=dmatrix(1,2,1,2);

  fprintf(stdout,"# Vlos(max)=%.2f\n# S/N(min)=%.2f\n# range(max)=%d\n",
                      max_vel, min_sn, max_range);
  fprintf(stdout, "# Verr(max)=%.2f\n# num_beams(min)=%d\n", max_v_err,
                   min_beams);
  fprintf(stdout, "# w_l(max)=%.2f\n", max_w_l);
  if (bm_type == 0) fprintf(stdout,
    "# beam_num=%d\n# wind=meridional\n",vm_beam);
  else fprintf(stdout,
    "# beam_num=%d\n# wind=zonal\n",vm_beam);
  fprintf(stdout, "# stid=%d\n", st_id);

  if (bm_type == 0) fprintf(stdout,
     "# year month day hour num_avgs frang rsep Vx Vy lat long Vm Vm_lat Vm_long sdev_Vx sdev_Vy\n");
  else fprintf(stdout,
     "# year month day hour num_avgs frang rsep Vx Vy lat long Vz Vz_lat Vz_long sdev_Vx sdev_Vy\n");

  /* now do the fitting */

  hr=0;
  if (req_hr !=-1) hr=req_hr;
  do {
    cnt=num[hr];
    if (cnt==0) {
      fprintf(stderr,"No data.\n");
      if (req_hr !=-1) break;
      hr++;
      continue;
    }

    year=met[hr][0].yr;
    month=met[hr][0].mo;
    day=met[hr][0].dy;

    for (i=0;i<max_beam+1;i++) {
      bm_total[i]=0;
      bm_count[i]=0;
      bm_sdtmp[i]=0;
    }
    num_avgs=0;
    beams=0;

    for (i=0;i<cnt;i++) {
      for (j=0;j<met[hr][i].max_gate;j++) {
        if (met[hr][i].flg[j]==0) continue;
        bm_total[met[hr][i].bmnum]+= met[hr][i].vlos[j];
        bm_count[met[hr][i].bmnum]++;
        num_avgs++;
      }
    }

    for (i=0;i<max_beam+1;i++) {
      if (bm_count[i] > 0) {
        beams++;
        vlos[i] = bm_total[i]/bm_count[i];
       } else vlos[i]=0;
    };

    for (i=0;i<cnt;i++) {
      for (j=0;j<met[hr][i].max_gate;j++) {
        if (met[hr][i].flg[j]==0) continue;
        bm_sdtmp[met[hr][i].bmnum]+=(met[hr][i].vlos[j]-
                                     vlos[met[hr][i].bmnum])*
                 (met[hr][i].vlos[j]-vlos[met[hr][i].bmnum]);

      }
    }
    for (i=0;i<max_beam+1;i++) {
      if (bm_count[i] > 1) {
        sdev[i]=sqrt(bm_sdtmp[i]/(bm_count[i]-1));
      } else {
        sdev[i]=1;
        vlos[i]=0;
      }
    }

    if (beams<min_beams) {
      fprintf(stderr,
              "Not enough beams of data to generate a velocity vector\n");
      if (req_hr !=-1) break;
      hr++;
      continue;
    }

    bc=0;
    for (i=0;i<max_beam+1;i++) {
      if (bm_count[i]>1) {
        x[++bc] = calc_azi(i);
        y[bc] = vlos[i]/coseps; /* mean velocity */
        sig[bc] = sdev[i];
      }
    }

    fprintf(stderr,"Fitting %d of %d beams\n",bc,max_beam+1);

    dsvdfit(x, y, sig, bc, a, 2, u, v, w, &chisq, &cosfunc);

    vx=a[1];
    vy=a[2];

    dsvdvar(v, 2, w, cvm);

    sdvx = sqrt(cvm[1][1]);
    sdvy = sqrt(cvm[2][2]);

    vm=vlos[vm_beam]/coseps;

    frang=met[hr][0].frang;
    rsep=met[hr][0].rsep;
    rxrise=met[hr][0].rxrise;
    RPosGeo(0,7,3,site,frang,rsep,rxrise,METEOR_HEIGHT,&rho,
             &lat,&lon,chisham);

    RPosGeo(0,vm_beam,3,site,frang,rsep,rxrise,METEOR_HEIGHT,&rho,
             &vmlat,&vmlon,chisham);

    fprintf(stdout, "%4d %02d %02d %02d %d %d %d %.0f %.0f %.1f %.1f %.0f %.1f %.1f %.2f %.2f\n",
            year,month,day,hr,num_avgs,frang,rsep,vx,vy,lat,lon,vm,vmlat,vmlon,sdvx,sdvy);

    if (req_hr !=-1) break;
    hr++;
  } while (hr<24);
  return 0;
}

