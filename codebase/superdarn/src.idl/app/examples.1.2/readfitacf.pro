; readfitacf.pro
; ==============
; Author: R.J.Barnes
; 
; See license.txt
; 
; 
; 
;
; A Simply demonstration program for reading rawacf files


pro readfitacf

; Open the fit index file and load it into the inx structure 

  openr,inp,'data/fitacf/test.fitinx',/GET_LUN,/STDIO,/SWAP_IF_BIG_ENDIAN
  s=FitLoadInx(inp,inx)
  free_lun,inp

; Open the raw file for read only 
 
  inp=FitOpen('data/fitacf/test.fitacf',/read)
 

;  Search for a specific time in the file

  s=FitSeek(inp,2002,8,30,2,30,10,inx,atme=atme)
  while FitRead(inp,prm,fit) ne -1 do begin
     print, prm.time.hr,prm.time.mt,prm.time.sc
  endwhile
  free_lun,inp

end


