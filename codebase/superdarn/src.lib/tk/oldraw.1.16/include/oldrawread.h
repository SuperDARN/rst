/* raw_read.h
   ==========
   Author: R.J.Barnes
*/


/*
   See license.txt
*/



#ifndef _OLDRAWREAD_H
#define _OLDRAWREAD_H


struct OldRawFp {
  int rawfp;
  int inxfp;
  struct stat rstat;
  struct stat istat;
  double ctime;
  double stime;
  int frec;
  int rlen;
  int ptr;
  int thr;
  int major_rev;
  int minor_rev;
  int (*rawread)(struct OldRawFp *ptr,struct RadarParm *,struct RawData *);

};




int OldRawRead(struct OldRawFp *fp,struct RadarParm *prm,struct RawData *raw);
int OldRawReadData(struct OldRawFp *fp,struct RadarParm *prm,
		   struct RawData *raw);
int OldRawSeek(struct OldRawFp *ptr,int yr,int mo,int dy,int hr,int mt,int sc,
	     double *atme);
void OldRawClose(struct OldRawFp *fp);

struct OldRawFp *OldRawOpenFd(int rawfd,int inxfd);
struct OldRawFp *OldRawOpen(char *rawfile,char *inxfile);

#endif



