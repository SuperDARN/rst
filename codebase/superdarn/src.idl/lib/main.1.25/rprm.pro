; rprm.pro
; ========
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
; RadarMakeRadarPrm          
; RadarEncodeRadarPrm
; RadarDecodeRadarPrm
;
; ---------------------------------------------------------------



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       RadarMakeRadarPrm
;
; PURPOSE:
;       Create a structure to store the radar operating 
;       parameters.
;       
;
; CALLING SEQUENCE:
;       RadarMakeRadarPrm,prm
;
;       This procedure creates a structure to store the rawACF data,
;       the structure is returns in rawdata.
;
;
;-----------------------------------------------------------------
;


pro RadarMakeRadarPrm,prm

  PULSE_SIZE=64
  LAG_SIZE=150
  MAX_RANGE=800

  prm={RadarPrm, $
         revision: {rbstr, major: 0B, minor: 0B}, $ 
         origin: { ogstr, code: 0B, time: ' ', command: ' '}, $
         cp: 0, $
         stid: 0, $
         time: {tmstr, yr:0, $
                      mo:0, $
                      dy:0, $
                      hr:0, $
                      mt:0, $
                      sc:0, $
                      us:0L  $
               }, $
         txpow: 0, $
         nave: 0, $
         atten: 0, $
         lagfr: 0, $
         smsep: 0, $
         ercod: 0, $
         stat: {ststr, agc:0, lopwr:0}, $
         noise: {nsstr, search:0.0, mean:0.0}, $
         channel: 0, $
         bmnum: 0, $
         bmazm: 0.0, $
         scan: 0, $
         rxrise: 0, $
         intt: {itstr, sc:0, us:0L}, $
         txpl: 0, $
         mpinc: 0, $
         mppul: 0, $
         mplgs: 0, $
         mplgexs: 0, $
         nrang: 0, $
         frang: 0, $
         rsep: 0, $
         xcf: 0, $
         tfreq: 0, $
         offset: 0, $
         ifmode: 0, $
         mxpwr: 0L, $
         lvmax: 0L, $
         pulse: intarr(PULSE_SIZE), $
         lag: intarr(LAG_SIZE,2), $
         combf: '' $
     }
end

function RadarEncodeRadarPrm,prm,sclvec,arrvec,new=new

  if KEYWORD_SET(new) then begin
    s=DataMapMakeScalar('radar.revision.major',prm.revision.major,sclvec,/new)
  endif else begin
    s=DataMapMakeScalar('radar.revision.major',prm.revision.major,sclvec,/new)
  endelse

  s=DataMapMakeScalar('radar.revision.minor',prm.revision.minor,sclvec)
  s=DataMapMakeScalar('origin.code',prm.origin.code,sclvec)
  s=DataMapMakeScalar('origin.time',prm.origin.time,sclvec)
  s=DataMapMakeScalar('origin.command',prm.origin.command,sclvec)
  s=DataMapMakeScalar('cp',prm.cp,sclvec)
  s=DataMapMakeScalar('stid',prm.stid,sclvec)
  s=DataMapMakeScalar('time.yr',prm.time.yr,sclvec)
  s=DataMapMakeScalar('time.mo',prm.time.mo,sclvec)
  s=DataMapMakeScalar('time.dy',prm.time.dy,sclvec)
  s=DataMapMakeScalar('time.hr',prm.time.hr,sclvec)
  s=DataMapMakeScalar('time.mt',prm.time.mt,sclvec)
  s=DataMapMakeScalar('time.sc',prm.time.sc,sclvec)
  s=DataMapMakeScalar('time.us',prm.time.us,sclvec)
  s=DataMapMakeScalar('txpow',prm.txpow,sclvec)
  s=DataMapMakeScalar('nave',prm.nave,sclvec)
  s=DataMapMakeScalar('atten',prm.atten,sclvec)
  s=DataMapMakeScalar('lagfr',prm.lagfr,sclvec)
  s=DataMapMakeScalar('smsep',prm.smsep,sclvec)
  s=DataMapMakeScalar('ercod',prm.ercod,sclvec)
  s=DataMapMakeScalar('stat.agc',prm.stat.agc,sclvec)
  s=DataMapMakeScalar('stat.lopwr',prm.stat.lopwr,sclvec)
  s=DataMapMakeScalar('noise.search',prm.noise.search,sclvec)
  s=DataMapMakeScalar('noise.mean',prm.noise.mean,sclvec)
  s=DataMapMakeScalar('channel',prm.channel,sclvec)
  s=DataMapMakeScalar('bmnum',prm.bmnum,sclvec)
  s=DataMapMakeScalar('bmazm',prm.bmazm,sclvec)
  s=DataMapMakeScalar('scan',prm.scan,sclvec)
  s=DataMapMakeScalar('offset',prm.offset,sclvec)
  s=DataMapMakeScalar('rxrise',prm.rxrise,sclvec)
  s=DataMapMakeScalar('intt.sc',prm.intt.sc,sclvec)
  s=DataMapMakeScalar('intt.us',prm.intt.us,sclvec)
  s=DataMapMakeScalar('txpl',prm.txpl,sclvec)
  s=DataMapMakeScalar('mpinc',prm.mpinc,sclvec)
  s=DataMapMakeScalar('mppul',prm.mppul,sclvec)
  s=DataMapMakeScalar('mplgs',prm.mplgs,sclvec)
  s=DataMapMakeScalar('mplgexs',prm.mplgexs,sclvec)
  s=DataMapMakeScalar('ifmode',prm.ifmode,sclvec)
  s=DataMapMakeScalar('nrang',prm.nrang,sclvec)
  s=DataMapMakeScalar('frang',prm.frang,sclvec)
  s=DataMapMakeScalar('rsep',prm.rsep,sclvec)
  s=DataMapMakeScalar('xcf',prm.xcf,sclvec)
  s=DataMapMakeScalar('tfreq',prm.tfreq,sclvec)
  s=DataMapMakeScalar('mxpwr',prm.mxpwr,sclvec)
  s=DataMapMakeScalar('lvmax',prm.lvmax,sclvec)
  s=DataMapMakeScalar('combf',prm.combf,sclvec)

  s=DataMapMakeArray('ptab',prm.pulse[0:prm.mppul-1],arrvec,/new)
  s=DataMapMakeArray('ltab',prm.lag[0:prm.mplgs,*],arrvec)

  return,s
end



function RadarDecodeRadarPrm,prm,sclvec,arrvec

  ; Possible scalar values in a rawacf dmap record
  sclname=['radar.revision.major','radar.revision.minor', $
           'origin.code','origin.time','origin.command','cp','stid', $
           'time.yr','time.mo','time.dy','time.hr','time.mt','time.sc', $
           'time.us','txpow','nave','atten','lagfr','smsep','ercod', $
           'stat.agc','stat.lopwr','noise.search','noise.mean','channel', $
           'bmnum','bmazm','scan','offset','rxrise','intt.sc','intt.us', $
           'txpl', 'mpinc','mppul','mplgs','mplgexs','ifmode','nrang', $
           'frang', 'rsep','xcf','tfreq', 'mxpwr','lvmax','combf']

  scltype=[1,1,1,9,9,2,2,2,2,2,2,2,2,3,2,2,2,2,2,2,2,2,4,4,2,2,4,2,2,2,2,3, $
           2,2,2,2,2,2,2,2,2,2,2,3,3,9]
  
  sclid=intarr(n_elements(sclname))
  sclid[*]=-1

  ; Possible array values in a rawacf dmap record
  arrname=['ptab','ltab']

  arrtype=[2,2]
  arrid=intarr(n_elements(arrname))  
  arrid[*]=-1
  
  ; Look for scalar variables in sclname in the data record and
  ; populate the result in sclid array
  if (n_elements(sclvec) ne 0) then begin
    for n=0,n_elements(sclname)-1 do $
      sclid[n]=DataMapFindScalar(sclname[n],scltype[n],sclvec)
  endif

  ; Look for array variables in arrname in the data record and
  ; populate the result in arrid array
  if (n_elements(arrvec) ne 0) then begin
    for n=0,n_elements(arrname)-1 do $
      arrid[n]=DataMapFindArray(arrname[n],arrtype[n],arrvec)
  endif
   
  q=where(sclid[0:1] eq -1,count)

  if (count ne 0) then begin
    print,'File is in the wrong format!'
    return, -2
  endif
 
  q=where(arrid[0:1] eq -1,count)
  if (count ne 0) then begin
    print,'File is in the wrong format!'
    return, -2  
  endif

  ; If the sclid is not -1, then the variable exists in the record, so
  ; populate the prm pointer with the appropriate value.
  if (sclid[0] ne -1) then prm.revision.major=*(sclvec[sclid[0]].ptr)
  if (sclid[1] ne -1) then prm.revision.minor=*(sclvec[sclid[1]].ptr)
  if (sclid[2] ne -1) then prm.origin.code=*(sclvec[sclid[2]].ptr)
  if (sclid[3] ne -1) then prm.origin.time=*(sclvec[sclid[3]].ptr)
  if (sclid[4] ne -1) then prm.origin.command=*(sclvec[sclid[4]].ptr)
  if (sclid[5] ne -1) then prm.cp=*(sclvec[sclid[5]].ptr)
  if (sclid[6] ne -1) then prm.stid=*(sclvec[sclid[6]].ptr)
  if (sclid[7] ne -1) then prm.time.yr=*(sclvec[sclid[7]].ptr)
  if (sclid[8] ne -1) then prm.time.mo=*(sclvec[sclid[8]].ptr)
  if (sclid[9] ne -1) then prm.time.dy=*(sclvec[sclid[9]].ptr)
  if (sclid[10] ne -1) then prm.time.hr=*(sclvec[sclid[10]].ptr)
  if (sclid[11] ne -1) then prm.time.mt=*(sclvec[sclid[11]].ptr)
  if (sclid[12] ne -1) then prm.time.sc=*(sclvec[sclid[12]].ptr)
  if (sclid[13] ne -1) then prm.time.us=*(sclvec[sclid[13]].ptr)
  if (sclid[14] ne -1) then prm.txpow=*(sclvec[sclid[14]].ptr)
  if (sclid[15] ne -1) then prm.nave=*(sclvec[sclid[15]].ptr)
  if (sclid[16] ne -1) then prm.atten=*(sclvec[sclid[16]].ptr)
  if (sclid[17] ne -1) then prm.lagfr=*(sclvec[sclid[17]].ptr)
  if (sclid[18] ne -1) then prm.smsep=*(sclvec[sclid[18]].ptr)
  if (sclid[19] ne -1) then prm.ercod=*(sclvec[sclid[19]].ptr)
  if (sclid[20] ne -1) then prm.stat.agc=*(sclvec[sclid[20]].ptr)
  if (sclid[21] ne -1) then prm.stat.lopwr=*(sclvec[sclid[21]].ptr)
  if (sclid[22] ne -1) then prm.noise.search=*(sclvec[sclid[22]].ptr)
  if (sclid[23] ne -1) then prm.noise.mean=*(sclvec[sclid[23]].ptr)
  if (sclid[24] ne -1) then prm.channel=*(sclvec[sclid[24]].ptr)
  if (sclid[25] ne -1) then prm.bmnum=*(sclvec[sclid[25]].ptr)
  if (sclid[26] ne -1) then prm.bmazm=*(sclvec[sclid[26]].ptr)
  if (sclid[27] ne -1) then prm.scan=*(sclvec[sclid[27]].ptr)
  if (sclid[28] ne -1) then prm.offset=*(sclvec[sclid[28]].ptr)
  if (sclid[29] ne -1) then prm.rxrise=*(sclvec[sclid[29]].ptr)
  if (sclid[30] ne -1) then prm.intt.sc=*(sclvec[sclid[30]].ptr)
  if (sclid[31] ne -1) then prm.intt.us=*(sclvec[sclid[31]].ptr)
  if (sclid[32] ne -1) then prm.txpl=*(sclvec[sclid[32]].ptr)
  if (sclid[33] ne -1) then prm.mpinc=*(sclvec[sclid[33]].ptr)
  if (sclid[34] ne -1) then prm.mppul=*(sclvec[sclid[34]].ptr)
  if (sclid[35] ne -1) then prm.mplgs=*(sclvec[sclid[35]].ptr)
  if (sclid[36] ne -1) then prm.mplgexs=*(sclvec[sclid[36]].ptr)
  if (sclid[37] ne -1) then prm.ifmode=*(sclvec[sclid[37]].ptr)
  if (sclid[38] ne -1) then prm.nrang=*(sclvec[sclid[38]].ptr)
  if (sclid[39] ne -1) then prm.frang=*(sclvec[sclid[39]].ptr)
  if (sclid[40] ne -1) then prm.rsep=*(sclvec[sclid[40]].ptr)
  if (sclid[41] ne -1) then prm.xcf=*(sclvec[sclid[41]].ptr)
  if (sclid[42] ne -1) then prm.tfreq=*(sclvec[sclid[42]].ptr)
  if (sclid[43] ne -1) then prm.mxpwr=*(sclvec[sclid[43]].ptr)
  if (sclid[44] ne -1) then prm.lvmax=*(sclvec[sclid[44]].ptr)
  if (prm.mppul gt 0) && (arrid[0] ne -1) then $
     prm.pulse[0:prm.mppul-1]=*(arrvec[arrid[0]].ptr)
  if (prm.mplgs gt 0) && (arrid[1] ne -1) then $
     prm.lag[0:prm.mplgs,*]=(*(arrvec[arrid[1]].ptr))[*,*]
  if (sclid[45] ne -1) then prm.combf=*(sclvec[sclid[45]].ptr)

  return,0
end
