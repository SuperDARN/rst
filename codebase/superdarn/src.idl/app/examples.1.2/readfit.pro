; readfit.pro
; ===========
; Author: R.J.Barnes
; 
; See license.txt
; 
; 
; 
;
; A Simply demonstration program for reading fit files


pro readfit

  
; Open the fit file

  fitfp=OldFitOpen('data/fit/test.fit','data/fit/test.inx')


;  Search for a specific time in the file

  s=OldFitSeek(fitfp,2002,8,30,1,30,10,atme=atme)
 
  while OldFitRead(fitfp,prm,fit) ne -1 do begin
     print, prm.time.hr,prm.time.mt,prm.time.sc
 endwhile
 s=OldFitClose(fitfp)
end


