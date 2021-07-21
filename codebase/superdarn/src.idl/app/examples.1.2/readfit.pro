; readfit.pro
; ===========
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
; A simple demonstration program for reading fit files

pro readfit

  
; Open the fit file

  fitfp=OldFitOpen('data/fit/test.fit','data/fit/test.inx')


;  Search for a specific time in the file

  s=OldFitSeek(fitfp,2002,8,30,1,30,10,atme=atme)
 
  while OldFitRead(fitfp,prm,fit) ne -1 do begin
     print, prm.time.hr,prm.time.mt,prm.time.sc
 endwhile
 s=OldFitClose(fitfp)
end

