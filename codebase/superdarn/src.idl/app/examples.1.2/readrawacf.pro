; readrawacf.pro
; ==============
; Author: R.J.Barnes
;
; Copyright (c) 2012 The Johns Hopkins University/Applied Physics Laboratory
; 
; This file is part of the Radar Software Toolkit (RST).
; 
; RST is free software: you can redistribute it and/or modify
; it under the terms of the GNU General Public License as published by
; the Free Software Foundation, either version 3 of the License, or
; (at your option) any later version.
; 
; This program is distributed in the hope that it will be useful,
; but WITHOUT ANY WARRANTY; without even the implied warranty of
; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
; GNU General Public License for more details.
; 
; You should have received a copy of the GNU General Public License
; along with this program. If not, see <https://www.gnu.org/licenses/>.
; 
; Modifications:
;
;
;
; A simple demonstration program for reading rawacf files

pro readrawacf

; Open the index file and load it into the inx structure 

  openr,inp,'data/rawacf/test.rawinx',/GET_LUN,/STDIO,/SWAP_IF_BIG_ENDIAN
  s=RawLoadInx(inp,inx)
  free_lun,inp
 
; Open the raw file for read only 
 
  inp=RawOpen('data/rawacf/test.rawacf',/read)
 

;  Search for a specific time in the file

  s=RawSeek(inp,2002,12,19,0,30,10,inx,atme=atme)
 
  while RawRead(inp,prm,raw) ne -1 do begin
     print, prm.time.hr,prm.time.mt,prm.time.sc
  endwhile
  free_lun,inp

end


