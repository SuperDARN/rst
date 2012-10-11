/* site.tst.h
   ==========
   Author: R.J.Barnes
*/


#ifndef _SITECVW_H
#define _SITECVW_H

int SiteCvwStart(char *host);
int SiteCvwKeepAlive();
int SiteCvwSetupRadar();
int SiteCvwStartScan();
int SiteCvwStartIntt(int intsc,int intus);
int SiteCvwFCLR(int stfreq,int edfreq);
int SiteCvwTimeSeq(int *ptab);
int SiteCvwIntegrate(int (*lags)[2]);
int SiteCvwEndScan(int bsc,int bus);
void SiteCvwExit(int signum);

#endif

