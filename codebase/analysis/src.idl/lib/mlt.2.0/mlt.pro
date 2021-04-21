; mlt.pro
; =======
; Author: R.J.Barnes
;
; LICENSE AND DISCLAIMER
; 
; Copyright (c) 2012 The Johns Hopkins University/Applied Physics Laboratory
; 
; This file is part of the Radar Software Toolkit (RST).
; 
; RST is free software: you can redistribute it and/or modify
; it under the terms of the GNU General Public License as published by
; the Free Software Foundation, either version 3 of the License, or
; any later version.
; 
; This program is distributed in the hope that it will be useful,
; but WITHOUT ANY WARRANTY; without even the implied warranty of
; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
; GNU General Public License for more details.
; 
; You should have received a copy of the GNU General Public License
; along with RST.  If not, see <http://www.gnu.org/licenses/>.
; 
; 
; 
;
; Public Functions:
; -----------------
;
; MLTConvertYMDHMS
; MLTConvertEpoch
; MLTConvertYrSec
;
; Private Functions:
; ------------------
;
; MLTDefaultMLT
; MLTAst
;
; ---------------------------------------------------------------

; Common blocks for the MLT algorithm

common MLTCom,sol_dec_old,told,mslon1,mslon2

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       MLTDefault
;
; PURPOSE:
;       Initialized the common block variables for the MLT routines
;       
;
; CALLING SEQUENCE:
;       MLTDefault
;
;       This procedure initializes the common block variables for
;       the MLT routine. It is automatically called the first
;       time the MLT routines are used.
;
;     
;
;-----------------------------------------------------------------
;


pro MLTDefault
  common MLTCom,sol_dec_old,told,mslon1,mslon2
  sol_dec_old=0
  told=1e12
  mslon1=0
  mslon2=0
end


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       MLTAst1
;
; PURPOSE:
;       Calculate Magnetic Local Time
;       
;
; CALLING SEQUENCE:
;       MLTAst1,t0,solar_dec,mlon,mlt,mslon
;
;
;       This procedure calculates magnetic local time.
;
;     
;-----------------------------------------------------------------
;



pro MLTAst1,t0,solar_dec,mlon,mlt,mslon
  common MLTCom,sol_dec_old,told,mslon1,mslon2
  if (n_elements(sol_dec_old) eq 0) then MLTDefault
  if ((abs(solar_dec-sol_dec_old) gt 0.1) or (sol_dec_old eq 0)) then told=1e12
  if (abs(mslon2-mslon1) gt 10) then told=1e12;
    
  if ((t0 ge told) and (t0 lt (told+600))) then $
    mslon=mslon1+(t0-told)*(mslon2-mslon1)/600.0 $
  else begin
    told=t0
    sol_dec_old=solar_dec

    slon1 = (43200.0-t0)*15.0/3600.0
    slon2 = (43200.0-t0-600)*15.0/3600.0

    height = 450
    AACGMConvertGeoCoord,solar_dec,slon1,height,mslat1,mslon1,4,err
    AACGMConvertGeoCoord,solar_dec,slon2,height,mslat2,mslon2,4,err
    mslon=mslon1
  end
  

  mlt = (mlon - mslon) /15.0 + 12.0
  if (mlt ge 24) then mlt=mlt-24;
  if (mlt lt 0) then mlt=mlt+24;  

end


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       MLTAst
;
; PURPOSE:
;       Calculate Magnetic Local Time
;       
;
; CALLING SEQUENCE:
;       MLTAst,yr,mo,dy,hr,mt,sc,mlon,mlt,mslon
;
;
;       This function calculates magnetic local time.
;
;     
;-----------------------------------------------------------------
;

pro MLTAst,yr,mo,dy,hr,mt,sc,mlon,mlt,mslon

   dd=AstAlg_dday(dy-1,hr,mt,sc)
   jd=AstAlg_jde(yr,mo,dd)
   eqt=AstAlg_equation_of_time(jd);
   dec=AstAlg_solar_declination(jd);

   ut=(hr*3600.0D) + (mt*60.0D)+sc;
   at = ut + (eqt*60.0D);
   
   MLTAst1,at, dec, mlon, mlt,mslon
 
end



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       MLTConvertYMDHMS
;
; PURPOSE:
;       Calculate Magnetic Local Time
;       
;
; CALLING SEQUENCE:
;       mlt=MLTConvertYMDHMS(yr,mo,dy,hr,mt,sc,mlon)
;
;
;       This function calculates magnetic local time.
;
;     
;-----------------------------------------------------------------
;

  
function MLTConvertYMDHMS,yr,mo,dy,hr,mt,sc,mlon
   mslon=0.0D
   mlt=0.0D

   if N_ELEMENTS(yr) ne 1 then begin
      n=N_ELEMENTS(yr)
      sze=SIZE(yr)
      mlt=dblarr(sze[1:sze[0]])
    
      for i=0,n-1 do begin
         if N_ELEMENTS(mlon) ne 1 then mtmp=mlon[i] $
         else mtmp=mlon
         MLTAst,yr[i],mo[i],dy[i],hr[i],mt[i],sc[i],mtmp,tmpmlt,mslon
         mlt[i]=tmpmlt
      endfor
      return, mlt  
   end else begin
     MLTAst,yr,mo,dy,hr,mt,sc,mlon,mlt,mslon
     return, mlt
   endelse

   return, mlt
end


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       MLTConvertYrsec
;
; PURPOSE:
;       Calculate Magnetic Local Time
;       
;
; CALLING SEQUENCE:
;       mlt=MLTConvertYrsec(yr,yrsec,mlon)
;
;
;       This function calculates magnetic local time.
;
;     
;-----------------------------------------------------------------
;

  
function MLTConvertYrsec,yr,yrsec,mlon
   mslon=0.0D
   mlt=0.0D


   if N_ELEMENTS(yrsec) ne 1 then begin
      n=N_ELEMENTS(yrsec)
      sze=SIZE(yrsec)
      mlt=dblarr(sze[1:sze[0]])
    
      for i=0,n-1 do begin
          if N_ELEMENTS(yr) ne 1 then yrtmp=yr[i] $
         else yrtmp=yr
         s=TimeYrSecToYMDHMS(yrtmp,mo,dy,hr,mt,sc,yrsec[i])
         if N_ELEMENTS(mlon) ne 1 then mtmp=mlon[i] $
         else mtmp=mlon
         MLTAst,yrtmp,mo,dy,hr,mt,sc,mtmp,tmpmlt,mslon
         mlt[i]=tmpmlt
      endfor
      return, mlt  
   end else begin
     s=TimeYrSecToYMDHMS(yr,mo,dy,hr,mt,sc,yrsec)
     MLTAst,yr,mo,dy,hr,mt,sc,mlon,mlt,mslon
     return, mlt
   endelse

end


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       MLTConvertEpoch
;
; PURPOSE:
;       Calculate Magnetic Local Time
;       
;
; CALLING SEQUENCE:
;       mlt=MLTConvertEpoch(epoch,mlon)
;
;
;       This function calculates magnetic local time.
;
;     
;-----------------------------------------------------------------
;

  
function MLTConvertEpoch,epoch,mlon
   mslon=0.0D
   mlt=0.0D

   if N_ELEMENTS(epoch) ne 1 then begin
      n=N_ELEMENTS(epoch)
      sze=SIZE(epoch)
      mlt=dblarr(sze[1:sze[0]])
    
      for i=0,n-1 do begin
         s=TimeEpochToYMDHMS(yr,mo,dy,hr,mt,sc,epoch[i])
         if N_ELEMENTS(mlon) ne 1 then mtmp=mlon[i] $
         else mtmp=mlon
         MLTAst,yr,mo,dy,hr,mt,sc,mtmp,tmpmlt,mslon
         mlt[i]=tmpmlt
      endfor
      return, mlt  
   end else begin
     s=TimeEpochToYMDHMS(yr,mo,dy,hr,mt,sc,epoch)
     MLTAst,yr,mo,dy,hr,mt,sc,mlon,mlt,mslon
     return, mlt
   endelse

end


