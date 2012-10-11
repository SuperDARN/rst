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
; 	plot_fitex2,time
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
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

pro plot_fitex2,time
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
  readf,unit,stid,yr,mo,dy,hr,mt,sc,bmnum,cpid

  lambda = 3.e8/(tfreq*1.e3)
	;get rad info
  radar_info,stid,glat,glon,mlat,mlon,oneletter,threeletter,name,fix(stid)
  date_str = name+'				'+$
							strtrim(fix(yr),2)+'/'+strtrim(fix(mo),2)+'/'+strtrim(fix(dy),2)
  date_str = date_str+'				'
  date_str = date_str+strtrim(fix(hr),2)+':'+strtrim(fix(mt),2)+':'+strtrim(fix(sc),2)
  date_str = date_str+'				'
  date_str = date_str+'Beam: '+strtrim(fix(bmnum),2)
    date_str = date_str+'				'
  date_str = date_str+'Freq: '+strtrim(fix(tfreq),2)

  ;declare the arrays
	lagnums = intarr(nrang,mplgs)
	acfs = dblarr(nrang,mplgs,2)
	good_lags = intarr(nrang,mplgs)
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


	;read the rest of the file
	for i=0,nrang-1 do begin
		readf,unit,r,fluct
		fluct_levs(i) = fluct
		;read the acfs
		for j=0,mplgs-1 do begin
			readf,unit,l,re,im,flg
			lagnums(i,j) = l
			acfs(i,j,0) = re
			acfs(i,j,1) = im
			good_lags(i,j) = flg
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

  ;start the plotting
  for i=0,nrang-1 do begin
		loadct,0
		;annotate the page
		xyouts,.5,.97,date_str+'				Range Gate : '+strtrim(i,2),align=.5,charsize=1.25,charthick=3.,/normal
		;plot the original ACF
		ymax = max(abs(acfs(i,*,*)))*1.5
		ptitle = 'Original Raw ACF'+'		'+$
							'CPID = '+strtrim(round(cpid),2)+'		'+$
							'Lag 0 Power = '+strtrim(round(acfs(i,0,0)),2)+'		'+$
							'Noise = '+strtrim(round(skynoise),2)+'		'+$
							'Pwr Flg = '+strtrim(round(pwr_flgs(i)),2)+'		'+$
							'Lag Flg = '+strtrim(round(lag_flgs(i)),2)
		plot,findgen(1),findgen(1),/nodata,xrange=[0,max(lagnums(i,*))],yrange=[-1.0*ymax,ymax],$
					xstyle=1,ystyle=1,xthick=4,ythick=4,pos=[.05,.77,.85,.92],/noerase,thick=2.,$
					yticklen=-.01,title=ptitle,charthick=3,charsize=.75
		loadct,34
		usersym,cos(S),sin(S)
		plots,0,acfs(i,0,0)+skynoise,psym=8,col=150
		for j=0,mplgs-1 do begin
			if(good_lags(i,j)) then p = 2 else p = 8
			plots,lagnums(i,j),acfs(i,j,0),psym=p,col=250
			plots,lagnums(i,j),acfs(i,j,1),psym=p,col=50
		endfor
		plots,lagnums(i,*),acfs(i,*,0),linestyle=0,col=250,thick=2
		plots,lagnums(i,*),acfs(i,*,1),linestyle=0,col=50,thick=2

		;annotate the plot
		plots,.89,.895,/normal,col=250,psym=2
		plots,.89,.865,/normal,col=50,psym=2
		plots,.89,.835,/normal,col=250,psym=8
		plots,.91,.835,/normal,col=50,psym=8
		plots,.89,.805,/normal,psym=8,col=150
		loadct,0
		xyouts,.91,.89,'Real',charthick=3.,/normal
		xyouts,.91,.86,'Imag',charthick=3.,/normal
		xyouts,.93,.83,'Bad',charthick=3.,/normal
		xyouts,.91,.80,'P0 + n',charthick=3.,/normal

		if(pwr_flgs(i) eq 0 OR lag_flgs(i) eq 0) then begin
			if(pwr_flgs(i) eq 0) then $
				xyouts,.5,.65,'Lag 0 Power too low for a fit',/normal,align=.5,charsize=1.5,charthick=3.
			if(lag_flgs(i) eq 0) then $
				xyouts,.5,.6,'Not enough food lags for a fit',/normal,align=.5,charsize=1.5,charthick=3.
			erase
			continue
		endif

		;plot the lag powers and fits
		loadct,0
		ymax = max(abs(acfs(i,*,*)))*1.5
		ptitle = 'Power Fit'+'		'+$
							'SNR = '+strtrim(round(snrs(i,0,0)),2)+'		'+$
							'Spectral Width = '+strtrim(round(widths(i)),2)

		plot,findgen(1),findgen(1),/nodata,xrange=[0,max(lagnums(i,*))],yrange=[0,ymax],$
					xstyle=1,ystyle=1,xthick=4,ythick=4,pos=[.05,.56,.85,.71],/noerase,thick=2.,$
					yticklen=-.01,title=ptitle,charthick=3,charsize=.75
		loadct,34
		usersym,cos(S),sin(S)
		plots,[0,max(lagnums(i,*))],[fluct_levs(i),fluct_levs(i)],linestyle=2,col=250,thick=3
		pwr_fit = dblarr(mplgs)
		for j=0,mplgs-1 do begin
			pwr_fit(j) = exp(decay_times(i)*j+y_inters(i))
		endfor
		for j=0,mplgs-1 do begin
			if(good_lags(i,j)) then p = 7 else p = 8
			plots,lagnums(i,j),sqrt(acfs(i,j,0)^2+acfs(i,j,1)^2),psym=p,col=0
		endfor
		plots,findgen(mplgs),pwr_fit,linestyle=1,col=75
 		plots,findgen(mplgs),pwr_fit,psym=4,col=75

		;plot the model comparisons
		loadct,0
		ymax = max(abs(models(i,*,1)))*1.2
		ptitle = 'Velocity Model Fit'+'		'+$
							'Initial Model Guess = '+strmid(strtrim(models(i,mininds(i),0),2),0,6)+'		'+$
							'Initial Velocity Guess = '+strtrim(round(model_vels(i)),2)

		plot,findgen(1),findgen(1),/nodata,xrange=[-180,180],yrange=[0,ymax],$
					xstyle=1,ystyle=1,xthick=4,ythick=4,pos=[.05,.35,.85,.5],/noerase,thick=2.,$
					yticklen=-.01,title=ptitle,charthick=3,charsize=.75
		loadct,34
		usersym,cos(S),sin(S),/FILL
		plots,[-180,180],[threshs(i),threshs(i)],linestyle=2,col=250
		plots,models(i,*,0),models(i,*,1),linestyle=0,col=50,thick=3
		plots,models(i,*,0),models(i,*,1),psym=8,col=50,symsize=.6

		if(sct_flgs(i) eq 0) then begin
			loadct,0
			if(pwr2_flgs(i) eq 0) then $
				xyouts,.5,.25,'Power too low for a fit',/normal,align=.5,charsize=1.5,charthick=3.
			if(err_flgs(i) eq 0) then $
				xyouts,.5,.2,'Model Error too high for a fit',/normal,align=.5,charsize=1.5,charthick=3.
			erase
			continue
		endif

		plots,[models(i,mininds(i),0)+diff,models(i,mininds(i),0)+diff],[0,ymax],linestyle=0,col=250,thick=1
		plots,[models(i,mininds(i),0)-diff,models(i,mininds(i),0)-diff],[0,ymax],linestyle=0,col=250,thick=1
		plots,models(i,mininds(i),0),models(i,mininds(i),1),psym=2,col=225,symsize=2

		;plot the results of the bisection method
		loadct,0
		ymax = max(abs(models(i,*,1)))*1.2
		ptitle = 'Bisection Method'+'		'+$
							'Final Model = '+strmid(strtrim(final_params(i,0),2),0,6)+'		'+$
							'Iterations = '+strtrim(round(b_nums(i)),2)

		ymax = models(i,mininds(i),1)*1.2

		plot,findgen(1),findgen(1),/nodata,xrange=[models(i,mininds(i),0)-diff,models(i,mininds(i),0)+diff],$
					yrange=[0,ymax],xstyle=1,ystyle=1,xthick=4,ythick=4,pos=[.05,.14,.4,.29],/noerase,thick=2.,$
					yticklen=-.01,title=ptitle,charthick=3,charsize=.75
		loadct,34
		usersym,cos(S),sin(S),/FILL

		plots,b_guesses(i,0:b_nums(i)-1,0),b_guesses(i,0:b_nums(i)-1,1),psym=8,col=250,symsize=.5
		plots,final_params(i,0),final_params(i,2),psym=2,col=225,symsize=2


		;plot the phase fit
		loadct,0
		ptitle = 'Phase Fit:'+'		'+$
							'Final Velocity = '+strtrim(round(final_params(i,1)),2)+'		'+$
							'Velocity Error= '+strtrim(round(final_params(i,2)),2)

		plot,findgen(1),findgen(1),/nodata,xrange=[0,max(lagnums(i,*))],yrange=[-!pi,!pi],$
					xstyle=1,ystyle=1,xthick=4,ythick=4,pos=[.5,.14,.85,.29],/noerase,thick=2.,$
					yticklen=-.01,title=ptitle,charthick=3,charsize=.75
; 		loadct,34
; 		usersym,cos(S),sin(S),/FILL
; 		phase_fit = dblarr(mplgs)
; 		for j=0,mplgs-1 do begin
; 			t = final_params(i,0)*!pi/180.
; 			phi = t*j
; 			r = fix(t*j/!pi)
; 			phase_fit(j) = phi - r*!pi*2
; 		endfor
; 		for j=0,mplgs-1 do begin
; 			if(good_lags(i,j)) then $
; 				plots,lagnums(i,j),atan(acfs(i,j,1),acfs(i,j,0)),psym=8,col=0
; 		endfor
; 		usersym,cos(S),sin(S)
; 		plots,findgen(mplgs),phase_fit,linestyle=1,col=150
;  		plots,findgen(mplgs),phase_fit,psym=8,col=150


		mag = 10.^(snrs(i)/10)*skynoise
		dopfreq = 2.*!pi*(final_params(i,1)*2./lambda)
		t_d = lambda/(2.*!pi*final_params(i,0))

		fitted_acfs = dblarr(mplgs,2)
 		for j=0,mplgs-1 do begin
			tau = mpinc*lagnums(i,j)
			fitted_acfs(j,0) = mag*exp(-1.0*tau/t_d)*cos(tau*dopfreq)
			fitted_acfs(j,1) = mag*exp(-1.0*tau/t_d)*sin(tau*dopfreq)
		endfor

		loadct,34
		phases = dblarr(mplgs)
		for j=0,mplgs-1 do begin
			;plot the actual ACF
			if(good_lags(i,j)) then begin
				usersym,cos(S),sin(S),/FILL
				plots,lagnums(i,j),atan(acfs(i,j,1),acfs(i,j,0)),psym=8,col=0
				usersym,cos(S),sin(S)
			endif
			plots,lagnums(i,j),atan(fitted_acfs(j,1),fitted_acfs(j,0)),psym=8,col=150
			phases(j) = atan(fitted_acfs(j,1),fitted_acfs(j,0))
		endfor
		plots,lagnums(i,*),phases(*),linestyle=1,col=150,thick=2

		erase
	endfor


  ;close the postscript file
  device,/close
end

