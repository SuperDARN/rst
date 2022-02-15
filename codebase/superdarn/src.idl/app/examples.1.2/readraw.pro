; readraw.pro
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
; A simple demonstration program for reading raw files

pro readraw

  
; Open the raw file

  rawfp=OldRawOpen('data/raw/test.dat')


;  Search for a specific time in the file

  s=OldRawSeek(rawfp,2002,12,19,1,30,10,atme=atme)

  while OldRawRead(rawfp,prm,raw) ne -1 do begin
     print, prm.time.hr,prm.time.mt,prm.time.sc
 endwhile
 s=OldRawClose(rawfp)
end


