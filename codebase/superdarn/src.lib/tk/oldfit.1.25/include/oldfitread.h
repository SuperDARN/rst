/* oldfitread.h
   ============
   Author: R.J.Barnes
*/


/*
   See license.txt
*/




#ifndef _OLDFITREAD_H
#define _OLDFITREAD_H

struct OldFitFp {
  int fitfp;
  int inxfp;
  int fit_recl;
  int inx_recl;
  struct stat fstat;
  struct stat istat;
  int blen;
  int inx_srec;
  int inx_erec;
  double ctime;
  double stime;
  double etime;
  int time;
  char header[80];
  char date[32];
  char extra[256];
  char major_rev; 
  char minor_rev;
  int (*fitread)(struct OldFitFp *ptr,struct RadarParm *prm,
                 struct FitData *fit);
};


int OldFitReadInx(struct OldFitFp *ptr,int32 *buffer,int recno);
int OldFitRead(struct OldFitFp *ptr,struct RadarParm *prm,
               struct FitData *fit);
int OldFitSeek(struct OldFitFp *ptr,
               int yr,int mo,int dy,int hr,int mt,int sc,
	       double *atme);
void OldFitClose(struct OldFitFp *ptr);
struct OldFitFp *OldFitOpenFd(int fitfp,int inxfp);

struct OldFitFp *OldFitOpen(char *fitfile,char *inxfile);

#endif


