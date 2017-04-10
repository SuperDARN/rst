/* smrwrite.h
   ==========
   Author: R.J.Barnes
*/

/*
   See license.txt
*/



#ifndef _SMRWRITE_H
#define _SMRWRITE_H

int SmrHeaderFwrite(FILE * fp,char *version,char *text,char *rname);
int SmrFwrite(FILE *fp,struct RadarParm *prm,struct FitData *fit,int pwr);
 
#endif
