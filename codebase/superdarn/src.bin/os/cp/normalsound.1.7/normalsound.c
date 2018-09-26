/* normalscan.c
   ============
   Author: J.Spaleta
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
#include <sys/types.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <zlib.h>
#include "rtypes.h"
#include "option.h"
#include "rtime.h"
#include "dmap.h"
#include "limit.h"
#include "radar.h"
#include "rprm.h"
#include "iq.h"
#include "rawdata.h"
#include "fitblk.h"
#include "fitdata.h"
#include "fitacf.h"


#include "errlog.h"
#include "freq.h"
#include "tcpipmsg.h"

#include "rmsg.h"
#include "rmsgsnd.h"

#include "radarshell.h"

#include "build.h"
#include "global.h"
#include "reopen.h"
#include "setup.h"
#include "sync.h"

#include "site.h"
#include "sitebuild.h"
#include "siteglobal.h"



#define NUM_SND_DATA 360
#define SND_NRANG 75
#define SND_NBM 16
#define SND_NFBIN 26
#define MAX_SND_FREQS 12


struct sounder_struct
  {
  double stime;
  char program_name[40];
  int site_id;
  int beam_num;
  int freq;
  int noise;
  int frange;
  int rsep;
  float pwr[ SND_NRANG];
  float vel[ SND_NRANG];
  float width[ SND_NRANG];
  float  AOA[ SND_NRANG];
  int gsct[ SND_NRANG];
  int qflg[ SND_NRANG];
  };


void average_scatter_percentages( float iscat_percent[ MAX_SND_FREQS][ SND_NBM], int sft, float average_iscat_percent[ MAX_SND_FREQS], int fw );
int find_optimal_freq(int start_freq, int cutlass, int fw, int sounder_freqs[], int sft, struct sounder_struct *sounder_data, int act_snd_rec);
void write_sounding_record( char *progname, struct RadarParm *prm, struct FitData *fit, struct sounder_struct *sounder_data, int *act_snd_rec);
void write_sounding_record_new( char *progname, struct RadarParm *prm, struct FitData *fit, struct sounder_struct *sounder_data, int *act_snd_rec);

#define RT_TASK 2

char *ststr=NULL;
char *dfststr="tst";

void *tmpbuf;
size_t tmpsze;

char progid[80]={"normalsound"};
char progname[256];

int arg=0;
struct OptionData opt;

char *roshost=NULL;
char *droshost={"127.0.0.1"};

int baseport=44100;

struct TCPIPMsgHost errlog={"127.0.0.1",44100,-1};

struct TCPIPMsgHost shell={"127.0.0.1",44101,-1};

int tnum=3;      
struct TCPIPMsgHost task[3]={
  /*  {"127.0.0.1",0,-1}, iqwrite */
  {"127.0.0.1",2,-1}, /* rawacfwrite */
  {"127.0.0.1",3,-1}, /* fitacfwrite */
  {"127.0.0.1",4,-1}  /* rtserver */
};




int main(int argc,char *argv[]) {

  int ptab[8] = {0,14,22,24,27,31,42,43};

  int lags[LAG_SIZE][2] = {
    { 0, 0},		/*  0 */
    {42,43},		/*  1 */
    {22,24},		/*  2 */
    {24,27},		/*  3 */
    {27,31},		/*  4 */
    {22,27},		/*  5 */

    {24,31},		/*  7 */
    {14,22},		/*  8 */
    {22,31},		/*  9 */
    {14,24},		/* 10 */
    {31,42},		/* 11 */
    {31,43},		/* 12 */
    {14,27},		/* 13 */
    { 0,14},		/* 14 */
    {27,42},		/* 15 */
    {27,43},		/* 16 */
    {14,31},		/* 17 */
    {24,42},		/* 18 */
    {24,43},		/* 19 */
    {22,42},		/* 20 */
    {22,43},		/* 21 */
    { 0,22},		/* 22 */

    { 0,24},		/* 24 */

    {43,43}};		/* alternate lag-0  */

    char logtxt[1024];

  int exitpoll=0;
  int scannowait=0;
 
  int scnsc=120;
  int scnus=0;
  int skip;
  int cnt=0;

  unsigned char fast=0;
  unsigned char discretion=0;

  int status=0,n;


  /* Variables for sounding */
  char snd_filename[ 100];
  FILE *snd_dat;
  /* If the file $SD_HDWPATH/sounder.dat exists, the next three parameters are read from it */
  /* the file contains one integer value per line */
  int freq_dwell=15; /* after so many minutes a new optimal frequency is evaluated */
  int sounder_freqs_total=8;
  int sounder_freqs[ MAX_SND_FREQS]= { 11000, 12000, 13000, 14000, 15000, 16000, 17000, 18000, 0, 0, 0, 0 };
  time_t last_freq_search, t_now;
  int fw=0; /* frequency weighting flag used in selecting the optimal freq */
  int sounder_beams[]={0,2,4,6,8,10,12,14};
  int sounder_freq_count=0, sounder_beam_count=0;
  int sounder_beams_total=8, odd_beams=0;
  int sounder_freq;
  int sounder_beam_loop=1;
  int normal_intt=6;
  int fast_intt=3;
  int sounder_intt=2;
  int do_new_freq_search=0;
  float sounder_time, time_needed=1.25;
  int cutlass=0;
  struct sounder_struct *sounder_data;
  int act_snd_rec= 0;

  char *snd_dir;
  char data_path[100];
  time(&last_freq_search);
  snd_dir= getenv("SD_SND_PATH");
  if( snd_dir==NULL )
    sprintf( data_path,"/data/ros/snd/");
  else
    memcpy( data_path,snd_dir,strlen(snd_dir));
  sprintf( snd_filename,"%s/sounder.dat", data_path);
  fprintf(stderr,"Checking Sounder File: %s\n",snd_filename);
  snd_dat= fopen( snd_filename, "r");
  if( snd_dat != NULL ) {
    fscanf( snd_dat, "%d", &freq_dwell);
    fscanf( snd_dat, "%d", &sounder_freqs_total);
    if (sounder_freqs_total > 12) sounder_freqs_total= 12;
    for ( sounder_freq_count=0; sounder_freq_count < sounder_freqs_total; sounder_freq_count++ )
      fscanf( snd_dat, "%d", &sounder_freqs[ sounder_freq_count] );
    sounder_freq_count= 0;
    fclose( snd_dat);
    fprintf(stderr,"Sounder File: %s read\n",snd_filename);
  } else {
    fprintf(stderr,"Sounder File: %s not found\n",snd_filename);
  }
  sounder_data= ( struct sounder_struct *) calloc( sizeof( struct sounder_struct), NUM_SND_DATA);


  cp=155;
  intsc=normal_intt;
  intus=0;
  mppul=8;
  mplgs=23;
  mpinc=1500;
  dmpinc=1500;
  nrang=75;
  rsep=45;
  txpl=300;

  /* ========= PROCESS COMMAND LINE ARGUMENTS ============= */

  OptionAdd(&opt,"di",'x',&discretion);

  OptionAdd(&opt,"frang",'i',&frang);
  OptionAdd(&opt,"rsep",'i',&rsep);

  OptionAdd( &opt, "dt", 'i', &day);
  OptionAdd( &opt, "nt", 'i', &night);
  OptionAdd( &opt, "df", 'i', &dfrq);
  OptionAdd( &opt, "nf", 'i', &nfrq);
  OptionAdd( &opt, "xcf", 'i', &xcnt);




  OptionAdd(&opt,"ep",'i',&errlog.port);
  OptionAdd(&opt,"sp",'i',&shell.port); 

  OptionAdd(&opt,"bp",'i',&baseport); 

  OptionAdd(&opt,"ros",'t',&roshost);


  OptionAdd(&opt,"stid",'t',&ststr); 
 
  OptionAdd(&opt,"fast",'x',&fast);

  OptionAdd( &opt, "nowait", 'x', &scannowait);
  OptionAdd(&opt,"sb",'i',&sbm);
  OptionAdd(&opt,"eb",'i',&ebm);
   
  arg=OptionProcess(1,argc,argv,&opt,NULL);  
 
  if (ststr==NULL) ststr=dfststr;

  if (roshost==NULL) roshost=getenv("ROSHOST");
  if (roshost==NULL) roshost=droshost;


  if ((errlog.sock=TCPIPMsgOpen(errlog.host,errlog.port))==-1) {    
    fprintf(stderr,"Error connecting to error log.\n");
  }

  if ((shell.sock=TCPIPMsgOpen(shell.host,shell.port))==-1) {    
    fprintf(stderr,"Error connecting to shell.\n");
  }

  for (n=0;n<tnum;n++) task[n].port+=baseport;

  OpsStart(ststr);

  status=SiteBuild(ststr,NULL); /* second argument is version string */

  if (status==-1) {
    fprintf(stderr,"Could not identify station.\n");
    exit(1);
  }

  SiteStart(roshost);

  strncpy(combf,progid,80);   
 
  OpsSetupCommand(argc,argv);
  OpsSetupShell();
   
  RadarShellParse(&rstable,"sbm l ebm l dfrq l nfrq l dfrang l nfrang l dmpinc l nmpinc l frqrng l xcnt l",                        
                  &sbm,&ebm,                              
                  &dfrq,&nfrq,                  
                  &dfrang,&nfrang,                            
                  &dmpinc,&nmpinc,                            
                  &frqrng,&xcnt);      
  
 
  status=SiteSetupRadar();

  fprintf(stderr,"Status:%d\n",status);
  
  if (status !=0) {
    ErrLog(errlog.sock,progname,"Error locating hardware.");
    exit (1);
  }


  if (fast) {
     cp=157;
     scnsc=60;
     scnus=0;
     intsc=fast_intt;
     intus=0;
  }

  if (discretion) cp= -cp;

  txpl=(rsep*20)/3;

  if (fast) sprintf(progname,"normalsound (fast)");
  else sprintf(progname,"normalsound");



  OpsLogStart(errlog.sock,progname,argc,argv);  

  OpsSetupTask(tnum,task,errlog.sock,progname);

  for (n=0;n<tnum;n++) {
    RMsgSndReset(task[n].sock);
    RMsgSndOpen(task[n].sock,strlen( (char *) command),command);     
  }

  
  OpsFitACFStart();

  tsgid=SiteTimeSeq(ptab);

  do {

    if (SiteStartScan() !=0) continue;
    
    if (OpsReOpen(2,0,0) !=0) {
      ErrLog(errlog.sock,progname,"Opening new files.");
      for (n=0;n<tnum;n++) {
        RMsgSndClose(task[n].sock);
        RMsgSndOpen(task[n].sock,strlen( (char *) command),command);     
      }
    }

    scan=1;
    
    ErrLog(errlog.sock,progname,"Starting scan.");
   
    if (xcnt>0) {
      cnt++;
      if (cnt==xcnt) {
        xcf=1;
        cnt=0;
      } else xcf=0;
    } else xcf=0;

    skip=OpsFindSkip(scnsc,scnus);
    
    if (backward) {
      bmnum=sbm-skip;
      if (bmnum<ebm) bmnum=sbm;
    } else {
      bmnum=sbm+skip;
      if (bmnum>ebm) bmnum=sbm;
    }

    do {

      TimeReadClock(&yr,&mo,&dy,&hr,&mt,&sc,&us);
      
      if (OpsDayNight()==1) {
        stfrq=dfrq;
        mpinc=dmpinc;
        frang=dfrang;
      } else {
        stfrq=nfrq;
        mpinc=nmpinc;
        frang=nfrang;
      }        

      sprintf(logtxt,"Integrating beam:%d intt:%ds.%dus (%d:%d:%d:%d)",bmnum,
                      intsc,intus,hr,mt,sc,us);
      ErrLog(errlog.sock,progname,logtxt);

      ErrLog(errlog.sock,progname,"Starting Integration.");
            
      SiteStartIntt(intsc,intus);

      ErrLog(errlog.sock,progname,"Doing clear frequency search."); 
   
      sprintf(logtxt, "FRQ: %d %d", stfrq, frqrng);
      ErrLog(errlog.sock,progname, logtxt);

            
      tfreq=SiteFCLR(stfrq,stfrq+frqrng);
      
      sprintf(logtxt,"Transmitting on: %d (Noise=%g)",tfreq,noise);
      ErrLog(errlog.sock,progname,logtxt);

    
      nave=SiteIntegrate(lags);   
      if (nave<0) {
        sprintf(logtxt,"Integration error:%d",nave);
        ErrLog(errlog.sock,progname,logtxt); 
        continue;
      }
      sprintf(logtxt,"Number of sequences: %d",nave);
      ErrLog(errlog.sock,progname,logtxt);

      OpsBuildPrm(prm,ptab,lags);
      
      OpsBuildIQ(iq,&badtr);
            
      OpsBuildRaw(raw);
   
      FitACF(prm,raw,fblk,fit);
      
      msg.num=0;
      msg.tsize=0;

      tmpbuf=RadarParmFlatten(prm,&tmpsze);
      RMsgSndAdd(&msg,tmpsze,tmpbuf,
		PRM_TYPE,0); 

      tmpbuf=IQFlatten(iq,prm->nave,&tmpsze);
      RMsgSndAdd(&msg,tmpsze,tmpbuf,IQ_TYPE,0);

      RMsgSndAdd(&msg,sizeof(unsigned int)*2*iq->tbadtr,
                 (unsigned char *) badtr,BADTR_TYPE,0);
		 
      RMsgSndAdd(&msg,strlen(sharedmemory)+1,(unsigned char *) sharedmemory,
		 IQS_TYPE,0);

      tmpbuf=RawFlatten(raw,prm->nrang,prm->mplgs,&tmpsze);
      RMsgSndAdd(&msg,tmpsze,tmpbuf,RAW_TYPE,0); 
 
      tmpbuf=FitFlatten(fit,prm->nrang,&tmpsze);
      RMsgSndAdd(&msg,tmpsze,tmpbuf,FIT_TYPE,0); 

        
      RMsgSndAdd(&msg,strlen(progname)+1,(unsigned char *) progname,
		NME_TYPE,0);   
     

     
      for (n=0;n<tnum;n++) RMsgSndSend(task[n].sock,&msg); 

      for (n=0;n<msg.num;n++) {
        if (msg.data[n].type==PRM_TYPE) free(msg.ptr[n]);
        if (msg.data[n].type==IQ_TYPE) free(msg.ptr[n]);
        if (msg.data[n].type==RAW_TYPE) free(msg.ptr[n]);
        if (msg.data[n].type==FIT_TYPE) free(msg.ptr[n]); 
      }          

      RadarShell(shell.sock,&rstable);

      if (exitpoll !=0) break;
      scan=0;
      if (bmnum==ebm) break;
      if (backward) bmnum--;
      else bmnum++;

    } while (1);

    ErrLog(errlog.sock,progname,"Waiting for scan boundary."); 
    /*if ((exitpoll==0) && (scannowait==0)) SiteEndScan(scnsc,scnus);*/


   if (exitpoll==0) {
      /* In here comes the sounder code */
      /* see if it's time for a new freq search */
      time(&t_now);
      do_new_freq_search= ( freq_dwell>0 && freq_dwell<=((t_now-last_freq_search)/60.) );
      printf("SND Code 0: %d %d %lf\n",do_new_freq_search,freq_dwell,(double)(t_now-last_freq_search)/60.);
      /* set the "sounder mode" scan variable */
      scan=-2;
      /* set the xcf variable to do cross-correlations (AOA) */
      xcf=1;
      /* we have time until the end of the minute to do sounding */
      /* minus a safety factor given in time_needed */
      TimeReadClock(&yr,&mo,&dy,&hr,&mt,&sc,&us);
      sounder_time= 60.0 - ( sc + us/ 1000000.0);
      /* we do not implement to no sounding mode here */
      /* do a new frequency search if it's time */
      if( do_new_freq_search && sounder_time>=5 ) {
            do_new_freq_search=0;
            stfrq= find_optimal_freq( stfrq, cutlass, fw, sounder_freqs, sounder_freqs_total, sounder_data, act_snd_rec);
            sprintf( logtxt,"New Opt Freq; %d\n", stfrq);
            ErrLog( errlog.sock, progname, logtxt);
              last_freq_search= t_now;
      }
      sounder_beam_loop= ( sounder_time-(float)sounder_intt > time_needed );
      while( sounder_beam_loop ) {
            intsc= sounder_intt;
            /* set the beam */
            bmnum=sounder_beams[sounder_beam_count]+odd_beams;
            /* sounder_freq will be an array of frequencies to step through */
            if( !cutlass )
              sounder_freq=sounder_freqs[sounder_freq_count];
/*
 *         else {
 *           sounder_freq=lsfreq[sounder_freqs[sounder_freq_count]];
 *           frqrng=lfreq_range[sounder_freqs[sounder_freq_count]];
 *         }
 */


          sprintf(logtxt,"Integrating SND beam:%d intt:%ds.%dus (%d:%d:%d:%d)", bmnum, intsc,intus,hr,mt,sc,us);
          ErrLog(errlog.sock,progname,logtxt);
          ErrLog(errlog.sock,progname,"Setting SND beam.");
          SiteStartIntt(intsc,intus);
          /*SiteSetIntt(intsc,intus); */
          /*SiteSetBeam(bmnum); */
          ErrLog( errlog.sock, progname, "Doing SND clear frequency search.");
          sprintf(logtxt, "FRQ: %d %d", sounder_freq, frqrng);
          ErrLog(errlog.sock,progname, logtxt);
          tfreq=SiteFCLR( sounder_freq, sounder_freq + frqrng);
          /*SiteSetFreq(tfreq);*/
/*
 *           sprintf(logtxt,"Transmitting SND on: %d (Noise=%g)",tfreq,noise);
 *                     ErrLog( errlog.sock, progname, logtxt);
 *                     */
          tsgid= SiteTimeSeq(ptab);
          nave= SiteIntegrate( lags);
          if (nave < 0) {
            sprintf( logtxt, "SND integration error: %d", nave);
            ErrLog(errlog.sock,progname, logtxt);
            continue;
          }
          sprintf(logtxt,"Number of SND sequences: %d",nave);
          ErrLog(errlog.sock,progname,logtxt);

          OpsBuildPrm(prm,ptab,lags);
          OpsBuildIQ(iq,&badtr);
          OpsBuildRaw(raw);
          FitACF(prm,raw,fblk,fit);

          ErrLog( errlog.sock, progname, "Sending SND messages.");
          msg.num= 0;
          msg.tsize= 0;

          tmpbuf=RadarParmFlatten(prm,&tmpsze);
          RMsgSndAdd(&msg,tmpsze,tmpbuf,
		PRM_TYPE,0); 

          tmpbuf=IQFlatten(iq,prm->nave,&tmpsze);
          RMsgSndAdd(&msg,tmpsze,tmpbuf,IQ_TYPE,0);

          RMsgSndAdd(&msg,sizeof(unsigned int)*2*iq->tbadtr,
                 (unsigned char *) badtr,BADTR_TYPE,0);
		 
          RMsgSndAdd(&msg,strlen(sharedmemory)+1,
                 (unsigned char *) sharedmemory,
		 IQS_TYPE,0);

          tmpbuf=RawFlatten(raw,prm->nrang,prm->mplgs,&tmpsze);
          RMsgSndAdd(&msg,tmpsze,tmpbuf,RAW_TYPE,0); 
 
          tmpbuf=FitFlatten(fit,prm->nrang,&tmpsze);
          RMsgSndAdd(&msg,tmpsze,tmpbuf,FIT_TYPE,0); 

        
          RMsgSndAdd(&msg,strlen(progname)+1,(unsigned char *) progname,
		NME_TYPE,0);   
     
          RMsgSndSend(task[RT_TASK].sock,&msg); 
            for (n=0;n<msg.num;n++) {
            if (msg.data[n].type==PRM_TYPE) free(msg.ptr[n]);
            if (msg.data[n].type==IQ_TYPE) free(msg.ptr[n]);
            if (msg.data[n].type==RAW_TYPE) free(msg.ptr[n]);
            if (msg.data[n].type==FIT_TYPE) free(msg.ptr[n]); 
          }          

          sprintf( logtxt, "SBC: %d  SFC: %d\n", sounder_beam_count, sounder_freq_count);
          ErrLog( errlog.sock, progname, logtxt);
          /* save the sounding mode data */
          write_sounding_record_new( progname,prm,fit, sounder_data, &act_snd_rec);

          ErrLog( errlog.sock, progname, "Polling SND for exit.");

          if (exitpoll !=0) break;
          /* check for the end of a beam loop */
          sounder_freq_count++;
          if( sounder_freq_count >= sounder_freqs_total ) {
            /* reset the freq counter and increment the beam counter */
            sounder_freq_count=0;
            sounder_beam_count++;
            if( sounder_beam_count>=sounder_beams_total ) {
              sounder_beam_count=0;
              if( odd_beams==0 )
                odd_beams=1;
              else
                odd_beams=0;
              sounder_freq_count=0;
            }
          }
          /* see if we have enough time for another go round */
          TimeReadClock( &yr, &mo, &dy, &hr, &mt, &sc, &us);
          sounder_time= 60.0 - ( sc + us/ 1000000.0);
          sounder_beam_loop= ( sounder_time-(float)sounder_intt > time_needed );
      }
      /* now wait for the next normal_scan */
      intsc=normal_intt;
      if ( fast) intsc= fast_intt;
      if (scannowait==0) SiteEndScan(scnsc,scnus);
   }
  } while (exitpoll==0);
  
  
  for (n=0;n<tnum;n++) RMsgSndClose(task[n].sock);
  

  ErrLog(errlog.sock,progname,"Ending program.");


  SiteExit(0);

  return 0;   
} 
 

/********************** function write_sounding_record() ************************/
void write_sounding_record( char *progname, struct RadarParm *prm, struct FitData *fit, struct sounder_struct *sounder_data, int *act_snd_rec)
{
  int i;

  struct header_struct
        {
        double stime;
        unsigned char site_id;
        unsigned char beam_no;
        unsigned short freq;
        unsigned short noise;
        unsigned short frange;
        unsigned short rsep;
        unsigned char gsct[10];
        unsigned char qflg[10];
        char program_name[40];
        short unused1;
        short unused2;
        short unused3;
        } header;

  struct data_struct
        {
        short vel;
        unsigned short width;
        unsigned short AOA;
        short unused1;
        short unused2;
        unsigned char unused3;
        unsigned char pwr;
        } data;
  char data_path[100], data_filename[50], filename[80];

  int byte, good_ranges[75];

  double min_vel=-3000, max_vel=3000;
  double max_width=1000;
  double min_power=0, max_power=50;
  double min_AOA=0, max_AOA=90.;

  char *snd_dir;
  FILE *out;

  struct sounder_struct *act_snd_data;


  /* set up the data directory */
  /* get the snd data dir */
  snd_dir= getenv("SD_SND_PATH");
  if( snd_dir==NULL )
    sprintf( data_path,"/data/ros/snd/");
  else
    memcpy( data_path,snd_dir,strlen(snd_dir));
  /* make up the filename */
  /* YYYYMMDDHH */
  sprintf( data_filename, "%04d%02d%02d%02d%s", prm->time.yr, prm->time.mo, prm->time.dy, (prm->time.hr/ 2)* 2, ststr);
  /* finally make the filename */
  sprintf( filename, "%s/%s.snd", data_path, data_filename);
  fprintf(stderr,"Sound Data File: %s\n",filename);

  /* open the output file */
  out= fopen(filename,"a");
  if( out==NULL ) {
        /* crap. might as well go home */
        return;
  }

  /* make the header */
  /* initialize the unused values */
  header.unused1= prm->atten;
  header.unused2= 0x0FFE;
  header.unused3= 0x0FFE;
  data.unused1= 0x0FFE;
  data.unused2= 0x0FFE;
  data.unused3= 0xFE;

  header.stime= TimeYMDHMSToEpoch( prm->time.yr, prm->time.mo, prm->time.dy, prm->time.hr, prm->time.mt, prm->time.sc);
  header.site_id= prm->stid;
  header.beam_no= prm->bmnum;
  header.freq= prm->tfreq;
  header.noise= prm->noise.mean;
  header.frange= prm->frang;
  header.rsep= prm->rsep;
  memcpy( header.program_name, progname, sizeof( header.program_name));
 /* zero out the gscat and qual bytes */
  for( i=0; i<10; i++ ) {
    header.gsct[i]= 0;
    header.qflg[i]= 0;
  }

  /* now fill them in */
  byte= 0;
  for( i=0; i< SND_NRANG; i++ ) {
    byte=i/8;
    if( fit->rng[i].gsct==1 ) header.gsct[byte]+=(0x01<<(i%8));
    if( fit->rng[i].qflg==1  ) {
      header.qflg[byte]+= (0x01<<(i%8));
      good_ranges[i]=1;
    } else {
      good_ranges[i]=0;
    }
  }
  /* write out the header */
  fwrite( &header, sizeof( header), 1, out);

  /* scale the fit data into the char/shorts */
  for( i=0; i< SND_NRANG; i++ ) {
    /* only do the good ranges */
    if( good_ranges[i] ) {
      /* do the power */
      if( fit->rng[i].p_l < min_power ) data.pwr= 0;
      else if ( fit->rng[i].p_l > max_power ) data.pwr= 255;
      else data.pwr= 255* fit->rng[i].p_l/ (max_power - min_power);
      /* do the AOA */
      if( fit->elv[i].normal < 0 ) data.AOA= 0;
      else if( fit->elv[i].normal > 90.0 ) data.AOA= 65535;
      else data.AOA= 65535* fit->elv[i].normal/ (max_AOA-min_AOA);
      /* do the velocity */
      if( fit->rng[i].v < 0 ) {
        if( fit->rng[i].v < min_vel ) data.vel=-32768;
        else data.vel= -(32767/ min_vel)* fit->rng[i].v-1;
      } else {
        if( fit->rng[i].v > max_vel ) data.vel=32767;
        else data.vel=(32767/ max_vel)* fit->rng[i].v;
      }
      /* do the width */
      if( fit->rng[i].w_l > max_width ) data.width= 65535;
      else data.width= ( 65535/ max_width)* fit->rng[i].w_l;
      /* write out the data structure */
      fwrite( &data, sizeof( data), 1, out);
    }
  }
  fclose(out);


  /* Fill the next sounder data record */
  act_snd_data= sounder_data + *act_snd_rec;
  act_snd_data->stime= TimeYMDHMSToEpoch( prm->time.yr, prm->time.mo, prm->time.dy, prm->time.hr, prm->time.mt, prm->time.sc);
  memcpy( act_snd_data->program_name, progname, sizeof(act_snd_data->program_name));
  act_snd_data->site_id= prm->stid;
  act_snd_data->beam_num= prm->bmnum;
  act_snd_data->freq= prm->tfreq;
  act_snd_data->noise= prm->noise.mean;
  act_snd_data->frange= prm->frang;
  act_snd_data->rsep= prm->rsep;
  for( i=0; i< SND_NRANG; i++ ) {
    act_snd_data->pwr[ i]= fit->rng[ i].p_l;
    act_snd_data->vel[ i]= fit->rng[ i].v;
    act_snd_data->width[ i]= fit->rng[ i].w_l;
    act_snd_data->AOA[ i]= fit->elv[ i].normal;
    act_snd_data->gsct[ i]= fit->rng[ i].gsct;
    act_snd_data->qflg[ i]= fit->rng[ i].qflg;
  }
  *act_snd_rec= *act_snd_rec + 1;
  if ( *act_snd_rec >= NUM_SND_DATA) *act_snd_rec= 0;
}

/********************** function write_sounding_record_new() ************************/
/* changed the data structure */

void write_sounding_record_new( char *progname, struct RadarParm *prm, struct FitData *fit, struct sounder_struct *sounder_data, int *act_snd_rec)
{
  int i;

  struct header_struct
        {
        long int stime;
        short int  site_id;
        short int beam_no;
        short int freq;
        short int noise;
        short int frange;
        short int rsep;
        short int gsct[ SND_NRANG];
        short int qflg[ SND_NRANG];
        char program_name[40];
        } header;

  struct data_struct
        {
        short int pwr;
        short int vel;
        short int width;
        short int AOA;
        } data;

  char data_path[100], data_filename[50], filename[80];


  int  good_ranges[ SND_NRANG];

  char *snd_dir;
  FILE *out;

  struct sounder_struct *act_snd_data;


  /* set up the data directory */
  /* get the snd data dir */
  snd_dir= getenv("SD_SND_PATH");
  if( snd_dir==NULL )
    sprintf( data_path,"/data/ros/snd/");
  else {
    memcpy( data_path,snd_dir,strlen(snd_dir));
    data_path[ strlen( snd_dir)]= '/';
    data_path[ strlen( snd_dir) + 1]= 0;
  }

  /* make up the filename */
  /* YYYYMMDDHH */
  sprintf( data_filename, "%04d%02d%02d%02d%s", prm->time.yr, prm->time.mo, prm->time.dy, (prm->time.hr/ 2)* 2, ststr);
  /* finally make the filename */
  sprintf( filename, "%s%s.snd", data_path, data_filename);
  /* open the output file */
  fprintf(stderr,"Sound Data File: %s\n",filename);
  out= fopen(filename,"a");
  if( out==NULL ) {
        /* crap. might as well go home */
        return;
  }
 /* make the header */

  header.stime= TimeYMDHMSToEpoch( prm->time.yr, prm->time.mo, prm->time.dy, prm->time.hr, prm->time.mt, prm->time.sc);
  header.site_id= prm->stid;
  header.beam_no= prm->bmnum;
  header.freq= prm->tfreq;
  header.noise= prm->noise.mean;
  header.frange= prm->frang;
  header.rsep= prm->rsep;
  memcpy( header.program_name, progname, sizeof( header.program_name));
 /* zero out the gscat and qual bytes */
  for( i=0; i< SND_NRANG; i++ ) {
    header.gsct[i]= fit->rng[i].gsct;
    header.qflg[i]= fit->rng[i].qflg;
    good_ranges[ i]= ( fit->rng[i].qflg == 1);
  }

  /* write out the header */
  fwrite( &header, sizeof( header), 1, out);

  /* scale the fit data into the char/shorts */
  for( i=0; i< SND_NRANG; i++ ) {
    /* only do the good ranges */
    if( good_ranges[i] ) {
      /* do the power */
      data.pwr= fit->rng[i].p_l;
      /* do the velocity */
      data.vel= fit->rng[i].v;
      /* do the AOA */
      data.AOA= fit->elv[i].normal;
      /* do the width */
      data.width= fit->rng[i].w_l;
      /* write out the data structure */
      fwrite( &data, sizeof( data), 1, out);
    }
  }
  fclose(out);


  /* Fill the next sounder data record */
  act_snd_data= sounder_data + *act_snd_rec;
  act_snd_data->stime= TimeYMDHMSToEpoch( prm->time.yr, prm->time.mo, prm->time.dy, prm->time.hr, prm->time.mt, prm->time.sc);
  memcpy( act_snd_data->program_name, progname, sizeof(act_snd_data->program_name));
  act_snd_data->site_id= prm->stid;
  act_snd_data->beam_num= prm->bmnum;
  act_snd_data->freq= prm->tfreq;
  act_snd_data->noise= prm->noise.mean;
  act_snd_data->frange= prm->frang;
  act_snd_data->rsep= prm->rsep;
  for( i=0; i< SND_NRANG; i++ ) {
    act_snd_data->pwr[ i]= fit->rng[ i].p_l;
    act_snd_data->vel[ i]= fit->rng[ i].v;
    act_snd_data->width[ i]= fit->rng[ i].w_l;
    act_snd_data->AOA[ i]= fit->elv[ i].normal;
    act_snd_data->gsct[ i]= fit->rng[ i].gsct;
    act_snd_data->qflg[ i]= fit->rng[ i].qflg;
  }
  *act_snd_rec= *act_snd_rec + 1;
  if ( *act_snd_rec >= NUM_SND_DATA) *act_snd_rec= 0;
}
/****************** function compute_scatter_percentage *************************/
/* Mod: 20050315                                                                                                                                */
/* Use ths last ~15min sounder data to calculate the ionospheric scatter percentage */
/* for each sounder frequency and in each beam                                                                  */
/* a lot more could be done, but for now we will keep it simple                                 */

void compute_scatter_percentage( struct sounder_struct *sounder_data, int act_snd_rec, int sounder_freqs[], int sft, float iscat_percent[ MAX_SND_FREQS][ SND_NBM] )
{
  struct sounder_struct *act_snd_data;
  int isnd, jsnd, ifrq, i;
  int returns=0, gscat_returns=0;
  for ( jsnd=0; jsnd< NUM_SND_DATA/ 4; jsnd++) {
    isnd= act_snd_rec - jsnd;
    if (isnd < 0) isnd= isnd + NUM_SND_DATA;
    act_snd_data= sounder_data + isnd;
    /* make sure this record has data */
    if (act_snd_data->stime > 0 ) {
      returns=0;
      gscat_returns=0;
      for( i=0; i< SND_NRANG; i++ ) {
        if( ( act_snd_data->qflg[i] == 1) && act_snd_data->pwr[i] >= 3.0 && act_snd_data->width[i] < 500.0 ) {
          returns++;
          if( act_snd_data->gsct[i] ) gscat_returns++;
        }
      }
     for ( ifrq=0; ifrq< sft; ifrq++) {
       if ( (act_snd_data->freq >= sounder_freqs[ ifrq]) && (act_snd_data->freq <= sounder_freqs[ ifrq] + frqrng) )
         break;
     }
     iscat_percent[ ifrq][act_snd_data->beam_num]= 100.0* (float)(returns - gscat_returns)/ SND_NRANG;
    }
  }
}
/****************** function average_scatter_percentage *************************/
/* what is fw good for ?? */

void average_scatter_percentages( float iscat_percent[ MAX_SND_FREQS][ SND_NBM], int sft, float average_iscat_percent[ MAX_SND_FREQS], int fw )
{
int ifrq, ibm;

  /* average the scatter percentages over all beams */
  for( ifrq=0; ifrq < sft; ifrq++ ) {
    average_iscat_percent[ifrq]= 0.0;
    for( ibm=0; ibm < SND_NBM; ibm++ )
      average_iscat_percent[ ifrq]+= iscat_percent[ ifrq][ ibm];
    average_iscat_percent[ ifrq]/= 16.0;
    if( fw ) average_iscat_percent[ ifrq]*= ifrq;
  }
}
/******************* function find_optimal_freq_local() ************************/
/* our frequency optimization scheme is this:                          */
/*                                                                     */
/* at each frequency, find the most recent set of range data for each  */
/* beam. filter this data and then find the percentage of data points  */
/* that were ionospheric backscatter. to get the optimal frequency,    */
/* average over all beams at each frequency and look for the freq      */
/* with the highest percentage of ionsopheric backscatter.             */

int find_optimal_freq(int start_freq, int cutlass, int fw, int sounder_freqs[], int sft, struct sounder_struct *sounder_data, int act_snd_rec)
{
  int i,j;
  int def_freq, def_freq_bin=0;
  /* array of iono scatter percentages (freq and beam number) */
  float iscat_percent[ MAX_SND_FREQS][ SND_NBM];
  float average_iscat_percent[ MAX_SND_FREQS];
  float max_scatter=-10;
  int max_freq_bin=0;
  int dlf;
  FILE *out;

  /* set the optimal freq to the sounder frequency closest to start_freq in case we don't find a good one; 20060308 DAndre */
  if( !cutlass ) {
    dlf= 100000;
    for ( i= 0; i < sft; i++) {
      if ( fabs( start_freq - sounder_freqs[ i]) < dlf) {
        dlf= fabs( start_freq - sounder_freqs[ i]);
        def_freq_bin= i;
      }
    }
  }
/*
 *   else
 *       def_freq_bin=lsfreq[def_freq]/1000;
 *       */
  /* initialize the arrays */
  for( i=0; i<MAX_SND_FREQS; i++ )
    for( j=0; j<SND_NBM; j++ ) {
      iscat_percent[i][j]=0;
    }

  compute_scatter_percentage( sounder_data, act_snd_rec, sounder_freqs, sft, iscat_percent);
  average_scatter_percentages( iscat_percent, sft, average_iscat_percent, fw );
  max_scatter=-10;
  for( i=0; i< sft; i++ )
    if( average_iscat_percent[i] >= max_scatter ) {
      max_scatter= average_iscat_percent[ i];
      max_freq_bin=i;
    }

  /* set a threshold on the improvement */
  if( max_scatter < 1.15* average_iscat_percent[def_freq_bin] || ( max_scatter - average_iscat_percent[def_freq_bin]) < 0.75 )
    max_freq_bin=def_freq_bin;

  def_freq= sounder_freqs[ max_freq_bin];

  out=fopen("/tmp/freq_search.out","w");
  if( out != NULL ) {
    for( i= 0; i < sft; i++ ) {
      if( !fw )
        fprintf(out,"\n%2d %d %10.7lf", i, sounder_freqs[ i], average_iscat_percent[i]);
      else
        fprintf(out,"\n%2d %d %10.7lf", i, sounder_freqs[ i], average_iscat_percent[i]/ i);
    }
    fprintf( out, "\nCutlass: %d",cutlass);
    fprintf( out, "\nFreq Weighting: %d",fw);
    fprintf( out, "\nReturned Frequency: %d kHz",def_freq);
    fprintf( out, "\n");
    fclose(out);
  }

  return(def_freq);
}
 

