/* oldfit.h
   ========
   Author: R.J.Barnes
*/

/*
   See license.txt
*/




double oldfit_find(struct OldFitFp *fitfp,struct RadarParm *prm,
                struct FitData *fit,double sdate,
                double stime);
 
double oldfit_scan(double ctime,struct OldFitFp *fitfp,int rflg,
                struct RadarParm *prm,struct FitData *fit,
                int bmnum,int chnum,int cpid,int sflg,int scan);

void oldfit_tplot(struct RadarParm *prm,struct FitData *fit,struct tplot *tptr);
 
