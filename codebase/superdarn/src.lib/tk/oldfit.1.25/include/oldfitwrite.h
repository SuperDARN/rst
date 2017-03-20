/* oldfitwrite.h
   ===========
   Author: R.J.Barnes
*/

/*
   See license.txt
*/



#ifndef _OLDFITWRITE_H
#define _OLDFITWRITE_H

int OldFitHeaderWrite(int fitfp, char *text, char *name,char *version);
int OldFitInxClose(int inxfp,struct RadarParm *prm,int irec);
int OldFitInxHeaderWrite(int inxfp,struct RadarParm *prm);
int OldFitInxWrite(int inxfp,int drec,int dnum,struct RadarParm *prm);
int OldFitWrite(int fitfp,struct RadarParm *prm,struct FitData *fit,
		int *rtab);

int OldFitHeaderFwrite(FILE *fitfp, char *text, char *name,char *version);
int OldFitInxFclose(FILE *inxfp,struct RadarParm *prm,int irec);
int OldFitInxHeaderFwrite(FILE *inxfp,struct RadarParm *prm);
int OldFitInxFwrite(FILE *inxfp,int drec,int dnum,struct RadarParm *prm);
int OldFitFwrite(FILE *fitfp,struct RadarParm *prm,struct FitData *fit,
                 int *rtab);

#endif
