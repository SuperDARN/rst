/* cfitseek.c
   =========
   Author: R.J.Barnes
*/

/*
   See license.txt
*/


#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>
#include <zlib.h>
#include "rtypes.h"
#include "rtime.h"
#include "cfitdata.h"
#include "cfitindex.h"
#include "cfitread.h"

int CFitSeek(struct CFitfp *ptr,
	     int yr,int mo,int dy,int hr,int mt,int sc,double *atme,
             struct CFitIndex *inx) {

  struct CFitdata *cfit=NULL;
  int status=0;
  double tval;
  int bjmp=0;


  tval=TimeYMDHMSToEpoch(yr,mo,dy,hr,mt,sc);
  if (inx !=NULL) {
    int rec=0,prec=-1;
    int srec;
    int erec;

    /* search using index records */

    double stime,etime;
    stime=inx->tme[0];
    etime=inx->tme[inx->num-1];
    srec=0;
    erec=inx->num;
    if (tval<stime) { /* before start of file */
      if (atme !=NULL) *atme=stime;
      if (ptr->fbuf==NULL) gzseek(ptr->fp,inx->inx[srec],SEEK_SET);
      else ptr->fptr=inx->inx[srec];
      return 0;
    } else if (tval>etime) { /* after end of file */
      if (atme !=NULL) *atme=stime;
      if (ptr->fbuf==NULL) gzseek(ptr->fp,inx->inx[erec-1],SEEK_SET);
      else ptr->fptr=inx->inx[erec-1];
      return -1;
    }

    do {
      prec=rec;
      rec=srec+(int) ((tval-stime)*(erec-srec)/(etime-stime));
      if (inx->tme[rec]==tval) break;
      if (inx->tme[rec]<tval) {
	srec=rec;
	stime=inx->tme[rec];
      } else {
	erec=rec;
	etime=inx->tme[rec];
      }
    } while (prec != rec);
    if (atme !=NULL) *atme=inx->tme[rec];
    if (ptr->fbuf==NULL) gzseek(ptr->fp,inx->inx[rec],SEEK_SET);
    else ptr->fptr=inx->inx[rec];
    return 0;
  } else {
    cfit=CFitMake();
    if (cfit==NULL) return -1;
    if (tval<ptr->ctime) {
      /* rewind the file */
      if (ptr->fbuf==NULL) gzseek(ptr->fp,0,SEEK_SET);
      else ptr->fptr=0;
      ptr->ctime=0;
      ptr->blen=0;
    }

    if (tval>=ptr->ctime) {
      do {
        bjmp=ptr->blen;
        status=CFitRead(ptr,cfit);
      } while ((tval>=ptr->ctime) && (status !=-1));

      if (status!=-1) {
        if (ptr->fbuf==NULL) gzseek(ptr->fp,-(ptr->blen+bjmp),SEEK_CUR);
        else ptr->fptr-=ptr->blen+bjmp;
     }   else return -1;
   }
   /* get the actual time */

    do {  
      status=CFitRead(ptr,cfit);
    } while (status !=0);

    if (ptr->fbuf==NULL) gzseek(ptr->fp,-ptr->blen,SEEK_CUR);
    else ptr->fptr-=ptr->blen;

    if (atme !=NULL) {
      *atme=ptr->ctime;
    }

    CFitFree(cfit);
    if (status==-1) return 0;
  }
  return 0;
} 





















