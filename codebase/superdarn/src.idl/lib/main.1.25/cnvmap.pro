; cnvmap.pro
; ==========
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
; 
; Public Functions
; ----------------
;
; CnvMapOpen  
; CnvMapClose         
; CnvMapLoadInx          
; CnvMapRead          
; CnvMapWrite
; CnvMapSeek           

; PivateFunctions
; ---------------
;
;
; CnvMapMakePrm
; CnvMapMakeBnd                   
; CnvMapGetTime              
; CnvMapPoint
;
; ---------------------------------------------------------------



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       CnvMapMakePrm
;
; PURPOSE:
;       Create a structure to store cnvmap parameter data.
;       
;
; CALLING SEQUENCE:
;       CnvMapMakePrm,prm
;
;       This procedure creates a structure to store the grid 
;       parameters
;
;
;-----------------------------------------------------------------
;

pro CnvMapMakePrm,prm

  prm={CnvMapPrm, stme: {CnvMapTime,yr:0,mo:0,dy:0,hr:0,mt:0,sc:0D}, $
                  etme: {CnvMapTime,yr:0,mo:0,dy:0,hr:0,mt:0,sc:0D}, $
                  stnum: 0L, $
                  vcnum: 0L, $
                  xtd:0, $
                  major_rev:0,minor_rev:0, $
                  source: '', $
                  modnum: 0L,  $
                  doping_level: 0, $ 
                  model_wt: 0, $
                  error_wt: 0, $
                  imf_flag: 0, $
                  imf_delay: 0, $
                  Bx: 0.0D, $
                  By: 0.0D, $
                  Bz: 0.0D, $
                  Vx: 0.0D, $
                  tilt: 0.0D, $
                  Kp: 0.0D, $
                  imf_model: strarr(4), $
                  hemisphere: 0, $
                  noigrf: 0, $
                  fit_order: 0, $
                  latmin: 0.0, $
                  coefnum: 0L, $
                  chi_sqr: 0.0D, $
                  chi_sqr_dat: 0.0D, $
                  rms_err: 0.0D, $
                  lon_shft: 0.0, $
                  lat_shft: 0.0, $
                  mlt: {CnvMapMLT,st:0.0D,ed:0.0D,av:0.0D}, $
                  pot_drop:0.0D, $
                  pot_drop_err:0.0D, $
                  pot_max:0.0D, $
                  pot_max_err:0.0D, $
                  pot_min:0.0D, $
                  pot_min_err:0.0D, $
                  bndnum: 0L}
end


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       CnvMapMakeBnd
;
; PURPOSE:
;       Create a structure to store cnvmap parameter data.
;       
;
; CALLING SEQUENCE:
;       CnvMapMakePrm,prm
;
;       This procedure creates a structure to store the grid 
;       parameters
;
;
;-----------------------------------------------------------------
;

pro CnvMapMakeBnd,bvec

  bvec={CnvMapBND, lat:0.0, lon:0.0}

end


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       CnvMapRead
;
; PURPOSE:
;       Read a record from a cnvmap file.
;       
;
; CALLING SEQUENCE:
;       status = CnvMapRead(unit,prm,stvec,gvec,mvec,coef,bvec)
;
;       This function reads a single record from the open file with
;       logical unit number, unit. 
;
;
;       The returned value is size in bytes of the record on success, 
;       or  -1 for failure
;
;-----------------------------------------------------------------
;

function CnvMapRead,unit,prm,stvec,gvec,mvec,coef,bvec

  s=DataMapRead(unit,sclvec,arrvec)

  if (s eq -1) then return,s

  CnvMapMakePrm,prm 
  GridMakeStVec,stvec
  GridMakeGVec,gvec
  
  sclname=['start.year','start.month','start.day', $
          'start.hour','start.minute','start.second', $ 
          'end.year','end.month','end.day', $
          'end.hour','end.minute','end.second', $
          'map.major.revision','map.minor.revision', $ 
          'source', $
          'doping.level', $
          'model.wt', $
          'error.wt', $
          'IMF.flag', $
          'IMF.delay', $
          'IMF.Bx', $
          'IMF.By', $
          'IMF.Bz', $
          'IMF.Vx', $
          'IMF.tilt', $
          'IMF.Kp', $
          'model.angle', $
          'model.level', $
          'model.tilt', $
          'model.name', $
          'hemisphere', $
          'noigrf', $
          'fit.order', $
          'latmin', $
          'chi.sqr', $ 
          'chi.sqr.dat', $
          'rms.err', $
          'lon.shft', $
          'lat.shft', $
          'mlt.start', $
          'mlt.end', $
          'mlt.av', $
          'pot.drop', $
          'pot.drop.err', $
          'pot.max', $
          'pot.max.err', $
          'pot.min', $
          'pot.min.err']


  scltype=[2,2,2,2,2,8, $
           2,2,2,2,2,8, $
           2,2,9,2,2,2,2,2,8,8,8,8,8,8,9,9,9,9,2,2,2,4,8,8,8,4,4,8,8, $
           8,8,8,8,8,8,8]


  sclid=intarr(n_elements(sclname))
  sclid[*]=-1

  arrname=['stid','channel','nvec', $
           'freq','major.revision','minor.revision', $
           'program.id','noise.mean','noise.sd','gsct', $
           'v.min','v.max','p.min','p.max','w.min','w.max','ve.min', $
           've.max', $
           'vector.mlat','vector.mlon','vector.kvect','vector.srng', $
           'vector.stid','vector.channel','vector.index', $
           'vector.vel.median','vector.vel.sd', $
           'vector.pwr.median','vector.pwr.sd', $
           'vector.wdt.median','vector.wdt.sd', $
           'N','N+1','N+2','N+3', $
           'model.mlat','model.mlon','model.kvect','model.vel.median', $
           'boundary.mlat','boundary.mlon']
           

  arrtype=[2,2,2,4,2,2,2,4,4,2,4,4,4,4,4,4,4,4, $
           4,4,4,4,2,2,3,4,4,4,4,4,4,8,8,8,8,4,4,4,4,4,4]

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
  if (sclid[14] eq -1) then count=count-1
  if (sclid[23] eq -1) then count=count-1
  if (sclid[24] eq -1) then count=count-1
  if (sclid[25] eq -1) then count=count-1
  if (sclid[26] eq -1) then count=count-1
  if (sclid[27] eq -1) then count=count-1
  if (sclid[28] eq -1) then count=count-1
  if (sclid[29] eq -1) then count=count-1
  if (sclid[31] eq -1) then count=count-1

  if (count ne 0) then begin
    print,'File is in the wrong format!'
    return, -2
  endif

  q=where(arrid[0:17] eq -1,count)
  if (count ne 0) then begin
    print,'File is in the wrong format!'
    return, -2  
  endif

  ; populate the structures

  prm.stme.yr=*(sclvec[sclid[0]].ptr)
  prm.stme.mo=*(sclvec[sclid[1]].ptr)
  prm.stme.dy=*(sclvec[sclid[2]].ptr)
  prm.stme.hr=*(sclvec[sclid[3]].ptr)
  prm.stme.mt=*(sclvec[sclid[4]].ptr)
  prm.stme.sc=*(sclvec[sclid[5]].ptr)

  prm.etme.yr=*(sclvec[sclid[6]].ptr)
  prm.etme.mo=*(sclvec[sclid[7]].ptr)
  prm.etme.dy=*(sclvec[sclid[8]].ptr)
  prm.etme.hr=*(sclvec[sclid[9]].ptr)
  prm.etme.mt=*(sclvec[sclid[10]].ptr)
  prm.etme.sc=*(sclvec[sclid[11]].ptr)
 
  prm.major_rev=*(sclvec[sclid[12]].ptr)
  prm.minor_rev=*(sclvec[sclid[13]].ptr)
 
  if (sclid[14] ne -1) then prm.source=*(sclvec[sclid[14]].ptr)
   
  prm.doping_level=*(sclvec[sclid[15]].ptr)
  prm.model_wt=*(sclvec[sclid[16]].ptr)
  prm.error_wt=*(sclvec[sclid[17]].ptr)
  prm.imf_flag=*(sclvec[sclid[18]].ptr)
  prm.imf_delay=*(sclvec[sclid[19]].ptr)
  prm.Bx=*(sclvec[sclid[20]].ptr)
  prm.By=*(sclvec[sclid[21]].ptr)
  prm.Bz=*(sclvec[sclid[22]].ptr)
  if (sclid[23] ne -1) then prm.Vx=*(sclvec[sclid[23]].ptr)
  if (sclid[24] ne -1) then prm.tilt=*(sclvec[sclid[24]].ptr)
  if (sclid[25] ne -1) then prm.Kp=*(sclvec[sclid[25]].ptr)
      
  if (sclid[26] ne -1) then prm.imf_model[0]=*(sclvec[sclid[26]].ptr)
  if (sclid[27] ne -1) then prm.imf_model[1]=*(sclvec[sclid[27]].ptr)
  if (sclid[28] ne -1) then prm.imf_model[2]=*(sclvec[sclid[28]].ptr)
  if (sclid[29] ne -1) then prm.imf_model[3]=*(sclvec[sclid[29]].ptr)

  prm.hemisphere=*(sclvec[sclid[30]].ptr)
  if (sclid[31] ne -1) then prm.noigrf=*(sclvec[sclid[31]].ptr)
  prm.fit_order=*(sclvec[sclid[32]].ptr)
  prm.latmin=*(sclvec[sclid[33]].ptr)
  prm.chi_sqr=*(sclvec[sclid[34]].ptr)
  prm.chi_sqr_dat=*(sclvec[sclid[35]].ptr)
  prm.rms_err=*(sclvec[sclid[36]].ptr)
  prm.lon_shft=*(sclvec[sclid[37]].ptr)
  prm.lat_shft=*(sclvec[sclid[38]].ptr)
  prm.mlt.st=*(sclvec[sclid[39]].ptr)
  prm.mlt.ed=*(sclvec[sclid[40]].ptr)
  prm.mlt.av=*(sclvec[sclid[41]].ptr)
  prm.pot_drop=*(sclvec[sclid[42]].ptr)
  prm.pot_drop_err=*(sclvec[sclid[43]].ptr)
  prm.pot_max=*(sclvec[sclid[44]].ptr)
  prm.pot_max_err=*(sclvec[sclid[45]].ptr)
  prm.pot_min=*(sclvec[sclid[46]].ptr)
  prm.pot_min_err=*(sclvec[sclid[47]].ptr)
                
  prm.stnum=N_ELEMENTS(*(arrvec[arrid[0]].ptr))
  stvec=replicate(stvec,prm.stnum)

  stvec[*].st_id=(*(arrvec[arrid[0]].ptr))[*]
  stvec[*].chn=(*(arrvec[arrid[1]].ptr))[*]
  stvec[*].npnt=(*(arrvec[arrid[2]].ptr))[*]
  stvec[*].freq=(*(arrvec[arrid[3]].ptr))[*]
  stvec[*].major_revision=(*(arrvec[arrid[4]].ptr))[*]
  stvec[*].minor_revision=(*(arrvec[arrid[5]].ptr))[*]
  stvec[*].prog_id=(*(arrvec[arrid[6]].ptr))[*]
  stvec[*].noise.mean=(*(arrvec[arrid[7]].ptr))[*]
  stvec[*].noise.sd=(*(arrvec[arrid[8]].ptr))[*]
  stvec[*].gsct=(*(arrvec[arrid[9]].ptr))[*]
  stvec[*].vel.min=(*(arrvec[arrid[10]].ptr))[*]
  stvec[*].vel.max=(*(arrvec[arrid[11]].ptr))[*]
  stvec[*].pwr.min=(*(arrvec[arrid[12]].ptr))[*]
  stvec[*].pwr.max=(*(arrvec[arrid[13]].ptr))[*]
  stvec[*].wdt.min=(*(arrvec[arrid[14]].ptr))[*]
  stvec[*].wdt.max=(*(arrvec[arrid[15]].ptr))[*]
  stvec[*].verr.min=(*(arrvec[arrid[16]].ptr))[*]
  stvec[*].verr.max=(*(arrvec[arrid[17]].ptr))[*]

  if arrid[18] ne -1 then begin
    prm.vcnum=N_ELEMENTS(*(arrvec[arrid[18]].ptr))
    gvec=replicate(gvec,prm.vcnum)
    gvec[*].mlat=(*(arrvec[arrid[18]].ptr))[*]
    gvec[*].mlon=(*(arrvec[arrid[19]].ptr))[*]
    gvec[*].azm=(*(arrvec[arrid[20]].ptr))[*]
    gvec[*].srng=(*(arrvec[arrid[21]].ptr))[*]
    gvec[*].st_id=(*(arrvec[arrid[22]].ptr))[*]
    gvec[*].chn=(*(arrvec[arrid[23]].ptr))[*]
    gvec[*].index=(*(arrvec[arrid[24]].ptr))[*]
 
    gvec[*].vel.median=(*(arrvec[arrid[25]].ptr))[*]
    gvec[*].vel.sd=(*(arrvec[arrid[26]].ptr))[*]

    if arrid[27] ne -1 then begin
      prm.xtd=1
      gvec[*].pwr.median=(*(arrvec[arrid[27]].ptr))[*]
      gvec[*].pwr.sd=(*(arrvec[arrid[28]].ptr))[*]
      gvec[*].wdt.median=(*(arrvec[arrid[29]].ptr))[*]
      gvec[*].wdt.sd=(*(arrvec[arrid[30]].ptr))[*]
    endif
  endif

  if arrid[31] ne -1 then begin
    prm.coefnum=N_ELEMENTS(*(arrvec[arrid[31]].ptr))
    coef=dblarr(prm.coefnum,4)
    coef[*,0]=(*(arrvec[arrid[31]].ptr))[*]
    coef[*,1]=(*(arrvec[arrid[32]].ptr))[*]
    coef[*,2]=(*(arrvec[arrid[33]].ptr))[*]
    coef[*,3]=(*(arrvec[arrid[34]].ptr))[*]
  endif

  if arrid[35] ne -1 then begin
    GridMakeGVec,mvec
    prm.modnum=N_ELEMENTS(*(arrvec[arrid[35]].ptr))
    mvec=replicate(mvec,prm.modnum)
    mvec[*].mlat=(*(arrvec[arrid[35]].ptr))[*]
    mvec[*].mlon=(*(arrvec[arrid[36]].ptr))[*]
    mvec[*].azm=(*(arrvec[arrid[37]].ptr))[*] 
    mvec[*].vel.median=(*(arrvec[arrid[38]].ptr))[*]
  endif
  
  if arrid[39] ne -1 then begin
    CnvMapMakeBnd,bvec
    prm.bndnum=N_ELEMENTS(*(arrvec[arrid[39]].ptr))
    bvec=replicate(bvec,prm.bndnum)
    bvec[*].lat=(*(arrvec[arrid[39]].ptr))[*]
    bvec[*].lon=(*(arrvec[arrid[40]].ptr))[*]
  endif

  st=DataMapFreeScalar(sclvec)
  st=DataMapFreeArray(arrvec)

  return,s
  
end


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       CnvMapWrite
;
; PURPOSE:
;       Write a record to a cnvmap file.
;       
;
; CALLING SEQUENCE:
;       status = CnvMapWrite(unit,prm,stvec,gvec)
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


function CnvMapWrite,unit,prm,stvec,gvec,mvec,coef,bvec

  s=DataMapMakeScalar('start.year',prm.stme.yr,sclvec,/new)
  s=DataMapMakeScalar('start.month',prm.stme.mo,sclvec)
  s=DataMapMakeScalar('start.day',prm.stme.dy,sclvec)
  s=DataMapMakeScalar('start.hour',prm.stme.hr,sclvec)
  s=DataMapMakeScalar('start.minute',prm.stme.mt,sclvec)
  s=DataMapMakeScalar('start.second',prm.stme.sc,sclvec)
  s=DataMapMakeScalar('end.year',prm.etme.yr,sclvec)
  s=DataMapMakeScalar('end.month',prm.etme.mo,sclvec)
  s=DataMapMakeScalar('end.day',prm.etme.dy,sclvec)
  s=DataMapMakeScalar('end.hour',prm.etme.hr,sclvec)
  s=DataMapMakeScalar('end.minute',prm.etme.mt,sclvec)
  s=DataMapMakeScalar('end.second',prm.etme.sc,sclvec)


  s=DataMapMakeScalar('map.major.revision',prm.major_rev,sclvec)
  s=DataMapMakeScalar('map.minor.revision',prm.minor_rev,sclvec)

  if strlen(prm.source) ne 0 then  $
     s=DataMapMakeScalar('source',prm.source,sclvec)
 
  s=DataMapMakeScalar('doping.level',prm.doping_level,sclvec)
  s=DataMapMakeScalar('model.wt',prm.model_wt,sclvec)
  s=DataMapMakeScalar('error.wt',prm.error_wt,sclvec)
  s=DataMapMakeScalar('IMF.flag',prm.imf_flag,sclvec)
  s=DataMapMakeScalar('IMF.delay',prm.imf_delay,sclvec)
  s=DataMapMakeScalar('IMF.Bx',prm.Bx,sclvec)
  s=DataMapMakeScalar('IMF.By',prm.By,sclvec)
  s=DataMapMakeScalar('IMF.Bz',prm.Bz,sclvec)

  if strlen(prm.imf_model[0]) ne 0 then $
     s=DataMapMakeScalar('model.angle',prm.imf_model[0],sclvec)
  if strlen(prm.imf_model[1]) ne 0 then $
     s=DataMapMakeScalar('model.level',prm.imf_model[1],sclvec)

  s=DataMapMakeScalar('hemisphere',prm.hemisphere,sclvec)
  s=DataMapMakeScalar('fit.order',prm.fit_order,sclvec)
  s=DataMapMakeScalar('latmin',prm.latmin,sclvec)
  s=DataMapMakeScalar('chi.sqr',prm.chi_sqr,sclvec)
  s=DataMapMakeScalar('chi.sqr.dat',prm.chi_sqr_dat,sclvec)
  s=DataMapMakeScalar('rms.err',prm.rms_err,sclvec)
  s=DataMapMakeScalar('lon.shft',prm.lon_shft,sclvec)
  s=DataMapMakeScalar('lat.shft',prm.lat_shft,sclvec)

  s=DataMapMakeScalar('mlt.start',prm.mlt.st,sclvec)
  s=DataMapMakeScalar('mlt.end',prm.mlt.ed,sclvec)
  s=DataMapMakeScalar('mlt.av',prm.mlt.av,sclvec)

  s=DataMapMakeScalar('pot.drop',prm.pot_drop,sclvec)
  s=DataMapMakeScalar('pot.drop.err',prm.pot_drop_err,sclvec)

  s=DataMapMakeScalar('pot.max',prm.pot_max,sclvec)
  s=DataMapMakeScalar('pot.max.err',prm.pot_max_err,sclvec)

  s=DataMapMakeScalar('pot.min',prm.pot_min,sclvec)
  s=DataMapMakeScalar('pot.min.err',prm.pot_min_err,sclvec)

  stnum=prm.stnum  
  
  s=DataMapMakeArray('stid',stvec[0:stnum-1].st_id,arrvec,/new)
  s=DataMapMakeArray('channel',stvec[0:stnum-1].chn,arrvec)
  s=DataMapMakeArray('nvec',stvec[0:stnum-1].npnt,arrvec)
  s=DataMapMakeArray('freq',stvec[0:stnum-1].freq,arrvec)
  s=DataMapMakeArray('major.revision',stvec[0:stnum-1].major_revision,arrvec)
  s=DataMapMakeArray('minor.revision',stvec[0:stnum-1].minor_revision,arrvec)
  s=DataMapMakeArray('program.id',stvec[0:stnum-1].prog_id,arrvec)
  s=DataMapMakeArray('noise.mean',stvec[0:stnum-1].noise.mean,arrvec)
  s=DataMapMakeArray('noise.sd',stvec[0:stnum-1].noise.sd,arrvec)
  s=DataMapMakeArray('gsct',stvec[0:stnum-1].gsct,arrvec)
  s=DataMapMakeArray('v.min',stvec[0:stnum-1].vel.min,arrvec)
  s=DataMapMakeArray('v.max',stvec[0:stnum-1].vel.max,arrvec)
  s=DataMapMakeArray('p.min',stvec[0:stnum-1].pwr.min,arrvec)
  s=DataMapMakeArray('p.max',stvec[0:stnum-1].pwr.max,arrvec)
  s=DataMapMakeArray('w.min',stvec[0:stnum-1].wdt.min,arrvec)
  s=DataMapMakeArray('w.max',stvec[0:stnum-1].wdt.max,arrvec)
  s=DataMapMakeArray('ve.min',stvec[0:stnum-1].verr.min,arrvec)
  s=DataMapMakeArray('ve.max',stvec[0:stnum-1].verr.max,arrvec)

  vcnum=prm.vcnum  
  if vcnum ne 0 then begin
    s=DataMapMakeArray('vector.mlat',gvec[0:vcnum-1].mlat,arrvec)
    s=DataMapMakeArray('vector.mlon',gvec[0:vcnum-1].mlon,arrvec)
    s=DataMapMakeArray('vector.kvect',gvec[0:vcnum-1].azm,arrvec)
    s=DataMapMakeArray('vector.srng',gvec[0:vcnum-1].srng,arrvec)
    s=DataMapMakeArray('vector.stid',gvec[0:vcnum-1].st_id,arrvec)
    s=DataMapMakeArray('vector.channel',gvec[0:vcnum-1].chn,arrvec)
    s=DataMapMakeArray('vector.index',gvec[0:vcnum-1].index,arrvec)

    s=DataMapMakeArray('vector.vel.median',gvec[0:vcnum-1].vel.median,arrvec)
    s=DataMapMakeArray('vector.vel.sd',gvec[0:vcnum-1].vel.sd,arrvec)
    if prm.xtd ne 0 then begin
      s=DataMapMakeArray('vector.pwr.median',gvec[0:vcnum-1].pwr.median,arrvec)
      s=DataMapMakeArray('vector.pwr.sd',gvec[0:vcnum-1].pwr.sd,arrvec)
      s=DataMapMakeArray('vector.wdt.median',gvec[0:vcnum-1].wdt.median,arrvec)
      s=DataMapMakeArray('vector.wdt.sd',gvec[0:vcnum-1].wdt.sd,arrvec)
    endif
  endif

  coefnum=prm.coefnum
  if coefnum ne 0 then begin
    s=DataMapMakeArray('N',coef[0:coefnum-1,0],arrvec)
    s=DataMapMakeArray('N+1',coef[0:coefnum-1,1],arrvec)
    s=DataMapMakeArray('N+2',coef[0:coefnum-1,2],arrvec)
    s=DataMapMakeArray('N+3',coef[0:coefnum-1,3],arrvec)
  endif

  modnum=prm.modnum
  if modnum ne 0 then begin
    s=DataMapMakeArray('model.mlat',mvec[0:modnum-1].mlat,arrvec)
    s=DataMapMakeArray('model.mlon',mvec[0:modnum-1].mlon,arrvec)
    s=DataMapMakeArray('model.kvect',mvec[0:modnum-1].azm,arrvec)
    s=DataMapMakeArray('model.vel.median',mvec[0:modnum-1].vel.median,arrvec)
  endif


  bndnum=prm.bndnum
  if bndnum ne 0 then begin
    s=DataMapMakeArray('boundary.mlat',bvec[0:bndnum-1].lat,arrvec)
    s=DataMapMakeArray('boundary.mlon',bvec[0:bndnum-1].lon,arrvec)
  endif

  s=DataMapWrite(unit,sclvec,arrvec)
  s=DataMapFreeScalar(sclvec)
  s=DataMapFreeArray(arrvec)
  return,s
end


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       CnvMapMakeCnvMapInx
;
; PURPOSE:
;       Create a structure to store cnvmap index.
;
;
; CALLING SEQUENCE:
;       CnvMapMakeCnvMapInx,inx
;
;       This procedure creates a structure to store the cnvmap index.
;
;
;-----------------------------------------------------------------
;

pro CnvMapMakeCnvMapIndex,inx,time,offset
  inx={CnvMapInx, time: time, offset: offset}
end

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       CnvMapLoadInx
;
; PURPOSE:
;       Read an index to a cnvmap file.
;
;
; CALLING SEQUENCE:
;       status = CnvMapLoadInx(unit,inx)
;
;       This function reads an index to a cnvmap file.
;
;       The returned value is size in bytes of the record on success,
;       or  -1 for failure
;
;-----------------------------------------------------------------
;

function CnvMapLoadInx,unit,inx

ON_IOERROR,iofail
    
  time=0.0D
  offset=0L
  s=0
  c=0L
  while (s eq 0) do begin
    readu,unit,time,offset
    CnvMapMakeCnvMapIndex,tinx,time,offset
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
;       CnvMapGetTime
;
; PURPOSE:
;       Extract the time from a cnvmap record.
;
;
; CALLING SEQUENCE:
;       status = CnvMapGetTime(sclvec)
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

function CnvMapGetTime,sclvec
 
  sclname=['start.year','start.month','start.day','start.hour', $
           'start.minute','start.second']

  scltype=[2,2,2,2,2,8]

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


  return, TimeYMDHMSToEpoch(yr,mo,dy,hr,mt,sc)
end



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       CnvMapSeek
;
; PURPOSE:
;       Find a record in a cnvmap file.
;
;
; CALLING SEQUENCE:
;       status = CnvMapSeek(unit,yr,mo,dy,hr,mt,sc,inx,atme=atime)
;
;       This function searchs for a record in a cnvmap file.
;
;       The returned value is zero on success,
;       or  -1 for failure
;
;-----------------------------------------------------------------
;


function CnvMapSeek,unit,yr,mo,dy,hr,mt,sc,inx,atme=atme

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
       tfile=CnvMapGetTime(sclvec)
       st=DataMapFreeScalar(sclvec)
       st=DataMapFreeArray(arrvec)
       if (tfile gt tval) then point_lun,unit,0 $
       else begin
          stat=fstat(unit)
          fptr=stat.cur_ptr
      endelse
     endif else point_lun,unit,0
     atme=tfile
     repeat begin
       stat=fstat(unit)
       s=DataMapRead(unit,sclvec,arrvec)
       if (s eq -1) then break
       tfile=CnvMapGetTime(sclvec)
      
       st=DataMapFreeScalar(sclvec)
       st=DataMapFreeArray(arrvec)
       if (tval gt tfile) then begin
         fptr=stat.cur_ptr
         atme=tfile
       endif
     endrep until (tval le tfile)
     if (tval gt tfile) then return,-1
     point_lun,unit,fptr
    endelse

 
  return,0 
end




;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       CnvMapPoint
;
; PURPOSE:
;       Sets the file pointer position in a cnvmap file.
;
;
; CALLING SEQUENCE:
;       status = CnvMapPoint(unit,ptr)
;
;       This function sets the file pointer in a cnvmap file.
;
;
;       The returned value is the offset on success,
;       or  -1 for failure
;
;-----------------------------------------------------------------
;

function CnvMapPoint,unit,off

ON_IOERROR,iofail

  point_lun,unit,off
  return,off

iofail:
  return, -1

end

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       CnvMapOpen
;
; PURPOSE:
;       Open a cnvmap file.
;
;
; CALLING SEQUENCE:
;       unit = CnvMapOpen(fname,/read,/write,/update)
;
;       This function opens a cnvmap file.
;
;       The returned value is the logical unit number on success,
;       or  -1 for failure
;
;       This function is provided as a convenience and opens
;       the cnvmap file using the appropriate flags to to open.
;
;-----------------------------------------------------------------
;

function CnvMapOpen,fname,read=read,write=write,update=update

    if KEYWORD_SET(read) then openr,unit,fname,/GET_LUN,/SWAP_IF_BIG_ENDIAN
    if KEYWORD_SET(write) then openw,unit,fname,/GET_LUN, /SWAP_IF_BIG_ENDIAN
    if KEYWORD_SET(update) then openu,unit,fname,/GET_LUN, /SWAP_IF_BIG_ENDIAN
    return, unit
end

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       CnvMapClose
;
; PURPOSE:
;       Close a fitacf file.
;
;
; CALLING SEQUENCE:
;       s = CnvMapClose(unit)
;
;       This function closes a grdmap file.
;
;       The returned value is zero on success,
;       or  -1 for failure
;
;-----------------------------------------------------------------
;

function CnvMapClose,unit

  free_lun,unit
  return,0

end


