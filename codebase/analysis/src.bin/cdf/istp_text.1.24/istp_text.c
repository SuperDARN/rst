/* istp_text.c
   ===========
   Author: R.J.Barnes
*/

/*
 LICENSE AND DISCLAIMER
 
 Copyright (c) 2012 The Johns Hopkins University/Applied Physics Laboratory
 
 This file is part of the Radar Software Toolkit (RST).
 
 RST is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with RST.  If not, see <http://www.gnu.org/licenses/>.
 
 
 
*/



/* This is a simple test program to demonstrate how to read a CDF file */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <math.h>

#include "rtypes.h"
#include "rtime.h"
#include "option.h"

#include "cdf.h"
#include "rcdf.h"

#include "locate.h"

#include "istp.h"
#include "wind.h"
#include "ace.h"
#include "imp.h"
#include "geotail.h"

#include "errstr.h"
#include "hlpstr.h"
  
struct imfdata imf;
struct posdata pos;
struct plasmadata plasma;

unsigned char pst=0;
unsigned char mfi=0;
unsigned char swe=0;
unsigned char mag=0;
unsigned char pla=0;
unsigned char mgf=0;
unsigned char lep=0;
unsigned char cpi=0;

char sat[256];
char inst[256];

char dpath[256]={"/data"};
 
double stime=-1;
double etime=-1;

struct file_list *fptr;

struct OptionData opt;
struct OptionFile *optf=NULL;

int find_imf(struct imfdata *ptr,double time) {
 int i;
 for (i=ptr->idx;(i<ptr->cnt) && (ptr->time[i]<=time);i++);
 if (i>0) ptr->idx=i-1;
 return ptr->idx;
}

int find_pos(struct posdata *ptr,double time) {
 int i;
 for (i=ptr->idx;(i<ptr->cnt) && (ptr->time[i]<=time);i++);
 if (i>0) ptr->idx=i-1;
 return ptr->idx;
}


int find_plasma(struct plasmadata *ptr,double time) {
 int i;
 for (i=ptr->idx;(i<ptr->cnt) && (ptr->time[i]<=time);i++);
 if (i>0) ptr->idx=i-1;
 return ptr->idx;
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


int load_wind() {
  int i;
  char path[256];

  CDFid id;
  CDFstatus status;
 

  sprintf(sat,"WIND");
  sprintf(path,"%s/%s",dpath,"wind");

  if ((mfi+swe+pst)==0) mfi=1; /* default to imf if not given */

  if ((swe !=0) || ((pst !=0) && (mfi==0))) {
    strcpy(inst,"SWE");
    fptr=locate_files(path,"swe",stime,etime);

    for (i=0;i<fptr->cnt;i++) {
      fprintf(stderr,"%s\n",fptr->fname[i]);

      status=CDFopen(fptr->fname[i],&id);
      if (status !=CDF_OK) {
        fprintf(stderr,"Could not open cdf file.\n");
        continue;
      }
   
      if (pst !=0) status=windswe_pos(id,&pos,stime,etime); 
      status=windswe_plasma(id,&plasma,stime,etime);
      CDFclose(id);
    }
    free_locate(fptr);
  }


  if ((mfi !=0) || ((pst !=0) && (swe==0))) {
    if (swe==0) strcpy(inst,"MFI");
    else strcat(inst,"+MFI");
    fptr=locate_files(path,"mfi",stime,etime);

    for (i=0;i<fptr->cnt;i++) {
      fprintf(stderr,"%s\n",fptr->fname[i]);

      status=CDFopen(fptr->fname[i],&id);
      if (status !=CDF_OK) {
        fprintf(stderr,"Could not open cdf file.\n");
        continue;
      }
   
      if ((swe==0) && (pst !=0)) status=windmfi_pos(id,&pos,stime,etime); 
      status=windmfi_imf(id,&imf,stime,etime);
      CDFclose(id);
    }
    free_locate(fptr);
  }
  return 0;
}
 
int load_ace() {
  int i;
  char path[256];

  CDFid id;
  CDFstatus status;
 
  if ((mfi+swe+pst)==0) mfi=1; /* default to imf if not given */

  sprintf(sat,"ACE");
  sprintf(path,"%s/%s",dpath,"ace");
 

  if (swe==1) strcpy(inst,"SWE");
 
  if ((pst !=0) || (swe !=0)) {
    fprintf(stderr,"Calling locate files.\n");
    fptr=locate_files(path,"h0_swe",stime,etime);
    for (i=0;i<fptr->cnt;i++) {
      fprintf(stderr,"%s\n",fptr->fname[i]);

      status=CDFopen(fptr->fname[i],&id);
      if (status !=CDF_OK) {
         fprintf(stderr,"Could not open cdf file.\n");
         continue;
      }
      if (pst !=0) status=aceswe_pos(id,&pos,stime,etime); 
      if (swe !=0) status=aceswe_plasma(id,&plasma,stime,etime);
      CDFclose(id);
    }
    free_locate(fptr);
  }
  if (mfi==1) {
    if (swe==0) strcpy(inst,"MFI");
    else strcat(inst,"+MFI");
    fprintf(stderr,"calling locate files\n");
    fptr=locate_files(path,"h0_mfi",stime,etime);
    if (fptr->cnt !=0) {
      for (i=0;i<fptr->cnt;i++) {
        fprintf(stderr,"%s\n",fptr->fname[i]);

        status=CDFopen(fptr->fname[i],&id);
        if (status !=CDF_OK) {
          fprintf(stderr,"Could not open cdf file.\n");
          continue;
        }
    
        status=acemfi_imf(id,&imf,stime,etime,0);
        CDFclose(id);
      }
      free_locate(fptr);
    } else {
     free_locate(fptr);    
     fptr=locate_files(path,"k1_mfi",stime,etime);
     for (i=0;i<fptr->cnt;i++) {
        fprintf(stderr,"%s\n",fptr->fname[i]);

        status=CDFopen(fptr->fname[i],&id);
        if (status !=CDF_OK) {
          fprintf(stderr,"Could not open cdf file.\n");
          continue;
        }
    
        status=acemfi_imf(id,&imf,stime,etime,1);
        CDFclose(id);
      }
      free_locate(fptr);
    }
  }
  return 0;
}



int load_imp() {
  int i;
  char path[256];

  CDFid id;
  CDFstatus status;
 

  sprintf(sat,"IMP8");
  sprintf(path,"%s/%s",dpath,"imp8");

  if ((mag+pla+pst)==0) mag=1; /* default to imf if not given */

  if ((pla !=0) || ((pst !=0) && (pla==0))) {

    strcpy(inst,"PLA");
   
    fptr=locate_files(path,"pla",stime,etime);

    for (i=0;i<fptr->cnt;i++) {
      fprintf(stderr,"%s\n",fptr->fname[i]);

      status=CDFopen(fptr->fname[i],&id);
      if (status !=CDF_OK) {
        fprintf(stderr,"Could not open cdf file.\n");
        continue;
      }
  
      if (pst !=0) status=imppla_pos(id,&pos,stime,etime); 
      status=imppla_plasma(id,&plasma,stime,etime);
      CDFclose(id);
    }
    free_locate(fptr);
  }

  if ((mag !=0) || ((pst !=0) && (pla==0))) {
    if (pla==0) strcpy(inst,"MAG");
    else strcat(inst,"+MAG");
    fptr=locate_files(path,"mag",stime,etime);

    for (i=0;i<fptr->cnt;i++) {
      fprintf(stderr,"%s\n",fptr->fname[i]);

      status=CDFopen(fptr->fname[i],&id);
      if (status !=CDF_OK) {
        fprintf(stderr,"Could not open cdf file.\n");
        continue;
      }
   
      if ((pla==0) && (pst !=0)) status=impmag_pos(id,&pos,stime,etime); 
      status=impmag_imf(id,&imf,stime,etime);
      CDFclose(id);
    }
    free_locate(fptr);
  }


  return 0;
}
 
int load_geo() {
  int i;
  char path[256];

  CDFid id;
  CDFstatus status;
 

  sprintf(sat,"GEOTAIL");
  sprintf(path,"%s/%s",dpath,"geotail");

  if ((mgf+lep+cpi+pst)==0) mgf=1; /* default to imf if not given */

  if (lep==1) strcpy(inst,"LEP");

  if ((lep !=0) || ((pst !=0) && (mgf==0))) {
    fptr=locate_files(path,"lep",stime,etime);

    for (i=0;i<fptr->cnt;i++) {
        fprintf(stderr,"%s\n",fptr->fname[i]);

        status=CDFopen(fptr->fname[i],&id);
        if (status !=CDF_OK) {
          fprintf(stderr,"Could not open cdf file.\n");
          continue;
        }
  
        if (pst !=0) status=geolep_pos(id,&pos,stime,etime);
        if (lep !=0) status=geolep_plasma(id,&plasma,stime,etime);
        CDFclose(id);
      }
      free_locate(fptr);
  }

  if (cpi==1) {

    if (lep==0) strcpy(inst,"CPI");
    else strcat(inst,"+CPI");
    fptr=locate_files(path,"cpi",stime,etime);

    for (i=0;i<fptr->cnt;i++) {
      fprintf(stderr,"%s\n",fptr->fname[i]);

      status=CDFopen(fptr->fname[i],&id);
      if (status !=CDF_OK) {
        fprintf(stderr,"Could not open cdf file.\n");
        continue;
      }
  
      status=geocpi_plasma(id,&plasma,stime,etime);
      CDFclose(id);
    }
    free_locate(fptr);
  }

  if (mgf==1) {
    if ((lep+cpi)==0) strcpy(inst,"MGF");
    else strcat(inst,"+MGF");
    fptr=locate_files(path,"mgf",stime,etime);

    for (i=0;i<fptr->cnt;i++) {
      fprintf(stderr,"%s\n",fptr->fname[i]);

      status=CDFopen(fptr->fname[i],&id);
      if (status !=CDF_OK) {
        fprintf(stderr,"Could not open cdf file.\n");
        continue;
      }
   
      if ((lep==0) && (pst !=0)) status=geomgf_pos(id,&pos,stime,etime); 
      status=geomgf_imf(id,&imf,stime,etime);
      CDFclose(id);
    }
    free_locate(fptr);
  }
  return 0;
}

int rst_opterr(char *txt) {
  fprintf(stderr,"Option not recognized: %s\n",txt);
  fprintf(stderr,"Please try: istp_text --help\n");
  return(-1);
}
 
int main(int argc,char *argv[]) {
  int arg;
  char *envstr;

  char *cfname=NULL;
  FILE *fp;

  unsigned char wind=0;
  unsigned char ace=0;
  unsigned char imp=0;
  unsigned char geo=0;

  unsigned char help=0;
  unsigned char option=0;
  unsigned char version=0;


  char *stime_txt=NULL;
  char *extime_txt=NULL;
  char *etime_txt=NULL;
  char *sdate_txt=NULL;
  char *edate_txt=NULL;
  char *dpath_txt=NULL;

  double extime=-1;
  double sdate=-1;
  double edate=-1;

  int i;
  unsigned char hdr=0;
  unsigned char mode=0;

  // int dflg=0;

  int pnum=0;
  double *ptme=NULL;

  int yr,mo,dy,hr,mt;
  double sc;

  envstr=getenv("ISTP_PATH");
  if (envstr !=NULL) strcpy(dpath,envstr);

  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"-option",'x',&option);
  OptionAdd(&opt,"-version",'x',&version);

  OptionAdd(&opt,"h",'x',&hdr);

  OptionAdd(&opt,"st",'t',&stime_txt);
  OptionAdd(&opt,"et",'t',&etime_txt);
  OptionAdd(&opt,"ex",'t',&extime_txt);
  OptionAdd(&opt,"sd",'t',&sdate_txt);
  OptionAdd(&opt,"ed",'t',&edate_txt);

  OptionAdd(&opt,"gse",'x',&mode);
  OptionAdd(&opt,"pos",'x',&pst);

  OptionAdd(&opt,"mfi",'x',&mfi);
  OptionAdd(&opt,"swe",'x',&swe);
  OptionAdd(&opt,"mag",'x',&mag);
  OptionAdd(&opt,"pla",'x',&pla);
  OptionAdd(&opt,"mgf",'x',&mgf);
  OptionAdd(&opt,"lep",'x',&lep);
  OptionAdd(&opt,"cpi",'x',&cpi);

  OptionAdd(&opt,"path",'t',&dpath_txt);

  OptionAdd(&opt,"ace",'x',&ace);
  OptionAdd(&opt,"wind",'x',&wind);
  OptionAdd(&opt,"imp8",'x',&imp);
  OptionAdd(&opt,"geotail",'x',&geo);

  OptionAdd(&opt,"cf",'t',&cfname);

  if (argc>1) { 
    arg=OptionProcess(1,argc,argv,&opt,rst_opterr);
    if (arg==-1) {
      exit(-1);
    }
    if (cfname !=NULL) { /* load the configuration file */
      do {
        fp=fopen(cfname,"r");
        if (fp==NULL) break;
        free(cfname);
        cfname=NULL;
        optf=OptionProcessFile(fp);
        if (optf !=NULL) {
          arg=OptionProcess(0,optf->argc,optf->argv,&opt,rst_opterr);
          if (arg==-1) {
            fclose(fp);
            OptionFreeFile(optf);
            exit(-1);
          }
          OptionFreeFile(optf);
	}   
        fclose(fp);
      } while (cfname !=NULL);
    }
  } else {
    OptionPrintInfo(stderr,errstr);
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

  if ((ace+wind+imp+geo)==0) ace=1;

  if ((ace+wind+imp+geo)>1) { 
    fprintf(stderr,"Only one satellite can be specified.\n");
    exit(-1);
  }

  if (dpath_txt !=NULL) strcpy(dpath,dpath_txt);

  if (stime_txt !=NULL) stime=strtime(stime_txt);
  if (etime_txt !=NULL) etime=strtime(etime_txt);
  if (extime_txt !=NULL) extime=strtime(extime_txt);
  if (sdate_txt !=NULL) sdate=strdate(sdate_txt);
  if (edate_txt !=NULL) edate=strdate(edate_txt);

  if (sdate==-1) sdate=TimeYMDHMSToEpoch(1997,1,1,0,0,0);
  if (stime==-1) stime=0;

  stime+=sdate;

  if (edate==-1) edate=sdate; 
  if (etime==-1) etime=0;  

  etime+=edate;
  if (extime !=-1) etime=stime+extime;


  if (etime<=stime) etime=stime+(24*3600); /* plot a day */
    
  TimeEpochToYMDHMS(stime,&yr,&mo,&dy,&hr,&mt,&sc);
  fprintf(stderr,"Start time:%d-%d-%d %d:%d:%d\n",yr,mo,dy,hr,mt,(int) sc);
  TimeEpochToYMDHMS(etime,&yr,&mo,&dy,&hr,&mt,&sc);
  fprintf(stderr,"End time:%d-%d-%d %d:%d:%d\n",yr,mo,dy,hr,mt,(int) sc);

    
  // TODO: this flag is not used any where
  //if ((etime-stime)>=(24*3600)) dflg=1;
 
 
  if (ace==1) load_ace();
  else if (wind==1)load_wind();
  else if (imp==1) load_imp();
  else if (geo==1) load_geo();
  
  pnum=pos.cnt;
  ptme=pos.time;

  if (plasma.cnt>pnum) {
    pnum=plasma.cnt;
    ptme=plasma.time;
  }

  if (imf.cnt>pnum) {
    pnum=imf.cnt;
    ptme=imf.time;
  }



  if (pnum==0) {
    fprintf(stderr,"Nothing to plot.\n");
    exit(-1);
  }

  if (hdr==1) {
     fprintf(stdout,"year mo dy hr mt %5s","sc");
     if (pos.cnt>0) {
       if (mode==0) fprintf(stdout,"%10s %10s %10s","PGSMx","PGSMy","PGSMz");
       else fprintf(stdout,"%10s %10s %10s","PGSEx","PGSEy","PGSEz");
     }
     if (imf.cnt>0) {
       if (mode==0) fprintf(stdout,"%10s %10s %10s","BGSMx","BGSMy","BGSMz");
       else fprintf(stdout,"%10s %10s %10s","BGSEx","BGSEy","BGSEz");
     }
     if (plasma.cnt>0) {
       if (mode==0) fprintf(stdout,"%10s %10s %10s %10s",
                            "P","VGSMx","VGSMy","VGSMz");
       else fprintf(stdout,"%10s %10s %10s %10s","P","VGSEx","VGSEy","VGSEz");
     }



     fprintf(stdout,"\n");

  }

  pos.idx=0;
  imf.idx=0;
  plasma.idx=0;

  for (i=0;i<pnum;i++) {
    TimeEpochToYMDHMS(ptme[i],&yr,&mo,&dy,&hr,&mt,&sc);
    fprintf(stdout,"%.4d %.2d %.2d %.2d %.2d %f ",yr,mo,dy,hr,mt,sc); 
    if (pos.cnt !=0) {
      int x;
      x=find_pos(&pos,ptme[i]);
      if (mode==0) fprintf(stdout,"%#12.4f %#12.4f %#12.4f",pos.PGSM[3*x],
                          pos.PGSM[3*x+1],pos.PGSM[3*x+2]);
      else fprintf(stdout,"%#10.4f %#10.4f %#10.4f",pos.PGSE[3*x],
                          pos.PGSE[3*x+1],pos.PGSE[3*x+2]);
    }


   if (imf.cnt !=0) {
      int x;
      x=find_imf(&imf,ptme[i]);
      if (mode==0) fprintf(stdout,"%#12.4f %#12.4f %#12.4f",imf.BGSMc[3*x],
                          imf.BGSMc[3*x+1],imf.BGSMc[3*x+2]);
      else fprintf(stdout,"%#10.4f %#10.4f %#10.4f",imf.BGSEc[3*x],
                          imf.BGSEc[3*x+1],imf.BGSEc[3*x+2]);
    }

   if (plasma.cnt !=0) {
      int x;
      x=find_plasma(&plasma,ptme[i]);
      if (mode==0) fprintf(stdout,"%#12.4f %#12.4f %#12.4f %#12.4f",
                           plasma.pre[x],
                           plasma.VGSM[3*x],plasma.VGSM[3*x+1],
                           plasma.VGSM[3*x+2]);
      else fprintf(stdout,"%#10.4f %#10.4f %#10.4f %#10.4f",plasma.pre[x],
                   plasma.VGSE[3*x],plasma.VGSE[3*x+1],plasma.VGSE[3*x+2]);
    }


    fprintf(stdout,"\n");
    
  }  	 	 
  return 0;
}
   


















