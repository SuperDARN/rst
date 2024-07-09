/* map_addhmb.c
   =============
   Author: R.J.Barnes and others


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
    2022-06-08 Emma Bland (UNIS) Added -wdt command line option

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <sys/types.h>
#include "rtypes.h"
#include "option.h"
#include "rtime.h"
#include "rmath.h"

#include "rfile.h"
#include "griddata.h"

#include "cnvgrid.h"
#include "cnvmap.h"
#include "cnvmapread.h"
#include "oldcnvmapread.h"
#include "cnvmapwrite.h"
#include "oldcnvmapwrite.h"
/*#include "aacgm.h"*/
#include "mlt.h"
#include "aacgmlib_v2.h"
#include "mlt_v2.h"
#include "igrflib.h"
#include "hlpstr.h"
#include "map_addhmb.h"

struct CnvMapData *map[3];
struct GridData *grd[3];


int latcnt[36];

struct OptionData opt;

struct hmbtab {
  int num;
  double *time;
  float *median;
  float *actual;
};

int rst_opterr(char *txt) {
  fprintf(stderr,"Option not recognized: %s\n",txt);
  fprintf(stderr,"Please try: map_addhmb --help\n");
  return(-1);
}

int main(int argc,char *argv[])
{
  int old=0;
  int old_aacgm=0;
  int ecdip=0;

  int arg;
  unsigned char help=0;
  unsigned char option=0;
  unsigned char version=0;

  unsigned char vb=0;

  FILE *fp;
  char *lname=NULL;
  char *fname=NULL;
  
  int tme;
  int yrsec=0;
  int i,j;
  int s=0;

  int cnt=0;

  int yr,mo,dy,hr,mt,sec,dyn;
  double sc;

  int cnt_req=3;
  int vel_min=100;
  int wdt_min=0;
  int tflg=0;

  int mlti;
  int c;

  float latmin[3]={0,0,0};
  float lattmp[3];
  float latmed=0;
  float latdef=62;

  float latref=59;
  int nlat=36;

  float bndstep=5;
  int bndnp;

  int buf=0;
  int idx;

  char *istr;
  int exid[64];
  int exnum=0;

  float latdif,l;
  int latc;

  struct hmbtab *hmbtab=NULL;
  float hmblat=0;

  char *exstr=NULL;

  int nodef = 0;

  int magflg = 0;

  /* function pointers for file reading/writing (old and new) and MLT */
  int (*Map_Read)(FILE *, struct CnvMapData *, struct GridData *);
  int (*Map_Write)(FILE *, struct CnvMapData *, struct GridData *);
  double (*MLTCnv)(int, int, double);

  for (i=0;i<3;i++) {
    grd[i]=GridMake();
    map[i]=CnvMapMake();
  }

  bndnp=360/bndstep+1;
  
  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"-option",'x',&option);
  OptionAdd(&opt,"-version",'x',&version);

  OptionAdd(&opt,"old",'x',&old);
  OptionAdd(&opt,"vb",'x',&vb);

  OptionAdd(&opt,"cnt",'i',&cnt_req);
  OptionAdd(&opt,"vel",'i',&vel_min);
  OptionAdd(&opt,"wdt",'i',&wdt_min);
  OptionAdd(&opt,"t",'x',&tflg);
  OptionAdd(&opt,"lf",'t',&lname);
  OptionAdd(&opt,"lat",'f',&hmblat);
  OptionAdd(&opt,"ex",'t',&exstr);

  OptionAdd(&opt,"old_aacgm",'x',&old_aacgm);
  OptionAdd(&opt,"ecdip",'x',&ecdip);
  OptionAdd(&opt,"nodef",'x',&nodef); /* no default latmin: EC */

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

  if (exstr !=NULL) {
    istr=strtok(exstr,",");
    do {
      exid[exnum]=atoi(istr);
      exnum++;
    } while ((istr=strtok(NULL,",")) !=NULL);
  }
   
  if (lname !=NULL) {
    fp=fopen(lname,"r");
    if (fp !=NULL) {
      hmbtab=load_hmb(fp);
      fclose(fp);
    }
  }

  if (fname==NULL) fp=stdin;
  else fp=fopen(fname,"r");
  if (fp==NULL) {
    fprintf(stderr,"File not found.\n");
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

  /* set function pointer to compute MLT or MLT_v2 */
  if (old_aacgm) MLTCnv = &MLTConvertYrsec;
  else           MLTCnv = &MLTConvertYrsec_v2;

  if (ecdip) magflg = 2;
  else if (old_aacgm) magflg = 1;
  else magflg = 0;

  if ((hmbtab != NULL) || (hmblat != 0))  {
    /*
     * take the latitude limits from a file or from a predefined value
     */
    cnt = 0;
    s = (*Map_Read)(fp,map[0],grd[0]);
    while (s != -1) { 

      tme = (grd[buf]->st_time + grd[buf]->ed_time)/2.0;
      TimeEpochToYMDHMS(tme,&yr,&mo,&dy,&hr,&mt,&sc);
      yrsec = TimeYMDHMSToYrsec(yr,mo,dy,hr,mt,(int) sc);

      if (hmbtab != NULL) {
        while ((cnt < hmbtab->num) && (hmbtab->time[cnt] < tme)) cnt++;
        if (cnt == 0) {
          latmed = hmbtab->median[0];
          latmin[0] = hmbtab->actual[0];
        } else {
          latmed = hmbtab->median[cnt-1];
          latmin[0] = hmbtab->actual[cnt-1];
        } 
      } else {
        latmed = hmblat;
        latmin[0] = hmblat;
      }  

      if (tflg == 0) {
        map_addhmb(yr,yrsec,map[0],bndnp,bndstep,latref,latmed,magflg);
        (*Map_Write)(stdout,map[0],grd[0]);
        TimeEpochToYMDHMS(grd[0]->st_time,&yr,&mo,&dy,&hr,&mt,&sc);
        if (vb==1) 
           fprintf(stderr,"%d-%d-%d %d:%d:%d latmin=%g\n",yr,mo,dy,
                           hr,mt,(int) sc,map[0]->latmin);
      } else {  /* write the boundary */
        TimeEpochToYMDHMS(grd[0]->st_time,&yr,&mo,&dy,&hr,&mt,&sc);
        fprintf(stdout,"%.4d %.2d %.2d %.2d %.2d %.2d %.2g %.2g\n",
                        yr,mo,dy,hr,mt,(int) sc,latmed,latmin[0]);
        if (vb==1) 
          fprintf(stderr,"%d-%d-%d %d:%d:%d latmin: median=%g actual=%g\n",
                yr,mo,dy,hr,mt,(int) sc,latmed,latmin[0]);
      }
      s = (*Map_Read)(fp,map[0],grd[0]);
    }

  } else {
    /*
     * generate the latitude limit from the data
     */

    make_hmb();
    (*Map_Read)(fp,map[buf],grd[buf]);
    if (ecdip) {
      IGRF_GetDateTime(&yr,&mo,&dy,&hr,&mt,&sec,&dyn);
      if (yr < 0) {
        tme=(grd[buf]->st_time+grd[buf]->ed_time)/2.0;
        TimeEpochToYMDHMS(tme,&yr,&mo,&dy,&hr,&mt,&sc);
        IGRF_SetDateTime(yr,mo,dy,hr,mt,(int)sc);
      }
    } else if (!old_aacgm) { /* check to see if time is already set */
      AACGM_v2_GetDateTime(&yr,&mo,&dy,&hr,&mt,&sec,&dyn);
      if (yr < 0) {
        tme=(grd[buf]->st_time+grd[buf]->ed_time)/2.0;
        TimeEpochToYMDHMS(tme,&yr,&mo,&dy,&hr,&mt,&sc);
        AACGM_v2_SetDateTime(yr,mo,dy,hr,mt,(int)sc);
        AACGM_v2_Lock();
      }
    }

    while (s !=-1) {  

      tme = (grd[buf]->st_time + grd[buf]->ed_time)/2.0;
      TimeEpochToYMDHMS(tme,&yr,&mo,&dy,&hr,&mt,&sc);
      yrsec = TimeYMDHMSToYrsec(yr,mo,dy,hr,mt,(int) sc);
   
      /* now do the HMB determination */
 
      c = 0;
      for (i=0; i<grd[buf]->vcnum; i++) 
        if (grd[buf]->data[i].vel.median > vel_min) c++;

      if (c > 0) { /* determine boundary */
        latc = 0;

        for (j=0; j<nlat; j++) latcnt[j] = 0;
        for (i=0; i<grd[buf]->vcnum; i++) {
        
          if ( (fabs(grd[buf]->data[i].vel.median) < vel_min) ||
               ((grd[buf]->xtd) && (grd[buf]->data[i].wdt.median < wdt_min)) ) continue;

          /* if on the exclusion list ignore this data */

          for (j=0; j<exnum; j++) if (grd[buf]->data[i].st_id == exid[j]) break;
          if (j != exnum) continue;

          if (ecdip) mlti = (int)(ecdip_mlt(yr,mo,dy,hr,mt,(int)sc,grd[buf]->data[i].mlon)+0.5);
          else       mlti = (int)((*MLTCnv)(yr,yrsec,grd[buf]->data[i].mlon)+0.5);
          if (mlti == 24) mlti = 0;
          latdif = 90;  
          for (j=0; j<nlat; j++) {
            if (map[buf]->hemisphere == 1)
               l = fabs(bndarr[mlti][j]-(grd[buf]->data[i].mlat-1));
            else
               l = fabs(bndarr[mlti][j]-(-grd[buf]->data[i].mlat-1));
            if (l < latdif) {
              latdif = l;
              latc = j;
            }
          }
          latcnt[latc]++;
        }
    
        for (i=0; (i<nlat) && (latcnt[i] < cnt_req); i++);

        if (i < nlat) latmin[buf] = lathmb[i];
        else          latmin[buf] = (nodef) ? -1 : latdef;

      } else latmin[buf] = (nodef) ? -1 : latdef;
     
      /* add the boundary to the map */
      if (cnt > 1) {

        /* do median filter here */
        memcpy(lattmp,latmin,3*sizeof(float));
        qsort(lattmp,3,sizeof(float),latcmp);
        latmed = lattmp[1];
       
        idx = buf-1;
        if (idx < 0) idx += 3;

        if (cnt == 2) { /* write the very first record*/

          if (tflg == 0) {
            if (latmed != -1) map_addhmb(yr,yrsec,map[0],bndnp,bndstep,
                                         latref,latmed,magflg);
            else map[0]->latmin = -1;
            (*Map_Write)(stdout,map[0],grd[0]);

            TimeEpochToYMDHMS(grd[0]->st_time,&yr,&mo,&dy,&hr,&mt,&sc);
            if (vb == 1) 
              fprintf(stderr,"%d-%d-%d %d:%d:%d latmin=%g\n",yr,mo,dy,
	                            hr,mt,(int) sc,map[0]->latmin);
          } else {
 
            TimeEpochToYMDHMS(grd[0]->st_time,&yr,&mo,&dy,&hr,&mt,&sc);
            fprintf(stdout,"%.4d %.2d %.2d %.2d %.2d %.2d %.2g %.2g\n",
                            yr,mo,dy,hr,mt,(int) sc,latmed,latmin[0]);
            if (vb == 1) 
              fprintf(stderr,"%d-%d-%d %d:%d:%d latmin: median=%g actual=%g\n",
                              yr,mo,dy,hr,mt,(int) sc,latmed,latmin[0]);
	        }
        }

        if (tflg == 0) {
          if (latmed != -1) map_addhmb(yr,yrsec,map[idx],bndnp,bndstep,
                                       latref,latmed,magflg);
          else map[idx]->latmin = -1;
          (*Map_Write)(stdout,map[idx],grd[idx]);
          TimeEpochToYMDHMS(grd[idx]->st_time,&yr,&mo,&dy,&hr,&mt,&sc);
          if (vb==1) 
            fprintf(stderr,"%d-%d-%d %d:%d:%d latmin=%g\n",yr,mo,dy,
	                          hr,mt,(int) sc,map[idx]->latmin);
        } else {
          TimeEpochToYMDHMS(grd[idx]->st_time,&yr,&mo,&dy,&hr,&mt,&sc);
          fprintf(stdout,"%.4d %.2d %.2d %.2d %.2d %.2d %.2g %.2g\n",
                          yr,mo,dy,hr,mt,(int) sc,latmed,latmin[idx]);
          if (vb==1) 
            fprintf(stderr,"%d-%d-%d %d:%d:%d latmin: median=%g actual=%g\n",
                            yr,mo,dy,hr,mt,(int) sc,latmed,latmin[idx]);
        }
      }
      cnt++;
      buf++;
      buf %= 3;

      s = (*Map_Read)(fp,map[buf],grd[buf]);
    }

    if (cnt == 0) exit(0); /* no record to write out */
  
    idx = buf-1;
    if (idx < 0) idx += 3;

    if (cnt < 3) latmed=latmin[idx];
    if (cnt == 2) { /* we must write out the first record */
      if (tflg == 0) {
        if (latmed != -1) map_addhmb(yr,yrsec,map[0],bndnp,bndstep,
                                     latref,latmed,magflg);
        else map[0]->latmin = -1;
        (*Map_Write)(stdout,map[0],grd[0]);
        TimeEpochToYMDHMS(grd[0]->st_time,&yr,&mo,&dy,&hr,&mt,&sc);
        if (vb == 1) 
          fprintf(stderr,"%d-%d-%d %d:%d:%d latmin=%g\n",yr,mo,dy,
                          hr,mt,(int) sc,map[0]->latmin);
	    } else {

        TimeEpochToYMDHMS(grd[0]->st_time,&yr,&mo,&dy,&hr,&mt,&sc);
        fprintf(stdout,"%.4d %.2d %.2d %.2d %.2d %.2d %.2g %.2g\n",
                        yr,mo,dy,hr,mt,(int) sc,latmed,latmin[0]);
        if (vb == 1) 
          fprintf(stderr,"%d-%d-%d %d:%d:%d latmin: median=%g actual=%g\n",
                          yr,mo,dy,hr,mt,(int) sc,latmed,latmin[0]);
	    }
    }
 
    if (tflg == 0) {  
      if (latmed != -1) map_addhmb(yr,yrsec,map[idx],bndnp,bndstep,
                                   latref,latmed,magflg);
      else map[idx]->latmin = -1;
      (*Map_Write)(stdout,map[idx],grd[idx]);
      TimeEpochToYMDHMS(grd[idx]->st_time,&yr,&mo,&dy,&hr,&mt,&sc);
      if (vb == 1) 
        fprintf(stderr,"%d-%d-%d %d:%d:%d latmin=%g\n",yr,mo,dy,
	                      hr,mt,(int) sc,map[idx]->latmin);
    } else {
      TimeEpochToYMDHMS(grd[idx]->st_time,&yr,&mo,&dy,&hr,&mt,&sc);
      fprintf(stdout,"%.4d %.2d %.2d %.2d %.2d %.2d %.2g %.2g\n",
                      yr,mo,dy,hr,mt,(int) sc,latmed,latmin[idx]);
      if (vb == 1) 
        fprintf(stderr,"%d-%d-%d %d:%d:%d latmin: median=%g actual=%g\n",
                        yr,mo,dy,hr,mt,(int) sc,latmed,latmin[idx]);
    }
  }

  return 0;
}

