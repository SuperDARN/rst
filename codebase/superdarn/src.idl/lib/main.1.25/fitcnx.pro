; fitcnx.pro
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
; FitCnxRead               
;
; ---------------------------------------------------------------


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       FitCnxRead
;
; PURPOSE:
;       Read a record from an internet connection file.
;       
;
; CALLING SEQUENCE:
;       status = FitCnxRead(unit,prm,fit)
;
;       This function reads an internet connections with
;       logical unit number, unit. The structure prm and fit are
;       populated accordingly.
;
;
;       The returned value is size in bytes of the record on success, 
;       or  -1 for failure
;
;-----------------------------------------------------------------
;

function FitCnxRead,unit,prm,fit

  dummy=lonarr(2)
  readu,unit,dummy
  s=DataMapRead(unit,sclvec,arrvec)
  if (s eq -1) then return,s
  
  RadarMakeRadarPrm,prm
  FitMakeFitData,fit
  
  sclname=['radar.revision.major','radar.revision.minor', $
           'origin.code','origin.time','origin.command','cp','stid', $
           'time.yr','time.mo','time.dy','time.hr','time.mt','time.sc', $
           'time.us','txpow','nave','atten','lagfr','smsep','ercod', $
           'stat.agc','stat.lopwr','noise.search','noise.mean','channel', $
           'bmnum','scan','offset','rxrise','intt.sc','intt.us','txpl', $
           'mpinc','mppul','mplgs','nrang','frang','rsep','xcf','tfreq', $
           'mxpwr','lvmax','fitacf.revision.major','fitacf.revision.minor', $
           'combf','noise.sky','noise.lag0','noise.vel']

  scltype=[1,1,1,9,9,2,2,2,2,2,2,2,2,3,2,2,2,2,2,2,2,2,4,4,2,2,4,2,2,2,2,3, $
           2,2,2,2,2,2,2,2,2,3,3,3,3,9,4,4,4]  
  sclid=intarr(n_elements(sclname))
  sclid[*]=-1

  arrname=['ptab','ltab','slist','pwr0','nlag','qflg','gflg', $
           'p_l','p_l_e','p_s','p_s_e','v','v_e','w_l','w_l_e','w_s', $
           'w_s_e','sd_l','sd_s','sd_phi', $
           'x_qflg','x_gflg','x_p_l','x_p_l_e','x_p_s','x_p_s_e','x_v', $
           'x_v_e','x_w_l','x_w_l_e','x_w_s','x_w_s_e','phi0','phi0_e', $
           'elv','elv_low','elv_high','x_sd_l','x_sd_s','x_sd_phi']


  arrtype=[2,2,2,4,2,1,1,4,4,4,4,4,4,4,4,4,4,4,4,4,1,1,4,4,4,4,4, $
           4,4,4,4,4,4,4,4,4,4,4,4,4]

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
    return, -2
  endif
 
  q=where(arrid[[0,1,3]] eq -1,count)
  if (count ne 0) then begin
    print,'File is in the wrong format!'
    return, -2  
  endif

  ; populate the structures

  prm.revision.major=*(sclvec[sclid[0]].ptr)
  prm.revision.minor=*(sclvec[sclid[1]].ptr)
  prm.origin.code=*(sclvec[sclid[2]].ptr)
  prm.origin.time=*(sclvec[sclid[3]].ptr)
  prm.origin.command=*(sclvec[sclid[4]].ptr)
  prm.cp=*(sclvec[sclid[5]].ptr)
  prm.stid=*(sclvec[sclid[6]].ptr)
  prm.time.yr=*(sclvec[sclid[7]].ptr)
  prm.time.mo=*(sclvec[sclid[8]].ptr)
  prm.time.dy=*(sclvec[sclid[9]].ptr)
  prm.time.hr=*(sclvec[sclid[10]].ptr)
  prm.time.mt=*(sclvec[sclid[11]].ptr)
  prm.time.sc=*(sclvec[sclid[12]].ptr)
  prm.time.us=*(sclvec[sclid[13]].ptr)
  prm.txpow=*(sclvec[sclid[14]].ptr)
  prm.nave=*(sclvec[sclid[15]].ptr)
  prm.atten=*(sclvec[sclid[16]].ptr)
  prm.lagfr=*(sclvec[sclid[17]].ptr)
  prm.smsep=*(sclvec[sclid[18]].ptr)
  prm.ercod=*(sclvec[sclid[19]].ptr)
  prm.stat.agc=*(sclvec[sclid[20]].ptr)
  prm.stat.lopwr=*(sclvec[sclid[21]].ptr)
  prm.noise.search=*(sclvec[sclid[22]].ptr)
  prm.noise.mean=*(sclvec[sclid[23]].ptr)
  prm.channel=*(sclvec[sclid[24]].ptr)
  prm.bmnum=*(sclvec[sclid[25]].ptr)
  prm.bmazm=*(sclvec[sclid[26]].ptr)
  prm.scan=*(sclvec[sclid[27]].ptr)
  prm.offset=*(sclvec[sclid[28]].ptr)
  prm.rxrise=*(sclvec[sclid[29]].ptr)
  prm.intt.sc=*(sclvec[sclid[30]].ptr)
  prm.intt.us=*(sclvec[sclid[31]].ptr)
  prm.txpl=*(sclvec[sclid[32]].ptr)
  prm.mpinc=*(sclvec[sclid[33]].ptr)
  prm.mppul=*(sclvec[sclid[34]].ptr)
  prm.mplgs=*(sclvec[sclid[35]].ptr)
  prm.nrang=*(sclvec[sclid[36]].ptr)
  prm.frang=*(sclvec[sclid[37]].ptr)
  prm.rsep=*(sclvec[sclid[38]].ptr)
  prm.xcf=*(sclvec[sclid[39]].ptr)
  prm.tfreq=*(sclvec[sclid[40]].ptr)
  prm.mxpwr=*(sclvec[sclid[41]].ptr)
  prm.lvmax=*(sclvec[sclid[42]].ptr)
  if (prm.mppul gt 0) then prm.pulse[0:prm.mppul-1]=*(arrvec[arrid[0]].ptr)
  if (prm.mplgs gt 0) then $
     prm.lag[0:prm.mplgs-1,*]=(*(arrvec[arrid[1]].ptr))[*,*]
  prm.combf=*(sclvec[sclid[45]].ptr)

  fit.revision.major=*(sclvec[sclid[43]].ptr)
  fit.revision.minor=*(sclvec[sclid[44]].ptr)
  fit.noise.sky=*(sclvec[sclid[46]].ptr)
  fit.noise.lag0=*(sclvec[sclid[47]].ptr)
  fit.noise.vel=*(sclvec[sclid[48]].ptr)

  if (prm.nrang gt 0) then fit.pwr0[0:prm.nrang-1]=*(arrvec[arrid[3]].ptr)

  if (arrid[2] eq -1) then begin
    st=DataMapFreeScalar(sclvec)
    st=DataMapFreeArray(arrvec)
    return, s
  endif

  if ~(ptr_valid(arrvec[arrid[2]].ptr)) then begin
     st=DataMapFreeScalar(sclvec)
     st=DataMapFreeArray(arrvec)
     return, s
  endif

  slist=*(arrvec[arrid[2]].ptr)
  if (n_elements(slist) eq 0) then return,s

  if (arrid[4] ne -1) then fit.nlag[slist]= (*(arrvec[arrid[4]].ptr))[*]
  if (arrid[5] ne -1) then fit.qflg[slist]= (*(arrvec[arrid[5]].ptr))[*] $
  else fit.qflg[slist]=1
  if (arrid[6] ne -1) then fit.gflg[slist]= (*(arrvec[arrid[6]].ptr))[*]
  if (arrid[7] ne -1) then fit.p_l[slist]= (*(arrvec[arrid[7]].ptr))[*]
  if (arrid[8] ne -1) then fit.p_l_e[slist]= (*(arrvec[arrid[8]].ptr))[*]
  if (arrid[9] ne -1) then fit.p_s[slist]= (*(arrvec[arrid[9]].ptr))[*]
  if (arrid[10] ne -1) then fit.p_s_e[slist]= (*(arrvec[arrid[10]].ptr))[*]
  if (arrid[11] ne -1) then fit.v[slist]= (*(arrvec[arrid[11]].ptr))[*]
  if (arrid[12] ne -1) then fit.v_e[slist]= (*(arrvec[arrid[12]].ptr))[*]
  if (arrid[13] ne -1) then fit.w_l[slist]= (*(arrvec[arrid[13]].ptr))[*]
  if (arrid[14] ne -1) then fit.w_l_e[slist]= (*(arrvec[arrid[14]].ptr))[*]
  if (arrid[15] ne -1) then fit.w_s[slist]= (*(arrvec[arrid[15]].ptr))[*]
  if (arrid[16] ne -1) then fit.w_s_e[slist]= (*(arrvec[arrid[16]].ptr))[*]
  if (arrid[17] ne -1) then fit.sd_l[slist]= (*(arrvec[arrid[17]].ptr))[*]
  if (arrid[18] ne -1) then fit.sd_s[slist]= (*(arrvec[arrid[18]].ptr))[*]
  if (arrid[19] ne -1) then fit.sd_phi[slist]= (*(arrvec[arrid[19]].ptr))[*]
      
  if (prm.xcf ne 0) and (arrid[20] ne -1) then begin
       if (arrid[20] ne -1) then fit.x_qflg[slist]= $
                                   (*(arrvec[arrid[20]].ptr))[*] $
       else fit.x_qflg[slist]=1

       if (arrid[21] ne -1) then fit.x_gflg[slist]= $
                                   (*(arrvec[arrid[21]].ptr))[*]
       if (arrid[22] ne -1) then fit.x_p_l[slist]= $ 
                                   (*(arrvec[arrid[22]].ptr))[*]
       if (arrid[23] ne -1) then fit.x_p_l_e[slist]= $
                                   (*(arrvec[arrid[23]].ptr))[*]
       if (arrid[24] ne -1) then fit.x_p_s[slist]= $
                                   (*(arrvec[arrid[24]].ptr))[*]
       if (arrid[25] ne -1) then fit.x_p_s_e[slist]= $ 
                                   (*(arrvec[arrid[25]].ptr))[*]
       if (arrid[26] ne -1) then fit.x_v[slist]= $
                                   (*(arrvec[arrid[26]].ptr))[*]
       if (arrid[27] ne -1) then fit.x_v_e[slist]= $
                                   (*(arrvec[arrid[27]].ptr))[*]
       if (arrid[28] ne -1) then fit.x_w_l[slist]= $ 
                                   (*(arrvec[arrid[28]].ptr))[*]
       if (arrid[29] ne -1) then fit.x_w_l_e[slist]= $
                                   (*(arrvec[arrid[29]].ptr))[*]
       if (arrid[30] ne -1) then fit.x_w_s[slist]= $
                                   (*(arrvec[arrid[30]].ptr))[*]
       if (arrid[31] ne -1) then fit.x_w_s_e[slist]= $ 
                                   (*(arrvec[arrid[31]].ptr))[*]
       if (arrid[32] ne -1) then fit.phi0[slist]= $ 
                                   (*(arrvec[arrid[32]].ptr))[*]
       if (arrid[33] ne -1) then fit.phi0_e[slist]= $
                                   (*(arrvec[arrid[33]].ptr))[*]
       if (arrid[34] ne -1) then fit.elv[slist]= $
                                   (*(arrvec[arrid[34]].ptr))[*]
       if (arrid[35] ne -1) then fit.elv_low[slist]= $
                                   (*(arrvec[arrid[35]].ptr))[*]
       if (arrid[36] ne -1) then fit.elv_high[slist]= $
                                   (*(arrvec[arrid[36]].ptr))[*]
       if (arrid[37] ne -1) then fit.x_sd_l[slist]= $
                                   (*(arrvec[arrid[37]].ptr))[*]
       if (arrid[38] ne -1) then fit.x_sd_s[slist]= $
                                   (*(arrvec[arrid[38]].ptr))[*]
       if (arrid[39] ne -1) then fit.x_sd_phi[slist]= $
                                   (*(arrvec[arrid[39]].ptr))[*]
  endif
  st=DataMapFreeScalar(sclvec)
  st=DataMapFreeArray(arrvec)

  return,s
end

