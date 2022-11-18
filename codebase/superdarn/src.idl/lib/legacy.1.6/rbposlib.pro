; rbposlib.pro
; ============
; Author: K.Baker, R.J.Barnes & D. Andre
; 
;  Copyright (c) 2012 The Johns Hopkins University/Applied Physics Laboratory
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
; Public Functions
; ----------------
; rbpos
;
; ---------------------------------------------------------------


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       rbpos
;
; PURPOSE:
;
;       Calculate the geographic or geomagnetic position of radar range/beam
;       cells.
;
; CALLING SEQUENCE:
;
;       pos = rbpos(range,[height=height],[station=station_id],
;           [beam=beam_number],[lagfr=lag_to_first_range],
;           [smsep=sample_separation],[data=data_ptr],[/CENTER],[/GEO],
;           [/CHISHAM])
;
;           inputs:  the range number (first range =1).  This may be
;                   a vector containing a list of ranges.
;               if the height is not specified a value of 300 km is
;                   used.
;               the following keywords specify the station id,
;                   beam number, lag to the first range,
;                   and lag separation:  "station",
;                   "beam", "lagfr", "smsep".  If these keywords
;                   are not specified, their values are taken from
;                   the data structure pointed to by the
;                   keyword "data" or from "fit_data" if no
;                   data structure is specified.
;               if the keyword data is given a value, the information
;                   on the bmnum, smsep, etc. is taken from the
;                   structure pointed to by that keyword.
;                   Otherwise, the data structure "fit_data"
;                   is assummed.
;               if the keyword CENTER is set, only the position of
;                   the center of the cell is return
;               if the keyword GEO is set, the coordinates are
;                   returned in geographic, otherwise PACE geomagnetic
;                   coordinates are used.
;               if the keyword CHISHAM is set, the Chisham virtual height
;                   model is used, otherwise the standard virtual
;                   height model is used.
;
;------------------------------------------------------------------------------
;

function rbpos,range,height=height,beam=beam,lagfr=first_lag,smsep=smsp, $
            rxrise=rxrs,station=station,data=dptr,center=center,geo=geo, $
            year=yr,yrsec=yrs,chisham=chisham

  common fitdata_com, fitfileptr, fit_data
  common radarinfo, network

; load the hardware data if we haven't done so already

 if (n_elements(network) eq 0) then begin
    rname=getenv('SD_RADAR')
    if (strlen(rname) eq 0) then begin
      print, 'Environment Variable SD_RADAR must be defined.'
      stop
    endif

    openr,inp,rname,/get_lun,/stdio
    network=RadarLoad(inp)
    free_lun,inp

    hname=getenv('SD_HDWPATH')
    if (strlen(hname) eq 0) then begin
      print, 'Environment Variable SD_HDWPATH must be defined.'
      stop
    endif
    s=RadarLoadHardware(network,path=getenv('SD_HDWPATH'))
    if (s ne 0) then begin
      print, 'Could not load hardware information'
      stop
    endif
  endif


;
;  check the keywords that set radar parameters
;

  
  if (keyword_set(height)) then h = float(height) else h= 300.
  if (keyword_set(dptr)) then dp = dptr else dp = fit_data
  if (keyword_set(beam)) then bmnum = beam else bmnum=dp.p.bmnum
  if (keyword_set(first_lag)) then lagfr=first_lag else lagfr=dp.p.lagfr
  if (keyword_set(smsp)) then smsep = smsp else smsep=dp.p.smsep

  if (keyword_set(rxrs)) then rxrise = rxrs else rxrise=dp.p.rxrise

  if (keyword_set(station)) then st_id = station else st_id = dp.p.st_id

  if (keyword_set(yr)) then year=yr else year=dp.p.year
  if (n_elements(yrs)) then yrsec=yrs else $
      yrsec=TimeYMDHMSToYrsec(dp.p.year,dp.p.month,dp.p.day,dp.p.hour, $
                                  dp.p.minut,dp.p.sec)

  yr=year
  s=TimeYrsecToYMDHMS(yr,mo,dy,hr,mt,sc,yrsec)

  rid=RadarGetRadar(network,st_id)
  s=TimeYrsecToYMDHMS(yr,mo,dy,hr,mt,sc,yrsec)
  site=RadarYMDHMSGetSite(rid,yr,mo,dy,hr,mt,sc)

  frang=lagfr*0.15
  rsep=smsep*0.15

;
;       if the center keyword is set then we return a 3 element array,
;       otherwise we return an array of 3,2,2
;

  if (keyword_set(center)) then pos=fltarr(3,n_elements(range)) else $
     pos=fltarr(3,2,2,n_elements(range))

  if (keyword_set(geo)) then mgflag = 0 else mgflag = 1
  if (keyword_set(center)) then cflag = 1 else cflag = 0
  pos1 = fltarr(3,2,2)



  for i=0, n_elements(range)-1 do begin
    if n_elements(range) EQ 1 then r = fix(range) else r=fix(range[i])
    if (cflag eq 1) then begin
      s=RadarPos(1,bmnum,r-1,site,frang,rsep,rxrise,h,rho,lat,lon,chisham=chisham)
      if (mgflag eq 1) then begin
        s=AACGMConvert(lat,lon,h,mlat,mlon,rad)
        lat=mlat
        lon=mlon
      endif
      pos1[0,0,0]=lat
      pos1[1,0,0]=lon
      pos1[2,0,0]=rho
    endif else begin
      s=RadarPos(0,bmnum,r-1,site,frang,rsep,rxrise,h,rho,lat,lon,chisham=chisham)
      if (mgflag eq 1) then begin
        s=AACGMConvert(lat,lon,h,mlat,mlon,rad)
        lat=mlat
        lon=mlon
      endif
      pos1[0,0,0]=lat
      pos1[1,0,0]=lon
      pos1[2,0,0]=rho
      s=RadarPos(0,bmnum+1,r-1,site,frang,rsep,rxrise,h,rho,lat,lon,chisham=chisham)
      if (mgflag eq 1) then begin
        s=AACGMConvert(lat,lon,h,mlat,mlon,rad)
        lat=mlat
        lon=mlon
      endif
        pos1[0,1,0]=lat
        pos1[1,1,0]=lon
        pos1[2,1,0]=rho
        s=RadarPos(0,bmnum,r,site,frang,rsep,rxrise,h,rho,lat,lon,chisham=chisham)
      if (mgflag eq 1) then begin
        s=AACGMConvert(lat,lon,h,mlat,mlon,rad)
        lat=mlat
        lon=mlon
      endif
        pos1[0,0,1]=lat
        pos1[1,0,1]=lon
        pos1[2,0,1]=rho
        s=RadarPos(0,bmnum+1,r,site,frang,rsep,rxrise,h,rho,lat,lon,chisham=chisham)
        if (mgflag eq 1) then begin
          s=AACGMConvert(lat,lon,h,mlat,mlon,rad)
          lat=mlat
          lon=mlon
       endif
        pos1[0,1,1]=lat
        pos1[1,1,1]=lon
        pos1[2,1,1]=rho
      endelse
    if (n_elements(range) GT 1) then $
      if (keyword_set(center)) then pos[*,i]=pos1[*,0,0] else $
      pos[*,*,*,i]=pos1 else $
      if (keyword_set(center)) then pos=pos1[*,0,0] else $
      pos = pos1
  endfor
  pos=reform(pos)
  return,pos
end

