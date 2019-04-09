; readefield.pro
; ==============
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
; A Simply demonstration program for reading electric field data files


pro readefield

; Open the raw index file and load it into the inx structure 

  openr,inp,'test.inx',/GET_LUN,/SWAP_IF_BIG_ENDIAN
  s=EFieldLoadInx(inp,inx)
  free_lun,inp
 
; Open the Electric field file for read only 
 

  openr,inp,'test.efield',/GET_LUN,/SWAP_IF_BIG_ENDIAN

;  Search for a specific time in the file

  s=EfieldSeek(inp,2002,12,19,0,30,10,inx,atme=atme)
 

  while EFieldRead(inp,prm,count,pos,pot,E,V) ne -1 do begin
     print, prm.stme.hr,prm.stme.mt,prm.stme.sc
     polar_contour,pot,pos[*,0,1]*!pi/180,90-pos[0,*,0]
     stop
  endwhile
  free_lun,inp

end


