; testmlt.pro
; ============
; Author: R.J.Barnes
; 
; See license.txt
; 
; 
; 
;
; A Simply demonstration program for testing the MLT library


pro testmlt



  yrsec=TimeYMDHMSToYrSec(2002,08,30,10,30,0)

  yrarr=fltarr(10);
  yrarr[*]=yrsec
  yrarr[5:9]+=3600

  mlt=MLTConvertYrsec(2002,yrarr,0.0)

  print, 'MLT=',mlt

end


