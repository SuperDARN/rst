/* site.fhw.h
   ==========
   Author: R.J.Barnes
*/


#ifndef _SITEFHW_H
#define _SITEFHW_H

int SiteFhwStart(char *host);
int SiteFhwKeepAlive();
int SiteFhwSetupRadar();
int SiteFhwStartScan();
int SiteFhwStartIntt(int intsc,int intus);
int SiteFhwFCLR(int stfreq,int edfreq);
int SiteFhwTimeSeq(int *ptab);
int SiteFhwIntegrate(int (*lags)[2]);
int SiteFhwEndScan(int bsc,int bus);
void SiteFhwExit(int signum);

#endif

