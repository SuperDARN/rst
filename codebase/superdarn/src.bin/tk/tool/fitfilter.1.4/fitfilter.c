/* fitfilter.c
   =========== 
   Author: R.J.Barnes
*/


/*
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
#include <sys/stat.h>
#include <sys/types.h>
#include <ctype.h>
#include <zlib.h>
#include "rtypes.h"
#include "option.h"
#include "dmap.h"
#include "rtime.h"
#include "rprm.h"
#include "fitdata.h"
#include "cfitdata.h"
#include "scandata.h"
#include "fitread.h"
#include "fitindex.h"
#include "fitseek.h"
#include "cfitread.h"
#include "cfitindex.h"
#include "cfitseek.h"
#include "cfitwrite.h"
#include "cfitscan.h"
#include "fitscan.h"
#include "filter.h"
#include "bound.h"
#include "checkops.h"

#include "oldfitread.h"
#include "oldfitscan.h"

#include "hlpstr.h"
#include "errstr.h"




struct CFitdata *ocfit=NULL;

struct CFitdata *cfit;
struct RadarParm *prm;
struct FitData *fit;
struct RadarScan *src[3];
struct RadarScan *dst;
struct RadarScan *out;

struct FitIndex *inx;

int nbox;

int ebmno=0;
int ebm[32]; 
int minrng=-1;
int maxrng=-1;

void exclude_range(struct RadarScan *ptr,int minrng,int maxrng) {
  int bm,rng;
  for (bm=0;bm<ptr->num;bm++) {
    if (ptr->bm[bm].bm==-1) continue;
    if (minrng !=-1) for (rng=0;rng<minrng;rng++) ptr->bm[bm].sct[rng]=0;     
    if (maxrng !=-1) for (rng=maxrng;rng<ptr->bm[bm].nrang;rng++) 
       ptr->bm[bm].sct[rng]=0; 
 
  } 
}

void parse_ebeam(char *str) {
  int i,j=0;
  for (i=0;str[i] !=0;i++) {
    if (str[i]==',') {
      str[i]=0;
      ebm[ebmno]=atoi(str+j);
      ebmno++;
      j=i+1;
    } 
  }
  ebm[ebmno]=atoi(str+j);
  ebmno++;
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
  if (text[i]==0) return atoi(text)*3600L;
  text[i]=0;
  hr=atoi(text);
  mn=atoi(text+i+1);
  return hr*3600L+mn*60L;
}   

struct OptionData opt;

int rst_opterr(char *txt) {
  fprintf(stderr,"Option not recognized: %s\n",txt);
  fprintf(stderr,"Please try: fitfilter --help\n");
  return(-1);
}

int main(int argc,char *argv[]) {

  int old=0;

  int farg=0;
  int fnum=0;

  unsigned char help=0;
  unsigned char option=0;
  unsigned char version=0;

  unsigned char vb=0;

  char *chnstr=NULL;
  char *bmstr=NULL;

  char *stmestr=NULL;
  char *etmestr=NULL;
  char *sdtestr=NULL;
  char *edtestr=NULL;
  char *exstr=NULL;

  double min[4]={35,3,10,0};
  double max[4]={2000,50,1000,200};
  int fmax=500;

  int mode=0;
  int tlen=0;
  int tflg=1;


  double stime=-1;
  double etime=-1;
  double extime=0;

  double sdate=-1;
  double edate=-1;

  int bxcar=0;
  int limit=0;
  int bflg=0;
  int isort=0;

  unsigned char gsflg=0,ionflg=0,bthflg=0;
  int channel=0;

  int syncflg=1;

  unsigned char catflg=0;
  
  int s=0,n,r;
  int state=0; 
  char *dname=NULL,*iname=NULL;
  FILE *fp=NULL;
  gzFile gzfp=NULL;
  struct OldFitFp *fitfp=NULL;
  struct CFitfp *cfitfp=NULL;

  int yr,mo,dy,hr,mt;
  double sc;

  int num,index,nbox;
  int chk;
  int cnum=0;

  unsigned char cfitflg=0;
  unsigned char fitflg=0;

  prm=RadarParmMake();
  fit=FitMake();
  cfit=CFitMake();
  ocfit=CFitMake();

  for (n=0;n<3;n++) src[n]=RadarScanMake();
  dst=RadarScanMake();

  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"-option",'x',&option);
  OptionAdd(&opt,"-version",'x',&version);

  OptionAdd(&opt,"old",'x',&old); 
  OptionAdd(&opt,"vb",'x',&vb);

  OptionAdd(&opt,"st",'t',&stmestr);
  OptionAdd(&opt,"et",'t',&etmestr);
  OptionAdd(&opt,"sd",'t',&sdtestr);
  OptionAdd(&opt,"ed",'t',&edtestr);
  OptionAdd(&opt,"ex",'t',&exstr);
  
  OptionAdd(&opt,"tl",'i',&tlen);
 
  OptionAdd(&opt,"cn",'t',&chnstr);
  OptionAdd(&opt,"ebm",'t',&bmstr);
  OptionAdd(&opt,"minrng",'i',&minrng);
  OptionAdd(&opt,"maxrng",'i',&minrng);

  OptionAdd(&opt,"fwgt",'i',&mode);

  OptionAdd(&opt,"pmax",'d',&max[0]);
  OptionAdd(&opt,"vmax",'d',&max[1]);
  OptionAdd(&opt,"wmax",'d',&max[2]);
  OptionAdd(&opt,"vemax",'d',&max[3]);
 
  OptionAdd(&opt,"pmin",'d',&min[0]);
  OptionAdd(&opt,"vmin",'d',&min[1]);
  OptionAdd(&opt,"wmin",'d',&min[2]);
  OptionAdd(&opt,"vemin",'d',&min[3]);

  OptionAdd(&opt,"fmax",'i',&fmax);

  OptionAdd(&opt,"nav",'x',&bxcar);
  OptionAdd(&opt,"nlm",'x',&limit);
  OptionAdd(&opt,"nb",'x',&bflg);
  OptionAdd(&opt,"isort",'x',&isort);

  OptionAdd(&opt,"ion",'x',&ionflg);
  OptionAdd(&opt,"gs",'x',&gsflg);
  OptionAdd(&opt,"both",'x',&bthflg);

  OptionAdd(&opt,"fit",'x',&fitflg);
  OptionAdd(&opt,"cfit",'x',&cfitflg);
 
  OptionAdd(&opt,"c",'x',&catflg);

  farg=OptionProcess(1,argc,argv,&opt,rst_opterr);

  if (farg==-1) {
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



  if (farg==argc) {
    OptionPrintInfo(stderr,errstr);
    exit(-1);
  }

  if (chnstr !=NULL) {
    if (tolower(chnstr[0])=='a') channel=1;
    if (tolower(chnstr[0])=='b') channel=2;
  }

  if (bmstr !=NULL)  parse_ebeam(bmstr);

  if (exstr !=NULL) extime=strtime(exstr);
  if (stmestr !=NULL) stime=strtime(stmestr);
  if (etmestr !=NULL) etime=strtime(etmestr);
  if (sdtestr !=NULL) sdate=strdate(sdtestr);
  if (edtestr !=NULL) edate=strdate(edtestr);

  if (mode>0) mode--;

  bxcar=!bxcar;
  bflg=!bflg;
  limit=!limit;

  if (gsflg) tflg=0;
  if (ionflg) tflg=1;
  if (bthflg) tflg=2;

  if (bxcar) nbox=3;
  else nbox=1;

  out=dst;

  if (cfitflg==0) fitflg=1;

  if (catflg==0) {
    if (argc-farg>1) {
      dname=argv[argc-2];
      iname=argv[argc-1];
    } else dname=argv[argc-1];


    if (fitflg) {
      if (old) {
        fitfp=OldFitOpen(dname,iname);
        if (fitfp==NULL) {
          fprintf(stderr,"File not found.\n");
          exit(-1);
        }
  
        s=OldFitReadRadarScan(fitfp,&state,src[0],prm,fit,
                            tlen,syncflg,channel);
        if (s==-1) {
          fprintf(stderr,"Error reading file.\n");
          exit(-1);
        }
      } else {
        if (iname !=NULL) {
          fp=fopen(iname,"r");
          if (fp==NULL) {
            fprintf(stderr,"Could not open index.\n");
	  } else {
            inx=FitIndexFload(fp);
            fclose(fp);
            if (inx==NULL) {
              fprintf(stderr,"Error loading index.\n");
	    }
	  }
	}
        fp=fopen(dname,"r");
        if (fp==NULL) {
          fprintf(stderr,"File not found.\n");
          exit(-1);
        }
  
        s=FitFreadRadarScan(fp,&state,src[0],prm,fit,
                            tlen,syncflg,channel);
        if (s==-1) {
          fprintf(stderr,"Error reading file.\n");
          exit(-1);
        }
      }
    } else {
      cfitfp=CFitOpen(dname);
      if (cfitfp==NULL) {
        fprintf(stderr,"File not found.\n");
        exit(-1);
      }
  
      s=CFitReadRadarScan(cfitfp,&state,src[0],cfit,tlen,syncflg,channel);
      if (s==-1) {
        fprintf(stderr,"Error reading file.\n");
        exit(-1);
      }
    }   

    if ((stime !=-1) || (sdate !=-1)) { 
      /* we must skip the start of the files */
      int yr,mo,dy,hr,mt;
      double sc;  
    
      if (stime==-1) stime= ( (int) src[0]->st_time % (24*3600));
      if (sdate==-1) stime+=src[0]->st_time - 
                            ( (int) src[0]->st_time % (24*3600));
      else stime+=sdate;

      if (bxcar==1) {
        /* subtract one src */
        if (tlen !=0) stime-=tlen;
        else stime-=15+src[0]->ed_time-src[0]->st_time; /* pad to make sure */
      }

      TimeEpochToYMDHMS(stime,&yr,&mo,&dy,&hr,&mt,&sc);
    
      if (fitflg) {
        if (old) s=OldFitSeek(fitfp,yr,mo,dy,hr,mt,sc,NULL);
        else s=FitFseek(fp,yr,mo,dy,hr,mt,sc,NULL,inx);
        if (s ==-1) {
          fprintf(stderr,"File does not contain the requested interval.\n");
          exit(-1);
        }
        if (tlen==0) {
          if (old) {
            while ((s=OldFitRead(fitfp,prm,fit)) !=-1) {
              if (abs(prm->scan)==1) break;
	    }
	  } else {
            while ((s=FitFread(fp,prm,fit)) !=-1) {
              if (abs(prm->scan)==1) break;
	    }
	  }
        } else state=0;
        if (old) s=OldFitReadRadarScan(fitfp,&state,src[0],prm,fit,
                              tlen,syncflg,channel);  
        else s=FitFreadRadarScan(fp,&state,src[0],prm,fit,
                              tlen,syncflg,channel);  
      } else {
        s=CFitSeek(cfitfp,yr,mo,dy,hr,mt,sc,NULL,NULL);
        if (s ==-1) {
          fprintf(stderr,"File does not contain the requested interval.\n");
          exit(-1);
        }
        if (tlen==0) {
          while ((s=CFitRead(cfitfp,cfit)) !=-1) {
            if (abs(cfit->scan)==1) break;
	  }
        } else state=0;
        s=CFitReadRadarScan(cfitfp,&state,src[0],cfit,tlen,syncflg,channel);
      }      
    } else stime=src[0]->st_time;
  
    if (etime !=-1) {
      if (edate==-1) etime+=src[0]->st_time - 
                            ( (int) src[0]->st_time % (24*3600));
      else etime+=edate;
    }

    if (extime !=0) etime=stime+extime;
    if ((etime !=-1) && (bxcar==1)) {
      if (tlen !=0) etime+=tlen;
      else etime+=15+src[0]->ed_time-src[0]->st_time;
    }

    num=1;
    index=0;
   
    gzfp=gzdopen(fileno(stdout),"w");

    do {

      RadarScanResetBeam(src[index],ebmno,ebm);
      exclude_range(src[index],minrng,maxrng);
      FilterBoundType(src[index],tflg);
      if (bflg) FilterBound(15,src[index],min,max);

      if ((num>=nbox) && (limit==1) && (mode !=-1))
	chk=FilterCheckOps(nbox,src,fmax);
      else chk=0;
      if ((chk==0) && (num>=nbox)) {

          if (mode !=-1) FilterRadarScan(mode,nbox,index,src,dst,15,isort);
          else out=src[index];

          TimeEpochToYMDHMS(out->st_time,&yr,&mo,&dy,&hr,&mt,&sc);   
          if (vb) fprintf(stderr,"%.4d-%.2d-%.2d %.2d:%.2d:%.2d\n",
                               yr,mo,dy,hr,mt,(int) sc);
          for (n=0;n<out->num;n++) {
            ocfit->stid=out->stid;
            ocfit->time=out->st_time;
	    ocfit->bmnum=out->bm[n].bm;
	    ocfit->cp=out->bm[n].cpid;
            ocfit->intt.sc=out->bm[n].intt.sc;
            ocfit->intt.us=out->bm[s].intt.us;
	    ocfit->nave=out->bm[n].nave;
            ocfit->frang=out->bm[n].frang;
	    ocfit->rsep=out->bm[n].rsep;
            ocfit->rxrise=out->bm[n].rxrise;
	    ocfit->tfreq=out->bm[n].freq;
            ocfit->noise=out->bm[n].noise;
	    ocfit->atten=out->bm[n].atten;
            ocfit->channel=out->bm[n].channel;
            ocfit->nrang=out->bm[n].nrang;
            if (n==0) ocfit->scan=1;
            else ocfit->scan=0;
	    cnum=0;
            for (r=0;r<out->bm[n].nrang;r++) if (out->bm[n].sct[r]!=0) cnum++;
            CFitSetRng(ocfit,cnum); 
            ocfit->num=cnum;
            cnum=0;
            for (r=0;r<ocfit->nrang;r++) {
	      if (out->bm[n].sct[r]==0) continue;
              ocfit->rng[cnum]=r;
              ocfit->data[cnum].gsct=out->bm[n].rng[r].gsct;
	      ocfit->data[cnum].p_0=out->bm[n].rng[r].p_0;
	      ocfit->data[cnum].p_0_e=out->bm[n].rng[r].p_0_e;
              ocfit->data[cnum].v=out->bm[n].rng[r].v;
	      ocfit->data[cnum].v_e=out->bm[n].rng[r].v_e;
	      ocfit->data[cnum].p_l=out->bm[n].rng[r].p_l;
              ocfit->data[cnum].p_l_e=out->bm[n].rng[r].p_l_e;
	      ocfit->data[cnum].w_l=out->bm[n].rng[r].w_l;
              ocfit->data[cnum].w_l_e=out->bm[n].rng[r].w_l_e;
              cnum++;
	    } 
                   

            CFitWrite(gzfp,ocfit);

	  }

      }

      if (bxcar) index++;
      if (index>2) index=0;
      if (fitflg) {

        if (old) s=OldFitReadRadarScan(fitfp,&state,src[index],prm,fit,
                              tlen,syncflg,channel);
        else s=FitFreadRadarScan(fp,&state,src[index],prm,fit,
                              tlen,syncflg,channel);
      } else 
        s=CFitReadRadarScan(cfitfp,&state,src[index],cfit,tlen,
                            syncflg,channel);
 
      if ((etime !=-1) && (src[index]->st_time>etime)) break;
      num++;

    } while (s!=-1);
    if (fitflg) {
      if (old) OldFitClose(fitfp);
      else fclose(fp);
    } else CFitClose(cfitfp);
  } else {
   index=0;
   num=0;
   for (fnum=farg;fnum<argc;fnum++) {
       fprintf(stderr,"Opening file:%s\n",argv[fnum]);
       if (fitflg) {
         if (old) {
           fitfp=OldFitOpen(argv[fnum],NULL);
           if (fitfp==NULL) {
             fprintf(stderr,"File not found.\n");
             continue;
           }
  
           s=OldFitReadRadarScan(fitfp,&state,src[index],prm,fit,
                               tlen,syncflg,channel);
           if (s==-1) {
	     OldFitClose(fitfp);
             continue;
           }
	 } else {
           fp=fopen(argv[fnum],"r");
           if (fp==NULL) {
             fprintf(stderr,"File not found.\n");
             continue;
	   } 

           s=FitFreadRadarScan(fp,&state,src[index],prm,fit,
                               tlen,syncflg,channel);
           if (s==-1) {
	     fclose(fp);
             continue;
           }
	 }
       } else {
         cfitfp=CFitOpen(argv[fnum]);
         if (fitfp==NULL) {
           fprintf(stderr,"File not found.\n");
           continue;
         }
         s=CFitReadRadarScan(cfitfp,&state,src[index],cfit,tlen,
                             syncflg,channel);
         if (s==-1) {
	   CFitClose(cfitfp);
           continue;
         }
       }

       num++;
       do {
	 /*
         RadarScanResetBeam(src[index],ebmno,ebm);
	 */
         exclude_range(src[index],minrng,maxrng);
         FilterBoundType(src[index],tflg);
         if (bflg) FilterBound(15,src[index],min,max);

         if ((num>=nbox) && (limit==1) && (mode !=-1))
  	   chk=FilterCheckOps(nbox,src,fmax);
         else chk=0;
   
         if ((chk==0) && (num>=nbox)) {
	
           if (mode !=-1) FilterRadarScan(mode,nbox,index,src,dst,15,isort);
           else out=src[index];
         
           TimeEpochToYMDHMS(out->st_time,&yr,&mo,&dy,&hr,&mt,&sc);

	   if (vb) fprintf(stderr,"%.4d-%.2d-%.2d %.2d:%.2d:%.2d\n",
			   yr,mo,dy,hr,mt,(int) sc);
	   for (n=0;n<out->num;n++) {
	     ocfit->stid=out->stid;
	     ocfit->time=out->bm[n].time;
	     ocfit->bmnum=out->bm[n].bm;
	     ocfit->cp=out->bm[n].cpid;
	     ocfit->intt.sc=out->bm[n].intt.sc;
             ocfit->intt.us=out->bm[n].intt.us;
	     ocfit->nave=out->bm[n].nave;
	     ocfit->frang=out->bm[n].frang;
	     ocfit->rsep=out->bm[n].rsep;
	     ocfit->rxrise=out->bm[n].rxrise;
	     ocfit->tfreq=out->bm[n].freq;
	     ocfit->noise=out->bm[n].noise;
	     ocfit->atten=out->bm[n].atten;
	     ocfit->channel=out->bm[n].channel;
	     ocfit->nrang=out->bm[n].nrang;
	     if (n==0) ocfit->scan=1;
	     else ocfit->scan=0;
	     num=0;
             for (r=0;r<out->bm[n].nrang;r++) if (out->bm[n].sct[r]!=0) num++;
             CFitSetRng(ocfit,num); 
             ocfit->num=num;
             num=0;
	     for (r=0;r<out->bm[n].nrang;r++) {
	       if (out->bm[n].sct[r]==0) continue;
	       ocfit->rng[num]=r;
	       ocfit->data[num].gsct=out->bm[n].rng[r].gsct;
	       ocfit->data[num].p_0=out->bm[n].rng[r].p_0;
	       ocfit->data[num].p_0_e=out->bm[n].rng[r].p_0_e;
	       ocfit->data[num].v=out->bm[n].rng[r].v;
	       ocfit->data[num].v_e=out->bm[n].rng[r].v_e;
	       ocfit->data[num].p_l=out->bm[n].rng[r].p_l;
	       ocfit->data[num].p_l_e=out->bm[n].rng[r].p_l_e;
	       ocfit->data[num].w_l=out->bm[n].rng[r].w_l;
	       ocfit->data[num].w_l_e=out->bm[n].rng[r].w_l_e;
               num++;
	     }
	   }
	   CFitWrite(gzfp,ocfit);           
         }

         if (bxcar) index++;
         if (index>2) index=0;
         if (fitflg) {
           if (old) s=OldFitReadRadarScan(fitfp,&state,src[index],
                                          prm,fit,tlen,syncflg,channel);
           else s=FitFreadRadarScan(fp,&state,src[index],
                                          prm,fit,tlen,syncflg,channel);

         } else 
           s=CFitReadRadarScan(cfitfp,&state,src[index],cfit,tlen,
                              syncflg,channel);
         if ((etime !=-1) && (src[index]->st_time>etime)) break;
         num++;
      } while (s!=-1);
      if (fitflg) {
         if (old) OldFitClose(fitfp);
         else fclose(fp);
      } else CFitClose(cfitfp);
     }
   }
   gzclose(gzfp);

  return 0;
}










