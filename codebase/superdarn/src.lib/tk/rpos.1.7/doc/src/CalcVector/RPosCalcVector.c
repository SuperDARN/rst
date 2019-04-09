/* RPosCalcVector
   ==============
   Author: R.J.Barnes */


#include <stdio.h>
#include <stdlib.h>

#include "calcvector.h"


int main(int argc,char *argv[]) {
 
  double alat=58.0;
  double alon=140.0;
  double mag=1500.0;
  double azm=18.0;

  double blat,blon;

  RPosCalcVector(alat,alon,mag,azm,&blat,&blon);


  fprintf(stdout,"RPosCalcVector\n");
  fprintf(stdout,"alat=%g, alon=%g, mag=%g, azm=%g\n",alat,alon,mag,azm);
  fprintf(stdout,"blat=%g blon=%g\n",blat,blon);

  return 0;
}
