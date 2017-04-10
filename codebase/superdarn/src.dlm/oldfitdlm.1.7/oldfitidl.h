/* oldfitidl.h
   =========== 
   Author: R.J.Barnes
*/

/*
   See license.txt
*/



#ifndef _OLDFITIDL_H
#define _OLDFITIDL_H
 
struct OldFitIDLFp {
  int32 fitunit;
  int32 inxunit;
  int32 fit_recl;
  int32 inx_recl;
  int32 blen;
  int32 inx_srec;
  int32 inx_erec;
  double ctime;
  double stime;
  double etime;
  int32 time;
  char header[80];
  char date[32];
  char extra[256];
  char major_rev;
  char minor_rev;  

  /* The actual IDL structure has an integer for the next member */

  int (*fitread)(struct OldFitFp *ptr,struct RadarParm *prm,
                 struct FitData *fit);

};

#endif
