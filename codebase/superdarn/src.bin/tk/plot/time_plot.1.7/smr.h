/* smr.h
   =====
   Author: R.J.Barnes
*/


/*
   See license.txt
*/



double smr_find(FILE *fp,struct RadarParm *prm,
                struct FitData *fit,int fbeam,double sdate,
                double stime);
  
double smr_scan(double ctime,FILE *fp,int fbeam,int rflg,
                struct RadarParm *prm,
                struct FitData *fit,
                int bmnum,int chnum,int cpid,int sflg,int scan);

