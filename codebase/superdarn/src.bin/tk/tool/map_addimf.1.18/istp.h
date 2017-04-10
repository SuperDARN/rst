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
  int cnt;
  double *time;
  float *BGSMc;
  float *BGSEc;
};


  
