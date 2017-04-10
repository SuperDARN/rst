/* cfitdata.h
   ==========
   Author: R.J.Barnes
*/


/*
   See license.txt
*/



#ifndef _CFITDATA_H
#define _CFITDATA_H


#define CFIT_MAJOR_REVISION 2
#define CFIT_MINOR_REVISION 1

struct CFitCell {
  int gsct;
  double p_0;
  double p_0_e;
  double v;
  double v_e;
  double w_l;
  double w_l_e;
  double p_l;
  double p_l_e;
};

struct CFitdata {
  struct {
    int major;
    int minor;
  } version;
  int16 stid; 
  double time;
  int16 scan;
  int16 cp;
  int16 bmnum;
  float bmazm;
  int16 channel;
  struct {
    int16 sc;
    int32 us;
  } intt;
  int16 frang;
  int16 rsep;
  int16 rxrise;
  int16 tfreq;
  float noise;
  int16 atten;
  int16 nave;
  int16 nrang;

  int16 num;
  int16 *rng;
  struct CFitCell *data;
};

struct CFitdata *CFitMake();
void CFitFree(struct CFitdata *ptr);
int CFitSetRng(struct CFitdata *ptr,int num);

#endif

 





