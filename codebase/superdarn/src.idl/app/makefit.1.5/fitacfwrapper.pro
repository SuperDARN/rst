; fitacfwrapper.pro
; =================
; Author: R.J.Barnes - Based on C code by K.Baker
; 
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
; FitACFStart 
; FitACF

; ---------------------------------------------------------------


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       FitACFStart
;
; PURPOSE:
;       Initialize a structure to store parameters used by
;       the fitacf algorithm.
;       
;
; CALLING SEQUENCE:
;
;        FitACFStart,hd,year,fitblk
;
;
;        The hardware data is given hd, the year by year,
;        the structure fitblk is set up accordingly.
;
;-----------------------------------------------------------------
;

pro FitACFStart,hd,year,fitblk

  PULSE_SIZE=64
  LAG_SIZE=96
  MAX_RANGE=300

  prm={FitPrm, $
       channel:0L, $
       offset:0L, $
       cp:0L, $
       xcf:0L, $
       tfreq:0L, $
       noise:0L, $
       nrang:0L, $
       smsep:0L, $
       nave:0L, $
       mplgs:0L, $
       mpinc:0L, $
       txpl:0L, $
       lagfr:0L, $
       mppul:0L, $
       bmnum:0L, $
       old:0L, $
       lag: intarr(LAG_SIZE,2), $
       pulse: intarr(PULSE_SIZE), $
       pwr0: lonarr(MAX_RANGE), $
       interfer: dblarr(3), $
       maxbeam: 0L, $
       bmsep:0.0D, $
       phidiff: 0.0D, $
       tdiff: 0.0D, $
       vdir: 0.0D $
      }

      fitblk={FitBlock, $
              prm: {FitPrm}, $
              acfd: dcomplexarr(MAX_RANGE,LAG_SIZE), $
              xcfd: dcomplexarr(MAX_RANGE,LAG_SIZE) $
             }
        
   if (year lt 1993) then fitblk.prm.old=1
   fitblk.prm.interfer=hd.interfer
   fitblk.prm.bmsep=hd.bmsep
   fitblk.prm.phidiff=hd.phidiff
   fitblk.prm.tdiff=hd.tdiff
   fitblk.prm.vdir=hd.vdir
   fitblk.prm.maxbeam=hd.maxbeam
end


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       FitACF
;
; PURPOSE:
;       Perform the FitACF algorithm.
;       
;
; CALLING SEQUENCE:
;
;        s=FitACF(prm,raw,input,fit)
;
;
;        The radar parameter block is given by prm,
;        the raw ACF data by raw, the FitACF parameters by input.
;        The fitted data is returned in the structure fit.
;
;
;-----------------------------------------------------------------
;


function FitACF,prm,raw,input,fit

  GOOSEBAY=1
  FITACF_MAJOR_REVISION=-5
  FITACF_MINOR_REVISION=0
  MAX_RANGE=300

  FitMakeFitData,fit
  
  if (prm.time.yr lt 1993) then input.prm.old=1
  fit.revision.major=FITACF_MAJOR_REVISION
  fit.revision.minor=FITACF_MINOR_REVISION
   
  input.prm.xcf=prm.xcf
  input.prm.tfreq=prm.tfreq
  input.prm.noise=prm.noise.search
  input.prm.nrang=prm.nrang
  input.prm.smsep=prm.smsep
  input.prm.nave=prm.nave
  input.prm.mplgs=prm.mplgs
  input.prm.mpinc=prm.mpinc
  input.prm.txpl=prm.txpl
  input.prm.lagfr=prm.lagfr
  input.prm.mppul=prm.mppul
  input.prm.bmnum=prm.bmnum
  input.prm.cp=prm.cp
  input.prm.channel=prm.channel
  input.prm.offset=prm.offset
 
  input.prm.pulse=prm.pulse
  input.prm.lag=prm.lag
 
  
  input.prm.pwr0=long(raw.pwr0)

  input.acfd[*,*]=dcomplex(raw.acfd[*,*,0],raw.acfd[*,*,1])
  input.xcfd[*,*]=dcomplex(raw.xcfd[*,*,0],raw.xcfd[*,*,1])

  goose=0
  if (prm.stid eq GOOSEBAY) then goose=1

 
  FitACFMakeFitRange,rng
  
  acf=replicate(rng,MAX_RANGE)
  xcf=replicate(rng,MAX_RANGE)

  FitACFMakeFitElv,elv
  elv=replicate(elv,MAX_RANGE)

  noise=fit.noise

  s=FitACFDoFit(input,5,goose,acf,xcf,elv,noise)

  ; pack the fitdata structure

  fit.noise.sky=noise.sky
  fit.noise.lag0=noise.lag0
  fit.noise.vel=noise.vel
  
  fit.pwr0=acf[*].p_0
  fit.nlag=acf[*].nump
  fit.qflg=acf[*].qflg
  fit.gflg=acf[*].gsct
  fit.pwr0=acf[*].p_0
  fit.p_l=acf[*].p_l
  fit.p_l_e= acf[*].p_l_err
  fit.p_s=acf[*].p_s
  fit.p_s_e=acf[*].p_s_err
  fit.v=acf[*].v
  fit.v_e=acf[*].v_err
  fit.w_l=acf[*].w_l
  fit.w_l_e=acf[*].w_l_err
  fit.w_s=acf[*].w_s
  fit.w_s_e=acf[*].w_s_err
  fit.sd_l=acf[*].sdev_l
  fit.sd_s=acf[*].sdev_s
  fit.sd_phi=acf[*].sdev_phi
      
  if (prm.xcf ne 0) then begin
     
    fit.x_qflg=xcf[*].qflg
    fit.x_gflg=xcf[*].gsct
    fit.x_p_l=xcf[*].p_l
    fit.x_p_l_e= xcf[*].p_l_err
    fit.x_p_s=xcf[*].p_s
    fit.x_p_s_e=xcf[*].p_s_err
    fit.x_v=xcf[*].v
    fit.x_v_e=xcf[*].v_err
    fit.x_w_l=xcf[*].w_l
    fit.x_w_l_e=xcf[*].w_l_err
    fit.x_w_s=xcf[*].w_s
    fit.x_w_s_e=xcf[*].w_s_err
    fit.x_sd_l=xcf[*].sdev_l
    fit.x_sd_s=xcf[*].sdev_s
    fit.x_sd_phi=xcf[*].sdev_phi

    fit.phi0=xcf[*].phi0
    fit.phi0_e=xcf[*].phi0_err
    fit.elv=elv.normal
    fit.elv_low=elv.low
    fit.elv_high=elv.high

  endif

 
  return,s 
end
