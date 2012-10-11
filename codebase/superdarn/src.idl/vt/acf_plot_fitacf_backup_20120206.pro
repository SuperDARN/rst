;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
;NAME:
; acf_plot_fitacf
;
; PURPOSE:
; Plots the results of fitting ACFs from 1 beam sounding with the fitacf algorithm
;
; CATEGORY:
; Graphics
;
; CALLING SEQUENCE:
; first call the c routine test_fitacf, e.g.
; 	test_fitacf [-new] -hr 5 -min 3 -beam 7 myfile.rawacf > /rst/output_files/timestamp.fitacf.test
;
; next, call the IDL routine, e.g.
; 	acf_plot_fitacf,time
;
;	INPUTS:
;		time:  a string with a timestamp to be used for a file name
;
; OPTIONAL INPUTS:
;
; KEYWORD PARAMETERS:
;
; EXAMPLE:
; test_fitacf -new -hr 5 -min 3 -beam 7 myfile.rawacf > /rst/output_files/timestamp.lmfit.test
; plot_fitacf
;
; OUTPUT:
; /rst/output_plots/timestamp.fitacf.ps
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
; Written by AJ Ribeiro 08/29/2011
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


pro acf_plot_fitacf,time
  ;the file we are reading data from
  file_in = '/rst/output_files/'+time+'.fitacf.test'
  set_plot,'PS',/copy
  device,/landscape,/COLOR,BITS_PER_PIXEL=8,filename='/rst/output_plots/'+time+'.fitacf.ps'
  S = findgen(17)*(!PI*2./16.)
  !p.multi = [0,1,1]

	;open the file
  openr,unit,file_in,/get_lun

	;read the first line
	readf,unit,stid,yr,mo,dy,hr,mt,sc,bmnum
  readf,unit,nrang,mplgs,skynoise,tfreq,mpinc,lagfr,smsep,nave,cpid
	lambda = 3.e8/(tfreq*1.e3)
	;get rad info
  radar_info,stid,glat,glon,mlat,mlon,oneletter,threeletter,name,fix(stid)

  date_str = name+'	 '+$
							strtrim(fix(yr),2)+'/'+strtrim(fix(mo),2)+'/'+strtrim(fix(dy),2)
  date_str = date_str+'	 '
  date_str = date_str+strtrim(fix(hr),2)+':'+strtrim(fix(mt),2)+':'+strtrim(fix(sc),2)+' UT'
  date_str = date_str+'		'
  date_str = date_str+'Beam: '+strtrim(fix(bmnum),2)
    date_str = date_str+'		'
  date_str = date_str+'Freq: '+strtrim(fix(tfreq),2)+' kHz'
  date_str = date_str + '  FITACF  '

  mystr = 'Nave: '+strtrim(round(nave),2)
  mystr = mystr + '  CPID: '+strtrim(round(cpid),2)+' ('+get_cp_name(round(cpid))+')'
  mystr = mystr + '  Noise: '+strtrim(round(skynoise),2)
  mystr = mystr + '  Lagfr: '+strtrim(round(lagfr),2)+' us'
  mystr = mystr + '  Smsep: '+strtrim(round(smsep),2)+' us'

	;declare the arrays
	lagnums = intarr(nrang,mplgs)
	acfs = dblarr(nrang,mplgs,2)
	fitted_acfs = dblarr(mplgs,2)
	good_lags = intarr(nrang,mplgs)
	more_lags = intarr(nrang,mplgs)
	fit_flgs = intarr(nrang)
	omega_loc = dblarr(nrang)
	sct_flgs = intarr(nrang)
	fit_params = dblarr(nrang,4)
	first_stat = intarr(nrang)
	second_stat = intarr(nrang)
	n_lags = intarr(nrang)
	bad_lags = intarr(nrang,mplgs)
	;read the rest of the file
	for i=0,nrang-1 do begin
		readf,unit,r,thresh
		readf,unit,flg
		first_stat(i) = flg
		if(flg ne 0) then continue
		;read the acfs
		for j=0,mplgs-1 do begin
			readf,unit,lag,re,im,good
			lagnums(i,j) = lag
			acfs(i,j,0) = re
			acfs(i,j,1) = im
			good_lags(i,j) = good
			bad_lags(i,j) = good
		endfor
		readf,unit,flg
		second_stat(i) = flg
		if(flg ne 0) then continue
		for j=0,mplgs-1 do begin
			readf,unit,lag,re,im,good
			lagnums(i,j) = lag
			acfs(i,j,0) = re
			acfs(i,j,1) = im
			more_lags(i,j) = good
			bad_lags(i,j) = bad_lags(i,j) + good
			if(good) then n_lags(i) = n_lags(i) + 1
		endfor
		;read the params that determine if fitting is performed
		readf,unit,flg
		fit_flgs(i) = flg
		;if a fit was performed
		if(flg eq 0) then begin
			readf,unit,o_l
			omega_loc(i) = o_l
			readf,unit,qflg
			sct_flgs(i) = qflg
			if(qflg eq 1) then begin
				;read final params
				readf,unit,v,v_e,p_l,w_l
				fit_params(i,0) = v
				fit_params(i,1) = v_e
				fit_params(i,2) = p_l
				fit_params(i,3) = w_l
			endif
		endif
	endfor
	;close the input file
  close,unit
  free_lun,unit

  acf_plot_frontpage,yr,mo,dy,hr,mt,sc,name,bmnum,tfreq,nave,cpid,nrang,$
								skynoise,0,sct_flgs,fit_params(*,2),fit_params(*,0),$
								fit_params(*,3),n_lags,glat,lagfr,smsep


  for i=0,nrang-1 do begin
		loadct,0
		;annotate the page
		xyouts,.5,.97,date_str+'Range: '+strtrim(i,2),align=.5,charsize=.8,charthick=3.,/normal
		xyouts,.5,.93,mystr,align=.5,charsize=.8,charthick=3.,/normal

		if(first_stat(i) ne 0) then begin
			xyouts,.5,.85,'Fitting exited with status: -1 ',/normal,align=.5,charsize=1.,charthick=3.
			erase
			continue
		endif

; 		;plot the original ACF
		ymax = max(abs(acfs(i,*,*)))*1.5
; 		ptitle = 'Original Raw ACF'+'		'+$
; 							'Lag 0 Power = '+strtrim(round(acfs(i,0,0)),2)+'		'+$
; 							'Noise = '+strtrim(round(skynoise),2)+'		'
; 		plot,findgen(1),findgen(1),/nodata,xrange=[0,max(lagnums(i,*))],yrange=[-1.0*ymax,ymax],$
; 					xstyle=1,ystyle=1,xthick=4,ythick=4,pos=[.05,.77,.85,.92],/noerase,thick=3.,$
; 					yticklen=-.01,title=ptitle,charthick=3,charsize=.75,xtitle='lag',ytitle='level'
; 		loadct,34
; 		for j=0,mplgs-1 do begin
; 			p = 2
; 			plots,lagnums(i,j),acfs(i,j,0),psym=p,col=250
; 			plots,lagnums(i,j),acfs(i,j,1),psym=p,col=50
; 		endfor
; 		plots,lagnums(i,*),acfs(i,*,0),linestyle=0,col=250,thick=3
; 		plots,lagnums(i,*),acfs(i,*,1),linestyle=0,col=50,thick=3

		acf_plot_rawacf,acfs(i,*,0),acfs(i,*,1),mplgs,lagnums(i,*),bad_lags(i,*)

; 		;annotate the plot
; 		plots,.89,.895,/normal,col=250,psym=2
; 		plots,.89,.865,/normal,col=50,psym=2
; 		loadct,0
; 		xyouts,.91,.89,'Real',charthick=3.,/normal
; 		xyouts,.91,.86,'Imag',charthick=3.,/normal

		if(second_stat(i) ne 0) then begin
			xyouts,.5,.71,'Fitting exited with status: 1 ',/normal,align=.5,charsize=1.5,charthick=3.
			erase
			continue
		endif
; 
; 		;plot the badlags
; 		loadct,0
; 		;plot the original ACF
; 		ptitle = 'Original Raw ACF with badlags [FitACFCkRng+]'
; 		plot,findgen(1),findgen(1),/nodata,xrange=[0,max(lagnums(i,*))],yrange=[-1.0*ymax,ymax],$
; 					xstyle=1,ystyle=1,xthick=4,ythick=4,pos=[.05,.56,.85,.71],/noerase,thick=3.,$
; 					yticklen=-.01,title=ptitle,charthick=3,charsize=.75,xtickname=replicate(' ',20)
; 		loadct,34
; 		usersym,cos(S),sin(S)
; 		for j=0,mplgs-1 do begin
; 			if(good_lags(i,j)) then p = 2 else p = 8
; 			plots,lagnums(i,j),acfs(i,j,0),psym=p,col=250
; 			plots,lagnums(i,j),acfs(i,j,1),psym=p,col=50
; 		endfor
; 		plots,lagnums(i,*),acfs(i,*,0),linestyle=0,col=250,thick=3
; 		plots,lagnums(i,*),acfs(i,*,1),linestyle=0,col=50,thick=3
; 
; 		;annotate the plot
; ; 		plots,.89,.895,/normal,col=250,psym=2
; ; 		plots,.89,.865,/normal,col=50,psym=2
; ; 		plots,.89,.835,/normal,col=250,psym=8
; ; 		plots,.91,.835,/normal,col=50,psym=8
; ; 		loadct,0
; ; 		xyouts,.91,.89,'Real',charthick=3.,/normal
; ; 		xyouts,.91,.86,'Imag',charthick=3.,/normal
; ; 		xyouts,.93,.83,'Bad',charthick=3.,/normal

; 		;plot the badlags
; 		loadct,0
; 		;plot the original ACF
; 		ptitle = 'Original Raw ACF with more badlags [more_badlags+]'
; 		plot,findgen(1),findgen(1),/nodata,xrange=[0,max(lagnums(i,*))],yrange=[-1.0*ymax,ymax],$
; 					xstyle=1,ystyle=1,xthick=4,ythick=4,pos=[.05,.35,.85,.5],/noerase,thick=3.,$
; 					yticklen=-.01,title=ptitle,charthick=3,charsize=.75,xtickname=replicate(' ',20)
; 		loadct,34
; 		usersym,cos(S),sin(S)
; 		for j=0,mplgs-1 do begin
; 			if(more_lags(i,j)) then p = 2 else p = 8
; 			plots,lagnums(i,j),acfs(i,j,0),psym=p,col=250
; 			plots,lagnums(i,j),acfs(i,j,1),psym=p,col=50
; 		endfor
; 		plots,lagnums(i,*),acfs(i,*,0),linestyle=0,col=250,thick=3
; 		plots,lagnums(i,*),acfs(i,*,1),linestyle=0,col=50,thick=3
; 
; 		;annotate the plot
; ; 		plots,.89,.895,/normal,col=250,psym=2
; ; 		plots,.89,.865,/normal,col=50,psym=2
; ; 		plots,.89,.835,/normal,col=250,psym=8
; ; 		plots,.91,.835,/normal,col=50,psym=8
; ; 		loadct,0
; ; 		xyouts,.91,.89,'Real',charthick=3.,/normal
; ; 		xyouts,.91,.86,'Imag',charthick=3.,/normal
; ; 		xyouts,.93,.83,'Bad',charthick=3.,/normal
; 
		if(fit_flgs(i) ne 0) then begin
			xyouts,.5,.30,'Fitting exited with status: '+strtrim(fix(fit_flgs(i)),2),/normal,align=.5,charsize=1.5,charthick=3.
			if(fit_flgs(i) eq 4) then $
				xyouts,.5,.25,'(not enough good lags)',/normal,align=.5,charsize=1.5,charthick=3.
			if(fit_flgs(i) eq 2) then $
				xyouts,.5,.25,'(calc_phi_res returned bad status)',/normal,align=.5,charsize=1.5,charthick=3.
			if(fit_flgs(i) eq 3) then $
				xyouts,.5,.25,'(c_log < 0)',/normal,align=.5,charsize=1.5,charthick=3.
			erase
			continue
		endif
; 
; 
; 				;plot the velocity
; 		loadct,0
; 		ptitle = 'Vel = '+strtrim(round(fit_params(i,0)),2)+' m/s	'+$
; 							' O_loc = '+strtrim(round(omega_loc(i)),2)+$
; 							'  Verr = '+strtrim(round(fit_params(i,1)),2)
; 
; 		plot,findgen(1),findgen(1),/nodata,xrange=[0,max(lagnums(i,*))],yrange=[-!pi,!pi],$
; 					xstyle=1,ystyle=1,xthick=4,ythick=4,pos=[.05,.15,.4,.3],/noerase,thick=3.,$
; 					yticklen=-.01,title=ptitle,charthick=3,charsize=1,xtitle='lag',ytitle='phase'
; 
		mag = 10.^(fit_params(i,2)/10)*skynoise
		dopfreq = 2.*!pi*(fit_params(i,0)*2./lambda)
		t_d = lambda/(2.*!pi*fit_params(i,3))
		print,mag,i,lambda,skynoise,fit_params(i,2)
		for j=0,mplgs-1 do begin
			tau = mpinc*lagnums(i,j)
			fitted_acfs(j,0) = mag*exp(-1.0*tau/t_d)*cos(tau*dopfreq)
			fitted_acfs(j,1) = mag*exp(-1.0*tau/t_d)*sin(tau*dopfreq)
		endfor

		acf_plot_phase_panel,atan(acfs(i,*,1),acfs(i,*,0)),atan(fitted_acfs(*,1),fitted_acfs(*,0)),$
													mplgs,lagnums(i,*),bad_lags(i,*),fit_params(i,0),omega_loc(i),fit_params(i,1),$
													[.1,.38,.39,.58]
		
; 
; 		loadct,34
; 		phases = dblarr(mplgs)
; 		for j=0,mplgs-1 do begin
; 			;plot the actual ACF
; 			if(good_lags(i,j)) then begin
; 				usersym,cos(S),sin(S),/FILL
; 				plots,lagnums(i,j),atan(acfs(i,j,1),acfs(i,j,0)),psym=8,col=0
; 				usersym,cos(S),sin(S)
; 			endif
; 			plots,lagnums(i,j),atan(fitted_acfs(j,1),fitted_acfs(j,0)),psym=8,col=150
; 			phases(j) = atan(fitted_acfs(j,1),fitted_acfs(j,0))
; 		endfor
; 		plots,lagnums(i,*),phases(*),linestyle=1,col=150,thick=3
; 
; 		;annotate the plot
; 		usersym,cos(S),sin(S),/FILL
; 		plots,.42,.255,/normal,col=0,psym=8
; 		usersym,cos(S),sin(S)
; 		plots,.42,.225,/normal,col=150,psym=8
; 		loadct,0
; 		xyouts,.44,.25,'ACF',charthick=3.,/normal
; 		xyouts,.44,.22,'Fit',charthick=3.,/normal
; 
; 
; 		;;;;;;;;;;;;;;;;;;;;;;;;
; 		;plot the spectral width
; 		;;;;;;;;;;;;;;;;;;;;;;;;
; 		loadct,0
; 		ymax = max(alog(sqrt(fitted_acfs(*,1)^2+fitted_acfs(*,0)^2)))*1.1
; 		ymin = min(alog(sqrt(fitted_acfs(*,1)^2+fitted_acfs(*,0)^2)))*.9
; 		ptitle = 'Width = '+strtrim(round(fit_params(i,3)),2)+' m/s '+$
; 							'Power = '+strtrim(round(fit_params(i,2)),2)+' dB'
; 
; 
; 		plot,findgen(1),findgen(1),/nodata,xrange=[0,max(lagnums(i,*))],yrange=[ymin,ymax],$
; 					xstyle=1,ystyle=1,xthick=4,ythick=4,pos=[.5,.15,.85,.3],/noerase,thick=3.,$
; 					yticklen=-.01,title=ptitle,charthick=3,charsize=1,xtitle='lag'

		acf_plot_power_panel,sqrt(acfs(i,*,1)^2+acfs(i,*,0)^2),sqrt(fitted_acfs(*,1)^2+fitted_acfs(*,0)^2),$
													mplgs,lagnums(i,*),bad_lags(i,*),fit_params(i,2),fit_params(i,3),$
													[.54,.38,.83,.58]
; 
; 
; 		loadct,34
; 		powers = dblarr(mplgs)
; 		for j=0,mplgs-1 do begin
; 			;plot the actual ACF
; 			if(good_lags(i,j)) then begin
; 				usersym,cos(S),sin(S),/FILL
; 				plots,lagnums(i,j),alog(sqrt(acfs(i,j,1)^2+acfs(i,j,0)^2)),psym=8,col=250
; 				usersym,cos(S),sin(S)
; 			endif
; 			plots,lagnums(i,j),alog(sqrt(fitted_acfs(j,1)^2+fitted_acfs(j,0)^2)),psym=8,col=150
; 			powers(j) = alog(sqrt(fitted_acfs(j,1)^2+fitted_acfs(j,0)^2))
; 		endfor
; 		plots,lagnums(i,*),powers(*),linestyle=1,col=150,thick=3
; 
; 		;annotate the plot
; 		usersym,cos(S),sin(S),/FILL
; 		plots,.87,.255,/normal,col=250,psym=8
; 		usersym,cos(S),sin(S)
; 		plots,.87,.225,/normal,col=150,psym=8
; 		loadct,0
; 		xyouts,.89,.25,'ACF',charthick=3.,/normal
; 		xyouts,.89,.22,'Fit',charthick=3.,/normal




		erase
  endfor


  ;close the postscript file
  device,/close


end