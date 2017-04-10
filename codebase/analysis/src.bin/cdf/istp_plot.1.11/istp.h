/* istp.h
   ======
   Author: R.J.Barnes
*/


/*
   See license.txt
*/




struct imfdata {
  char sat[8];
  char ins[8];
  int cnt;
  double *time;
  float *BGSMc;
  float *BGSEc;
};

struct posdata {
  char sat[8];
  char ins[8];
  int cnt;
  double *time;
  float *PGSM;
  float *PGSE;
};

struct plasmadata {
  char sat[8];
  char ins[8];
  int cnt;
  double *time;
  float *VGSM;
  float *VGSE;
  float *vth;
  float *den;
  float *pre;
};

  
