; readraw.pro
; ==============
; Author: R.J.Barnes
; 
; See license.txt
; 
; 
; 
;
; A Simply demonstration program for reading raw files


pro readraw

  
; Open the raw file

  rawfp=OldRawOpen('data/raw/test.dat')


;  Search for a specific time in the file

  s=OldRawSeek(rawfp,2002,12,19,1,30,10,atme=atme)

  while OldRawRead(rawfp,prm,raw) ne -1 do begin
     print, prm.time.hr,prm.time.mt,prm.time.sc
 endwhile
 s=OldRawClose(rawfp)
end


