/* imp.h
   =====
   Author: R.J.Barnes
*/

/*
   See license.txt
*/




int imppla_pos(CDFid id,struct posdata *ptr,double stime,double etime);
int imppla_plasma(CDFid id,struct plasmadata *ptr,double stime,double etime);
int impmag_pos(CDFid id,struct posdata *ptr,double stime,double etime);
int impmag_imf(CDFid id,struct imfdata *ptr,double stime,double etime);
  
