; readgrd.pro
; ===========
; Author: R.J.Barnes
; 
; See license.txt
; 
; 
; 
;
; A Simply demonstration program for reading grid files


pro readgrd

; Open the raw index file and load it into the inx structure 

  openr,inp,'data/grd/test.inx',/GET_LUN,/STDIO,/SWAP_IF_BIG_ENDIAN
  s=OldGridLoadInx(inp,inx)
  free_lun,inp
 
; Open the grid file for read only 
 
  inp=OldGridOpen('data/grd/test.grd',/read)
 

;  Search for a specific time in the file

;  s=OldGridSeek(inp,2002,12,19,0,30,10,inx,atme=atme)
 
  while OldGridRead(inp,prm,stvec,gvec) ne -1 do begin
     print, prm.stme.hr,prm.stme.mt,prm.stme.sc
     stop
  endwhile
  free_lun,inp

end


