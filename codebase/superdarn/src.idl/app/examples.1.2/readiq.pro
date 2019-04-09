; readriq.pro
; ===========
; Author: R.J.Barnes
; 
; LICENSE AND DISCLAIMER
; 
; Copyright (c) 2012 The Johns Hopkins University/Applied Physics Laboratory
; 
; This file is part of the Radar Software Toolkit (RST).
; 
; RST is free software: you can redistribute it and/or modify
; it under the terms of the GNU Lesser General Public License as published by
; the Free Software Foundation, either version 3 of the License, or
; any later version.
; 
; RST is distributed in the hope that it will be useful,
; but WITHOUT ANY WARRANTY; without even the implied warranty of
; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
; GNU Lesser General Public License for more details.
; 
; You should have received a copy of the GNU Lesser General Public License
; along with RST.  If not, see <http://www.gnu.org/licenses/>.
; 
; 
; 
;
; A Simply demonstration program for reading iqdat files


pro readiq

; Open the raw index file and load it into the inx structure 

;  openr,inp,'test.rawinx',/GET_LUN,/STDIO,/SWAP_IF_BIG_ENDIAN
;  s=RawLoadInx(inp,inx)
;  free_lun,inp
 
; Open the raw file for read only 
 
  inp=IQOpen('data/iq/test.iqdat',/read)

;  out=IQOpen('test2.iqdat',/write) 

;  Search for a specific time in the file

;  s=IQSeek(inp,2002,12,19,0,30,10,inx,atme=atme)
 
  while IQRead(inp,prm,iq,badtr,samples) ne -1 do begin
 
  
     
 ;    s=IQWrite(out,prm,iq,samples)
     print, prm.time.hr,prm.time.mt,prm.time.sc
     s=IQExtractIQ(iq,0,0,samples,iqc)
    
     plot,real_part(iqc),linestyle=1
     oplot,imaginary(iqc),linestyle=5
  endwhile
  free_lun,inp

end


