; readcfit.pro
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
; A simple demonstration program for reading cfit files

pro readcfit

  ; Open input file for reading
 

  openr,inp,'data/cfit/test.cfit',/GET_LUN,/SWAP_IF_BIG_ENDIAN,/COMPRESS


  c=0 
  vstore=fltarr(100,1440)
  while  CFitRead(inp,prm,fit) ne -1 do begin
   print, prm.time.hr,prm.time.mt,prm.time.sc
   if (prm.bmnum eq 8) then begin
     vstore[0:prm.nrang-1,c]=fit.v[0:prm.nrang-1]
     c++
     print, c
   endif
 endwhile
 surface,vstore[0:74,0:c-1]
 free_lun,inp
end


