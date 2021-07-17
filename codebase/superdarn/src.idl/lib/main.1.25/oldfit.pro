; oldfit.pro
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
; OldFitOpen
; OldFitClose
; OldFitRead
; OldFitWrite
; OldFitWriteHeader
; OldFitInxWrite
; OldFitInxWriteHeader
; OldFitInxOpen
; OldFitInxClose
; OldFitSeek
;
; Internal Functions:
; -------------------
;
; OldFitMakeOldFitFP
; OldFitMakeOldFitRadarParm
; OldFitMakeOldFitRec1
; OldFitMakeOldFitRec2
; OldFitReadInx
; OldFitPoint

; ---------------------------------------------------------------


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       OldFitMakeOldFitFP
;
; PURPOSE:
;       Create a structure to store fit file pointer.
;       
;
; CALLING SEQUENCE:
;       OldFitMakeOldFitFP,fitfp
;
;       This procedure creates a structure to store the fit file pointer,
;       the structure is returns in fitfp.
;
;
;-----------------------------------------------------------------
;


pro OldFitMakeOldFitFP,fitfp

  fitfp={OldFitFP, $ 
                fitunit: 0L, $ 
                inxunit: 0L, $
                fit_recl: 0L, $
                inx_recl: 0L, $
                blen: 0L, $
                inx_srec: 0L,$
                inx_erec: 0L, $
                ctime: 0.0D, $
                stime: 0.0D, $
                etime: 0.0D, $
                time: 0L, $
                header: bytarr(80), $
                date: bytarr(32), $
                extra: bytarr(256),  $ 
                major_rev: 0B, $
                minor_rev: 0B, $ 
                fitread: 0L, $
                bnum:0L $
      }
   
end



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       OldFitMakeOldFitRadarParm
;
; PURPOSE:
;       Create a structure to store the radar parameter block as
;       stored in the fit files.
;.
;       
;
; CALLING SEQUENCE:
;       OldFitMakeOldFitRadarParm,radarparm
;
;       This procedure creates a structure to store the radar
;       parameter block.
;
;
;-----------------------------------------------------------------
;
   
pro OldFitMakeOldFitRadarParm,radarparm
    radarparm={OldFitRadarParm, $
                rev: {rev_no, major: byte(0), minor: byte(0)}, $
                nparm: 0, $
                st_id: 0, $
                year: 0, $
                month: 0, $
                day: 0, $
                hour: 0, $
                minut: 0, $
                sec: 0, $
                txpow: 0, $
                nave: 0, $
                atten: 0, $
                lagfr: 0, $
                smsep: 0, $
                ercod: 0, $
                agc_stat: 0, $
                lopwr_stat: 0, $
                nbaud: 0, $
                noise: 0L, $
                noise_mean: 0L, $
                channel: 0, $
                rxrise: 0, $
                intt: 0, $
                txpl: 0, $
                mpinc: 0, $
                mppul: 0, $
                mplgs: 0, $
                nrang: 0, $
                frang: 0, $
                rsep: 0, $
                bmnum: 0, $
                xcf: 0, $
                tfreq: 0, $
                scan: 0, $
                mxpwr: 0L, $
                lvmax: 0L, $
                usr_resL1: 0L, $
                usr_resL2: 0L, $
                cp: 0, $
                usr_resS1: 0, $
                usr_resS2: 0, $
                usr_resS3: 0 $
              }


end


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       OldFitMakeFitRec1
;
; PURPOSE:
;       Create a structure to store a type 1 fit record.
;.
;       
;
; CALLING SEQUENCE:
;       OldFitMakeOldFitRec1,rec
;
;       This procedure creates a structure to store a type 1 
;       fit record.
;
;
;-----------------------------------------------------------------
;


pro OldFitMakeOldFitRec1,rec,reclen
  
    ORIG_PULSE_PAT_LEN=16
    ORIG_LAG_TAB_LEN=48
    ORIG_MAX_RANGE=75
    ORIG_COMBF_SIZE=80
    OldFitMakeOldFitRadarParm,prm

    psze=reclen-5*4-2*ORIG_PULSE_PAT_LEN- $
          4*ORIG_LAG_TAB_LEN-ORIG_COMBF_SIZE-ORIG_MAX_RANGE*5- $
          N_TAGS(prm,/LENGTH)
   
    rec={OldFitRec1, $
                rrn: 0L, $
                r_time: 0L, $
                plist: prm, $
                ppat: intarr(ORIG_PULSE_PAT_LEN), $
                lags: intarr(ORIG_LAG_TAB_LEN,2), $
                comment: bytarr(ORIG_COMBF_SIZE), $
                r_noise_lev: 0L, $
                r_noise_lag0: 0L, $
                r_noise_vel: 0L, $
                r_pwr0: intarr(ORIG_MAX_RANGE), $
                r_slist: intarr(ORIG_MAX_RANGE), $
                r_numlags: bytarr(ORIG_MAX_RANGE), $
                pad: bytarr(psze) $
         } 
end         
        
     


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       OldFitMakeFitRec2
;
; PURPOSE:
;       Create a structure to store a type 2 fit record
;.
;       
;
; CALLING SEQUENCE:
;       OldFitMakeOldFitRec2,rec,reclen
;
;       This procedure creates a structure to store a type 2
;       fit record.
;
;
;-----------------------------------------------------------------
;


pro OldFitMakeOldFitRec2,rec,reclen
  
    psze=reclen-3*4-25*40
  
    rec={OldFitRec2, $
                rrn: 0L, $
                r_time: 0L, $
                r_xflag: 0L, $
                range: bytarr(25), $
                r_qflag: bytarr(25), $
                r_pwr_l: intarr(25), $
                r_pwr_l_err: intarr(25), $
                r_pwr_s: intarr(25), $
                r_pwr_s_err: intarr(25), $
                r_vel: intarr(25), $
                r_vel_err: intarr(25), $
                r_w_l: intarr(25), $
                r_w_l_err: intarr(25), $
                r_w_s: intarr(25), $
                r_w_s_err: intarr(25), $
                r_phi0: intarr(25), $
                r_phi0_err: intarr(25), $
                r_elev: intarr(25), $
                r_elev_low: intarr(25), $
                r_elev_high: intarr(25), $
                r_sdev_l: intarr(25), $
                r_sdev_s: intarr(25), $
                r_sdev_phi: intarr(25), $
                r_gscat: intarr(25), $
                pad: bytarr(psze) $
         } 
end         
        



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       OldFitReadInx
;
; PURPOSE:
;       Read a record from an index file.
;
;
; CALLING SEQUENCE:
;       status = OldFitReadInx(fitfp,inx,recno)
;
;       This function reads a single index record from the open file with
;       logical unit number, unit. The index is stored in inx 
;
;       The returned value zero on success,
;       or  -1 for failure
;
;-----------------------------------------------------------------
;

function OldFitReadInx,fitfp,inx,recno

  ON_IOERROR,iofail

  inx=lonarr(4)
  if (fitfp.inxunit ne -1) then begin    
    if (recno ne -1) then point_lun,fitfp.inxunit,(recno+1)*fitfp.inx_recl    
    readu, fitfp.inxunit,inx
    return, 0
  endif 
 
iofail:

   return,-1

end

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       OldFitOpen
;
; PURPOSE:
;       Open a fit file.
;
; CALLING SEQUENCE:
;       fitfp = OldFitOpen(fname,iname)
;
;       This function opens the fit file specified by fname and the
;       optional index file specified by iname.
;
;       Returns a valid file pointer structure on success, or -1
;       if an error occurred.


function OldFitOpen,fname,iname

   ON_IOERROR,iofail

   OldFitMakeOldFitFP,fitfp

   funit=-1L
   iunit=-1L

   openr,funit,fname,/GET_LUN,/SWAP_IF_BIG_ENDIAN 
   if (N_ELEMENTS(iname) ne 0) then  openr,iunit,iname,/GET_LUN, $ 
                                              /SWAP_IF_BIG_ENDIAN 

   fitfp.fitunit=funit
   fitfp.inxunit=iunit
   fitfp.ctime=-1.0D
   fitfp.stime=-1.0D
   fitfp.etime=-1.0D

   rlen=0
   ilen=0
   readu,funit,rlen
   readu,funit,ilen

   fitfp.fit_recl=rlen
   fitfp.inx_recl=ilen

   buf=bytarr(rlen)
   readu,funit,buf

   ; decode the header information here        
   txt=string(buf)
   vp=strpos(txt,'version')
   ve=strpos(txt,'.',vp+8)
   vstr=strmid(txt,vp+8,ve-vp-8)
   reads,vstr,vnum
   fitfp.major_rev=long(vnum)
   vp=ve+1
   ve=strpos(txt,' ',vp)
   vstr=strmid(txt,vp,ve-vp)
   reads,vstr,vnum
   fitfp.minor_rev=long(vnum)
 
   point_lun,funit,rlen

   OldFitMakeOldFitRec1,rec1,rlen  
 
   readu,funit,rec1
 
   fitfp.ctime=TimeYMDHMSToEpoch(rec1.plist.year,rec1.plist.month, $
                                 rec1.plist.day,rec1.plist.hour, $
                                 rec1.plist.minut,rec1.plist.sec)  
   fitfp.stime=fitfp.ctime 
   fitfp.bnum=1
   point_lun,funit,rlen

   if (OldFitReadInx(fitfp,inx,-1) eq 0) then begin
      fitfp.inx_srec=inx[2]-2;
      fitfp.inx_erec=inx[3]
      fitfp.etime=TimeYMDHMSToEpoch(rec1.plist.year,1,1,0,0,0) +inx[1]   

   endif else begin
     if (fitfp.inxunit ne -1) then free_lun,fitfp.inxunit
     fitfp.inxunit=-1
   endelse  



   return, fitfp


iofail:

   return,0

end


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       OldFitRead
;
; PURPOSE:
;       Read a record from a fit file.
;
;
; CALLING SEQUENCE:
;       status = OldFitRead(fitfp,prm,fit)
;
;       This function reads a single record from the open file
;       specified by fitfp. The structure prm and fit are 
;       populated accordingly.
;
;       The returned value is zero on success,
;       or  -1 for failure
;;
;-----------------------------------------------------------------
;

function OldFitRead,fitfp,prm,fit


  ON_IOERROR,iofail

  RadarMakeRadarPrm, prm
  FitMakeFitData, fit
  OldFitMakeOldFitRec1,rec1,fitfp.fit_recl  
  OldFitMakeOldFitRec2,rec2,fitfp.fit_recl  

  readu,fitfp.fitunit,rec1
 
  prm.revision.major=rec1.plist.rev.major
  prm.revision.minor=rec1.plist.rev.minor
  prm.origin.code=-1
  prm.origin.time=''
  prm.origin.command=''
  prm.cp=rec1.plist.cp
  prm.stid=rec1.plist.st_id
  prm.time.yr=rec1.plist.year
  prm.time.mo=rec1.plist.month
  prm.time.dy=rec1.plist.day
  prm.time.hr=rec1.plist.hour
  prm.time.mt=rec1.plist.minut
  prm.time.sc=rec1.plist.sec
  prm.time.us=0
  prm.txpow=rec1.plist.txpow
  prm.nave=rec1.plist.nave
  prm.atten=rec1.plist.atten
  prm.lagfr=rec1.plist.lagfr
  prm.smsep=rec1.plist.smsep
  prm.ercod=rec1.plist.ercod
  prm.lagfr=rec1.plist.lagfr
  prm.stat.agc=rec1.plist.agc_stat
  prm.stat.lopwr=rec1.plist.lopwr_stat
  prm.noise.search=rec1.plist.noise
  prm.noise.mean=rec1.plist.noise_mean
  prm.channel=rec1.plist.channel
  prm.bmnum=rec1.plist.bmnum
  prm.bmazm=-999
  prm.scan=rec1.plist.scan
  prm.rxrise=rec1.plist.rxrise
  prm.intt.sc=rec1.plist.intt
  prm.intt.us=0
  prm.txpl=rec1.plist.txpl
  prm.mpinc=rec1.plist.mpinc
  prm.mppul=rec1.plist.mppul
  prm.mplgs=rec1.plist.mplgs
  prm.nrang=rec1.plist.nrang
  prm.frang=rec1.plist.frang
  prm.rsep=rec1.plist.rsep
  prm.xcf=rec1.plist.xcf
  prm.tfreq=rec1.plist.tfreq
  prm.offset=rec1.plist.usr_resL1
  prm.mxpwr=rec1.plist.mxpwr
  prm.lvmax=rec1.plist.lvmax
  prm.pulse[0:prm.mppul-1]=rec1.ppat[0:prm.mppul-1]
  prm.lag[0:prm.mplgs-1,0]=rec1.lags[0:prm.mplgs-1,0]
  prm.lag[0:prm.mplgs-1,1]=rec1.lags[0:prm.mplgs-1,1]
  prm.combf=string(rec1.comment)

  fit.revision.major=fitfp.major_rev
  fit.revision.minor=fitfp.minor_rev

  fit.noise.sky=rec1.r_noise_lev
  fit.noise.lag0=rec1.r_noise_lag0
  fit.noise.vel=rec1.r_noise_vel/10.0

  ; zero out the fit structure

  fitfp.ctime=TimeYMDHMSToEpoch(rec1.plist.year,rec1.plist.month, $
                                 rec1.plist.day,rec1.plist.hour, $
                                 rec1.plist.minut,rec1.plist.sec) 
  fitfp.blen=1
  fitfp.bnum++
  fit.pwr0[0:prm.nrang-1]=rec1.r_pwr0[0:prm.nrang-1]/100.0

  for n=3,n_tags(fit)-1 do begin
    fit.(n)=0
  endfor


  stmp=intarr(prm.nrang)
  stmp[0:prm.nrang-1]=rec1.r_slist[0:prm.nrang-1]-1
  q=where(stmp ne -1,count)
  if (count ne 0) then slist=stmp[q]
  if (count ne 0) then fit.nlag[slist]=rec1.r_numlags[0:count-1]

  n=0
 
  while (n lt count) do begin
      readu,fitfp.fitunit,rec2
      fitfp.blen++
      fitfp.bnum++
      if (rec2.rrn eq 0) or (rec2.r_xflag ne 0) then return, -1
      for i=0,24 do begin
        if (n eq count) then break;
        rng=rec2.range[i]
       
        fit.qflg[rng-1]=rec2.r_qflag[i];
        fit.gflg[rng-1]=rec2.r_gscat[i];
        fit.p_l[rng-1]=rec2.r_pwr_l[i]/100.0;
        fit.p_s[rng-1]=rec2.r_pwr_s[i]/100.0;
        fit.p_l_e[rng-1]=rec2.r_pwr_l_err[i]/100.0;
        fit.p_s_e[rng-1]=rec2.r_pwr_s_err[i]/100.0;
        fit.w_l[rng-1]=rec2.r_w_l[i]/10.0;
        fit.w_s[rng-1]=rec2.r_w_s[i]/10.0;
        fit.w_l_e[rng-1]=rec2.r_w_l_err[i]/10.0;
        fit.w_s_e[rng-1]=rec2.r_w_s_err[i]/10.0;
        fit.v[rng-1]=rec2.r_vel[i]/10.0;
        fit.v_e[rng-1]=rec2.r_vel_err[i]/10.0;
        fit.sd_l[rng-1]=rec2.r_sdev_l[i]/1000.0;
        fit.sd_s[rng-1]=rec2.r_sdev_s[i]/1000.0;
        fit.sd_phi[rng-1]=rec2.r_sdev_s[i]/100.0;       
        n++
    endfor
  endwhile
  n=0

  if (prm.xcf ne 0) then begin  
    while (n lt count) do begin
      readu,fitfp.fitunit,rec2
      fitfp.blen++
      fitfp.bnum++
      if (rec2.rrn eq 0) or (rec2.r_xflag ne 1) then return, -1
      for i=0,24 do begin
        if (n eq count) then break;
        rng=rec2.range[i] 
        fit.x_qflg[rng-1]=rec2.r_qflag[i];
        fit.x_gflg[rng-1]=rec2.r_gscat[i];
        fit.x_p_l[rng-1]=rec2.r_pwr_l[i]/100.0;
        fit.x_p_s[rng-1]=rec2.r_pwr_s[i]/100.0;
        fit.x_p_l_e[rng-1]=rec2.r_pwr_l_err[i]/100.0;
        fit.x_p_s_e[rng-1]=rec2.r_pwr_s_err[i]/100.0;
        fit.x_w_l[rng-1]=rec2.r_w_l[i]/10.0;
        fit.x_w_s[rng-1]=rec2.r_w_s[i]/10.0;
        fit.x_w_l_e[rng-1]=rec2.r_w_l_err[i]/10.0;
        fit.x_w_s_e[rng-1]=rec2.r_w_s_err[i]/10.0;
        fit.x_v[rng-1]=rec2.r_vel[i]/10.0;
        fit.x_v_e[rng-1]=rec2.r_vel_err[i]/10.0;
        fit.x_sd_l[rng-1]=rec2.r_sdev_l[i]/1000.0;
        fit.x_sd_s[rng-1]=rec2.r_sdev_s[i]/1000.0;
        fit.x_sd_phi[rng-1]=rec2.r_sdev_s[i]/100.0;  

        fit.phi0[rng-1]=rec2.r_phi0[i]/100.0;
        fit.phi0_e[rng-1]=rec2.r_phi0_err[i]/100.0;  

        fit.elv[rng-1]=rec2.r_elev[i]/100.0;
        fit.elv_low[rng-1]=rec2.r_elev_low[i]/100.0;  
        fit.elv_high[rng-1]=rec2.r_elev_high[i]/100.0;  

     
        n++
    endfor
    endwhile
endif

  return, 0
iofail:

   return,-1

end



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       OldFitWriteHeader
;
; PURPOSE:
;       Write the header to a fit file.
;
;
; CALLING SEQUENCE:
;       status = OldFitWriteHeader(unit,prm,fit)
;
;       This function writes the fit header record to the open file with
;       logical unit number, unit.
;
;       The returned value is zero on success,
;       or  -1 for failure
;
;
;-----------------------------------------------------------------
;

function OldFitWriteHeader,unit,prm,fit


ON_IOERROR,iofail

 
  FIT_RECL=1024
  INX_RECL=16

  vstr=string(format='(I1,".",I3.3)',fit.revision.major,fit.revision.minor)
  hdr='fitacf version '+vstr+' (IDL)'
  txt='IDL output'
  time=systime()
  psze=FIT_RECL-7-strlen(hdr)-strlen(txt)-strlen(time)
  pad=bytarr(psze)

  writeu,unit,FIT_RECL,INX_RECL,byte(hdr),10B,byte(txt),10B,time,10B,pad

  return, 0

iofail:
  return, -1

end


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       OldFitInxWrite
;
; PURPOSE:
;       Write a record to an index file.
;
;
; CALLING SEQUENCE:
;       status = OldFitInxWrite(unit,drec,dnum,prm)
;
;       This function writes a record to an index file.
;
;       The returned value is zero on success,
;       or  -1 for failure
;
;
;-----------------------------------------------------------------
;


function OldFitInxWrite,unit,drec,dnum,prm

ON_IOERROR,iofail

  inxrec=lonarr(4)

  ctime = TimeYMDHMSToYrsec(prm.time.yr,prm.time.mo,prm.time.dy, $
                            prm.time.hr,prm.time.mt,prm.time.sc)


  inxrec[0]=ctime;
  inxrec[1]=drec;
  inxrec[2]=dnum;
  inxrec[3]=(prm.xcf ne 0)

  writeu,unit,inxrec
  return, 0

iofail:
  return, -1

end


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       OldFitInxWriteHeader
;
; PURPOSE:
;       Write the header record to an index file.
;
;
; CALLING SEQUENCE:
;       status = OldFitInxWriteHeader(unit,prm)
;
;       This function writes a header record to an index file.
;
;       The returned value is zero on success,
;       or  -1 for failure
;
;
;-----------------------------------------------------------------
;



function OldFitInxWriteHeader,unit,prm

 ON_IOERROR,iofail

  inxrec=lonarr(4)

  ctime = TimeYMDHMSToYrsec(prm.time.yr,prm.time.mo,prm.time.dy, $
                            prm.time.hr,prm.time.mt,prm.time.sc)


  inxrec[0]=ctime;
  inxrec[1]=0;
  inxrec[2]=2;
  inxrec[3]=0

  writeu,unit,inxrec
  return, 0

iofail:
  return, -1

end


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       OldFitInxOpen 
;
; PURPOSE:
;       Opens an Index file for writing
;
;
; CALLING SEQUENCE:
;       status = OldFitInxOpen(fname)
;
;       This function opens an index file for writing
;
;       The returned value is the unit number on success,
;       or  -1 for failure
;
;
;-----------------------------------------------------------------
;

function OldFitInxOpen,fname
  openw,unit,fname,/GET_LUN,/SWAP_IF_BIG_ENDIAN
  return, unit
end

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       OldFitInxClose
;
; PURPOSE:
;       Updates the header record of an index file.
;
;
; CALLING SEQUENCE:
;       status = OldFitInxClose(unit,prm,irec)
;
;       This function updates the header record of an index file.
;       It should be called just before the index file is closed.
;
;       The returned value is zero on success,
;       or  -1 for failure
;
;
;-----------------------------------------------------------------
;



function OldFitInxClose,unit,prm,irec


  ON_IOERROR,iofail

  inxrec=lonarr(4)

  ctime = TimeYMDHMSToYrsec(prm.time.yr,prm.time.mo,prm.time.dy, $
                            prm.time.hr,prm.time.mt,prm.time.sc)

  point_lun,unit,0
  readu,unit,inxrec
  point_lun,unit,0 

  inxrec[1]=ctime
  inxrec[3]=irec-1
  
  writeu,unit,inxrec
  free_lun,unit

  return, 0

iofail:
  return, -1

end




;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       OldFitWrite
;
; PURPOSE:
;       Write a record to a fit file.
;
;
; CALLING SEQUENCE:
;       status = OldFitWrite(unit,prm,fit)
;
;       This function writes a single record to the open file with
;       logical unit number, unit.
;
;       The returned value is the number blocks used to store the data,
;       or  -1 for failure
;
;
;-----------------------------------------------------------------
;

function OldFitWrite,unit,prm,fit


ON_IOERROR,iofail


  FIT_RECL=1024
  ORIG_COMBF_SIZE=80
  ORIG_MAX_RANGE=75

  OldFitMakeOldFitRec1,rec1,FIT_RECL 
  OldFitMakeOldFitRec2,rec2,FIT_RECL  

  ctime=TimeYMDHMSToYrsec(prm.time.yr,prm.time.mo,prm.time.dy, $
				prm.time.hr,prm.time.mt,prm.time.sc)
   
  rrn=0L
  rec1.rrn=rrn  
  rec1.r_time=ctime
  rec1.plist.rev.major=prm.revision.major
  rec1.plist.rev.minor=prm.revision.minor
  rec1.plist.nparm=48
  rec1.plist.cp=prm.cp
  rec1.plist.st_id=prm.stid
  rec1.plist.year=prm.time.yr
  rec1.plist.month= prm.time.mo
  rec1.plist.day=prm.time.dy
  rec1.plist.hour=prm.time.hr
  rec1.plist.minut=prm.time.mt
  rec1.plist.sec=prm.time.sc
  rec1.plist.txpow=prm.txpow
  rec1.plist.nave=prm.nave
  rec1.plist.atten=prm.atten
  rec1.plist.lagfr=prm.lagfr
  rec1.plist.smsep=prm.smsep
  rec1.plist.ercod=prm.ercod
  rec1.plist.lagfr=prm.lagfr
  rec1.plist.agc_stat=prm.stat.agc
  rec1.plist.lopwr_stat=prm.stat.lopwr
  rec1.plist.noise=prm.noise.search
  rec1.plist.noise_mean=prm.noise.mean
  rec1.plist.channel=prm.channel
  rec1.plist.bmnum=prm.bmnum
  rec1.plist.scan=prm.scan
  rec1.plist.rxrise=prm.rxrise
  rec1.plist.intt=prm.intt.sc
  rec1.plist.txpl=prm.txpl
  rec1.plist.mpinc=prm.mpinc
  rec1.plist.mppul=prm.mppul
  rec1.plist.mplgs=prm.mplgs
  rec1.plist.nrang=prm.nrang
  rec1.plist.frang=prm.frang
  rec1.plist.rsep=prm.rsep
  rec1.plist.xcf=prm.xcf
  rec1.plist.tfreq=prm.tfreq
  rec1.plist.usr_resL1=prm.offset
  rec1.plist.mxpwr= prm.mxpwr
  rec1.plist.lvmax=prm.lvmax
  rec1.ppat[0:prm.mppul-1]=prm.pulse[0:prm.mppul-1]
  rec1.lags[0:prm.mplgs-1,0]=prm.lag[0:prm.mplgs-1,0]
  rec1.lags[0:prm.mplgs-1,1]=prm.lag[0:prm.mplgs-1,1]

  combf=bytarr(ORIG_COMBF_SIZE)
  l=strlen(prm.combf)
  if (l gt ORIG_COMBF_SIZE) then l=ORIG_COMBF_SIZE
  tmp=byte(prm.combf)

  rec1.comment[0:l-1]=tmp[0:l-1]
  rec1.r_noise_lev=fit.noise.sky
  rec1.r_noise_lag0=fit.noise.lag0
  rec1.r_noise_vel=fix(fit.noise.vel*10)

  q=fit.qflg[0:prm.nrang-1]+fit.x_qflg[0:prm.nrang-1]

  slist=fix(where(q gt 0,count))
  if (count ne 0) then begin
    rec1.r_slist=slist+1
    rec1.r_numlags[0:count-1]=fit.nlag[slist]
  endif
 
  rec1.r_pwr0[0:prm.nrang-1]=fix(fit.pwr0[0:prm.nrang-1]*100.0)
  if (unit ne -1) then writeu,unit,rec1
  if (count eq 0) then return, 1

  rrn=1
  dnum=1 
  j=0
  
  for n=0,count-1 do begin
    rec2.r_time = ctime;
    rec2.r_xflag = 0;
    rec2.rrn=rrn;
    rec2.range[j] = slist[n]+1;
    rng=slist[n]
    
    rec2.r_qflag[j]=fit.qflg[rng]
    rec2.r_gscat[j]=fit.gflg[rng]
    rec2.r_pwr_l[j]=fix(fit.p_l[rng]*100.0)
    rec2.r_pwr_s[j]=fix(fit.p_s[rng]*100.0)
    rec2.r_pwr_l_err[j]=fix(fit.p_l_e[rng]*100.0)
    rec2.r_pwr_s_err[j]=fix(fit.p_s_e[rng]*100.0)
    rec2.r_w_l[j]=fix(fit.w_l[rng]*10.0)
    rec2.r_w_s[j]=fix(fit.w_s[rng]*10.0)
    rec2.r_w_l_err[j]=fix(fit.w_l_e[rng]*10.0)
    rec2.r_w_s_err[j]=fix(fit.w_s_e[rng]*10.0)
    rec2.r_vel[j]=fix(fit.v[rng]*10.0)
    rec2.r_vel_err[j]=fix(fit.v_e[rng]*10.0)
    rec2.r_sdev_l[j]=fix(fit.sd_l[rng]*1000.0)
    rec2.r_sdev_s[j]=fix(fit.sd_s[rng]*1000.0)
    rec2.r_sdev_phi[j]=fix(fit.sd_phi[rng]*100.0)    


    j++
    if (j eq 25) then begin
       if (unit ne -1) then writeu,unit,rec2
       dnum++
       rrn++
       j=0
    endif
  endfor
  if (j ne 0) then begin
     if (unit ne -1) then writeu,unit,rec2
     dnum++
     rrn++
  endif

  j=0
  if (prm.xcf ne 0) then begin
     for n=0,count-1 do begin
       rec2.r_time = ctime;
       rec2.r_xflag = 1;
       rec2.rrn=rrn;
       rec2.range[j] = slist[n]+1;
       rng=slist[n]

       rec2.r_qflag[j]=fit.x_qflg[rng]
       rec2.r_gscat[j]=fit.x_gflg[rng]
       rec2.r_pwr_l[j]=fix(fit.x_p_l[rng]*100.0)
       rec2.r_pwr_s[j]=fix(fit.x_p_s[rng]*100.0)
       rec2.r_pwr_l_err[j]=fix(fit.x_p_l_e[rng]*100.0)
       rec2.r_pwr_s_err[j]=fix(fit.x_p_s_e[rng]*100.0)
       rec2.r_w_l[j]=fix(fit.x_w_l[rng]*10.0)
       rec2.r_w_s[j]=fix(fit.x_w_s[rng]*10.0)
       rec2.r_w_l_err[j]=fix(fit.x_w_l_e[rng]*10.0)
       rec2.r_w_s_err[j]=fix(fit.x_w_s_e[rng]*10.0)
       rec2.r_vel[j]=fix(fit.x_v[rng]*10.0)
       rec2.r_vel_err[j]=fix(fit.x_v_e[rng]*10.0)
       rec2.r_sdev_l[j]=fix(fit.x_sd_l[rng]*1000.0)
       rec2.r_sdev_s[j]=fix(fit.x_sd_s[rng]*1000.0)
       rec2.r_sdev_phi[j]=fix(fit.x_sd_phi[rng]*100.0) 

       rec2.r_phi0[j]=fix(fit.phi0[rng]*100.0)
       rec2.r_phi0_err[j]=fix(fit.phi0_e[rng]*100.0)

       rec2.r_elev[j]=fix(fit.elv[rng]*100.0)
       rec2.r_elev_low[j]=fix(fit.elv_low[rng]*100.0)
       rec2.r_elev_high[j]=fix(fit.elv_high[rng]*100.0)  

       j++
       if (j eq 25) then begin
         if (unit ne -1) then writeu,unit,rec2
         dnum++
         rrn++
         j=0
      endif
    endfor
  endif 
  if (j ne 0) then begin
     if (unit ne -1) then writeu,unit,rec2
     dnum++
     rrn++
  endif

  return,dnum

iofail:
  return, -1
end



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       OldFitSeek
;
; PURPOSE:
;       Find a record in a fit file.
;
;
; CALLING SEQUENCE:
;       status = OldFitSeek(fitfp,yr,mo,dy,hr,mt,sc,atme=atime)
;
;       This function searchs for a record in a fit file.
;
;
;       The returned value is zero on success,
;       or  -1 for failure
;
;-----------------------------------------------------------------
;

function OldFitSeek,fitfp,yr,mo,dy,hr,mt,sc,atme=atme

ON_IOERROR,iofail

  tval=TimeYMDHMSToEpoch(yr,mo,dy,hr,mt,sc)

  if (fitfp.inxunit ne -1) then begin   
    if (fitfp.etime ne -1) and (tval gt fitfp.etime) then begin
       s=OldFitReadInx(fitfp,inx_rec,fitfp.inx_erec)
       point_lun,fitfp.fitunit,(inx_rec[1]-1)*fitfp.fit_recl
       fitfp.ctime=fitfp.etime;
       atme=fitfp.ctime;
       return, 3
   endif

    prec=-1L
    recno=0L

    stime=fitfp.stime;
    etime=fitfp.etime;
    srec=fitfp.inx_srec;
    erec=fitfp.inx_erec; 

    s=TimeEpochToYMDHMS(yr,mo,dy,hr,mt,sec,stime)
    syrsec=TimeYMDHMSToYrsec(yr,mo,dy,hr,mt,sec)

    repeat begin

      prec=recno
      recno=srec+fix(((tval-stime)*(erec-srec))/(etime-stime))
      s=OldFitReadInx(fitfp,inx_rec,recno)
      if (s ne 0) then break
      
      if (inx_rec[0] lt syrsec) then itme=TimeYMDHMSToEpoch(yr+1,1,1,0,0,0) $ 
      else itme=TimeYMDHMSToEpoch(yr,1,1,0,0,0)
      itme+=inx_rec[0]
      if (itme eq tval) then break $
      else if (itme<tval) then begin
        srec=recno
        stime=itme
      endif else begin
        erec=recno
        etime=itme
      endelse 
    endrep until (prec eq recno)

    if (s eq 0) then begin
      if (itme ne tval) then begin
        recno=srec;
        if (recno gt erec) then recno=erec
      endif 
    endif

    s=OldFitReadInx(fitfp,inx_rec,recno)

    point_lun,fitfp.fitunit,(inx_rec[1]-1)*fitfp.fit_recl
    atme=itme
    if (itme eq tval) then return,0 $
    else return,1

  endif else begin

    if (tval<fitfp.ctime) then begin

       point_lun,fitfp.fitunit,fitfp.fit_recl
       s=OldFitRead(fitfp,prm,fit)
    endif
    repeat begin
     bjmp=long(fitfp.blen)
     atme=fitfp.ctime;
     s=OldFitRead(fitfp,prm,fit)
     if (s eq -1) then break
    endrep until (tval lt fitfp.ctime)

    fitfp.bnum=fitfp.bnum-fitfp.blen-bjmp
    fitfp.blen=bjmp 
    point_lun,fitfp.fitunit,fitfp.fit_recl*fitfp.bnum  
  endelse

  return,0

iofail:
  return, -1

end

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       OldFitPoint
;
; PURPOSE:
;       Sets the file pointer position in a fit file.
;
;
; CALLING SEQUENCE:
;       status = OldFitPoint(fitfp,ptr)
;
;       This function sets the file pointer in a fit file.
;
;
;       The returned value is the offset on success,
;       or  -1 for failure
;
;-----------------------------------------------------------------
;

function OldFitPoint,fitfp,off

ON_IOERROR,iofail

  point_lun,fitfp.fitunit,off
  fitfp.ptr=off
  return,off

iofail:
  return, -1

end

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       OldFitClose
;
; PURPOSE:
;       Close a fit file.
;
; CALLING SEQUENCE:
;       s = OldFitClose(fitfp)
;
;       This function closes the fit file with the file pointer fitfp
;
;       Returns a zero on success, or -1
;       if an error occurred.

;
;-----------------------------------------------------------------
;


function OldFitClose,fitfp
   free_lun, fitfp.fitunit
   if (fitfp.inxunit ne -1) then free_lun, fitfp.inxunit
   return, 0
end


