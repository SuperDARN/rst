;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
;NAME:
; plot_fitex2
;
; PURPOSE:
; Plots the results of fitting ACFs from 1 beam sounding with the fitacfex2 algorithm
;
; CATEGORY:
; Graphics
;
; CALLING SEQUENCE:
; first call the c routine test_fitex2, e.g.
; 	test_fitex2 [-new] -hr 5 -min 3 -beam 7 myfile.rawacf > /rst/output_files/timestamp.fitex2.test
;
; next, call the IDL routine, e.g.
; 	plot_fitex2,timestamp
;
;	INPUTS:
;		time:  a string with a timestamp to be used for a file name
;
; OPTIONAL INPUTS:
;
; KEYWORD PARAMETERS:
;
; EXAMPLE:
; test_fitex2 -new -hr 5 -min 3 -beam 7 myfile.rawacf > /rst/output_files/timestamp.fitex2.test
; plot_fitex2
;
; OUTPUT:
; /rst/output_plots/timestamp.fitex2.ps
;
;
; COPYRIGHT:
; Copyright (C) 2011 by Virginia Tech
;
; Permission is hereby granted, free of charge, to any person obtaining a copy
; of this software and associated documentation files (the "Software"), to deal
; in the Software without restriction, including without limitation the rights
; to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
; copies of the Software, and to permit persons to whom the Software is
; furnished to do so, subject to the following conditions:
;
; The above copyright notice and this permission notice shall be included in
; all copies or substantial portions of the Software.
;
; THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
; IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
; FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
; AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
; LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
; OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
; THE SOFTWARE.
;
;
; MODIFICATION HISTORY:
; Written by AJ Ribeiro 07/15/2011
;	Updated 02/08/2012 AJR
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

pro acf_plot_fitex2,time
  ;the file we are reading data from
  file_in = '/rst/output_files/'+time+'.fitex2.test'

  ;the file we are plotting to
  set_plot,'PS',/copy
  device,/landscape,/COLOR,BITS_PER_PIXEL=8,filename='/rst/output_plots/'+time+'.fitex2.ps'

  S = findgen(17)*(!PI*2./16.)
  !p.multi = [0,1,1]

	;open the file
  openr,unit,file_in,/get_lun

	;read the first lines
  readf,unit,nrang,mplgs,skynoise,tfreq,mpinc,nslopes,diff
  readf,unit,stid,yr,mo,dy,hr,mt,sc,bmnum,cpid,nave,lagfr,smsep,vdir


  lambda = 3.e8/(tfreq*1.e3)
	;get rad info
  radar_info,stid,glat,glon,mlat,mlon,oneletter,threeletter,name,fix(stid)

    if(hr lt 10) then hrstr = '0'+strtrim(round(hr),2) $
  else hrstr = strtrim(round(hr),2)

  if(mt lt 10) then mtstr = '0'+strtrim(round(mt),2) $
  else mtstr = strtrim(round(mt),2)

  if(sc lt 10) then scstr = '0'+strtrim(round(sc),2) $
  else scstr = strtrim(round(sc),2)

  date_str = name+'	 '+$
							strtrim(fix(yr),2)+'/'+strtrim(fix(mo),2)+'/'+strtrim(fix(dy),2)
  date_str = date_str+'	 '
  date_str = date_str+hrstr+':'+mtstr+':'+scstr+' UT'
  date_str = date_str+'		'
  date_str = date_str+'Beam: '+strtrim(fix(bmnum),2)
    date_str = date_str+'		'
  date_str = date_str+'Freq: '+strtrim(fix(tfreq),2)+' kHz'
  date_str = date_str + '  FITEX2  '

  mystr = 'Nave: '+strtrim(round(nave),2)
  mystr = mystr + '  CPID: '+strtrim(round(cpid),2)+' ('+get_cp_name(round(cpid))+')'
  mystr = mystr + '  Noise: '+strtrim(round(skynoise),2)
  mystr = mystr + '  Lagfr: '+strtrim(round(lagfr),2)+' us'
  mystr = mystr + '  Smsep: '+strtrim(round(smsep),2)+' us'

  ;declare the arrays
	lagnums = intarr(nrang,mplgs)
	acfs = dblarr(nrang,mplgs,2)
	pwr_flgs = intarr(nrang)
	lag_flgs = intarr(nrang)
	fluct_levs = intarr(nrang)
	widths = dblarr(nrang)
	snrs = dblarr(nrang)
	models = dblarr(nrang,nslopes*2+1,2)
	threshs = dblarr(nrang)
	mininds = intarr(nrang)
	model_vels = dblarr(nrang)
	pwr2_flgs = intarr(nrang)
	err_flgs = intarr(nrang)
	b_guesses = dblarr(nrang,101,2)
	b_nums = intarr(nrang)
	final_params = dblarr(nrang,3)
	decay_times = dblarr(nrang)
	y_inters = dblarr(nrang)
	sct_flgs = intarr(nrang)
	n_lags = intarr(nrang)
	bad_lags = intarr(nrang,mplgs)


	;read the rest of the file
	for i=0,nrang-1 do begin
		readf,unit,r,fluct
		fluct_levs(i) = fluct
		;read the acfs
		for j=0,mplgs-1 do begin
			readf,unit,l,re,im,bad
			lagnums(i,j) = l
			acfs(i,j,0) = re
			acfs(i,j,1) = im
			bad_lags(i,j) = bad
			if(bad eq 0) then n_lags(i) = n_lags(i) + 1
		endfor
		;read the params that determine if fitting is performed
		readf,unit,pflg,lflg
		pwr_flgs(i) = pflg
		lag_flgs(i) = lflg
		;if a fit was performed
		if(pflg AND lflg) then begin
			readf,unit,p,w,y,d
			widths(i) = w
			snrs(i) = p
			decay_times(i) = d
			y_inters(i) = y
			;read the models and errors
			for k=0,nslopes*2 do begin
				readf,unit,sl,e
				models(i,k,0) = sl
				models(i,k,1) = e
			endfor
			readf,unit,t,mini,vt,pflg,eflg,sflg
			err_flgs(i) = eflg
			mininds(i) = mini
			model_vels(i) = vt
			pwr2_flgs(i) = pflg
			threshs(i) = t
			sct_flgs(i) = sflg
			;if the fit is "good"
			if(sflg) then begin
				;read results of bisection
				for g=0,500 do begin
					readf,unit,temp
					if(temp eq 4321) then begin
						break
					endif else begin
						b_guesses(i,b_nums(i),0) = temp
						readf,unit,temp
						b_guesses(i,b_nums(i),1) = temp
						b_nums(i) = b_nums(i) + 1
					endelse
				endfor
				;read the resulting velocity fit params
				readf,unit,w,v,ve
				final_params(i,0) = w
				final_params(i,1) = v
				final_params(i,2) = ve
			endif
		endif
	endfor
	;close the input file
  close,unit
  free_lun,unit

	;plot the frontpage
	acf_plot_frontpage,yr,mo,dy,hr,mt,sc,name,bmnum,tfreq,nave,cpid,nrang,$
								skynoise,1,sct_flgs,snrs(*),final_params(*,1),$
								widths(*),n_lags,glat,lagfr,smsep

								
  ;start the plotting
  for i=0,nrang-1 do begin
		print,'range',i
		loadct,0

		;annotate the page
		xyouts,.5,.97,date_str+'Range: '+strtrim(i,2),align=.5,charsize=.8,charthick=3.,/normal
		xyouts,.5,.93,mystr,align=.5,charsize=.8,charthick=3.,/normal

		;plot the rawacf
		acf_plot_rawacf,acfs(i,*,0),acfs(i,*,1),mplgs,lagnums(i,*),bad_lags(i,*),[.1,.67,.5,.87]

		;check fitting status
		if(pwr_flgs(i) eq 0 OR lag_flgs(i) eq 0) then begin
			if(pwr_flgs(i) eq 0) then $
				xyouts,.5,.45,'Lag 0 Power too low for a fit',/normal,align=.5,charsize=1.5,charthick=3.
			if(lag_flgs(i) eq 0) then $
				xyouts,.5,.4,'Not enough good lags for a fit',/normal,align=.5,charsize=1.5,charthick=3.
			erase
			continue
		endif

		acf_plot_model_panel,models(i,*,0),models(i,*,1),threshs(i),[.1,.38,.83,.58]

		if(sct_flgs(i) eq 0) then begin
			loadct,0
			if(pwr2_flgs(i) eq 0) then $
				xyouts,.5,.25,'Power too low for a fit',/normal,align=.5,charsize=1.5,charthick=3.
			if(err_flgs(i) eq 0) then $
				xyouts,.5,.2,'Model Error too high for a fit',/normal,align=.5,charsize=1.5,charthick=3.
			erase
			continue
		endif

		;calculate fitted ACF
		mag = 10.^(snrs(i)/10)*skynoise
		dopfreq = 2.*!pi*(final_params(i,1)/vdir*2./lambda)
		t_d = lambda/(2.*!pi*widths(i))

		fitted_acfs = dblarr(mplgs,2)
 		for j=0,mplgs-1 do begin
			tau = mpinc*lagnums(i,j)
			fitted_acfs(j,0) = mag*exp(-1.0*tau/t_d)*cos(tau*dopfreq)
			fitted_acfs(j,1) = mag*exp(-1.0*tau/t_d)*sin(tau*dopfreq)
		endfor
		;plot the phase panel
		acf_plot_phase_panel,atan(acfs(i,*,1),acfs(i,*,0)),atan(fitted_acfs(*,1),fitted_acfs(*,0)),$
													mplgs,lagnums(i,*),bad_lags(i,*),final_params(i,1),-1,final_params(i,2),$
													[.1,.09,.39,.29]

		;plot the power panel
		acf_plot_power_panel,sqrt(acfs(i,*,1)^2+acfs(i,*,0)^2),sqrt(fitted_acfs(*,1)^2+fitted_acfs(*,0)^2),$
													mplgs,lagnums(i,*),bad_lags(i,*),snrs(i),widths(i),$
													[.54,.09,.83,.29]

		erase
	endfor


  ;close the postscript file
  device,/close
end

