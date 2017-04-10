/* nrfit.c
   =======
   Numerical Recipes 
*/

/*
   See license.txt
*/


#include <stdio.h>
#include <stdlib.h>
#include <math.h>




void nrfit(float *x,float *y,int ndata,float *sig,int mwt,
                  float *a,float *b,float *siga,float *sigb,float *chi2,
                  float *q) {

  int i;
  float wt,t,sxoss,sx=0.0,sy=0.0,st2=0.0,ss,sigdat;
 
  *b=0.0;

  if (mwt) {
    ss=0.0;
    for (i=0;i<ndata;i++) {
      wt=1.0/(sig[i]*sig[i]);
      ss+=wt;
      sx+=x[i]*wt;
      sy+=y[i]*wt;
    }
  } else {
    for (i=0;i<ndata;i++) {
      sx+=x[i];
      sy+=y[i];
    }
    ss=ndata;
  }
  sxoss=sx/ss;

  if (mwt) {
    for (i=0;i<ndata;i++) {
      t=(x[i]-sxoss)/sig[i];
      st2+=t*t;
      *b+=t*y[i]/sig[i];
    }
  } else {
    for (i=0;i<ndata;i++) {
      t=x[i]-sxoss;
      st2+=t*t;
      *b+=t*y[i];
    }
  }
  *b/=st2;
  *a=(sy-sx*(*b))/ss;
  *siga=sqrt((1.0+sx*sx/(ss*st2))/ss);
  *sigb=sqrt(1.0/st2);
  
  *chi2=0.0;
  if (mwt==0) {
    for (i=0;i<ndata;i++)
      *chi2+=(y[i]-(*a)-(*b)*x[i])*(y[i]-(*a)-(*b)*x[i]);
    *q=1.0;
    sigdat=sqrt((*chi2)/(ndata-2));
    *siga *= sigdat; 
    *sigb *= sigdat;
  } else {
    for (i=0;i<ndata;i++) 
      *chi2+=((y[i]-(*a)-(*b)*x[i])/sig[i])*((y[i]-(*a)-(*b)*x[i])/sig[i]);
    /*    *q=gammaq(0.5*(ndata-2),0.5*(*chi2)); */
    *q=1.0;
  }
}
