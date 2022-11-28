; radar.pro
; =======
; Author: R.J.Barnes & D.Andre
;
; Copyright (c) 2012 The Johns Hopkins University/Applied Physics Laboratory
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
;   E.G.Thomas 2021-08: added support for new hdw file fields
;   E.G.Thomas 2022-04: added support for tdiff calibration files
; 
; Public Functions:
; -----------------
;
;  RadarMakeSite
;  RadarMakeTdiff
;  RadarMakeRadar
;  RadarLoad
;  RadarLoadHardware
;  RadarLoadTdiff
;  RadarEpochGetSite
;  RadarYMDHMSGetSite
;  RadarEpochGetTdiff
;  RadarYMDHMSGetTdiff
;  RadarGetRadar
;  RadarPos
;  RadarPosGS
;
; Private Functions:
; ------------------
;
; RadarSlantRange
; RadarGeoTGC
; RadarFldPnt
; RadarGeoCnvrt
; RadarFldPnth
; RadarFldPnthGS

; ---------------------------------------------------------------


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       RadarMakeSite
;
; PURPOSE:
;       Create a structure to store the hardware tables
;
;
; CALLING SEQUENCE:
;       RadarMakeSite,site
;
;       This procedure creates a structure to store the hardware table,
;       the structure is returns in site.
;


pro RadarMakeSite,site

  site={RadarSite, $
         status: 0L, $
         tval: 0.0D, $
         geolat: 0.0D, $
         geolon: 0.0D, $
         alt: 0.0D, $
         boresite: 0.0D, $
         bmoff:0.0D, $
         bmsep:0.0D, $
         vdir: 0.0D, $
         atten: 0.0D, $
         tdiff: dblarr(2), $
         phidiff: 0.0D, $
         interfer: dblarr(3), $
         recrise: 0.0D, $
         maxatten: 0L, $
         maxrange: 0L, $
         maxbeam: 0L $
     }
end


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       RadarMakeTdiff
;
; PURPOSE:
;       Create a structure to store the tdiff calibration tables
;
;
; CALLING SEQUENCE:
;       RadarMakeTdiff,tdiff
;
;       This procedure creates a structure to store the tdiff values,
;       the structure is returned in tdiff.
;


pro RadarMakeTdiff,tdiff

  tdiff={RadarTdiff, $
          method: 0L, $
          channel: 0L, $
          freq: dblarr(2), $
          tval: dblarr(2), $
          tdiff: 0.0D, $
          tdiff_err: 0.0D $
        }
end


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       RadarMakeRadar
;
; PURPOSE:
;       Create a structure to store the radar identification table
;
;
; CALLING SEQUENCE:
;       RadarMakeRadar,radar
;
;       This procedure creates a structure to store the radar identification,
;       table, the structure is returns in radar.
;


pro RadarMakeRadar,radar
    
  RadarMakeSite,site
  RadarMakeTdiff,tdiff
  radar={Radar, $
          id: 0L, $
          status: 0L, $
          cnum: 0L, $
          code: strarr(8), $
          name: '', $
          operator: '', $
          hdwfname: '', $
          st_time: 0.0D, $
          ed_time: 0.0D, $
          snum: 0L, $
          site: replicate(site,32), $
          tnum: 0L, $
          tdiff: replicate(tdiff,32) $
        }

end



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       RadarLoad
;
; PURPOSE:
;       Loads the radar identification table.
;       
;
; CALLING SEQUENCE:
;       radar = RadarLoad(unit)
;
;       This function reads the radar identification table from
;       the open file with logical unit number, unit
;
;
;       The returned value is the radar identification table or
;       zero if an error occurred.
;
;-----------------------------------------------------------------
;
 
function RadarLoad,unit

ON_IOERROR, iofail

   RadarMakeRadar, rad
   radar=replicate(rad,1)
   c=0

   id=0L
   status=0L
   txt=''
   stnum=0L
   ednum=0L
   yr=0
   mo=0
   dy=0
   line=''

   repeat begin
      ; Different than main.1.21 version of radar.pro
      readf,unit,line
      s=strpos(line,'#')
      if (s ne -1) then continue
      if (strlen(line) eq 0) then continue
      reads,line,id,status,stnum,ednum,txt
      rad.id=id
      rad.status=status

      dy=stnum mod 100;
      mo=(stnum/100) mod 100
      yr=stnum/10000

      rad.st_time=TimeYMDHMSToEpoch(yr,mo,dy,0,0,0)

      dy=ednum mod 100;
      mo=(ednum/100) mod 100
      yr=ednum/10000

      rad.ed_time=TimeYMDHMSToEpoch(yr,mo,dy,0,0,0)

      ; decode the text strings
      s=strpos(txt,'"')
      e=strpos(txt,'"',s+1)
      rad.name=strmid(txt,s+1,e-s-1)
      s=strpos(txt,'"',e+1)
      e=strpos(txt,'"',s+1)
      rad.operator=strmid(txt,s+1,e-s-1)

      s=strpos(txt,'"',e+1)
      e=strpos(txt,'"',s+1)
      rad.hdwfname=strmid(txt,s+1,e-s-1)
      n=0
      while (s ne -1) do begin
         s=strpos(txt,'"',e+1)
         if (s eq -1) then break
         e=strpos(txt,'"',s+1)
         rad.code[n]=strmid(txt,s+1,e-s-1)
         n=n+1
      endwhile
      rad.cnum=n

      if (c ne 0) then radar=[radar,rad] $
      else radar[0]=rad
      c=c+1
   endrep until eof(unit)

iofail:
   return, radar
end



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       RadarLoadHardware
;
; PURPOSE:
;       Loads the radar hardware tables
;       
;
; CALLING SEQUENCE:
;       s = RadarLoadHardware(radar)
;
;       This function reads in the radar hardware tables and
;       populates the structure radar. The structure should
;       be created using the RadarLoad function.
;
;       The returned value is zero
;
;-----------------------------------------------------------------
;


function RadarLoadHardware,radar,path=path
   txt=''
   for n=0,N_ELEMENTS(radar)-1 do begin
      if (KEYWORD_SET(path)) then fname=path+radar[n].hdwfname $
      else fname=radar[n].hdwfname    
      ON_IOERROR, trap
      openr,unit,fname,/GET_LUN
      x=0

      while (~eof(unit)) do begin
        readf,unit,txt
        s=strpos(txt,'#')
        if (s ne -1) then continue
        if (strlen(txt) eq 0) then continue
        tmp = strsplit(txt, ' ', /extract)
        if (long(tmp[0]) eq radar[n].id) then begin
          yr = long(tmp[2]) / 10000
          mo = (long(tmp[2]) / 100) mod 100
          dy = long(tmp[2]) mod 100

          time = fix(strsplit(tmp[3], ':', /extract))

          radar[n].site[x].status=double(tmp[1])
          radar[n].site[x].tval=TimeYMDHMSToEpoch(yr,mo,dy,time[0],time[1],time[2])
          radar[n].site[x].geolat=double(tmp[4])
          radar[n].site[x].geolon=double(tmp[5])
          radar[n].site[x].alt=double(tmp[6])
          radar[n].site[x].boresite=double(tmp[7])
          radar[n].site[x].bmoff=double(tmp[8])
          radar[n].site[x].bmsep=double(tmp[9])
          radar[n].site[x].vdir=double(tmp[10])
          radar[n].site[x].phidiff=double(tmp[11])
          radar[n].site[x].tdiff[0]=double(tmp[12])
          radar[n].site[x].tdiff[1]=double(tmp[13])
          radar[n].site[x].interfer[0]=double(tmp[14])
          radar[n].site[x].interfer[1]=double(tmp[15] )
          radar[n].site[x].interfer[2]=double(tmp[16])
          radar[n].site[x].recrise=double(tmp[17])
          radar[n].site[x].atten=double(tmp[18])
          radar[n].site[x].maxatten=long(tmp[19])
          radar[n].site[x].maxrange=long(tmp[20])
          radar[n].site[x].maxbeam=long(tmp[21])
          x=x+1
        endif
      endwhile
      radar[n].snum=x
      trap:
        free_lun,unit
   endfor


iofail:
   return, 0
end



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       RadarLoadTdiff
;
; PURPOSE:
;       Loads the radar tdiff calibration tables
;
;
; CALLING SEQUENCE:
;       s = RadarLoadTdiff(radar)
;
;       This function reads in the radar tdiff calibration tables and
;       populates the structure radar. The structure should
;       be created using the RadarLoad function.
;
;       The returned value is zero
;
;-----------------------------------------------------------------
;


function RadarLoadTdiff,radar,path=path
   txt=''
   for n=0,N_ELEMENTS(radar)-1 do begin
      if (KEYWORD_SET(path)) then fname=path+'tdiff.dat.'+radar[n].code[0] $
      else fname='tdiff.dat.'+radar[n].code[0]
      ON_IOERROR, trap
      openr,unit,fname,/GET_LUN
      x=0

      while (~eof(unit)) do begin
        readf,unit,txt
        s=strpos(txt,'#')
        if (s ne -1) then continue
        if (strlen(txt) eq 0) then continue
        tmp = strsplit(txt, ' ', /extract)

        syr = long(tmp[4]) / 10000
        smo = (long(tmp[4]) / 100) mod 100
        sdy = long(tmp[4]) mod 100
        stime = fix(strsplit(tmp[5], ':', /extract))

        eyr = long(tmp[6]) / 10000
        emo = (long(tmp[6]) / 100) mod 100
        edy = long(tmp[6]) mod 100
        etime = fix(strsplit(tmp[7], ':', /extract))

        radar[n].tdiff[x].method=long(tmp[0])
        radar[n].tdiff[x].channel=long(tmp[1])
        radar[n].tdiff[x].freq[0]=double(tmp[2])
        radar[n].tdiff[x].freq[1]=double(tmp[3])
        radar[n].tdiff[x].tval[0]=TimeYMDHMSToEpoch(syr,smo,sdy,stime[0],stime[1],stime[2])
        radar[n].tdiff[x].tval[1]=TimeYMDHMSToEpoch(eyr,emo,edy,etime[0],etime[1],etime[2])
        radar[n].tdiff[x].tdiff=double(tmp[8])
        radar[n].tdiff[x].tdiff_err=double(tmp[9])
        x=x+1
      endwhile
      radar[n].tnum=x
      trap:
        free_lun,unit
   endfor

iofail:
   return, 0
end


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       RadarEpochGetSite
;
; PURPOSE:
;       Get the hardware information for a radar
;
;
; CALLING SEQUENCE:
;       site = RadarEpochGetSite(radar,tval)
;
;       This function finds the hardware information for a radar.
;
;       The returned structure is the hardware table or zero
;       if an error occurred.
;
;-----------------------------------------------------------------
;


function RadarEpochGetSite,radar,tval

  if (radar.st_time ne -1) && (tval lt radar.st_time) then return,0
  if (radar.ed_time ne -1) && (tval gt radar.ed_time) then return,0 

  for s=0,radar.snum do begin
    if  (radar.site[s].tval gt tval) then break
  endfor

  return, radar.site[s-1]

end



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       RadarYMDHMSGetSite
;
; PURPOSE:
;       Get the hardware information for a radar
;
;
; CALLING SEQUENCE:
;       site = RadarYMDHMSGetSite(radar,yr,mo,dy,hr,mt,sc)
;
;       This function finds the hardware information for a radar.
;
;       The returned structure is the hardware table or zero
;       if an error occurred.
;
;-----------------------------------------------------------------
;


function RadarYMDHMSGetSite,radar,yr,mo,dy,hr,mt,sc

  tval=TimeYMDHMSToEpoch(yr,mo,dy,hr,mt,sc)
  return, RadarEpochGetSite(radar,tval)

end



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       RadarEpochGetTdiff
;
; PURPOSE:
;       Get the tdiff calibration information for a radar
;
;
; CALLING SEQUENCE:
;       tdiff = RadarEpochGetTdiff(radar,tval,method,channel,tfreq)
;
;       This function finds the tdiff information for a radar.
;
;       The returned structure is the tdiff calibration table or zero
;       if an error occurred.
;
;-----------------------------------------------------------------
;


function RadarEpochGetTdiff,radar,tval,method,channel,tfreq

  for t=0,radar.tnum-1 do begin
    if (radar.tdiff[t].method ne method) then continue
    if (radar.tdiff[t].channel ne channel) then continue
    if (radar.tdiff[t].freq[0] gt tfreq) then continue
    if (radar.tdiff[t].freq[1] lt tfreq) then continue
    if (radar.tdiff[t].tval[0] gt tval) then continue
    if (radar.tdiff[t].tval[1] lt tval) then continue
    return, radar.tdiff[t]
  endfor

  return, 0

end



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       RadarYMDHMSGetTdiff
;
; PURPOSE:
;       Get the tdiff calibration information for a radar
;
;
; CALLING SEQUENCE:
;       tdiff = RadarYMDHMSGetTdiff(radar,yr,mo,dy,hr,mt,sc,method,channel,tfreq)
;
;       This function finds the tdiff information for a radar.
;
;       The returned structure is the tdiff calibration table or zero
;       if an error occurred.
;
;-----------------------------------------------------------------
;


function RadarYMDHMSGetTdiff,radar,yr,mo,dy,hr,mt,sc,method,channel,tfreq

  tval=TimeYMDHMSToEpoch(yr,mo,dy,hr,mt,sc)
  return, RadarEpochGetTdiff(radar,tval,method,channel,tfreq)

end



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       RadarGetRadar
;
; PURPOSE:
;       Get the hardware information for a radar
;
;
; CALLING SEQUENCE:
;       radar = RadarGetRadar(radar,id)
;
;       This function finds the information for a radar.
;
;       The returned structure is the information table or zero
;       if an error occurred.
;
;-----------------------------------------------------------------
;


function RadarGetRadar,radar,id
  q=where(radar[*].id eq id,count) 
  if (count ne 0) then return, radar[q[0]]
  return, 0
end



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       RadarSlantRange
;
; PURPOSE:
;       Calculate the slant range
;       
;
; CALLING SEQUENCE:
;       srng = RadarSlantRange(frang,rsep,rxrise,range_edge,range_gate)
;
;       The returned value is the slant range
;
;-----------------------------------------------------------------
;



function RadarSlantRange,frang,rsep,rxrise,range_edge,range_gate
   lagfr=frang*20/3.D
   smsep=rsep*20/3.D
   return, (lagfr-rxrise+(range_gate-1)*smsep+range_edge)*0.15D
end


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       RadarGeoTGC
;
; PURPOSE:
;       Internal function for RPos
; 			Converts from geodetic coordinates (gdlat,gdlon) to geocentric spherical
; 			coordinates (glat,glon). The radius of the Earth (grho) and the deviation
; 			off the vertical (del) are calculated. The WGS 84 oblate spheroid model
; 			of the Earth is adopted.
;       
;
; CALLING SEQUENCE:
;       RadarGeoTGC,iopt,gdlat,gdlon,grho,glat,glon,del
;
;-----------------------------------------------------------------
;


pro RadarGeoTGC,iopt,gdlat,gdlon,grho,glat,glon,del

   a=6378.137D
   f=1.0D/298.257223563D
  

   b=a*(1.0-f)
   e2=(a*a)/(b*b)-1
   if (iopt gt 0) then begin
     glat=atan( (b*b)/(a*a)*tan(!PI*gdlat/180.0))*180.0/!PI
     glon=gdlon
     if (glon gt 180) then glon=glon-360
   endif else begin
     gdlat=atan( (a*a)/(b*b)*tan(!PI*glat/180.0))*180.0/!PI
     gdlon=glon
   endelse
   grho=a/sqrt(1.0+e2*sin(!PI*glat/180.0)*sin(!PI*glat/180.0));
   del=gdlat-glat;
end


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       RadarFldPnt
;
; PURPOSE:
;       Internal function for RPos
; 			This subroutine calculates the geocentric coordinates (frho,flat,flon)
; 			of a field point given the angular geocentric coordinates (rrho,rlat,rlon)
; 			of the point of origin, the azimuth (ral), the elevation (rel), and the
; 			slant range (r). Note that the azimuth and elevation are reckoned from the
; 			horizon that is defined by the plane perpendicular to the radial vector from
; 			the center of the Earth through the point of origin. For conversion from the
; 			radarpointing coordinates to these coordinates, call RadarGeoCnvrt.
;
; CALLING SEQUENCE:
;       RadarFldPnt,rrho,rlat,rlon,ral,rel,r,frho,flat,flon
;
;-----------------------------------------------------------------
;


pro RadarFldPnt,rrho,rlat,rlon,ral,rel,r,frho,flat,flon 

	; Convert from global spherical (rrho,lat,lon) to global cartesian (rx,ry,rz: Earth centered)
   sinteta=sin(!PI*(90.0-rlat)/180.0)
   rx=rrho*sinteta*cos(!PI*rlon/180.0)
   ry=rrho*sinteta*sin(!PI*rlon/180.0)
   rz=rrho*cos(!PI*(90.0-rlat)/180.0)

	; Convert from local spherical (r,ral,rel) to local cartesian (sx,sy,sz: South,East,Up)
   sx=-r*cos(!PI*rel/180.0)*cos(!PI*ral/180.0)
   sy=r*cos(!PI*rel/180.0)*sin(!PI*ral/180.0)
   sz=r*sin(!PI*rel/180.0)

	; Convert from local cartesian to global cartesian
   tx  =  cos(!PI*(90.0-rlat)/180.0)*sx + sin(!PI*(90.0-rlat)/180.0)*sz
   ty  =  sy
   tz  = -sin(!PI*(90.0-rlat)/180.0)*sx + cos(!PI*(90.0-rlat)/180.0)*sz
   sx  =  cos(!PI*rlon/180.0)*tx - sin(!PI*rlon/180.0)*ty
   sy  =  sin(!PI*rlon/180.0)*tx + cos(!PI*rlon/180.0)*ty
   sz  =  tz

	; Find global cartesian coordinates of new point by vector addition
   tx=rx+sx
   ty=ry+sy
   tz=rz+sz

	; Convert from global cartesian to global spherical
   frho=sqrt((tx*tx)+(ty*ty)+(tz*tz))
   flat=90.0-acos(tz/(frho))*180.0/!PI
   if ((tx eq 0) and (ty eq 0)) then flon=0 $
   else flon=atan(ty,tx)*180.0/!PI
end

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       RadarGeoCnvrt
;
; PURPOSE:
;       Internal function for RPos
;				Converts from pointing azimuth and elevation (xal,xel) measured
; 			with respect to the local horizon to azimuth and elevation angles
; 			(ral,rel) appropriate to a horizon defined by the plane perpendicular
; 			to the Earth-centered radial vector drawn through the point of origin.
; 			(this is an adjustment to the measured azimuth and elevation for the
; 			oblateness of the Earth). The conversion is effected by means of a cartesian
; 			coordinate transformation that rotates the local vertical velocity onto the
; 			direction of the Earth-centeredradial vecrtor. The angle of rotation about
; 			the x-axis is the deviation off the vertical. (x: east, y:west, z:up).
;       
;
; CALLING SEQUENCE:
;       RadarGeoCnvrt,gdlat,gdlon,xal,xel,ral,rel
;
;-----------------------------------------------------------------
;


pro RadarGeoCnvrt,gdlat,gdlon,xal,xel,ral,rel

  rrad=0.0D
  rlat=0.0D
  rlon=0.0D
  del=0.0D
  

  kxg=cos(!PI*xel/180.0)*sin(!PI*xal/180.0)
  kyg=cos(!PI*xel/180.0)*cos(!PI*xal/180.0)
  kzg=sin(!PI*xel/180.0)
 
  RadarGeoTGC,1,gdlat,gdlon,rrad,rlat,rlon,del

  kxr=kxg
  kyr=kyg*cos(!PI*del/180.0)+kzg*sin(!PI*del/180.0)
  kzr=-kyg*sin(!PI*del/180.0)+kzg*cos(!PI*del/180.0)

  ral=atan(kxr,kyr)*180.0/!PI
  rel=atan(kzr/sqrt((kxr*kxr)+(kyr*kyr)))*180.0/!PI
end


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       RadarFldPnth
;
; PURPOSE:
;       Internal function for RPos
;       This subroutine calculates the geocentric coordinates (frho,flat,flon)
; 			of a radar field point given the angular coordinates (glat, glon) of the
; 			radar site, given the cone angle (psi), the field point height (fh),
; 			and the slant range (r). Note that the elevation angle is found that 
; 			accomodate (fh,r).
; 
;
; CALLING SEQUENCE:
;       RadarFldPnth,gdlat,gdlon,psi,bor,fh,r,frho,flat,flon
;
;-----------------------------------------------------------------
;

pro RadarFldPnth,gdlat,gdlon,psi,bore,fh,r,frho,flat,flon,chisham=chisham
 
    rrad=0.0D
    rlat=0.0D
    rlon=0.0D
    del=0.0D 
    dum1=0.0D
    dum2=0.0D
    dum3=0.0D

    if keyword_set(chisham) then begin
        ; Chisham virtual height model
        A = [108.974, 384.416, 1098.28]
        B = [0.0191271, -0.178640, -0.354557]
        C = [6.68283e-5, 1.81405e-4, 9.39961e-5]

        if r lt 787.5 then $
            xh = A[0] + B[0]*r + C[0]*r*r $
        else if r le 2137.5 then $
            xh = A[1] + B[1]*r + C[1]*r*r $
        else $
            xh = A[2] + B[2]*r + C[2]*r*r
        
        if (r lt 115) then xh=(r/115.0D)*112.0D
    endif else begin
        ; Standard virtual height model
        if (fh le 150) then xh=fh $
        else begin
            if (r le 600) then xh=115 $
            else if ((r gt 600) and (r lt 800)) then $
                xh=(r-600.0D)/200.0D*(fh-115.0D)+115.0D $
            else xh=fh
        endelse

        if (r lt 150) then xh=(r/150.0D)*115.0D
    endelse

    RadarGeoTGC,1,gdlat,gdlon,rrad,rlat,rlon,del

    ; Radius of the Earth beneath the radar site
    rrho=rrad

    ; Radius of the Earth beneath the field point
    ; (this is updated in the loop below)
    frad=rrad

    ; Check for zero slant range which will cause an error in the
    ; elevation angle calculation below, leading to a NAN result
    if (r eq 0.) then r=0.1

    ; Iterate until the altitude corresponding to the calculated elevation
    ; matches the desired altitude
    repeat begin 
    
        ; Distance from center of Earth fo field point location
        frho=frad+xh
 
        ; Pointing elevation (spherical Earth value)
        if r gt 2*rrad then r = 2*rrad  ; This was a problem with p-code when an incorrect number of gates was passed

        ; Elevation angle relative to local horizon [deg]
        rel=asin(((frho*frho)-(rrad*rrad)-(r*r))/(2*rrad*r))*180.0/!PI
    
        ; Need to calculate actual elevation angle for 1.5-hop propagation
        ; when using Chisham model for coning angle correction
        if keyword_set(chisham) and r gt 2137.5 then begin
            gamma = acos((rrad*rrad + frho*frho - r*r)/(2.0*rrad*frho))
            beta = asin(rrad*sin(gamma/3.0)/(r/3.0))
            rel_chisham = !PI/2.0 - beta - gamma/3.
            xel = rel_chisham*180.0/!pi
        endif else $
            xel=rel

        ; Estimate the off-array-normal azimuth
        if (((cos(!PI*psi/180.0)*cos(!PI*psi/180.0))- $
            (sin(!PI*xel/180.0)*sin(!PI*xel/180.0))) lt 0) then tan_azi=1e32 $
        else tan_azi=sqrt( (sin(!PI*psi/180.0)*sin(!PI*psi/180.0))/ $
                ((cos(!PI*psi/180.0)*cos(!PI*psi/180.0))- $ 
                (sin(!PI*xel/180.0)*sin(!PI*xel/180.0))))
    
        if (psi gt 0) then azi=atan(tan_azi)*180.0/!PI $
        else azi=-atan(tan_azi)*180.0/!PI

        ; Obtain the corresponding value of pointing azimuth
        xal=azi+bore

        ; Adjust azimuth and elevation for the oblateness of the Earth
        RadarGeoCnvrt,gdlat,gdlon,xal,xel,ral,dum
    
        ; Obtain the global spherical coordinates of the field point
        RadarFldPnt,rrho,rlat,rlon,ral,rel,r,frho,flat,flon

        ; Recomputes the radius of the Earth beneath the field point
        RadarGeoTGC,-1,dum1,dum2,frad,flat,flon,dum3

        ; Check altitude
        fhx=frho-frad    
  
    endrep until (abs(fhx-xh) le 0.5)

end
 

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       RadarFldPnthGS
;
; PURPOSE:
;       Internal function for RPos
;       
;
; CALLING SEQUENCE:
;       RadarFldPnthGS,gdlat,gdlon,psi,bor,fh,r,frho,flat,flon
;
;-----------------------------------------------------------------
;


pro RadarFldPnthGS,gdlat,gdlon,psi,bore,fh,r,frho,flat,flon
 
  rrad=0.0D
  rlat=0.0D
  rlon=0.0D
  del=0.0D 
  dum1=0.0D
  dum2=0.0D
  dum3=0.0D

  if (fh le 150) then xh=fh $
  else begin
    if (r le 300) then xh=115 $
    else if ((r gt 300) and (r lt 500)) then $
      xh=(r-300.0D)/200.0D*(fh-115.0D)+115.0D $
    else xh=fh
  endelse

  if (r lt 150) then xh=(r/150.0D)*115.0D;
  RadarGeoTGC,1,gdlat,gdlon,rrad,rlat,rlon,del

  rrho=rrad
  frad=rrad
 

  repeat begin 
    frho=frad+xh

    rel=asin(((frho*frho)-(rrad*rrad)-(r*r))/(2*rrad*r))*180.0/!PI
  
    xel=rel

    if (((cos(!PI*psi/180.0)*cos(!PI*psi/180.0))- $
         (sin(!PI*xel/180.0)*sin(!PI*xel/180.0))) lt 0) then tan_azi=1e32 $
      else tan_azi=sqrt( (sin(!PI*psi/180.0)*sin(!PI*psi/180.0))/ $
                ((cos(!PI*psi/180.0)*cos(!PI*psi/180.0))- $ 
                (sin(!PI*xel/180.0)*sin(!PI*xel/180.0))))
    if (psi gt 0) then azi=atan(tan_azi)*1.0*180/!PI $
    else azi=-atan(tan_azi)*180/!PI

    xal=azi+bore

    RadarGeoCnvrt,gdlat,gdlon,xal,xel,ral,dum
    RadarFldPnt,rrho,rlat,rlon,ral,rel,r,frho,flat,flon
    RadarGeoTGC,-1,dum1,dum2,frad,flat,flon,dum3
    fhx=frho-frad 
   
  endrep until (abs(fhx-xh) le 0.5) 
end




;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       RadarPos
;
; PURPOSE:
;       Convert a range/beam coordinatee to geographic position
;       
;
; CALLING SEQUENCE:
;       s=RadarPos(center,bcrd,rcrd,site,frang,rsep,rxrise,
;                      height,rho,lat,lng)
;
;       This function converts a range/beam coordinate to 
;       geographic positiion. The range (starting at zero) is 
;       given by rcrd, the beam by bcrd. The radar hardware
;       table is given by the structure site. The position
;       of the first range gate in kilometers is given by
;       frang and the range seperation in kilometers is given
;       by rsep. The receiver rise time is given by rxrise, if
;       this value is zero then the rise time is taken from
;       the parameter block. The height of the transformation is
;       given by height, if this value is less than 90 then it
;       is asuumed to be the elevation angle from the radar.
;       If center is not equal to zero, then the calculation is
;       assumed to be for the center of the cell, not the edge.       
;
;       The calculated values are returned in rho, lat and lng
;       The returned value is zero.
;-----------------------------------------------------------------
;

 
function RadarPos,center,bcrd,rcrd,site,frang,rsep,rxrise,$
                        height,rho,lat,lng,chisham=chisham


                
  re=6356.779D
  bm_edge=0.0D;
  range_edge=0.0D

  offset=site.maxbeam/2.0-0.5

  if  N_ELEMENTS(bcrd) ne 1 then begin

     n=N_ELEMENTS(bcrd)

     if N_ELEMENTS(rcrd) ne n then begin
        message, "Beam and range arrays must be of equal length"
     end

     if (N_ELEMENTS(frang) ne 1) && (N_ELEMENTS(frang) ne n) then begin
        message, "Beam and frang arrays must be of equal length"
     end

     if (N_ELEMENTS(rsep) ne 1) && (N_ELEMENTS(rsep) ne n) then begin
        message, "Beam and rsep arrays must be of equal length"
     end

     if (N_ELEMENTS(rxrise) ne 1) && (N_ELEMENTS(rxrise) ne n) then begin
        message, "Beam and rxrise arrays must be of equal length"
     end

     if (N_ELEMENTS(height) ne 1) && (N_ELEMENTS(height) ne n) then begin
        message, "Beam and height arrays must be of equal length"
     end

     sze=SIZE(bcrd)

     rho=dblarr(sze[1:sze[0]])
     lat=dblarr(sze[1:sze[0]])
     lng=dblarr(sze[1:sze[0]])
 
     if (center eq 0) then bm_edge=-site.bmsep*0.5;
    
     if (N_ELEMENTS(frang) eq 1) then fr=frang
     if (N_ELEMENTS(rsep) eq 1) then begin 
       if (center eq 0) then range_edge=-0.5*rsep*20.0/3.0
       rs=rsep
     endif

     if N_ELEMENTS(rxrise) eq 1 then begin
       if (rxrise eq 0) then rx=site.recrise $
       else rx=rxrise
     endif
     if N_ELEMENTS(height) eq 1 then hgt=height

     for i=0,n-1 do begin
        if N_ELEMENTS(frang) ne 1 then fr=frang[i]
        if N_ELEMENTS(rsep) ne 1 then begin 
          if (center eq 0) then range_edge=-0.5*rsep*20.0/3.0
          rs=rsep[i]
        endif
        if N_ELEMENTS(rxrise) ne 1 then rx=rxrise[i]
        if N_ELEMENTS(height) ne 1 then hgt=height[i]
      

       psi=site.bmsep*(bcrd[i]-offset)+bm_edge+site.bmoff
       d=RadarSlantRange(fr,rs,rx,range_edge,rcrd[i]+1)

       if (hgt lt 90) then $
         hgt=-re+sqrt((re*re)+2*d*re*sin(!PI*hgt/180.0)+(d*d));

       RadarFldPnth,site.geolat,site.geolon,psi,site.boresite,hgt,$ 
                    d,r,la,ln,chisham=chisham 


       rho[i]=r
       lat[i]=la
       lng[i]=ln         

     endfor
  end else begin

    if (rxrise eq 0) then rx=site.recrise $
    else rx=rxrise

    if (center eq 0) then begin
      bm_edge=-site.bmsep*0.5;
      range_edge=-0.5*rsep*20.0/3.0
    endif
    
    rho=0.0D
    lat=0.0D
    lng=0.0D 


    psi=site.bmsep*(bcrd-offset)+bm_edge+site.bmoff
    d=RadarSlantRange(frang,rsep,rx,range_edge,rcrd+1)

    if (height lt 90) then $
      height=-re+sqrt((re*re)+2*d*re*sin(!PI*height/180.0)+(d*d));
    RadarFldPnth,site.geolat,site.geolon,psi,site.boresite,height,d,rho,lat,lng,chisham=chisham
  endelse

  return, 0
end

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       RadarPosGS
;
; PURPOSE:
;       Convert a range/beam coordinates to geographic position for
;       ground scatter.
;       
;
; CALLING SEQUENCE:
;       RadarPosGS(center,bcrd,rcrd,site,frang,rsep,rxrise,
;                      height,rho,lat,lng)
;
;       This function converts a range/beam coordinate to 
;       geographic positiion for ground scatter
;       The range (starting at zero) is 
;       given by rcrd, the beam by bcrd. The radar hardware
;       table is given by the structure site. The position
;       of the first range gate in kilometers is given by
;       frang and the range seperation in kilometers is given
;       by rsep. The receiver rise time is given by rxrise, if
;       this value is zero then the rise time is taken from
;       the parameter block. The height of the transformation is
;       given by height, if this value is less than 90 then it
;       is asuumed to be the elevation angle from the radar.
;       If center is not equal to zero, then the calculation is
;       assumed to be for the center of the cell, not the edge.       
;
;       The calculated values are returned in rho, lat and lng
;       The returned value is zero.
;-----------------------------------------------------------------
;



function RadarPosGS,center,bcrd,rcrd,site,frang,rsep,rxrise,$
                        height,rho,lat,lng


                
  re=6356.779D
  bm_edge=0.0D;
  range_edge=0.0D

  offset=site.maxbeam/2.0-0.5

  if  N_ELEMENTS(bcrd) ne 1 then begin

     n=N_ELEMENTS(bcrd)

     if N_ELEMENTS(rcrd) ne n then begin
        message, "beam and range arrays must be of equal length"
     end

     if (N_ELEMENTS(frang) ne 1) && (N_ELEMENTS(frang) ne n) then begin
        message, "beam and frang arrays must be of equal length"
     end

     if (N_ELEMENTS(rsep) ne 1) && (N_ELEMENTS(rsep) ne n) then begin
        message, "beam and rsep arrays must be of equal length"
     end

     if (N_ELEMENTS(rxrise) ne 1) && (N_ELEMENTS(rxrise) ne n) then begin
        message, "beam and rxrise arrays must be of equal length"
     end

     if (N_ELEMENTS(height) ne 1) && (N_ELEMENTS(height) ne n) then begin
        message, "beam and height arrays must be of equal length"
     end

     sze=SIZE(bcrd)

     rho=dblarr(sze[1:sze[0]])
     lat=dblarr(sze[1:sze[0]])
     lng=dblarr(sze[1:sze[0]])
 
     if (center eq 0) then bm_edge=-site.bmsep*0.5;
    
     if (N_ELEMENTS(frang) eq 1) then fr=frang
     if (N_ELEMENTS(rsep) eq 1) then begin 
       if (center eq 0) then re=-0.5*rsep*20.0/3.0
       rs=rsep
     endif

     if N_ELEMENTS(rxrise) eq 1 then begin
       if (rxrise eq 0) then rx=site.recrise $
       else rx=rxrise
     endif
     if N_ELEMENTS(height) eq 1 then hgt=height

     for i=0,n-1 do begin
        if N_ELEMENTS(frang) ne 1 then fr=frang[i]
        if N_ELEMENTS(rsep) ne 1 then begin 
          if (center eq 0) then re=-0.5*rsep*20.0/3.0
          rs=rsep[i]
        endif
        if N_ELEMENTS(rxrise) ne 1 then rx=rxrise[i]
        if N_ELEMENTS(height) ne 1 then hgt=height[i]
      

       psi=site.bmsep*(bcrd[i]-offset)+bm_edge+site.bmoff
       d=RadarSlantRange(fr,rs,rx,range_edge,rcrd[i]+1)/2.0D

       if (hgt lt 90) then $
         hgt=-re+sqrt((re*re)+2*d*re*sin(!PI*hgt/180.0)+(d*d));

       RadarFldPnthGS,site.geolat,site.geolon,psi,site.boresite,hgt,$ 
                    d,r,la,ln 


       rho[i]=r
       lat[i]=la
       lng[i]=ln         

     endfor
  end else begin

    if (rxrise eq 0) then rx=site.recrise $
    else rx=rxrise

    if (center eq 0) then begin
      bm_edge=-site.bmsep*0.5;
      range_edge=-0.5*rsep*20.0/3.0
    endif
    
    rho=0.0D
    lat=0.0D
    lng=0.0D 


    psi=site.bmsep*(bcrd-offset)+bm_edge+site.bmoff
    d=RadarSlantRange(frang,rsep,rx,range_edge,rcrd+1)/2.0D

    if (height lt 90) then $
      height=-re+sqrt((re*re)+2*d*re*sin(!PI*height/180.0)+(d*d));
    RadarFldPnthGS,site.geolat,site.geolon,psi,site.boresite,height,d,$ 
                   rho,lat,lng 
  endelse

  return, 0
end

