; fitlib.pro
; ============
; Author: K.Baker & R.J.Barnes
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
; along with this program.  If not, see <https://www.gnu.org/licenses/>.
; 
; Modifications:
;
; Public Functions
; ----------------
; fitropen
; fit_close
; find_fit_rec
; pack_fit
; read_fit
; readfit
; fitdef
;
;
; ---------------------------------------------------------------
; 


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;	fitropen
;
; PURPOSE:
;	Open a FIT/INX file pair for input
;
; CALLING SEQUENCE:
;
;	  status = fitopen(filename)
;
;	arguments:
;	  filename contains the name of the file WITH NO EXTENSION.
;	    two files will be opened, filename.INX and filename.FIT
;	  iunit, dunit contain the unit numbers for the INX, FIT file pair.
;	    normally these unit numbers should not be specified.  They
;	    are only needed if multiple FIT files are being used simultaneously
;
;------------------------------------------------------------------------------
;

function fitropen, fname

  common fitdata_com, fitfileptr, fit_data
  common fitfp_com, ffp

  fileptr = long(0)
  if (n_params() EQ 0) then name = dialog_pickfile() else name = fname
  s = strpos(name,".")
  if (s NE -1) then name = strmid(name,0,s)

; build the filename

  splitpath,fname,path,name,ext

  fitpath=getenv('SD_FITROPEN_PATH')
  if (strlen(fitpath) ne 0) then begin
    search=strsplit(fitpath,PATH_SEP(/SEARCH_PATH),/EXTRACT)
    if (strlen(path) ne 0) then search=[search,path]
  endif else search=path

  for n=0,n_elements(search)-1 do begin
    fitname=search[n]+PATH_SEP()+name+'.fit'
    inxname=search[n]+PATH_SEP()+name+'.inx'
    fitfp=OldFitOpen(fitname,inxname)
    if (n_tags(fitfp) gt 0) then break
  endfor

  if (n_tags(fitfp) gt 0) then begin
    if (n_elements(ffp) eq 0) then begin
       ffp=replicate(fitfp,32)
       fitfileptr=1
     endif else begin
       ffp[fitfileptr]=fitfp
       fitfileptr++
    endelse
  endif else return, -1 
  return, fitfileptr
end


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;	fit_close
;
; PURPOSE:
;
;	close the fit and index files
;
; CALLING SEQUENCE:
;
;	  status = fit_close([fileptr])
;
;	arguments
;	  the file pointer is optional.  In normal
;	  use it should be omitted.  If it is omitted, the filepointer in
;	  the common block fitdata_com will be closed.
;
;------------------------------------------------------------------------------
;


function fit_close, fileptr
  common fitdata_com, fitfileptr, fit_data
  common fitfp_com, ffp

  if (n_params() GE 1) then fitfp = ffp[fileptr-1] $
  else fitfp = ffp[fitfileptr-1]
  
  s=OldFitClose(fitfp)

  return, s
end


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;	find_fit_rec
;
; PURPOSE:
;
;	find a record in a fit file according to time
;
; CALLING SEQUENCE:
;
;	  status = find_fit_rec(t_requested, [t_found], [fileptr])
;
;	arguments:
;	  t_requested is the time (in seconds from the beginning of the year)
;	    of the data to be located.
;	  t_found is optional and will contain the actual time found.
;	  fileptr is optional and should only be used if multiple INX,FIT
;	     files are in use.  In that case, i_unit should contain the
;	     file pointer to the INX,FIT pair to be searched.
;
;------------------------------------------------------------------------------
;

function find_fit_rec, sttim, time_found, fileptr
  common fitdata_com, fitfileptr, fit_data
  common fitfp_com, ffp

  status = long(0)
  time_found = long(0)
  if n_params() EQ 3 then fp = fileptr else fp = fitfileptr
  if n_params() GE 2 then time_found = sttim

  atme=0.0D
  s=TimeEpochToYMDHMS(yr,mo,dy,hr,mt,sc,ffp[fp-1].stime)
  s=TimeYrsecToYMDHMS(yr,mo,dy,hr,mt,sc,sttim)
 
  fitfp=ffp[fp-1]
  s=OldFitSeek(fitfp,yr,mo,dy,hr,mt,sc,atme=atme)         
  ffp[fp-1]=fitfp

  s=TimeEpochToYMDHMS(yr,mo,dy,hr,mt,sc,atme)
 
  ttemp=TimeYMDHMSToYrsec(yr,mo,dy,hr,mt,sc)

  if n_params() GE 2 then time_found = ttemp
  return, s
end

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME
;	pack_fit
;
; PURPOSE:
;	Pack data in a fit_data structure into a byte array that can be
;	passed to external routines
;
;-------------------------------------------------------------------------
;

function pack_fit, fd
  common fit_data_com, fitfileptr, fit_data
	
  if n_params() GE 1 then fit_data = fd

  fd_byte = bytarr(40000L)
  fd_byte(0:39999) = 0

  offset = 0L
;
;  first pack the parameter block
;

  fd_byte(offset:offset+3) = fit_data.rec_time
  offset = 4
  fd_byte(offset) = fit_data.rev.major
  offset = offset +1
  fd_byte(offset) = fit_data.rev.minor
  offset = offset + 1
  fd_byte(offset:offset+1) = fit_data.p.nparm
  offset = offset + 2
  fd_byte(offset:offset+1) = fit_data.p.st_id
  offset = offset + 2
  fd_byte(offset:offset+1) = fit_data.p.year
  offset = offset + 2
  fd_byte(offset:offset+1) = fit_data.p.month
  offset = offset + 2
  fd_byte(offset:offset+1) = fit_data.p.day
  offset = offset + 2
  fd_byte(offset:offset+1) = fit_data.p.hour
  offset = offset + 2
  fd_byte(offset:offset+1) = fit_data.p.minut
  offset = offset + 2
  fd_byte(offset:offset+1) = fit_data.p.sec
  offset = offset + 2
  fd_byte(offset:offset+1) = fit_data.p.txpow
  offset = offset + 2
  fd_byte(offset:offset+1) = fit_data.p.nave
  offset = offset + 2
  fd_byte(offset:offset+1) = fit_data.p.atten
  offset = offset + 2
  fd_byte(offset:offset+1) = fit_data.p.lagfr
  offset = offset + 2
  fd_byte(offset:offset+1) = fit_data.p.smsep
  offset = offset + 2
  fd_byte(offset:offset+1) = fit_data.p.ercod
  offset = offset + 2
  fd_byte(offset:offset+1) = fit_data.p.agc_stat
  offset = offset + 2
  fd_byte(offset:offset+1) = fit_data.p.lopwr_stat
  offset = offset + 2
  fd_byte(offset:offset+1) = fit_data.p.radops_sys_resS
  offset = offset + 2
  fd_byte(offset:offset+3) = fit_data.p.noise
  offset = offset + 4
  fd_byte(offset:offset+3) = fit_data.p.radops_sys_resL
  offset = offset + 4
  fd_byte(offset:offset+1) = fit_data.p.radops_sys_resS
  offset = offset + 2
  fd_byte(offset:offset+1) = fit_data.p.rxrise
  offset = offset+2
  fd_byte(offset:offset+1) = fit_data.p.intt
  offset = offset + 2
  fd_byte(offset:offset+1) = fit_data.p.txpl
  offset = offset + 2
  fd_byte(offset:offset+1) = fit_data.p.mpinc
  offset = offset + 2
  fd_byte(offset:offset+1) = fit_data.p.mppul
  offset = offset + 2
  fd_byte(offset:offset+1) = fit_data.p.mplgs
  offset = offset + 2
  fd_byte(offset:offset+1) = fit_data.p.nrang
  offset = offset + 2
  fd_byte(offset:offset+1) = fit_data.p.frang
  offset = offset + 2
  fd_byte(offset:offset+1) = fit_data.p.rsep
  offset = offset + 2
  fd_byte(offset:offset+1) = fit_data.p.bmnum
  offset = offset + 2
  fd_byte(offset:offset+1) = fit_data.p.xcf
  offset = offset + 2
  fd_byte(offset:offset+1) = fit_data.p.tfreq
  offset = offset + 2
  fd_byte(offset:offset+1) = fit_data.p.scan
  offset = offset + 2
  fd_byte(offset:offset+3) = fit_data.p.mxpwr
  offset = offset + 4
  fd_byte(offset:offset+3) = fit_data.p.lvmax
  offset = offset + 4
  fd_byte(offset:offset+3) = fit_data.p.usr_resL1
  offset = offset + 4
  fd_byte(offset:offset+3) = fit_data.p.usr_resL2
  offset = offset + 4
  fd_byte(offset:offset+2) = fit_data.p.cp
  offset = offset + 2
 fd_byte(offset:offset+1) = fit_data.p.usr_resS1
  offset = offset + 2
  fd_byte(offset:offset+1) = fit_data.p.usr_resS2
  offset = offset + 2
  fd_byte(offset:offset+1) = fit_data.p.usr_resS3
  offset = offset + 2
;
; now pack the pulse table and the lag table
;
  for i=0, pulse_pat_len - 1 do begin
    fd_byte(offset:offset+1) = fit_data.pulse_pattern(i)
    offset = offset + 2
  endfor

  for j=0, 1 do $
    for i=0, lag_tab_len - 1 do begin
      fd_byte(offset:offset+1) = fit_data.lag_table(j,i)
      offset = offset + 2 
    endfor

;
; pack in the comment buffer
;

  for i=0, combf_size - 1 do begin
     fd_byte(offset) = fit_data.combf(i)
     offset = offset + 1
  endfor

  fd_byte(offset:offset+3) = long(0)  ;padding
  offset = offset + 4

;
;  now pack in the actual data
;
  fd_byte(offset:offset+7) = fit_data.noise_lev
  offset = offset + 8
  fd_byte(offset:offset+7) = fit_data.noise_lag0
  offset = offset + 8
  fd_byte(offset:offset+7) = fit_data.noise_vel
  offset = offset + 8

  for i=0,max_range-1 do begin
    fd_byte(offset:offset+7) = fit_data.pwr_lag0(i)
    offset = offset + 8
  endfor

  for i=0,max_range-1 do begin
     fd_byte(offset:offset+1) = fit_data.slist(i)
     offset = offset + 2
  endfor

  fd_byte(offset:offset+1) = fit_data.nsel
  offset = offset + 2

  for i=0,max_range-1 do begin
    fd_byte(offset:offset+3) = fit_data.qflag(i)
    offset = offset + 4
  endfor

  fd_byte(offset:offset+3) = 0L	;padding
  offset = offset + 4

  for i=0,max_range-1 do begin
    fd_byte(offset:offset+7) = fit_data.pwr_l(i)
    offset = offset + 8
  endfor

  for i=0,max_range-1 do begin
     fd_byte(offset:offset+7) = fit_data.pwr_l_err(i)
     offset = offset + 8 
  endfor

  for i=0,max_range-1 do begin
     fd_byte(offset:offset+7) = fit_data.pwr_s(i)
     offset = offset + 8
  endfor

  for i=0,max_range-1 do begin
    fd_byte(offset:offset+7) = fit_data.pwr_s_err(i)
    offset = offset + 8
  endfor

  for i=0,max_range-1 do begin
     fd_byte(offset:offset+7) = fit_data.vel(i)
     offset = offset + 8
  endfor

  for i=0,max_range-1 do begin
     fd_byte(offset:offset+7) = fit_data.vel_err(i)
     offset = offset + 8
  endfor

  for i=0,max_range-1 do begin
    fd_byte(offset:offset+7) = fit_data.width_l(i)
    offset = offset + 8
  endfor

  for i=0,max_range-1 do begin
    fd_byte(offset:offset+7) = fit_data.width_l_err(i)
    offset = offset + 8
  endfor

  for i=0,max_range-1 do begin
    fd_byte(offset:offset+7) = fit_data.width_s(i)
    offset = offset + 8
  endfor
	
  for i=0,max_range-1 do begin
     fd_byte(offset:offset+7) = fit_data.width_s_err(i)
     offset = offset + 8
  endfor

  for i=0,max_range-1 do begin
     fd_byte(offset:offset+7) = fit_data.stnd_dev_l(i)
     offset = offset + 8
  endfor

  for i=0,max_range-1 do begin
    fd_byte(offset:offset+7) = fit_data.stnd_dev_s(i)
    offset = offset + 8
  endfor

  for i=0, max_range-1 do begin
    fd_byte(offset:offset+7) = fit_data.stnd_dev_phi(i)
    offset = offset + 8
  endfor

  for i=0, max_range-1 do begin
    fd_byte(offset:offset+1) = fit_data.gscat(i)
    offset = offset + 2
  endfor
	
  fd_byte(offset:offset+1) = 0	;padding
    offset = offset + 2
;
; now transfer the cross correlation data
;

  for i=0,max_range-1 do begin
    fd_byte(offset:offset+3) = fit_data.x_qflag(i)
    offset = offset + 4
  endfor

  fd_byte(offset:offset+3) = 0L	;padding
  offset = offset + 4

  for i=0,max_range-1 do begin
    fd_byte(offset:offset+7) = fit_data.x_pwr_l(i)
    offset = offset + 8
  endfor

  for i=0,max_range-1 do begin
    fd_byte(offset:offset+7) = fit_data.x_pwr_l_err(i)
    offset = offset + 8
  endfor

  for i=0,max_range-1 do begin
    fd_byte(offset:offset+7) = fit_data.x_pwr_s(i)
    offset = offset + 8
  endfor

  for i=0,max_range-1 do begin
     fd_byte(offset:offset+7) = fit_data.x_pwr_s_err(i)
     offset = offset + 8
   endfor


  for i=0,max_range-1 do begin
    fd_byte(offset:offset+7) = fit_data.x_vel(i)
    offset = offset + 8
  endfor


  for i=0,max_range-1 do begin
    fd_byte(offset:offset+7) = fit_data.x_vel_err(i)
    offset = offset + 8
  endfor

  for i=0,max_range-1 do begin
    fd_byte(offset:offset+7) = fit_data.x_width_l(i)
    offset = offset + 8
  endfor

  for i=0,max_range-1 do begin
    fd_byte(offset:offset+7) = fit_data.x_width_l_err(i)
    offset = offset + 8
  endfor


  for i=0,max_range-1 do begin
    fd_byte(offset:offset+7) = fit_data.x_width_s(i)
    offset = offset + 8
  endfor
	
  for i=0,max_range-1 do begin
    fd_byte(offset:offset+7) = fit_data.x_width_s_err(i)
    offset = offset + 8
  endfor

  for i=0,max_range-1 do begin
     fd_byte(offset:offset+7) = fit_data.phi0(i)
     offset = offset + 8
  endfor

  for i=0,max_range-1 do begin
    fd_byte(offset:offset+7) = fit_data.phi0_err(i)
    offset = offset + 8
  endfor

  for i=0,max_range-1 do begin
    fd_byte(offset:offset+7) = fit_data.elev(i)
    offset = offset +8
  endfor

  for i=0,max_range-1 do begin
    fd_byte(offset:offset+7) = fit_data.elev_low(i)
    offset = offset +8
  endfor


  for i=0,max_range-1 do begin
    fd_byte(offset:offset+7) = fit_data.elev_high(i)
    offset = offset +8
  endfor

  for i=0,max_range-1 do begin
    fd_byte(offset:offset+7) = fit_data.x_stnd_dev_l(i)
    offset = offset + 8
  endfor

  for i=0,max_range-1 do begin
    fd_byte(offset:offset+7) = fit_data.x_stnd_dev_s(i)
    offset = offset + 8
  endfor

  for i=0, max_range-1 do begin
    fd_byte(offset:offset+7) = fit_data.x_stnd_dev_phi(i)
    offset = offset + 8
  endfor

  for i=0, max_range-1 do begin
     fd_byte(offset:offset+1) = fit_data.num_lags(i)
     offset = offset + 2
  endfor
;
; OK, the data have all been packed into a byte array
;

  return,fd_byte

end

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;	read_fit
;
; PURPOSE:
;
;	This function is used to read a logical record from a .FIT file
;	and fill in the data arrays.  It also reads the .INX file record
;	corresponding to the .FIT record.  You must use the function
;	FIND_REC before using this routine, in order to initialize the
;	record pointers.
;
;	The data are read into the common block /PROCDAT/
;	the input to read_datrec is fitptr
;	
;	The returned values of the function are:
;	value	meaning
;	---------------------------------------
;	0	successful read
;	-2	invalid record.  expecting parameter record
;	-4	time in data record inconsistent with time in index record
;	-8	range in data record inconsistent with slist 
;	-10	invalid record.  expecting ACF data record
;	-12	time in ACF data record inconsistent with index record
;	-34	invalid record.  expecting XCF data record
;	-36	time in XCF data record inconsistent with index record
;
;-------------------------------------------------------------------------

function read_fit, fp, fdata
  common fitdata_com, fitfileptr, fit_data
  common fitfp_com, ffp


  if n_params() GE 1 then ifileptr = fp else ifileptr = fitfileptr
  
  fd_byte = bytarr(40000L)
  fd_byte(0:39999) = 0
  fitdef,fd

  pulse_pat_len = 16
  lag_tab_len = 48
  combf_size = 80
  max_range = 75


  fitfp=ffp[ifileptr-1]  

  status=OldFitRead(fitfp,prm,fit)
  ffp[ifileptr-1]=fitfp
  if (status eq -1) then return, status

  fd.p.rev.major=prm.revision.major
  fd.p.rev.minor=prm.revision.minor
  fd.p.nparm=48
  fd.p.st_id=prm.stid
  fd.p.year=prm.time.yr
  fd.p.month=prm.time.mo
  fd.p.day=prm.time.dy
  fd.p.hour=prm.time.hr
  fd.p.minut=prm.time.mt
  fd.p.sec=prm.time.sc 
  fd.p.txpow=prm.txpow
  fd.p.nave=prm.nave
  fd.p.atten=prm.atten 
  fd.p.lagfr=prm.lagfr
  fd.p.smsep=prm.smsep
  fd.p.ercod=prm.ercod
  fd.p.agc_stat=prm.stat.agc
  fd.p.lopwr_stat=prm.stat.lopwr 
  fd.p.nbaud=0
  fd.p.noise=prm.noise.search
  fd.p.noise_mean =prm.noise.mean
  fd.p.rxrise=prm.rxrise
  fd.p.intt=prm.intt.sc
  fd.p.txpl=prm.txpl
  fd.p.mpinc=prm.mpinc
  fd.p.mppul=prm.mppul
  fd.p.mplgs=prm.mplgs
  fd.p.nrang=prm.nrang
  fd.p.frang=prm.frang
  fd.p.rsep=prm.rsep
  fd.p.bmnum=prm.bmnum
  fd.p.xcf=prm.xcf
  fd.p.tfreq=prm.tfreq
  fd.p.scan=prm.scan
  fd.p.mxpwr=prm.mxpwr
  fd.p.lvmax=prm.lvmax
  fd.p.usr_resL1=0
  fd.p.usr_resL2=0
  fd.p.cp=prm.cp
  fd.p.usr_resS1=0
  fd.p.usr_resS2=0
  fd.p.usr_resS3=0

  fd.pulse_pattern[0:pulse_pat_len-1]=prm.pulse[0:pulse_pat_len-1]
  fd.lag_table[0,0:lag_tab_len-1]=prm.lag[0:lag_tab_len-1,0]
  fd.lag_table[1,0:lag_tab_len-1]=prm.lag[0:lag_tab_len-1,1]

  l=strlen(prm.combf)
  if (l gt combf_size) then l=combf_size
  
  tmp=byte(prm.combf)
  if (l gt 0) then fd.combf[0:l-1]=tmp[0:l-1]

  fd.pwr_lag0[0:max_range-1]=fit.pwr0[0:max_range-1]


  fd.noise_lev=fit.noise.sky
  fd.noise_lag0=fit.noise.lag0
  fd.noise_vel=fit.noise.vel

  fd.qflag[0:max_range-1]=fit.qflg[0:max_range-1]
  fd.pwr_l[0:max_range-1]=fit.p_l[0:max_range-1]
  fd.pwr_l_err[0:max_range-1]=fit.p_l_e[0:max_range-1]
  fd.pwr_s[0:max_range-1]=fit.p_s[0:max_range-1]
  fd.pwr_s_err[0:max_range-1]=fit.p_s_e[0:max_range-1]
  fd.vel[0:max_range-1]=fit.v[0:max_range-1]
  fd.vel_err[0:max_range-1]=fit.v_e[0:max_range-1]
  fd.width_l[0:max_range-1]=fit.w_l[0:max_range-1]
  fd.width_l_err[0:max_range-1]=fit.w_l_e[0:max_range-1]
  fd.width_s[0:max_range-1]=fit.w_s[0:max_range-1]
  fd.width_s_err[0:max_range-1]=fit.w_s_e[0:max_range-1]
  fd.stnd_dev_l[0:max_range-1]=fit.sd_l[0:max_range-1]
  fd.stnd_dev_s[0:max_range-1]=fit.sd_s[0:max_range-1]
  fd.stnd_dev_phi[0:max_range-1]=fit.sd_phi[0:max_range-1]
  fd.gscat[0:max_range-1]=fit.gflg[0:max_range-1]
	
  fd.x_qflag[0:max_range-1]=fit.x_qflg[0:max_range-1]
  fd.x_pwr_l[0:max_range-1]=fit.x_p_l[0:max_range-1]
  fd.x_pwr_l_err[0:max_range-1]=fit.x_p_l_e[0:max_range-1]
  fd.x_pwr_s[0:max_range-1]=fit.x_p_s[0:max_range-1]
  fd.x_pwr_s_err[0:max_range-1]=fit.x_p_s_e[0:max_range-1]
  fd.x_vel[0:max_range-1]=fit.x_v[0:max_range-1]
  fd.x_vel_err[0:max_range-1]=fit.x_v_e[0:max_range-1]
  fd.x_width_l[0:max_range-1]=fit.x_w_l[0:max_range-1]
  fd.x_width_l_err[0:max_range-1]=fit.x_w_l_e[0:max_range-1]
  fd.x_width_s[0:max_range-1]=fit.x_w_s[0:max_range-1]
  fd.x_width_s_err[0:max_range-1]=fit.x_w_s_e[0:max_range-1]
 
  fd.phi0[0:max_range-1]=fit.phi0[0:max_range-1]
  fd.phi0_err[0:max_range-1]=fit.phi0_e[0:max_range-1]
  fd.elev[0:max_range-1]=fit.elv[0:max_range-1]
  fd.elev_low[0:max_range-1]=fit.elv_low[0:max_range-1]
  fd.elev_high[0:max_range-1]=fit.elv_high[0:max_range-1]
  fd.x_stnd_dev_l[0:max_range-1]=fit.x_sd_l[0:max_range-1]
  fd.x_stnd_dev_s[0:max_range-1]=fit.x_sd_s[0:max_range-1]
  fd.x_stnd_dev_phi[0:max_range-1]=fit.x_sd_phi[0:max_range-1]
  fd.num_lags=fit.nlag[0:max_range-1]
  if n_params() GE 1 then fdata = fd else fit_data=fd
  return, status
end

;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;	readfit
;
; PURPOSE:
;	This is an alternate function to be used for reading FIT data
;	In addition to providing the normal functionality of READ_FIT, it
;	allows the user to specify a time to be read.  If the time
;	is specified, then the routine FIND_FIT_REC is called in order
;	to locate the specified time
;
;---------------------------------------------------------------------

function readfit, fp, fdata, t1, t2, t3, t4, t5, t6
  common fitdata_com, fitfileptr, fit_data
  status=long(0)
  temp = status
  fitdef,fd
  if n_params() GE 1 then fileptr = fp else fileptr = fitfileptr
  if n_params() GE 3 then begin
    if n_params() EQ 3 then sttime = t1 $
    else begin
      year = t1
      month = 1
      day = 1
      hour = 0
      minut = 0
      sec = 0
      if n_params() GE 4 then	month = t2 
      if n_params() GE 5 then day = t3
      if n_params() GE 6 then hour = t4
      if n_params() GE 7 then minut = t5
      if n_params() GE 8 then sec = t6
      sttime = cnvtime(year, month, day, hour, minut, sec)
    endelse
    status = find_fit_rec(sttime, temp)
  endif
  status = read_fit(fileptr, fd)
  if n_params() GE 2 then fdata = fd else fit_data = fd
  return, status
end

;
;-----------------------------------------------------------------------------
;  NAME:
;        fitdef
;
;  PURPOSE:
;
;	define a variable to be of type 'fit'
;
;	This procedure is used to define a variable for the storage of 
;	one fit data record.  Normally, this routine does not need to
;	be used, since the fit data is usually read into a pre-defined
;	variable, "fit_data" in the common block "fitdata_com"
;
;	NOTE:  the fit data variable is a structure
;
;-----------------------------------------------------------------------------
;



pro fitdef, f
f = {fit, rec_time: long(0), $
	p: {parms, rev: {rev_no, major: byte(0), minor: byte(0)}, $
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
		noise: long(0), $
		noise_mean: long(0), $
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
		mxpwr: long(0), $
		lvmax: long(0), $
		usr_resL1: long(0), $
		usr_resL2: long(0), $
		cp: 0, $
		usr_resS1: 0, $
		usr_resS2: 0, $
		usr_resS3: 0}, $
	pulse_pattern: intarr(16), $
	lag_table: intarr(2,48), $
	combf: bytarr(80), $
	pad1:long(0), $
	noise_lev: double(0.0), $
	noise_lag0: double(0.0), $
	noise_vel: double(0.0), $
	pwr_lag0: dblarr(75), $
	slist: intarr(75), $
	nsel: 0, $
	qflag: lonarr(75), $
	pad2:long(0), $
	pwr_l: dblarr(75), $
	pwr_l_err: dblarr(75), $
	pwr_s: dblarr(75), $
	pwr_s_err: dblarr(75), $
	vel: dblarr(75), $
	vel_err: dblarr(75), $
	width_l: dblarr(75), $
	width_l_err: dblarr(75), $
	width_s: dblarr(75), $
	width_s_err: dblarr(75), $
	stnd_dev_l: dblarr(75), $
	stnd_dev_s: dblarr(75), $
	stnd_dev_phi: dblarr(75), $
	gscat: intarr(75), $
	pad3: 0, $
	x_qflag: lonarr(75), $
	pad4: long(0), $
	x_pwr_l: dblarr(75), $
	x_pwr_l_err: dblarr(75), $
	x_pwr_s: dblarr(75), $
	x_pwr_s_err: dblarr(75), $
	x_vel: dblarr(75), $
	x_vel_err: dblarr(75), $
	x_width_l: dblarr(75), $
	x_width_l_err: dblarr(75), $
	x_width_s: dblarr(75), $
	x_width_s_err: dblarr(75), $
	phi0: dblarr(75), $
	phi0_err: dblarr(75), $
	elev: dblarr(75), $
	elev_low: dblarr(75), $
	elev_high: dblarr(75), $
	x_stnd_dev_l: dblarr(75), $
	x_stnd_dev_s: dblarr(75), $
	x_stnd_dev_phi: dblarr(75), $
	num_lags: intarr(75)}
return

end

;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;	beam_read
;
; PURPOSE:
;       reads data from st (start time) to et (end time) from file
;	pointed to by ifileptr. Reads data types p1, p2 and p3 into arrays
;	a1, a2, and a3. Also returns arrays for time, lagfr, smsep, tfreq
;	grnscat, noise_lev, noise_lag0, and noise_vel. It also returns the
;	station id.
;
;---------------------------------------------------------------------



FUNCTION beam_read,st,et,ifileptr,p1,p2,p3,a1,a2,a3,time,dir,lagfr, $
	 smsep,tfreq,grnscat,st_id,icount,noise_lev,$
	 noise_lag0,noise_vel,atten,scan_id

  common radarinfo, network
  common fitfp_com, ffp

  PWR_LAG0=796
  PWR_L=548
  PWR_L_ERR=972
  PWR_S=555
  PWR_S_ERR=979
  VEL=327
  VEL_ERR=751
  WIDTH_L=747
  WIDTH_L_ERR=1171
  WIDTH_S=754
  WIDTH_S_ERR=1178
  STND_DEV_L=1058
  STND_DEV_S=1065
  STND_DEV_PHI=1271
  X_PWR_L=763
  X_PWR_L_ERR=1187
  X_PWR_S=770
  X_PWR_S_ERR=1194
  X_VEL=542
  X_VEL_ERR=966
  X_WIDTH_L=962
  X_WIDTH_L_ERR=1386
  X_WIDTH_S=969
  X_WIDTH_S_ERR=1393
  PHI0=369
  PHI0_ERR=793
  ELEV=428
  ELEV_LOW=861
  ELEV_HIGH=939
  NUM_LAGS=854


  pval1 = 0
  pval2 = 0
  pval3 = 0
  pval1=total(byte(p1))
  pval2=total(byte(p2))
  pval3=total(byte(p3))

  if (n_elements(network) eq 0) then begin
    rname=getenv('SD_RADAR')
    if (strlen(rname) eq 0) then begin
      print, 'Environment Variable SD_RADAR must be defined.'
      stop
    endif

    openr,inp,rname,/get_lun
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


   openw,u1,'posg.dat',/get_lun
   openw,u2,'posm.dat',/get_lun
   
   print,'Date         time        scan id   lag to 1st range   gate length'
   print,'-----------------------------------------------------------------'

   icount=-1
   scan_id=0
   ct=0
   frangl=0
   rsepl=0
   rxrisel=0
   count=0
   lat=0.0D
   lon=0.0D
   rho=0.0D

   height=400.0

   MAX_RANGE=75
   N_BMS=16

   posc=fltarr(2,MAX_RANGE+1,N_BMS+1)

   fitfp=ffp[ifileptr-1]

   while (ct lt et) do begin
      
     s=OldFitRead(fitfp,prm,fit)
     

     if (s ne 0) then begin
       count++
       if (s eq -1) then begin
         free_lun,u1
         free_lun,u2
	 s=OldFitClose(fitfp)
	 return,0
       endif
       print,'ERROR: status number:'+status+' count: '+count
       if (count gt 10) then begin
         status = -1
	 break
         endif else continue
     endif
     count=0

     ct=TimeYMDHMSToYrsec(prm.time.yr,prm.time.mo,prm.time.dy, $
                          prm.time.hr,prm.time.mt,prm.time.sc)


     rid=RadarGetRadar(network,prm.stid)
     site=RadarYMDHMSGetSite(rid,prm.time.yr,prm.time.mo,prm.time.dy, $
                             prm.time.hr,prm.time.mt,prm.time.sc)

     if ((frangl ne prm.frang) or (rsepl ne prm.rsep) or $
        (scan_id ne prm.cp)) then print,prm.time.dy,prm.time.mo,prm.time.yr, $
                                        prm.time.hr,prm.time.mt,prm.time.sc, $
                                        prm.cp,prm.frang,prm.rsep




     st_id=prm.stid
     scan_id=prm.cp
  
     icount++
     
     time[icount]= ct
     lagfr[icount]=prm.lagfr
     smsep[icount]=fix(prm.rsep/0.15)
     dir[icount]=prm.bmnum
     tfreq[icount]=prm.tfreq;
     noise_lev[icount]=fit.noise.sky
     noise_lag0[icount]=fit.noise.lag0
     noise_vel[icount]=fit.noise.vel
     atten[icount]=prm.atten
    
     if ((frangl ne prm.frang) or (rsepl ne prm.rsep) or $
         (rxrisel ne prm.rxrise)) then begin
       frangl=prm.frang
       rsepl=prm.rsep 
       rxrisel=prm.rxrise
       
      for ibm=0,16 do begin
        for igate=1,76 do begin 
           s=RadarPos(0,ibm,igate,site,frangl,rsepl,rxrisel,height,rho,lat,lon)
           posc[0,igate-1,ibm]=lat
	   posc[1,igate-1,ibm]=lon 
        endfor
      endfor
 
      writeu,u1,transpose(posc)
     for ibm=0,16 do begin
        for igate=1,76 do begin 
           s=RadarPos(0,ibm,igate,site,frangl,rsepl,rxrisel,height,rho,lat,lon)
           s=AACGMConvert(lat,lon,height,mlat,mlon,rho)
           posc[0,igate-1,ibm]=mlat
	   posc[1,igate-1,ibm]=mlon 
        endfor
     endfor
     writeu,u2,transpose(posc)
   endif

   jndx=0L
   for i=0,MAX_RANGE-1 do begin

     jndx = long(icount)*MAX_RANGE+i
     a1[jndx]=10000.0
     a2[jndx]=10000.0
     a3[jndx]=10000.0
     grnscat[jndx]=fit.gflg[i]
     
     if (pval1 eq PWR_LAG0) then  a1[jndx] =fit.pwr0[i]
     if (pval2 eq PWR_LAG0) then  a2[jndx] =fit.pwr0[i]
     if (pval3 eq PWR_LAG0) then  a3[jndx] =fit.pwr0[i]

     if (fit.qflg[i] eq 1) then begin

        case pval1 of 
          PWR_L:         a1[jndx] =fit.p_l[i]
          PWR_L_ERR:     a1[jndx] =fit.p_l_e[i]
          PWR_S:         a1[jndx] =fit.p_s[i]
          PWR_S_ERR:     a1[jndx] =fit.p_s_e[i]
          VEL:           a1[jndx] =fit.v[i]
          VEL_ERR:       a1[jndx] =fit.v_e[i]
          WIDTH_L:       a1[jndx] =fit.w_l[i]
          WIDTH_L_ERR:   a1[jndx] =fit.w_l_e[i]
          WIDTH_S:       a1[jndx] =fit.w_s[i]
          WIDTH_S_ERR:   a1[jndx] =fit.w_s_e[i]
          STND_DEV_L:    a1[jndx] =fit.sd_l[i]
          STND_DEV_S:    a1[jndx] =fit.sd_s[i]
          STND_DEV_PHI:  a1[jndx] =fit.sd_phi[i]
          NUM_LAGS:      a1[jndx] =fit.nlag[i]
          ELSE: dummy=0
       endcase

       case pval2 of 
          PWR_L:         a2[jndx] =fit.p_l[i]
          PWR_L_ERR:     a2[jndx] =fit.p_l_e[i]
          PWR_S:         a2[jndx] =fit.p_s[i]
          PWR_S_ERR:     a2[jndx] =fit.p_s_e[i]
          VEL:           a2[jndx] =fit.v[i]
          VEL_ERR:       a2[jndx] =fit.v_e[i]
          WIDTH_L:       a2[jndx] =fit.w_l[i]
          WIDTH_L_ERR:   a2[jndx] =fit.w_l_e[i]
          WIDTH_S:       a2[jndx] =fit.w_s[i]
          WIDTH_S_ERR:   a2[jndx] =fit.w_s_e[i]
          STND_DEV_L:    a2[jndx] =fit.sd_l[i]
          STND_DEV_S:    a2[jndx] =fit.sd_s[i]
          STND_DEV_PHI:  a2[jndx] =fit.sd_phi[i]
          NUM_LAGS:      a2[jndx] =fit.nlag[i]
          ELSE: dummy=0
      endcase

      case pval3 of 
          PWR_L:         a3[jndx] =fit.p_l[i]
          PWR_L_ERR:     a3[jndx] =fit.p_l_e[i]
          PWR_S:         a3[jndx] =fit.p_s[i]
          PWR_S_ERR:     a3[jndx] =fit.p_s_e[i]
          VEL:           a3[jndx] =fit.v[i]
          VEL_ERR:       a3[jndx] =fit.v_e[i]
          WIDTH_L:       a3[jndx] =fit.w_l[i]
          WIDTH_L_ERR:   a3[jndx] =fit.w_l_e[i]
          WIDTH_S:       a3[jndx] =fit.w_s[i]
          WIDTH_S_ERR:   a3[jndx] =fit.w_s_e[i]
          STND_DEV_L:    a3[jndx] =fit.sd_l[i]
          STND_DEV_S:    a3[jndx] =fit.sd_s[i]
          STND_DEV_PHI:  a3[jndx] =fit.sd_phi[i]
          NUM_LAGS:      a3[jndx] =fit.nlag[i]
          ELSE: dummy=0
       endcase
   endif


   if (fit.x_qflg[i] eq 1) then begin
       
       case pval2 of 
          X_PWR_L:         a1[jndx] =fit.x_p_l[i]
          X_PWR_L_ERR:     a1[jndx] =fit.x_p_l_e[i]
          X_PWR_S:         a1[jndx] =fit.x_p_s[i]
          X_PWR_S_ERR:     a1[jndx] =fit.x_p_s_e[i]
          X_VEL:           a1[jndx] =fit.x_v[i]
          X_VEL_ERR:       a1[jndx] =fit.x_v_e[i]
          X_WIDTH_L:       a1[jndx] =fit.x_w_l[i]
          X_WIDTH_L_ERR:   a1[jndx] =fit.x_w_l_e[i]
          X_WIDTH_S:       a1[jndx] =fit.x_w_s[i]
          X_WIDTH_S_ERR:   a1[jndx] =fit.x_w_s_e[i]
          PHI0:            a1[jndx] =fit.phi0[i]
          PHI0_ERR:        a1[jndx] =fit.phi0_e[i]
          ELEV:            a1[jndx] =fit.elv[i]
          ELEV_LOW:        a1[jndx] =fit.elv_low[i]
          ELEV_HIGH:       a1[jndx] =fit.elv_high[i]
          ELSE: dummy=0
       endcase

       case pval2 of 
          X_PWR_L:         a2[jndx] =fit.x_p_l[i]
          X_PWR_L_ERR:     a2[jndx] =fit.x_p_l_e[i]
          X_PWR_S:         a2[jndx] =fit.x_p_s[i]
          X_PWR_S_ERR:     a2[jndx] =fit.x_p_s_e[i]
          X_VEL:           a2[jndx] =fit.x_v[i]
          X_VEL_ERR:       a2[jndx] =fit.x_v_e[i]
          X_WIDTH_L:       a2[jndx] =fit.x_w_l[i]
          X_WIDTH_L_ERR:   a2[jndx] =fit.x_w_l_e[i]
          X_WIDTH_S:       a2[jndx] =fit.x_w_s[i]
          X_WIDTH_S_ERR:   a2[jndx] =fit.x_w_s_e[i]
          PHI0:            a2[jndx] =fit.phi0[i]
          PHI0_ERR:        a2[jndx] =fit.phi0_e[i]
          ELEV:            a2[jndx] =fit.elv[i]
          ELEV_LOW:        a2[jndx] =fit.elv_low[i]
          ELEV_HIGH:       a2[jndx] =fit.elv_high[i]
          ELSE: dummy=0
      endcase

      case pval3 of 
          X_PWR_L:         a3[jndx] =fit.x_p_l[i]
          X_PWR_L_ERR:     a3[jndx] =fit.x_p_l_e[i]
          X_PWR_S:         a3[jndx] =fit.x_p_s[i]
          X_PWR_S_ERR:     a3[jndx] =fit.x_p_s_e[i]
          X_VEL:           a3[jndx] =fit.x_v[i]
          X_VEL_ERR:       a3[jndx] =fit.x_v_e[i]
          X_WIDTH_L:       a3[jndx] =fit.x_w_l[i]
          X_WIDTH_L_ERR:   a3[jndx] =fit.x_w_l_e[i]
          X_WIDTH_S:       a3[jndx] =fit.x_w_s[i]
          X_WIDTH_S_ERR:   a3[jndx] =fit.x_w_s_e[i]
          PHI0:            a3[jndx] =fit.phi0[i]
          PHI0_ERR:        a3[jndx] =fit.phi0_e[i]
          ELEV:            a3[jndx] =fit.elv[i]
          ELEV_LOW:        a3[jndx] =fit.elv_low[i]
          ELEV_HIGH:       a3[jndx] =fit.elv_high[i]
          ELSE: dummy=0
       endcase
     endif

   endfor



  endwhile

  ffp[ifileptr-1]=fitfp
   
  free_lun,u1
  free_lun,u2

RETURN,status
END	

