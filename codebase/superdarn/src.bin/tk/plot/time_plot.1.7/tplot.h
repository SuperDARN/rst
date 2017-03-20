/* tplot.h
   =======
   Author: R.J.Barnes
*/

/*
   See license.txt
*/




struct tplot {
  int bmnum;
  int channel;
  int cpid;
  int scan;
  int nrang;
  int nave;
  struct {
    int sc;
    int us;
  } intt;
  int frang;
  int rsep;
  int rxrise;
  int noise;
  int tfreq;
  int atten;
  int *qflg;
  int *gsct;
  double *p_l;
  double *p_l_e;
  double *v;
  double *v_e;
  double *w_l;
  double *w_l_e;
}; 

int tplotset(struct tplot *ptr,int nrang);

