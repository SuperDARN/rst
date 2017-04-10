/* istp.h
   ======
   Author: R.J.Barnes
*/

/*
   See license.txt
*/

#define FILL_VALUE 9999.0

struct imfdata {
  char sat[8];
  char ins[8];
  int cnt,idx;
  double *time;
  float *BGSMc;
  float *BGSEc;
};

struct posdata {
  char sat[8];
  char ins[8];
  int cnt,idx;
  double *time;
  float *PGSM;
  float *PGSE;
};

struct plasmadata {
  char sat[8];
  char ins[8];
  int cnt,idx;
  double *time;
  float *VGSM;
  float *VGSE;
  float *vth;
  float *den;
  float *pre;
};

  
