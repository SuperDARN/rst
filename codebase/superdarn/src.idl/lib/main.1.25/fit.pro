; fit.pro
; =======
; Author: R.J.Barnes
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
;      2022-02-02 Emma Bland (UNIS): Added "elv_error" and "elv_fitted" fields to support FitACF v3
; 
; Public Functions
; ----------------
;
; FitLoadInx          
; FitRead               
; FitWrite             
; FitSeek            
; FitOpen              
; FitClose

; Private Functions
; ---------------
;

; FitMakeFitData
; FitMakeFitInx
; FitPoint
; FitGetTime

;
; ---------------------------------------------------------------


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       FitMakeFitData
;
; PURPOSE:
;       Create a structure to store fitACF data.
;       
;
; CALLING SEQUENCE:
;       FitMakeFitData,fit
;
;       This procedure creates a structure to store the fitACF data,
;       the structure is returns in fitdata.
;
;
;-----------------------------------------------------------------
;


pro FitMakeFitData,fit

  MAX_RANGE=800

  fit={FitData, $
         algorithm: ' ', $
         revision: {rlstr, major: 0L, minor: 0L}, $ 
         noise: {nfstr, sky: 0.0, lag0: 0.0, vel: 0.0}, $
         pwr0: fltarr(MAX_RANGE), $
         nlag: intarr(MAX_RANGE), $
         qflg: bytarr(MAX_RANGE), $
         gflg: bytarr(MAX_RANGE), $
         p_l:  fltarr(MAX_RANGE), $ 
         p_l_e: fltarr(MAX_RANGE), $
         p_s: fltarr(MAX_RANGE), $
         p_s_e: fltarr(MAX_RANGE), $
         v: fltarr(MAX_RANGE), $
         v_e: fltarr(MAX_RANGE), $
         w_l: fltarr(MAX_RANGE), $
         w_l_e: fltarr(MAX_RANGE), $
         w_s: fltarr(MAX_RANGE), $
         w_s_e: fltarr(MAX_RANGE), $
         sd_l: fltarr(MAX_RANGE), $
         sd_s: fltarr(MAX_RANGE), $
         sd_phi: fltarr(MAX_RANGE), $
         x_qflg: bytarr(MAX_RANGE), $
         x_gflg: bytarr(MAX_RANGE), $
         x_p_l: fltarr(MAX_RANGE), $
         x_p_l_e: fltarr(MAX_RANGE), $
         x_p_s: fltarr(MAX_RANGE), $
         x_p_s_e: fltarr(MAX_RANGE), $
         x_v: fltarr(MAX_RANGE), $
         x_v_e: fltarr(MAX_RANGE), $
         x_w_l: fltarr(MAX_RANGE), $
         x_w_l_e: fltarr(MAX_RANGE), $
         x_w_s: fltarr(MAX_RANGE), $
         x_w_s_e: fltarr(MAX_RANGE), $
         phi0: fltarr(MAX_RANGE), $
         phi0_e: fltarr(MAX_RANGE), $
         elv: fltarr(MAX_RANGE), $
         elv_fitted: fltarr(MAX_RANGE), $
         elv_error: fltarr(MAX_RANGE), $
         elv_low: fltarr(MAX_RANGE), $
         elv_high: fltarr(MAX_RANGE), $
         x_sd_l: fltarr(MAX_RANGE), $
         x_sd_s: fltarr(MAX_RANGE), $
         x_sd_phi: fltarr(MAX_RANGE) $

      }

end
   
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       FitRead
;
; PURPOSE:
;       Read a record from a fitacf file.
;       
;
; CALLING SEQUENCE:
;       status = FitRead(unit,prm,fit)
;
;       This function reads a single record from the open file with
;       logical unit number, unit. The structure prm and fit are
;       populated accordingly.
;
;
;       The returned value is size in bytes of the record on success, 
;       or  -1 for failure
;
;-----------------------------------------------------------------
;

function FitRead,unit,prm,fit

  s=DataMapRead(unit,sclvec,arrvec)
  if (s eq -1) then return,s
  

  RadarMakeRadarPrm,prm
  FitMakeFitData,fit

  s=RadarDecodeRadarPrm(prm,sclvec,arrvec)

  if (s eq -1) then begin 
   print,'File is in the wrong format!'
   st=DataMapFreeScalar(sclvec)
   st=DataMapFreeArray(arrvec)
   return,s
  endif

  sclname=['algorithm','fitacf.revision.major','fitacf.revision.minor', $
           'noise.sky','noise.lag0','noise.vel']

  scltype=[9,3,3,4,4,4]
  
  sclid=intarr(n_elements(sclname))
  sclid[*]=-1

  arrname=['slist','pwr0','nlag','qflg','gflg', $
           'p_l','p_l_e','p_s','p_s_e','v','v_e','w_l','w_l_e','w_s', $
           'w_s_e','sd_l','sd_s','sd_phi', $
           'x_qflg','x_gflg','x_p_l','x_p_l_e','x_p_s','x_p_s_e','x_v', $
           'x_v_e','x_w_l','x_w_l_e','x_w_s','x_w_s_e','phi0','phi0_e', $
           'elv','elv_fitted','elv_error','elv_low','elv_high','x_sd_l','x_sd_s','x_sd_phi']


  arrtype=[2,4,2,1,1,4,4,4,4,4,4,4,4,4,4,4,4,4,1,1,4,4,4,4,4, $
           4,4,4,4,4,4,4,4,4,4,4,4,4,4,4]

  arrid=intarr(n_elements(arrname))  
  arrid[*]=-1
  
  if (n_elements(sclvec) ne 0) then begin
    for n=0,n_elements(sclname)-1 do $
      sclid[n]=DataMapFindScalar(sclname[n],scltype[n],sclvec)
  endif

  if (n_elements(arrvec) ne 0) then begin
    for n=0,n_elements(arrname)-1 do $
      arrid[n]=DataMapFindArray(arrname[n],arrtype[n],arrvec)
  endif
   
  q=where(sclid eq -1,count)

  if (count ne 0) then begin
    print,'File is in the wrong format!'
    st=DataMapFreeScalar(sclvec)
    st=DataMapFreeArray(arrvec)
    return, -2
  endif
 
  ; populate the structures

  if (sclid[0] ne -1) then fit.algorithm=*(sclvec[sclid[0]].ptr)
  fit.revision.major=*(sclvec[sclid[1]].ptr)
  fit.revision.minor=*(sclvec[sclid[2]].ptr)
  fit.noise.sky=*(sclvec[sclid[3]].ptr)
  fit.noise.lag0=*(sclvec[sclid[4]].ptr)
  fit.noise.vel=*(sclvec[sclid[5]].ptr)

  if (prm.nrang gt 0) then fit.pwr0[0:prm.nrang-1]=*(arrvec[arrid[1]].ptr)

  if (arrid[0] eq -1) then begin
    st=DataMapFreeScalar(sclvec)
    st=DataMapFreeArray(arrvec)
    return, s
  endif

  if ~(ptr_valid(arrvec[arrid[0]].ptr)) then begin
     st=DataMapFreeScalar(sclvec)
     st=DataMapFreeArray(arrvec)
     return, s
  endif

  slist=*(arrvec[arrid[0]].ptr)
  if (n_elements(slist) eq 0) then begin
     st=DataMapFreeScalar(sclvec)
     st=DataMapFreeArray(arrvec)
    return,s
  endif
  
  fit.nlag[slist]= (*(arrvec[arrid[2]].ptr))[*]
  fit.qflg[slist]= (*(arrvec[arrid[3]].ptr))[*]
  fit.gflg[slist]= (*(arrvec[arrid[4]].ptr))[*]
  fit.p_l[slist]= (*(arrvec[arrid[5]].ptr))[*]
  fit.p_l_e[slist]= (*(arrvec[arrid[6]].ptr))[*]
  fit.p_s[slist]= (*(arrvec[arrid[7]].ptr))[*]
  fit.p_s_e[slist]= (*(arrvec[arrid[8]].ptr))[*]
  fit.v[slist]= (*(arrvec[arrid[9]].ptr))[*]
  fit.v_e[slist]= (*(arrvec[arrid[10]].ptr))[*]
  fit.w_l[slist]= (*(arrvec[arrid[11]].ptr))[*]
  fit.w_l_e[slist]= (*(arrvec[arrid[12]].ptr))[*]
  fit.w_s[slist]= (*(arrvec[arrid[13]].ptr))[*]
  fit.w_s_e[slist]= (*(arrvec[arrid[14]].ptr))[*]
  fit.sd_l[slist]= (*(arrvec[arrid[15]].ptr))[*]
  fit.sd_s[slist]= (*(arrvec[arrid[16]].ptr))[*]
  fit.sd_phi[slist]= (*(arrvec[arrid[17]].ptr))[*]
  
  if (prm.xcf ne 0) then begin
    
    ; XCF fitted parameters for FitACF 3
    ;   NB: fit.revision.major has values of 4 and 5 in 
    ;       some historical data. The logic of the if
    ;       statement below should be changed if a new major
    ;       version of FitACF is created in the future
    if (fit.revision.major eq 3) then begin
      fit.phi0[slist]= (*(arrvec[arrid[30]].ptr))[*]
      fit.phi0_e[slist]= (*(arrvec[arrid[31]].ptr))[*]
      fit.elv[slist]= (*(arrvec[arrid[32]].ptr))[*]
      fit.elv_fitted[slist]= (*(arrvec[arrid[33]].ptr))[*]
      fit.elv_error[slist]= (*(arrvec[arrid[34]].ptr))[*]
      fit.x_sd_phi[slist]= (*(arrvec[arrid[39]].ptr))[*]
    endif else begin
    
    ; XCF fitted parameters for FitACF 1-2
      if (arrid[18] ne -1) then begin
        fit.x_qflg[slist]= (*(arrvec[arrid[18]].ptr))[*]
        fit.x_gflg[slist]= (*(arrvec[arrid[19]].ptr))[*]
        fit.x_p_l[slist]= (*(arrvec[arrid[20]].ptr))[*]
        fit.x_p_l_e[slist]= (*(arrvec[arrid[21]].ptr))[*]
        fit.x_p_s[slist]= (*(arrvec[arrid[22]].ptr))[*]
        fit.x_p_s_e[slist]= (*(arrvec[arrid[23]].ptr))[*]
        fit.x_v[slist]= (*(arrvec[arrid[24]].ptr))[*]
        fit.x_v_e[slist]= (*(arrvec[arrid[25]].ptr))[*]
        fit.x_w_l[slist]= (*(arrvec[arrid[26]].ptr))[*]
        fit.x_w_l_e[slist]= (*(arrvec[arrid[27]].ptr))[*]
        fit.x_w_s[slist]= (*(arrvec[arrid[28]].ptr))[*]
        fit.x_w_s_e[slist]= (*(arrvec[arrid[29]].ptr))[*]
        fit.phi0[slist]= (*(arrvec[arrid[30]].ptr))[*]
        fit.phi0_e[slist]= (*(arrvec[arrid[31]].ptr))[*]
        fit.elv[slist]= (*(arrvec[arrid[32]].ptr))[*]
        fit.elv_low[slist]= (*(arrvec[arrid[35]].ptr))[*]
        fit.elv_high[slist]= (*(arrvec[arrid[36]].ptr))[*]
        fit.x_sd_l[slist]= (*(arrvec[arrid[37]].ptr))[*]
        fit.x_sd_s[slist]= (*(arrvec[arrid[38]].ptr))[*]
        fit.x_sd_phi[slist]= (*(arrvec[arrid[39]].ptr))[*]
      endif
    endelse
    
  endif
  st=DataMapFreeScalar(sclvec)
  st=DataMapFreeArray(arrvec)

  return,s
end


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       FitWrite
;
; PURPOSE:
;       Write a record from a fitacf file.
;       
;
; CALLING SEQUENCE:
;       status = FitWrite(unit,prm,fit)
;
;       This function writes a single record to the open file with
;       logical unit number, unit.
;
;
;       The returned value is size in bytes of the record on success, 
;       or  -1 for failure
;
;-----------------------------------------------------------------
;

function FitWrite,unit,prm,fit

  s=RadarEncodeRadarPrm(prm,sclvec,arrvec,/new)

  s=DataMapMakeScalar('algorithm',fit.algorithm,sclvec)
  s=DataMapMakeScalar('fitacf.revision.major',fit.revision.major,sclvec)
  s=DataMapMakeScalar('fitacf.revision.minor',fit.revision.minor,sclvec)
  s=DataMapMakeScalar('noise.sky',fit.noise.sky,sclvec)
  s=DataMapMakeScalar('noise.lag0',fit.noise.lag0,sclvec)
  s=DataMapMakeScalar('noise.vel',fit.noise.vel,sclvec)

  s=DataMapMakeArray('pwr0',fit.pwr0[0:prm.nrang-1],arrvec)

  q=fit.qflg[0:prm.nrang-1]+fit.x_qflg[0:prm.nrang-1]

  slist=fix(where(q gt 0,count))

  if (count ne 0) then begin

    s=DataMapMakeArray('slist',slist,arrvec)
    
    s=DataMapMakeArray('nlag',fit.nlag[slist],arrvec)

    s=DataMapMakeArray('qflg',fit.qflg[slist],arrvec)
    s=DataMapMakeArray('gflg',fit.gflg[slist],arrvec)
    s=DataMapMakeArray('p_l',fit.p_l[slist],arrvec)
    s=DataMapMakeArray('p_l_e',fit.p_l_e[slist],arrvec)
    s=DataMapMakeArray('p_s',fit.p_s[slist],arrvec)
    s=DataMapMakeArray('p_s_e',fit.p_s_e[slist],arrvec)
    s=DataMapMakeArray('v',fit.v[slist],arrvec)
    s=DataMapMakeArray('v_e',fit.v_e[slist],arrvec)
    s=DataMapMakeArray('w_l',fit.w_l[slist],arrvec)
    s=DataMapMakeArray('w_l_e',fit.w_l_e[slist],arrvec)
    s=DataMapMakeArray('w_s',fit.w_s[slist],arrvec)
    s=DataMapMakeArray('w_s_e',fit.w_s_e[slist],arrvec)
    s=DataMapMakeArray('sd_l',fit.sd_l[slist],arrvec)
    s=DataMapMakeArray('sd_s',fit.sd_s[slist],arrvec)
    s=DataMapMakeArray('sd_phi',fit.sd_phi[slist],arrvec)

    if (prm.xcf eq 1) then begin
      s=DataMapMakeArray('x_qflg',fit.x_qflg[slist],arrvec)
      s=DataMapMakeArray('x_gflg',fit.x_gflg[slist],arrvec)
      s=DataMapMakeArray('x_p_l',fit.x_p_l[slist],arrvec)
      s=DataMapMakeArray('x_p_l_e',fit.x_p_l_e[slist],arrvec)
      s=DataMapMakeArray('x_p_s',fit.x_p_s[slist],arrvec)
      s=DataMapMakeArray('x_p_s_e',fit.x_p_s_e[slist],arrvec)
      s=DataMapMakeArray('x_v',fit.x_v[slist],arrvec)
      s=DataMapMakeArray('x_v_e',fit.x_v_e[slist],arrvec)
      s=DataMapMakeArray('x_w_l',fit.x_w_l[slist],arrvec)
      s=DataMapMakeArray('x_w_l_e',fit.x_w_l_e[slist],arrvec)
      s=DataMapMakeArray('x_w_s',fit.x_w_s[slist],arrvec)
      s=DataMapMakeArray('x_w_s_e',fit.x_w_s_e[slist],arrvec)
      s=DataMapMakeArray('phi0',fit.phi0[slist],arrvec)
      s=DataMapMakeArray('phi0_e',fit.phi0_e[slist],arrvec)
      s=DataMapMakeArray('elv',fit.elv[slist],arrvec)
      s=DataMapMakeArray('elv_fitted',fit.elv_fitted[slist],arrvec)
      s=DataMapMakeArray('elv_error',fit.elv_error[slist],arrvec)
      s=DataMapMakeArray('elv_low',fit.elv_low[slist],arrvec)
      s=DataMapMakeArray('elv_high',fit.elv_high[slist],arrvec)
      s=DataMapMakeArray('x_sd_l',fit.x_sd_l[slist],arrvec)
      s=DataMapMakeArray('x_sd_s',fit.x_sd_s[slist],arrvec)
      s=DataMapMakeArray('x_sd_phi',fit.x_sd_phi[slist],arrvec)
    endif
  endif

  s=DataMapWrite(unit,sclvec,arrvec)

  s=DataMapFreeScalar(sclvec)
  s=DataMapFreeArray(arrvec)
  return,s
end


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       FitMakeFitInx
;
; PURPOSE:
;       Create a structure to store fitACF index.
;
;
; CALLING SEQUENCE:
;       FitMakeFitInx,inx
;
;       This procedure creates a structure to store the fitacf index.
;
;
;-----------------------------------------------------------------
;

pro FitMakeFitIndex,inx,time,offset
  inx={FitInx, time: time, offset: offset}
end

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       FitLoadInx
;
; PURPOSE:
;       Read an index to a fitacf file.
;
;
; CALLING SEQUENCE:
;       status = FitLoadInx(unit,inx)
;
;       This function reads an index to a fitacf file.
;
;       The returned value is size in bytes of the record on success,
;       or  -1 for failure
;
;-----------------------------------------------------------------
;

function FitLoadInx,unit,inx

ON_IOERROR,iofail
    
  time=0.0D
  offset=0L
  s=0
  c=0L
  while (s eq 0) do begin
    readu,unit,time,offset
    FitMakeFitIndex,tinx,time,offset
    if (n_elements(inx) eq 0) then inx=tinx $
    else inx=[inx,tinx] 
  endwhile
 
  return, 0

iofail:
  return,0 


end

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       FitGetTime
;
; PURPOSE:
;       Extract the time from a fitacf record.
;
;
; CALLING SEQUENCE:
;       status = FitGetTime(sclvec)
;
;       This function extracts the time infornation from the scalar
;       vector array generated by reading a DataMap file.
;
;
;       The returned value is zero on success,
;       or  -1 for failure
;
;-----------------------------------------------------------------
;

function FitGetTime,sclvec
 
  sclname=['time.yr','time.mo','time.dy','time.hr', $
           'time.mt','time.sc','time.us']

  scltype=[2,2,2,2,2,2,3]

  sclid=intarr(n_elements(sclname))
  sclid[*]=-1

  if (n_elements(sclvec) ne 0) then begin
    for n=0,n_elements(sclname)-1 do $
      sclid[n]=DataMapFindScalar(sclname[n],scltype[n],sclvec)
  endif

  q=where(sclid eq -1,count)
  if (count ne 0) then begin
    print,'File is in the wrong format!'
    return, -1
  endif

  yr=*(sclvec[sclid[0]].ptr)
  mo=*(sclvec[sclid[1]].ptr)
  dy=*(sclvec[sclid[2]].ptr)
  hr=*(sclvec[sclid[3]].ptr)
  mt=*(sclvec[sclid[4]].ptr)
  sc=*(sclvec[sclid[5]].ptr)
  us=*(sclvec[sclid[6]].ptr)
  return, TimeYMDHMSToEpoch(yr,mo,dy,hr,mt,sc+us/1.0e6)
end



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       FitSeek
;
; PURPOSE:
;       Find a record in a fitacf file.
;
;
; CALLING SEQUENCE:
;       status = FitSeek(unit,yr,mo,dy,hr,mt,sc,inx,atme=atime)
;
;       This function searchs for a record in a fitacf file.
;
;       The returned value is zero on success,
;       or  -1 for failure
;
;-----------------------------------------------------------------
;


function FitSeek,unit,yr,mo,dy,hr,mt,sc,inx,atme=atme

  tval=TimeYMDHMSToEpoch(yr,mo,dy,hr,mt,sc);

  if (n_elements(inx) ne 0) then begin
   
    rec=0L
    prec=-1L
    num=n_elements(inx)
    stime=inx[0].time
    etime=inx[num-1].time
   
    srec=0L
    erec=num; 
    if (tval lt stime) then begin
      atme=stime
      point_lun,unit,inx[srec].offset
      return, 0
    endif else if (tval gt etime) then begin
      atme=stime
      point_lun,unit,inx[erec-1].offset
      return, -1
    endif 

   repeat begin
      prec=rec;
      rec=srec+fix(((tval-stime)*(erec-srec))/(etime-stime))
      if (inx[rec].time eq tval) then break
      if (inx[rec].time lt tval) then begin
         srec=rec
         stime=inx[rec].time
      endif else begin
         erec=rec
         etime=inx[rec].time
      endelse
    endrep until (prec eq rec)
    atme=inx[rec].time
    point_lun,unit,inx[rec].offset
    return,0
  endif else begin
     s=DataMapRead(unit,sclvec,arrvec)
     fptr=0L
     if (n_elements(sclvec) ne 0) then begin
       tfile=FitGetTime(sclvec)
       st=DataMapFreeScalar(sclvec)
       st=DataMapFreeArray(arrvec)
       if (tfile gt tval) then point_lun,unit,0 $
       else begin
          stat=fstat(unit)
          fptr=long(stat.cur_ptr)
      endelse
     endif else point_lun,unit,0
     atme=tfile
     repeat begin
       stat=fstat(unit)
       s=DataMapRead(unit,sclvec,arrvec)
       if (s eq -1) then break
       tfile=FitGetTime(sclvec)
      
       st=DataMapFreeScalar(sclvec)
       st=DataMapFreeArray(arrvec)
       if (tval ge tfile) then begin
         fptr=stat.cur_ptr
         atme=tfile
       endif
     endrep until (tval lt tfile)
     if (tval gt tfile) then return,-1
     point_lun,unit,fptr
    endelse

 
  return,0 
end


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       FitPoint
;
; PURPOSE:
;       Sets the file pointer position in a raw file.
;
;
; CALLING SEQUENCE:
;       status = FitPoint(unit,ptr)
;
;       This function sets the file pointer in a raw file.
;
;
;       The returned value is the offset on success,
;       or  -1 for failure
;
;-----------------------------------------------------------------
;

function FitPoint,unit,off

ON_IOERROR,iofail

  point_lun,unit,off
  return,off

iofail:
  return, -1

end
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       FitOpen
;
; PURPOSE:
;       Open a fitacf file.
;
;
; CALLING SEQUENCE:
;       unit = FitOpen(fname,/read,/write,/update)
;
;       This function opens a fitacf file.
;
;       The returned value is the logical unit number on success,
;       or  -1 for failure
;
;
;
;-----------------------------------------------------------------
;

function FitOpen,fname,read=read,write=write,update=update
  if KEYWORD_SET(read) then $
    openr,unit,fname,/GET_LUN,/SWAP_IF_BIG_ENDIAN
  if KEYWORD_SET(write) then $
     openw,unit,fname,/GET_LUN,/SWAP_IF_BIG_ENDIAN
  if KEYWORD_SET(update) then $
     openu,unit,fname,/GET_LUN,/SWAP_IF_BIG_ENDIAN
  return, unit
end

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       FitClose
;
; PURPOSE:
;       Close a fitacf file.
;
;
; CALLING SEQUENCE:
;       s = FitClose(unit)
;
;       This function closes a fitacf file.
;
;       The returned value is zero on success,
;       or  -1 for failure
;
;-----------------------------------------------------------------
;

function FitClose,unit

  free_lun,unit
  return,0

end
