/* site.c
   ====== 
   Author R.J.Barnes
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
#include <signal.h>
#include <sys/types.h>
#include <sys/time.h>
#include <errno.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <math.h>
#include "rtypes.h"
#include "limit.h"
#include "tsg.h"
#include "maketsg.h"
#include "acf.h"
#include "acfex.h"
#include "tcpipmsg.h"
#include "rosmsg.h"
#include "shmem.h"
#include "global.h"
#include "site.h"
#include "siteglobal.h"

#define REAL_BUF_OFFSET 0
#define IMAG_BUF_OFFSET 1
#define USEC 1000000.0

void SiteCvwExit(int signum) {

  struct ROSMsg msg;
  switch(signum) {
    case 2:
      cancel_count++;
      *exit_flag=signum;
      if (cancel_count < 3 )
        break;
    case 0:
      if(*exit_flag!=0) {
        msg.type=QUIT;
        TCPIPMsgSend(sock, &msg, sizeof(struct ROSMsg));
        TCPIPMsgRecv(sock, &msg, sizeof(struct ROSMsg));
        if (debug) {
          fprintf(stderr,"QUIT:type=%c\n",msg.type);
          fprintf(stderr,"QUIT:status=%d\n",msg.status);
        }
        close(sock);
        if (samples !=NULL)
          ShMemFree((unsigned char *) samples,sharedmemory,IQBUFSIZE,1,shmemfd);
        if(exit_flag!=NULL)
          free(exit_flag);
        exit(errno);
      } 
      break;
    default:
      if(*exit_flag==0) {
        *exit_flag=signum;
      }
      break;
  }
}



int SiteCvwStart(char *host) {

  signal(SIGPIPE,SiteCvwExit);
  signal(SIGINT,SiteCvwExit);
  signal(SIGUSR1,SiteCvwExit);

  exit_flag=malloc(sizeof(int));
  *exit_flag=0;
  cancel_count=0;

  sock=0;

  fprintf(stderr,"ROS server:%s\n",host);  
  strcpy(server,host);

  port=45000;
/* Radar number to register */
  rnum=2;
/* Channel number to register */
  cnum=1;
/* Beam Scan Direction settings */
  backward=1;
  sbm=19;
  ebm=4;
/* rxchn number of channels typically 1*/
/* rngoff argument in ACFCalculate.. is 2*rxchn and is normally set to 2 */
  rxchn=1;
 
  return 0;
}


int SiteCvwSetupRadar() {

  int32 temp32;
  struct ROSMsg smsg,rmsg;
  struct SiteSettings settings;
  if ((sock=TCPIPMsgOpen(server,port)) == -1) {
    return -1;
  }
  smsg.type=SET_RADAR_CHAN;
  TCPIPMsgSend(sock, &smsg,sizeof(struct ROSMsg)); 
  temp32=rnum;
  TCPIPMsgSend(sock, &temp32, sizeof(int32)); 
  temp32=cnum;
  TCPIPMsgSend(sock, &temp32, sizeof(int32));
  TCPIPMsgRecv(sock, &rmsg, sizeof(struct ROSMsg)); 
  if (rmsg.status < 0) {
    fprintf(stderr,"Requested radar channel unavailable\nSleeping 1 second and exiting\n");
    fprintf(stderr,"Sleeping 1 second and exiting\n");
    sleep(1);
    exit(0); 
  } 
  if (debug) {
    fprintf(stderr,"SET_RADAR_CHAN:type=%c\n",rmsg.type);
    fprintf(stderr,"SET_RADAR_CHAN:status=%d\n",rmsg.status);
  }
  smsg.type=GET_SITE_SETTINGS;
  TCPIPMsgSend(sock, &smsg, sizeof(struct ROSMsg));
  TCPIPMsgRecv(sock, &settings, sizeof(struct SiteSettings));
  TCPIPMsgRecv(sock, &rmsg, sizeof(struct ROSMsg));
  if (debug) {
    fprintf(stderr,"GET_SITE_SETTINGS:type=%c\n",rmsg.type);
    fprintf(stderr,"GET_SITE_SETTINGS:status=%d\n",rmsg.status);
  }
  ifmode=settings.ifmode;

  smsg.type=GET_PARAMETERS;
  TCPIPMsgSend(sock, &smsg, sizeof(struct ROSMsg));
  TCPIPMsgRecv(sock, &rprm, sizeof(struct ControlPRM));
  TCPIPMsgRecv(sock, &rmsg, sizeof(struct ROSMsg));
  if (debug) {
    fprintf(stderr,"GET_PARAMETERS:type=%c\n",rmsg.type);
    fprintf(stderr,"GET_PARAMETERS:status=%d\n",rmsg.status);
  }

  sprintf(sharedmemory,"IQBuff_Cvw_%d_%d",rnum,cnum);


  samples=(int16 *)
    ShMemAlloc(sharedmemory,IQBUFSIZE,O_RDWR | O_CREAT,1,&shmemfd);

  return 0;
}

 
int SiteCvwStartScan() {
  return 0;
}



int SiteCvwStartIntt(int sec,int usec) {

  struct ROSMsg smsg,rmsg;
  int total_samples=0;
  
  SiteCvwExit(0);
  total_samples=tsgprm.samples+tsgprm.smdelay;
  smsg.type=PING; 
  TCPIPMsgSend(sock, &smsg, sizeof(struct ROSMsg));
  TCPIPMsgRecv(sock, &rmsg, sizeof(struct ROSMsg));
  if (debug) {
    fprintf(stderr,"PING:type=%c\n",rmsg.type);
    fprintf(stderr,"PING:status=%d\n",rmsg.status);
  }

  smsg.type=GET_PARAMETERS;  
  TCPIPMsgSend(sock, &smsg, sizeof(struct ROSMsg));
  TCPIPMsgRecv(sock, &rprm, sizeof(struct ControlPRM));
  TCPIPMsgRecv(sock, &rmsg, sizeof(struct ROSMsg));
  if (debug) {
    fprintf(stderr,"GET_PARAMETERS:type=%c\n",rmsg.type);
    fprintf(stderr,"GET_PARAMETERS:status=%d\n",rmsg.status);
  }

  rprm.tbeam=bmnum;   
  rprm.tfreq=12000;   
  rprm.trise=5000;   
  rprm.baseband_samplerate=((double)nbaud/(double)txpl)*1E6; 
  rprm.filter_bandwidth=rprm.baseband_samplerate; 
  rprm.match_filter=1;
  rprm.number_of_samples=total_samples+10; 

  smsg.type=SET_PARAMETERS;
  TCPIPMsgSend(sock,&smsg,sizeof(struct ROSMsg));
  TCPIPMsgSend(sock,&rprm,sizeof(struct ControlPRM));
  TCPIPMsgRecv(sock,&rmsg,sizeof(struct ROSMsg));
  if (debug) {
    fprintf(stderr,"SET_PARAMETERS:type=%c\n",rmsg.type);
    fprintf(stderr,"SET_PARAMETERS:status=%d\n",rmsg.status);
  }

  if (gettimeofday(&tock,NULL)==-1) return -1;
  tock.tv_sec+=sec;
  tock.tv_usec+=usec;


  return 0;


}


int SiteCvwFCLR(int stfreq,int edfreq) {
  int32 tfreq;
  struct ROSMsg smsg,rmsg;
  struct CLRFreqPRM fprm;
  int total_samples=0;

  SiteCvwExit(0);

  total_samples=tsgprm.samples+tsgprm.smdelay;
  rprm.tbeam=bmnum;   
  rprm.tfreq=tfreq;   
  rprm.rfreq=tfreq;   
  rprm.trise=5000;   
  rprm.baseband_samplerate=((double)nbaud/(double)txpl)*1E6; 
  rprm.filter_bandwidth=rprm.baseband_samplerate; 
  rprm.match_filter=1;
  rprm.number_of_samples=total_samples+10; 

  smsg.type=SET_PARAMETERS;
  TCPIPMsgSend(sock,&smsg,sizeof(struct ROSMsg));
  TCPIPMsgSend(sock,&rprm,sizeof(struct ControlPRM));
  TCPIPMsgRecv(sock,&rmsg,sizeof(struct ROSMsg));
  if (debug) {
    fprintf(stderr,"SET_PARAMETERS:type=%c\n",rmsg.type);
    fprintf(stderr,"SET_PARAMETERS:status=%d\n",rmsg.status);
  }

  fprm.start=stfreq; 
  fprm.end=edfreq;  
  fprm.nave=20;  
  fprm.filter_bandwidth=250;  

  smsg.type=REQUEST_CLEAR_FREQ_SEARCH;
  TCPIPMsgSend(sock, &smsg, sizeof(struct ROSMsg));
  TCPIPMsgSend(sock, &fprm, sizeof(struct CLRFreqPRM));
  TCPIPMsgRecv(sock, &rmsg, sizeof(struct ROSMsg));
  if (debug) {
    fprintf(stderr,"REQUEST_CLEAR_FREQ_SEARCH:type=%c\n",rmsg.type);
    fprintf(stderr,"REQUEST_CLEAR_FREQ_SEARCH:status=%d\n",rmsg.status);
  }

  smsg.type=REQUEST_ASSIGNED_FREQ;
  TCPIPMsgSend(sock, &smsg, sizeof(struct ROSMsg));
  TCPIPMsgRecv(sock,&tfreq, sizeof(int32)); 
  TCPIPMsgRecv(sock,&noise, sizeof(float));  
  TCPIPMsgRecv(sock,&rmsg, sizeof(struct ROSMsg)); 
  if (debug) {
    fprintf(stderr,"REQUEST_ASSIGNED_FREQ:type=%c\n",rmsg.status);
    fprintf(stderr,"REQUEST_ASSIGNED_FREQ:status=%d\n",rmsg.status);
  }

  return tfreq;
}



int SiteCvwTimeSeq(int *ptab) {

  int i;
  int flag,index=0;
  struct ROSMsg smsg,rmsg;

  struct SeqPRM tprm;
  SiteCvwExit(0);
  if (tsgbuf !=NULL) TSGFree(tsgbuf);
  if (tsgprm.pat !=NULL) free(tsgprm.pat);
  memset(&tsgprm,0,sizeof(struct TSGprm));

  tsgprm.nrang=nrang;         
  tsgprm.frang=frang;
  tsgprm.rtoxmin=0;      
  tsgprm.stdelay=18+2;
  tsgprm.gort=1;
  tsgprm.rsep=rsep;          
  tsgprm.smsep=smsep;
  tsgprm.txpl=txpl; 
  tsgprm.mpinc=mpinc;
  tsgprm.mppul=mppul; 
  tsgprm.mlag=0;
  tsgprm.nbaud=nbaud;
  tsgprm.code=pcode;
  tsgprm.pat=malloc(sizeof(int)*tsgprm.mppul);
  for (i=0;i<tsgprm.mppul;i++) tsgprm.pat[i]=ptab[i];

  tsgbuf=TSGMake(&tsgprm,&flag);

  if (tsgbuf==NULL) return -1;

  tprm.index=index;
/*  memcpy(&tprm.buf,tsgbuf,sizeof(struct TSGbuf));*/
  tprm.len=tsgbuf->len;
  tprm.step=CLOCK_PERIOD;
  tprm.samples=tsgprm.samples;
  tprm.smdelay=tsgprm.smdelay;

  smsg.type=REGISTER_SEQ;
  TCPIPMsgSend(sock, &smsg, sizeof(struct ROSMsg));
  TCPIPMsgSend(sock, &tprm, sizeof(struct SeqPRM));
  TCPIPMsgSend(sock, tsgbuf->rep, sizeof(unsigned char)*tprm.len);
  TCPIPMsgSend(sock, tsgbuf->code, sizeof(unsigned char)*tprm.len);
  TCPIPMsgRecv(sock, &rmsg, sizeof(struct ROSMsg));
  if (debug) {
    fprintf(stderr,"REGISTER_SEQ:type=%c\n",rmsg.type);
    fprintf(stderr,"REGISTER_SEQ:status=%d\n",rmsg.status);
  }

  if (rmsg.status !=1) return -1;

  lagfr=tsgprm.lagfr;
  smsep=tsgprm.smsep;
  txpl=tsgprm.txpl;

  return index;
}

int SiteCvwIntegrate(int (*lags)[2]) {

  int *lagtable[2]={NULL,NULL};
  int lagsum[LAG_SIZE];

  int badrng=0;
  int i,j;
  int roff=REAL_BUF_OFFSET;
  int ioff=IMAG_BUF_OFFSET;

  struct timeval tick;
  struct timeval tack;
  double tval=0,tavg=0;

  struct ROSMsg smsg,rmsg;

  int iqoff=0; /* Sequence offset in bytes for current sequence relative to start of samples buffer*/
  int iqsze=0; /* Total number of bytes so far recorded into samples buffer*/

  int nave=0;

  int atstp=0;
  int thr=0,lmt=0;
  int aflg=0,abflg=0;
  void * dest; /*AJ*/
  int total_samples=0; /*AJ*/

  short I,Q;

  /* phase code declarations */
  int n,nsamp, *code,   Iout, Qout;
  debug=0;
  if (debug) {
    fprintf(stderr,"CVW SiteIntegrate: start\n");
  }
  debug=0;
  SiteCvwExit(0);
  if (nrang>=MAX_RANGE) return -1;
  for (j=0;j<LAG_SIZE;j++) lagsum[j]=0;

  if (mplgexs==0) {
    lagtable[0]=malloc(sizeof(int)*(mplgs+1));
    lagtable[1]=malloc(sizeof(int)*(mplgs+1));

    for (i=0;i<=mplgs;i++) {
      lagtable[0][i]=lags[i][0];
      lagtable[1][i]=lags[i][1];
    }
  } else {
    lagtable[0]=malloc(sizeof(int)*(mplgexs+1));
    lagtable[1]=malloc(sizeof(int)*(mplgexs+1));

    for (i=0;i<=mplgexs;i++) {
      lagtable[0][i]=lags[i][0];
      lagtable[1][i]=lags[i][1];
      j=abs(lags[i][0]-lags[i][1]);
      lagsum[j]++;
    }
  }

  total_samples=tsgprm.samples+tsgprm.smdelay;
  smpnum=total_samples;
  skpnum=tsgprm.smdelay;  /*skpnum != 0  returns 1, which is used as the dflg argument in ACFCalculate to enable smdelay usage in offset calculations*/

  badrng=ACFBadLagZero(&tsgprm,mplgs,lagtable);

  gettimeofday(&tick,NULL);
  gettimeofday(&tack,NULL);

  for (i=0;i<MAX_RANGE;i++) {
      pwr0[i]=0;
      for (j=0;j<LAG_SIZE*2;j++) {
        acfd[i*LAG_SIZE*2+j]=0;
        xcfd[i*LAG_SIZE*2+j]=0;
      }
  }

  while (1) {
  debug=0;
  if (debug) {
  printf("Size of Struct ROSMsg  %ld\n",sizeof(struct ROSMsg));
  printf("Size of Struct int32  %ld\n",sizeof(int32));
  printf("Size of Struct float  %ld\n",sizeof(float));
  printf("Size of Struct unsigned char  %ld\n",sizeof(unsigned char));
  printf("Size of Struct ControlPRM  %ld\n",sizeof(struct ControlPRM));
  printf("Size of Struct CLRFreqPRM  %ld\n",sizeof(struct CLRFreqPRM));
  printf("Size of Struct SeqPRM  %ld\n",sizeof(struct SeqPRM));
  printf("Size of Struct DataPRM  %ld\n",sizeof(struct DataPRM));
   }
  debug=0;
    seqtval[nave].tv_sec=tick.tv_sec;
    seqtval[nave].tv_nsec=tick.tv_usec*1000;
    seqatten[nave]=0;
    seqnoise[nave]=0;

    seqbadtr[nave].num=0;
    if (seqbadtr[nave].start !=NULL) free(seqbadtr[nave].start);
    if (seqbadtr[nave].length !=NULL) free(seqbadtr[nave].length);
    seqbadtr[nave].start=NULL;
    seqbadtr[nave].length=NULL;
  

    tval=(tick.tv_sec+tick.tv_usec/1.0e6)-
         (tack.tv_sec+tack.tv_usec/1.0e6);

    if (nave>0) tavg=tval/nave; 
     
    tick.tv_sec+=floor(tavg);
    tick.tv_usec+=1.0e6*(tavg-floor(tavg));

    if (tick.tv_sec>tock.tv_sec) break;
    if ((tick.tv_sec>=tock.tv_sec) && (tick.tv_usec>tock.tv_usec)) break;

    rprm.tbeam=bmnum;   
    rprm.tfreq=tfreq;   
    rprm.rfreq=tfreq;   
    rprm.trise=5000;   
    rprm.baseband_samplerate=((double)nbaud/(double)txpl)*1E6; 
    rprm.filter_bandwidth=rprm.baseband_samplerate; 
    rprm.match_filter=1;
    rprm.number_of_samples=total_samples+10; 
    smsg.type=SET_PARAMETERS;
    TCPIPMsgSend(sock,&smsg,sizeof(struct ROSMsg));
    TCPIPMsgSend(sock,&rprm,sizeof(struct ControlPRM));
    TCPIPMsgRecv(sock,&rmsg,sizeof(struct ROSMsg));
    if (debug) {
      fprintf(stderr,"SET_PARAMETERS:type=%c\n",rmsg.type);
      fprintf(stderr,"SET_PARAMETERS:status=%d\n",rmsg.status);
    }


    smsg.type=SET_READY_FLAG;
    TCPIPMsgSend(sock,&smsg,sizeof(struct ROSMsg));
    TCPIPMsgRecv(sock,&rmsg,sizeof(struct ROSMsg));
    if (debug) {
      fprintf(stderr,"SET_READY_FLAG:type=%c\n",rmsg.type);
      fprintf(stderr,"SET_READY_FLAG:status=%d\n",rmsg.status);
    }

    debug=0;
    smsg.type=GET_DATA;
    if (rdata.main!=NULL) free(rdata.main);
    if (rdata.back!=NULL) free(rdata.back);
    rdata.main=NULL;
    rdata.back=NULL;
    TCPIPMsgSend(sock,&smsg,sizeof(struct ROSMsg));
    if (debug) {
      fprintf(stderr,"CVW GET_DATA: recv dprm\n");
    }
    TCPIPMsgRecv(sock,&dprm,sizeof(struct DataPRM));
    if(rdata.main) free(rdata.main);
    if(rdata.back) free(rdata.back);
      if (debug) {
        fprintf(stderr,"CVW GET_DATA: samples %d status %d\n",dprm.samples,dprm.status);
      }
    if(dprm.status==0) {
      if (debug) {
        fprintf(stderr,"CVW GET_DATA: rdata.main: uint32: %ld array: %ld\n",sizeof(uint32),sizeof(uint32)*dprm.samples);
      }
      rdata.main=malloc(sizeof(uint32)*dprm.samples);
      rdata.back=malloc(sizeof(uint32)*dprm.samples);
      if (debug) {
        fprintf(stderr,"CVW GET_DATA: recv main\n");
      }
      TCPIPMsgRecv(sock, rdata.main, sizeof(uint32)*dprm.samples);
      if (debug) {
        fprintf(stderr,"CVW GET_DATA: recv back\n");
      }
      TCPIPMsgRecv(sock, rdata.back, sizeof(uint32)*dprm.samples);
      if (badtrdat.start_usec !=NULL) free(badtrdat.start_usec);
      if (badtrdat.duration_usec !=NULL) free(badtrdat.duration_usec);
      if (debug) {
        fprintf(stderr,"CVW GET_DATA: trtimes length %d\n",badtrdat.length);
      }
      TCPIPMsgRecv(sock, &badtrdat.length, sizeof(badtrdat.length));
      if (debug) 
        fprintf(stderr,"CVW GET_DATA: badtrdat.start_usec: uint32: %ld array: %ld\n",sizeof(uint32),sizeof(uint32)*badtrdat.length);
      badtrdat.start_usec=malloc(sizeof(uint32)*badtrdat.length);
      badtrdat.duration_usec=malloc(sizeof(uint32)*badtrdat.length);
      if (debug) {
        fprintf(stderr,"CVW GET_DATA: start_usec\n");
      }
      TCPIPMsgRecv(sock, badtrdat.start_usec,
                 sizeof(uint32)*badtrdat.length);
      if (debug) {
        fprintf(stderr,"CVW GET_DATA: duration_usec\n");
      }
      TCPIPMsgRecv(sock, badtrdat.duration_usec,
                 sizeof(uint32)*badtrdat.length);
/*
      TCPIPMsgRecv(sock, &num_transmitters, sizeof(int));
      TCPIPMsgRecv(sock, &txstatus.AGC, sizeof(int)*num_transmitters);
      TCPIPMsgRecv(sock, &txstatus.LOWPWR, sizeof(int)*num_transmitters);
*/
    }
    TCPIPMsgRecv(sock, &rmsg, sizeof(struct ROSMsg));
    if (debug) {
      fprintf(stderr,"CVW GET_DATA:type=%c\n",rmsg.type);
      fprintf(stderr,"CVW GET_DATA:status=%d\n",rmsg.status);
    }

    smsg.type=GET_PARAMETERS;
    TCPIPMsgSend(sock, &smsg, sizeof(struct ROSMsg));
    TCPIPMsgRecv(sock, &rprm, sizeof(struct ControlPRM));
    TCPIPMsgRecv(sock, &rmsg, sizeof(struct ROSMsg));
    if (debug) {
      fprintf(stderr,"CVW GET_PARAMETERS:type=%c\n",rmsg.type);
      fprintf(stderr,"CVW GET_PARAMETERS:status=%d\n",rmsg.status);
      fprintf(stderr,"CVW Number of samples: dprm.samples:%d tsprm.samples:%d total_samples:%d\n",dprm.samples,tsgprm.samples,total_samples);
      fprintf(stderr,"CVW nave=%d\n",nave);
    }

    debug=0;


    if(dprm.status==0) {
    /* decode phase coding here */
      if(nbaud>1){
        nsamp=(int)dprm.samples;
        code=pcode;
        
        for(n=0;n<(nsamp-nbaud);n++){
          Q=((rdata.main)[n+i] & 0xffff0000) >> 16;
          I=(rdata.main)[n+i] & 0x0000ffff;
          Iout=0;
          Qout=0;
          for(i=0;i<nbaud;i++){
            Q=((rdata.main)[n+i] & 0xffff0000) >> 16;
            I=(rdata.main)[n+i] & 0x0000ffff;
            Iout+=(int)I*(int)code[i];
            Qout+=(int)Q*(int)code[i];
          }

          Iout/=nbaud;
          Qout/=nbaud;
          I=(short)Iout;
          Q=(short)Qout;
                
          (rdata.main)[n]=(Q<<16)|I;
          Iout=0;
          Qout=0;
          for(i=0;i<nbaud;i++){
            Q=((rdata.back)[n+i] & 0xffff0000) >> 16;
            I=(rdata.back)[n+i] & 0x0000ffff;
            Iout+=(int)I*(int)code[i];
            Qout+=(int)Q*(int)code[i];
          }
          Iout/=nbaud;
          Qout/=nbaud;
          I=(short)Iout;
          Q=(short)Qout;
          (rdata.back)[n]=(Q<<16)|I;
        }
      }
      if(dprm.samples<total_samples) {
        fprintf(stderr,"Not enough  samples from the ROS in SiteIntegrate\n");
        fflush(stderr);
      }

    /* copy samples here */

      seqoff[nave]=iqsze/2;/*Sequence offset in 16bit units */
      seqsze[nave]=total_samples*2*2; /* Sequence length in 16bit units */

      seqbadtr[nave].num=badtrdat.length;
      seqbadtr[nave].start=malloc(sizeof(uint32)*badtrdat.length);
      seqbadtr[nave].length=malloc(sizeof(uint32)*badtrdat.length);

      memcpy(seqbadtr[nave].start,badtrdat.start_usec,
           sizeof(uint32)*badtrdat.length);
      memcpy(seqbadtr[nave].length,badtrdat.duration_usec,
           sizeof(uint32)*badtrdat.length);

/* AJ new way, does work for some reason */
/* samples is natively an int16 pointer */
/* rdata.main is natively an uint32 pointer */
/* rdata.back is natively an uint32 pointer */
/* total_samples*8 represents number of bytes for main and back samples */

      
      debug=0;
      dest = (void *)(samples);  /* look iqoff bytes into samples area */
      dest+=iqoff;
      if ((iqoff+total_samples*2*sizeof(uint32) )<IQBUFSIZE) {
        memmove(dest,rdata.main,total_samples*sizeof(uint32));
        dest += total_samples*sizeof(uint32); /* skip ahead number of samples * 32 bit per sample to account for rdata.main*/
        memmove(dest,rdata.back,total_samples*sizeof(uint32));
      } else {
        fprintf(stderr,"IQ Buffer overrun in SiteIntegrate\n");
        fflush(stderr);
      }
      iqsze+=total_samples*sizeof(uint32)*2;  /*  Total of number bytes so far copied into samples array */
      debug=0;
      if (debug) {
        fprintf(stderr,"CVW seq %d :: ioff: %8d\n",nave,iqoff);
        fprintf(stderr,"CVW seq %d :: rdata.main 16bit 8-11: %8d %8d %8d %8d\n",nave,
           ((int16 *)rdata.main)[8],((int16 *)rdata.main)[9],
           ((int16 *)rdata.main)[10],((int16 *)rdata.main)[11]);
        dest = (void *)(samples);
        dest += iqoff;
        fprintf(stderr,"CVW seq %d :: samples    16bit 8-11: %8d %8d %8d %8d\n",nave,
           ((int16 *)dest)[8],((int16 *)dest)[9],
           ((int16 *)dest)[10],((int16 *)dest)[11]);
        fprintf(stderr,"CVW seq %d :: rdata.back 16bit 8-11: %8d %8d %8d %8d\n",nave,
           ((int16 *)rdata.back)[8],((int16 *)rdata.back)[9],
           ((int16 *)rdata.back)[10],((int16 *)rdata.back)[11]);
        dest += total_samples*4;
        fprintf(stderr,"CVW seq %d :: samples    16bit 8-11: %8d %8d %8d %8d\n",nave,
           ((int16 *)dest)[8],((int16 *)dest)[9],
           ((int16 *)dest)[10],((int16 *)dest)[11]);
        fprintf(stderr,"CVW seq %d :: iqsze: %8d\n",nave,iqsze);
      }
      debug=0;

    /* calculate ACF */   
      if (mplgexs==0) {
        dest = (void *)(samples);
        dest += iqoff;
        aflg=ACFSumPower(&tsgprm,mplgs,lagtable,pwr0,
		     (int16 *) dest,2*rxchn,skpnum!=0,
                     roff,ioff,badrng,
                     noise,mxpwr,seqatten[nave]*atstp,
                     thr,lmt,&abflg);
        ACFCalculate(&tsgprm,(int16 *) dest,2*rxchn,skpnum!=0,
          roff,ioff,mplgs,lagtable,acfd,ACF_PART,total_samples,badrng,seqatten[nave]*atstp,NULL);
        if (xcf ==1 ){
          ACFCalculate(&tsgprm,(int16 *) dest,2*rxchn,skpnum!=0,
                    roff,ioff,mplgs,lagtable,xcfd,XCF_PART,total_samples,badrng,seqatten[nave]*atstp,NULL);
        }
        if ((nave>0) && (seqatten[nave] !=seqatten[nave]))
              ACFNormalize(pwr0,acfd,xcfd,tsgprm.nrang,mplgs,atstp); 
          


      }
      nave++;
      iqoff=iqsze;  /* set the offset bytes for the next sequence */

    } else {
      fprintf(stderr,"SiteCvwIntegrate:: Bad pulse sequence status: %d\n",dprm.status);
      fflush(stderr);
    }
    gettimeofday(&tick,NULL);


  }

  /* Now divide by nave to get the average pwr0 and acfd values for the 
     integration period */ 

   if (mplgexs==0) {

     if (nave > 0 ) {
       ACFAverage(pwr0,acfd,xcfd,nave,tsgprm.nrang,mplgs);
/*
       for(range=0; range < nrang;range++) {
         pwr0[range]=(double)pwr0[range]/(double)nave;

         for(lag=0;lag < mplgs; lag++) {     
           acfd[range*(2*mplgs)+2*lag]= (double) acfd[range*(2*mplgs)+2*lag]/
                                       (double) nave;
           acfd[range*(2*mplgs)+2*lag+1]= (double) acfd[range*(2*mplgs)+2*lag+1]/
                                       (double) nave;
         }
       }
*/
     }
   } else if (nave>0) {
     /* ACFEX calculation */
     ACFexCalculate(&tsgprm,(int16 *) samples,nave*smpnum,nave,smpnum,
                   roff,ioff,
                   mplgs,mplgexs,lagtable,lagsum,
                   pwr0,acfd,&noise);

   }
   free(lagtable[0]);
   free(lagtable[1]);
   SiteCvwExit(0);
   debug=0;
   if (debug) {
     fprintf(stderr,"CVW SiteIntegrate: iqsize in bytes: %ld in 16bit samples:  %ld in 32bit samples: %ld\n",iqsze,iqsze/2,iqsze/4);
     fprintf(stderr,"CVW SiteIntegrate: end\n");
   }
   debug=0;
   return nave;
}

int SiteCvwEndScan(int bsc,int bus) {

  struct ROSMsg smsg,rmsg;
  
  struct timeval tock;
  struct timeval tick;
  double bnd;
  double tme;
  int count=0;
  SiteCvwExit(0);
  bnd=bsc+bus/USEC;

  if (gettimeofday(&tock,NULL)==-1) return -1;

  tme=tock.tv_sec+tock.tv_usec/USEC;
  tme=bnd*floor(1.0+tme/bnd);
  tock.tv_sec=tme;
  tock.tv_usec=(tme-floor(tme))*USEC;

  smsg.type=SET_INACTIVE;
  TCPIPMsgSend(sock, &smsg, sizeof(struct ROSMsg));
  TCPIPMsgRecv(sock, &rmsg, sizeof(struct ROSMsg));

  gettimeofday(&tick,NULL);
  while (1) {
    if (tick.tv_sec>tock.tv_sec) break;
    if ((tick.tv_sec==tock.tv_sec) && (tick.tv_usec>tock.tv_usec)) break;
    smsg.type=PING;
    TCPIPMsgSend(sock, &smsg, sizeof(struct ROSMsg));
    TCPIPMsgRecv(sock, &rmsg, sizeof(struct ROSMsg));

    if (debug) {
      fprintf(stderr,"PING:type=%c\n",rmsg.type);
      fprintf(stderr,"PING:status=%d\n",rmsg.status);
      fprintf(stderr,"PING:count=%d\n",count);
      fflush(stderr);
    }
    count++;
    usleep(50000);
    gettimeofday(&tick,NULL);
  }

  smsg.type=SET_ACTIVE;
  TCPIPMsgSend(sock, &smsg, sizeof(struct ROSMsg));
  TCPIPMsgRecv(sock, &rmsg, sizeof(struct ROSMsg));

  return 0;
}







