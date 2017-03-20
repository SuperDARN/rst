/* ace.h
   =====
   Author: R.J.Barnes
*/


/*
   See license.txt
*/




int aceswe_pos(CDFid id,struct posdata *ptr,double stime,double etime);  
int aceswe_plasma(CDFid id,struct plasmadata *ptr,double stime,double etime);
int acemfi_imf(CDFid id,struct imfdata *ptr,double stime,double etime,int cnv);
 
