/* site.tst.h
   ==========
   Author: R.J.Barnes
*/


#ifndef _SITETST_H
#define _SITETST_H

int SiteTstStart(char *host);
int SiteTstKeepAlive();
int SiteTstSetupRadar();
int SiteTstStartScan();
int SiteTstStartIntt(int intsc,int intus);
int SiteTstFCLR(int stfreq,int edfreq);
int SiteTstTimeSeq(int *ptab);
int SiteTstIntegrate(int (*lags)[2]);
int SiteTstEndScan(int bsc,int bus);
void SiteTstExit(int signum);

#endif

