/* nrutil.h
   ======== 
   Author R.J.Barnes
*/

/*
   See license.txt
*/



#define SQR(a) (((a)==0) ? 0 : (a)*(a))
#define FMAX(a,b) ((a)>(b) ? (a) : (b))
#define SIGN(a,b) ((b)>=0.0 ? fabs(a) : -fabs(a))

#define IMIN(a,b) ((a)<(b) ? (a) : (b))
