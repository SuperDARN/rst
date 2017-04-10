/* geotail.h
   =========
   Author: R.J.Barnes
*/

/*
   See license.txt
*/




int geolep_pos(CDFid id,struct posdata *ptr,double stime,double etime);
int geolep_plasma(CDFid id,struct plasmadata *ptr,double stime,double etime);
int geomgf_pos(CDFid id,struct posdata *ptr,double stime,double etime);
int geomgf_imf(CDFid id,struct imfdata *ptr,double stime,double etime);
  
int geocpi_plasma(CDFid id,struct plasmadata *ptr,double stime,double etime);
