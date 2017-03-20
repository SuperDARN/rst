/* fit.h
   =====
   Author: R.J.Barnes
*/

/*
   See license.txt
*/




double fit_find(FILE *fp,struct RadarParm *prm,
                struct FitData *fit,double sdate,
                double stime,struct FitIndex *inx);
 
double fit_scan(double ctime,FILE *fp,int rflg,
                struct RadarParm *prm,struct FitData *fit,
                int bmnum,int chnum,int cpid,int sflg,int scan);

void fit_tplot(struct RadarParm *prm,struct FitData *fit,struct tplot *tptr);
 
