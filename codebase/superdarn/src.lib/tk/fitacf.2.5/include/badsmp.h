/* badsmp.h
   ========
   Author: R.J.Barnes
*/





#ifndef _BADSMP_H
#define _BADSMP_H

#define MAXBAD 500

struct FitACFBadSample {
  int nbad;
  int badsmp[MAXBAD];
};
      
#endif
