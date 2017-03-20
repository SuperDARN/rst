/* wind.h
   ======
   Author: R.J.Barnes
*/

/*
   See license.txt
*/




int windswe_pos(CDFid id,struct posdata *ptr,double stime,double etime);
int windswe_plasma(CDFid id,struct plasmadata *ptr,double stime,double etime);
int windmfi_pos(CDFid id,struct posdata *ptr,double stime,double etime);
int windmfi_imf(CDFid id,struct imfdata *ptr,double stime,double etime);
  
