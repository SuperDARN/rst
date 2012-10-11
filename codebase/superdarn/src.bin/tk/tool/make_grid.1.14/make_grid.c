/* make_grid.c
   =========== */

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
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <ctype.h>
#include <zlib.h>
#include "rtypes.h"
#include "dmap.h"
#include "option.h"
#include "rtime.h"
#include "radar.h"
#include "rprm.h"
#include "fitdata.h"
#include "cfitdata.h"
#include "scandata.h"
#include "fitread.h"
#include "fitscan.h"
#include "fitindex.h"
#include "fitseek.h"
#include "oldfitread.h"
#include "oldfitscan.h"
#include "cfitread.h"
#include "cfitindex.h"
#include "cfitseek.h"
#include "cfitscan.h"
#include "fitscan.h"
#include "filter.h"
#include "bound.h"
#include "checkops.h"

#include "gtable.h"
#include "gtablewrite.h"
#include "oldgtablewrite.h"

#include "hlpstr.h"
#include "errstr.h"



struct RadarParm *prm;
struct FitData *fit;
struct CFitdata *cfit;
struct RadarScan *src[3];
struct RadarScan *dst;
struct RadarScan *out;

struct FitIndex *inx;


struct RadarNetwork *network;  
struct Radar *radar;
struct RadarSite *site;


int nbox;

int ebmno=0;
int ebm[32*3]; 
int minrng=-1;
int maxrng=-1;

struct GridTable *grid;

int exclude_outofscan(struct RadarScan *ptr) {
  int n,num=0;
  struct RadarBeam *tmp;
  if (ptr==NULL) return -1;
  if (ptr->num==0) return -1;
  tmp=malloc(sizeof(struct RadarBeam)*ptr->num);
  if (tmp==NULL) return -1;

  for (n=0;n<ptr->num;n++) {

    if (ptr->bm[n].scan<0) continue;

    memcpy(&tmp[num],&ptr->bm[n],sizeof(struct RadarBeam));
    num++;
  }
  free(ptr->bm);
  if (num>0) {
    ptr->bm=realloc(tmp,sizeof(struct RadarBeam)*num);
    if (ptr->bm==NULL) {
      free(tmp);
      ptr->num=0;
      return -1;
    }
  } else {
    free(tmp);
    ptr->bm=NULL;
  }
  ptr->num=num;
  return 0;
}




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

  int farg=0;
  int fnum=0;

  unsigned char help=0;
  unsigned char option=0;

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
  int fmax=500*1000;

  int mode=0;
  int tlen=0;


  double stime=-1;
  double etime=-1;
  double extime=0;

  double sdate=-1;
  double edate=-1;

  char *envstr;
  FILE *fp;

  int bxcar=0;
  int limit=0;
  int bflg=0;

  unsigned char gsflg=0,ionflg=0,bthflg=0;
  unsigned char nsflg=0,isflg=0;
  int channel=0;

  int syncflg=1;

  unsigned char catflg=0;
  
  int s=0,i;
  int state=0; 
  char *dname=NULL,*iname=NULL;
  FILE *fitfp=NULL;
  struct OldFitFp *oldfitfp=NULL;
  struct CFitfp *cfitfp=NULL;

  int yr,mo,dy,hr,mt;
  double sc;

  int num,index,nbox;
  int chk;

  char vstr[256];
  char *vbuf=NULL;
  double alt=300.0;
  int avlen=120;
  int iflg=0;
  unsigned char xtd=0;

  unsigned char cfitflg=0;
  unsigned char fitflg=0;

  prm=RadarParmMake();
  fit=FitMake();
  cfit=CFitMake(); 
  for (i=0;i<3;i++) src[i]=RadarScanMake();
  dst=RadarScanMake();

  grid=GridTableMake();

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

  OptionAdd(&opt,"new",'x',&new); 

  OptionAdd(&opt,"vb",'x',&vb);

  OptionAdd(&opt,"st",'t',&stmestr);
  OptionAdd(&opt,"et",'t',&etmestr);
  OptionAdd(&opt,"sd",'t',&sdtestr);
  OptionAdd(&opt,"ed",'t',&edtestr);
  OptionAdd(&opt,"ex",'t',&exstr);
  
  OptionAdd(&opt,"tl",'i',&tlen);
  OptionAdd(&opt,"i",'i',&avlen);

  OptionAdd(&opt,"cn",'t',&chnstr);
  OptionAdd(&opt,"ebm",'t',&bmstr);
  OptionAdd(&opt,"minrng",'i',&minrng);
  OptionAdd(&opt,"maxrng",'i',&minrng);

  OptionAdd(&opt,"fwgt",'i',&mode);

  OptionAdd(&opt,"pmax",'d',&max[1]);
  OptionAdd(&opt,"vmax",'d',&max[0]);
  OptionAdd(&opt,"wmax",'d',&max[2]);
  OptionAdd(&opt,"vemax",'d',&max[3]);
 
  OptionAdd(&opt,"pmin",'d',&min[1]);
  OptionAdd(&opt,"vmin",'d',&min[0]);
  OptionAdd(&opt,"wmin",'d',&min[2]);
  OptionAdd(&opt,"vemin",'d',&min[3]);

  OptionAdd(&opt,"alt",'d',&alt);


  OptionAdd(&opt,"fmax",'i',&fmax);

  OptionAdd(&opt,"nav",'x',&bxcar);
  OptionAdd(&opt,"nlm",'x',&limit);
  OptionAdd(&opt,"nb",'x',&bflg);
  OptionAdd(&opt,"is",'x',&isflg);
  OptionAdd(&opt,"xtd",'x',&xtd);


  OptionAdd(&opt,"ion",'x',&ionflg);
  OptionAdd(&opt,"gs",'x',&gsflg);
  OptionAdd(&opt,"both",'x',&bthflg);

  OptionAdd(&opt,"inertial",'x',&iflg);


  OptionAdd(&opt,"fit",'x',&fitflg);
  OptionAdd(&opt,"cfit",'x',&cfitflg);
 
  OptionAdd(&opt,"c",'x',&catflg);

  farg=OptionProcess(1,argc,argv,&opt,NULL);

  old=!new;


  if (help==1) {
    OptionPrintInfo(stdout,hlpstr);
    exit(0);
  }
  if (option==1) {
    OptionDump(stdout,&opt);
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
  nsflg=!isflg;

  grid->gsct=1;
  if (gsflg) grid->gsct=0;
  if (ionflg) grid->gsct=1;
  if (bthflg) grid->gsct=2;

  if (vb) vbuf=vstr;

  if (channel !=-1) grid->chn=channel;
  else grid->chn=0;
  
  for (i=0;i<4;i++) {
    grid->min[i]=min[i];
    grid->max[i]=max[i];
  }

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
        oldfitfp=OldFitOpen(dname,iname);
        if (oldfitfp==NULL) {
          fprintf(stderr,"File not found.\n");
          exit(-1);
        }
  
        s=OldFitReadRadarScan(oldfitfp,&state,src[0],prm,
			 fit,tlen,syncflg,channel);
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
        fitfp=fopen(dname,"r");
        if (fitfp==NULL) {
          fprintf(stderr,"File not found.\n");
          exit(-1);
        }
        
        s=FitFreadRadarScan(fitfp,&state,src[0],prm,fit,
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
        if (old) s=OldFitSeek(oldfitfp,yr,mo,dy,hr,mt,sc,NULL);
        else s=FitFseek(fitfp,yr,mo,dy,hr,mt,sc,NULL,inx);
        if (s ==-1) {
          fprintf(stderr,"File does not contain the requested interval.\n");
          exit(-1);
        }
        if (tlen==0) {
          if (old) {
            while ((s=OldFitRead(oldfitfp,prm,fit)) !=-1) {
              if (prm->scan==1) break;
	    }
	  } else {
            while ((s=FitFread(fitfp,prm,fit)) !=-1) {
              if (prm->scan==1) break;
	    }
	  }
        } else state=0;
        if (old) s=OldFitReadRadarScan(oldfitfp,&state,src[0],prm,fit,
                              tlen,syncflg,channel);  
        else s=FitFreadRadarScan(fitfp,&state,src[0],prm,fit,
                              tlen,syncflg,channel);  
      } else {
	 s=CFitSeek(cfitfp,yr,mo,dy,hr,mt,sc,NULL,NULL);
        if (s ==-1) {
          fprintf(stderr,"File does not contain the requested interval.\n");
          exit(-1);
        }
        if (tlen==0) {
          while ((s=CFitRead(cfitfp,cfit)) !=-1) {
            if (cfit->scan==1) break;
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
  
    do {
      
      RadarScanResetBeam(src[index],ebmno,ebm);
      if (nsflg) exclude_outofscan(src[index]);
      exclude_range(src[index],minrng,maxrng);
      FilterBoundType(src[index],grid->gsct);
      if (bflg) FilterBound(15,src[index],min,max);

      if ((num>=nbox) && (limit==1) && (mode !=-1))
	chk=FilterCheckOps(nbox,src,fmax);
      else chk=0;
     
      if ((chk==0) && (num>=nbox)) {

          if (mode !=-1) FilterRadarScan(mode,nbox,index,src,dst,15);
          else out=src[index];

          TimeEpochToYMDHMS(out->st_time,&yr,&mo,&dy,&hr,&mt,&sc);
          
          if (site==NULL) {
            radar=RadarGetRadar(network,out->stid);
            if (radar==NULL) {
              fprintf(stderr,"Failed to get radar information.\n");
              exit(-1);
            }
            site=RadarYMDHMSGetSite(radar,yr,mo,dy,hr,mt,(int) sc);
	  }
       
          s=GridTableTest(grid,out,avlen);
          
          if ((s==1) && (grid->st_time>=stime)) {
            if (old) OldGridTableFwrite(stdout,grid,vbuf,xtd);
            else GridTableFwrite(stdout,grid,vbuf,xtd);
            if (vbuf !=NULL) fprintf(stderr,"Storing:%s\n",vbuf);
          }    
          
          s=GridTableMap(grid,out,site,avlen,iflg,alt);     
          if (s !=0) { 
            fprintf(stderr,"Error mapping beams.\n");  
            break;
	  }
      }  

      if (bxcar) index++;
      if (index>2) index=0;
      if (fitflg) {

        if (old) s=OldFitReadRadarScan(oldfitfp,&state,src[index],prm,fit,
                              tlen,syncflg,channel);
        else s=FitFreadRadarScan(fitfp,&state,src[index],prm,fit,
                              tlen,syncflg,channel);
      } else 
        s=CFitReadRadarScan(cfitfp,&state,src[index],
                            cfit,tlen,syncflg,channel);
 
      if ((etime !=-1) && (src[index]->st_time>etime)) break;
      num++;

    } while (s!=-1);
    if (fitflg) {
      if (old) OldFitClose(oldfitfp);
      else fclose(fitfp);
    } else CFitClose(cfitfp);
  } else {
   index=0;
   num=0;
   for (fnum=farg;fnum<argc;fnum++) {
       fprintf(stderr,"Opening file:%s\n",argv[fnum]);

       if (fitflg) {
         if (old) {
           oldfitfp=OldFitOpen(argv[fnum],NULL);
           if (oldfitfp==NULL) {
             fprintf(stderr,"File not found.\n");
             continue;
           }
  
           s=OldFitReadRadarScan(oldfitfp,&state,src[index],prm,fit,
                               tlen,syncflg,channel);
           
           if (s !=0) {
	     OldFitClose(oldfitfp);
             continue;
           }
	 } else {
           fitfp=fopen(argv[fnum],"r");
           if (fitfp==NULL) {
             fprintf(stderr,"File not found.\n");
             continue;
	   } 

           s=FitFreadRadarScan(fitfp,&state,src[index],prm,fit,
                               tlen,syncflg,channel);
           if (s !=0) {
	     fclose(fitfp);
             continue;
           }
	 }
       } else {
         cfitfp=CFitOpen(argv[fnum]);
         if (cfitfp==NULL) {
           fprintf(stderr,"File not found.\n");
           continue;
         }
         s=CFitReadRadarScan(cfitfp,&state,src[index],cfit,tlen,
                             syncflg,channel);
         if (s !=0) {
	   CFitClose(cfitfp);
           continue;
         }
       }

       num++;
       do {
         RadarScanResetBeam(src[index],ebmno,ebm);
         if (nsflg) exclude_outofscan(src[index]); 
         exclude_range(src[index],minrng,maxrng);
         FilterBoundType(src[index],grid->gsct);
         if (bflg) FilterBound(15,src[index],min,max);

         if ((num>=nbox) && (limit==1) && (mode !=-1))
  	   chk=FilterCheckOps(nbox,src,fmax);
         else chk=0;
         
         if ((chk==0) && (num>=nbox)) {
	
           if (mode !=-1) FilterRadarScan(mode,nbox,index,src,dst,15);
           else out=src[index];
         
           TimeEpochToYMDHMS(out->st_time,&yr,&mo,&dy,&hr,&mt,&sc);
       

           if (site==NULL) {
             radar=RadarGetRadar(network,out->stid);
             if (radar==NULL) {
               fprintf(stderr,"Failed to get radar information.\n");
               exit(-1);
             }
             site=RadarYMDHMSGetSite(radar,yr,mo,dy,hr,mt,(int) sc);
	   }

           s=GridTableTest(grid,out,avlen);
  
           if ((s==1) && (grid->st_time>=stime)) {
             if (old) OldGridTableFwrite(stdout,grid,vbuf,xtd);
             else GridTableFwrite(stdout,grid,vbuf,xtd);
             if (vbuf !=NULL) fprintf(stderr,"Storing:%s\n",vbuf);
           }    
           s=GridTableMap(grid,out,site,avlen,iflg,alt);
           if (s !=0) { 
            fprintf(stderr,"Error mapping beams.\n");  
            break;
	  }
         }

         if (bxcar) index++;
         if (index>2) index=0;
         if (fitflg) {
           if (old) s=OldFitReadRadarScan(oldfitfp,&state,src[index],
                                          prm,fit,tlen,syncflg,channel);
           else s=FitFreadRadarScan(fitfp,&state,src[index],
                                          prm,fit,tlen,syncflg,channel);

         } else 
           s=CFitReadRadarScan(cfitfp,&state,src[index],cfit,tlen,
                              syncflg,channel);
         if ((etime !=-1) && (src[index]->st_time>etime)) break;
         num++;
      } while (s!=-1);
      if (fitflg) {
         if (old) OldFitClose(oldfitfp);
         else fclose(fitfp);
      } else CFitClose(cfitfp);
     }
   }
  return 0;
}



