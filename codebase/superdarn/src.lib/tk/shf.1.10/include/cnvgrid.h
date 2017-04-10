/* cnvgrid.h
   =========  
   Author: R.J.Barnes
*/


/*
   See license.txt
*/




#ifndef _CNVGRID_H
#define _CNVGRID_H

#define GRIDPOTENTIAL 0
#define GRIDVELOCITY 1

struct CnvGrid {
  int type;
  int num;
  int nlat;
  int nlon;
  int poly;
  int *vertex;
  double *lat;
  double *lon;
  double *mag;
  double *azm;
  double *ex;
  double *ey;
};

struct CnvGrid *CnvGridMake();
void CnvGridFree(struct CnvGrid *ptr);

#endif








