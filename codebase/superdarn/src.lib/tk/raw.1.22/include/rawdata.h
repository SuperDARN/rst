/* rawdata.h
   ========== 

  Author: R.J.Barnes
*/

/*
   See license.txt
*/

#ifndef _RAWDATA_H
#define _RAWDATA_H

struct RawData {
  struct {
    int major;
    int minor;
  } revision;
  float thr;
  float *pwr0;
  float *acfd[2];
  float *xcfd[2];
};


struct RawData *RawMake();
void RawFree(struct RawData *ptr);
int RawSetPwr(struct RawData *ptr,int nrang,float *pwr0,int snum,int *slist);
int RawSetACF(struct RawData *ptr,int nrang,int mplgs,float *acfd,int snum,int *slist);
int RawSetXCF(struct RawData *ptr,int nrang,int mplgs,float *xcfd,int snum,int *slist);


void *RawFlatten(struct RawData *ptr,int nrang,int mplgs,size_t *size);
int RawExpand(struct RawData *ptr,int nrang,int mplgs,void *buffer);




#endif
