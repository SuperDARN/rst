; readcnvmap.pro
; ==============
; Author: R.J.Barnes
; 
; See license.txt
; 
; 
; 
;
; A Simply demonstration program for reading cnvmap files


pro readcnvmap

; Open the raw index file and load it into the inx structure 

  openr,inp,'data/cnvmap/test.mapinx',/GET_LUN,/STDIO,/SWAP_IF_BIG_ENDIAN
  s=CnvMapLoadInx(inp,inx)
  free_lun,inp
 
; Open the cnvmap file for read only 
 
  inp=CnvMapOpen('data/cnvmap/test.cnvmap',/read)
 

;  Search for a specific time in the file

  s=CnvMapSeek(inp,2002,12,19,0,30,10,inx,atme=atme)
 
  while CnvMapRead(inp,prm,stvec,gvec,mvec,coef,bvec) ne -1 do begin
     print, prm.stme.hr,prm.stme.mt,prm.stme.sc
     stop
  endwhile
  free_lun,inp

end


