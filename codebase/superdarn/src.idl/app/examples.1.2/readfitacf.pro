; readfitacf.pro
; ==============
; Author: R.J.Barnes
; 
; Copyright (C) <year>  <name of author>

Copyright (C) <year>  <name of author>

RST is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of

MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see <https://www.gnu.org/licenses/>.

Modifications:
; 
; RST is free software: you can redistribute it and/or modify
; it under the terms of the GNU General Public License as published by

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


