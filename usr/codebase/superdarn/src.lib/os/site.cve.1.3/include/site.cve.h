/* site.tst.h
   ==========
   Author: R.J.Barnes
*/


#ifndef _SITECVE_H
#define _SITECVE_H

int SiteCveStart(char *host);
int SiteCveKeepAlive();
int SiteCveSetupRadar();
int SiteCveStartScan();
int SiteCveStartIntt(int intsc,int intus);
int SiteCveFCLR(int stfreq,int edfreq);
int SiteCveTimeSeq(int *ptab);
int SiteCveIntegrate(int (*lags)[2]);
int SiteCveEndScan(int bsc,int bus);
void SiteCveExit(int signum);

#endif

