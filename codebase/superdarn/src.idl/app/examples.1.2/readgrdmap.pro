; readgrdmap.pro
; ==============
; Author: R.J.Barnes
; 
; See license.txt
; 
; 
; 
;
; A Simply demonstration program for reading grdmap files


pro readgrdmap

; Open the raw index file and load it into the inx structure 

  openr,inp,'data/grdmap/test.grdinx',/GET_LUN,/STDIO,/SWAP_IF_BIG_ENDIAN
  s=GridLoadInx(inp,inx)
  free_lun,inp
 
; Open the grdmap file for read only 
 
  inp=GridOpen('data/grdmap/test.grdmap',/read)
 

;  Search for a specific time in the file

;  s=GridSeek(inp,2002,12,19,0,30,10,inx,atme=atme)
 
  while GridRead(inp,prm,stvec,gvec) ne -1 do begin
     print, prm.stme.hr,prm.stme.mt,prm.stme.sc
     stop
  endwhile
  free_lun,inp

end


