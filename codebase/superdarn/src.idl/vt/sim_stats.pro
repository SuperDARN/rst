pro sim_stats

  set_plot,'PS',/copy
  device,/landscape,/COLOR,BITS_PER_PIXEL=8,filename='/rst/output_plots/sim_stats.ps'
	;go back to davit ct
	init_colors


	nave = 50.
	mplgs = 23
	mpinc = 1.5
	t_d = 30
	vel=350
	acf_power = dblarr(1000,mplgs)
	acf_phase = dblarr(1000,mplgs)
	lagnums = dblarr(1000,mplgs)
	n_acfs = 0L
	freq = 12.e6
	lambda = 3e8/freq
	dfreq = 2.*vel/lambda
	print,dfreq
	
	;*****************************************************
	;************* READ THE RAWACF FILE ******************
	;*****************************************************
	inp=FitOpen('/data/fit/aj/sim_test.rawacf',/read)
  while RawRead(inp,prm,raw) ne -1 do begin
		for i=0,prm.nrang-1 do begin
				for j=0,prm.mplgs-1 do begin
					acf_power(n_acfs,j) = sqrt(raw.acfd(i,j,0)^2+raw.acfd(i,j,1)^2)
					acf_phase(n_acfs,j) = atan(raw.acfd(i,j,1),raw.acfd(i,j,0))
					lagnums(n_acfs,j) = abs(prm.lag(j,0) - prm.lag(j,1))*mpinc
				endfor
				n_acfs = n_acfs+1
		endfor
  endwhile
	free_lun,inp


	;*****************************************************
	;************* PLOT THE POWER STUFF ******************
	;*****************************************************
	plot_colorbar,position=[.73,.35,.76,.7],scale=[0,160],/continuous,legend='Number of measurements',parameter='power'
	

  plot,findgen(1),findgen(1),/nodata,xrange=[0,max(lagnums(0,*))],yrange=[0,1.5],$
				xstyle=1,ystyle=1,xtitle='ACF Lag, ms',ytitle='ACF Power (normalized)',$
				position=[.1,.1,.7,.8],title='ACF Histogram for t_d='+strtrim(t_d,2)+' ms, V=350 m/s, N_avg=50, N=1000',$
				xthick=5.,ythick=5.,charthick=3.,xticklen=-.01,yticklen=-.01


	last_coord = 0.
	bsize = .025
	loadct,34
	for i=0,mplgs-1 do begin
		powhist = histogram(acf_power(*,i),max=1.5,min=0,binsize=bsize)
		for j=0,n_elements(powhist)-1 do begin
			if(powhist(j) eq 0) then continue
			color = (powhist(j)/160.)*255.
			if(color lt 0) then color = 0
			if(color gt 255) then color = 255
			if(i ne mplgs-1) then next_coord = lagnums(0,i) + (lagnums(0,i+1) - lagnums(0,i))/2. $
			else next_coord = max(lagnums(0,*))
; 			print,last_coord,next_coord,j*.1,(j+1)*.1,color
			polyfill,[last_coord,next_coord,next_coord,last_coord],[j*bsize,j*bsize,(j+1)*bsize,(j+1)*bsize],col=color,/data
		endfor
		last_coord = next_coord
	endfor
	
	;go back to davit ct
	init_colors


	;plot the statistical fluctuation level
	plots,[0,max(lagnums(0,*))],[1./sqrt(nave),1./sqrt(nave)],linestyle=1,thick=5.
	;plot the e-folding time
	plots,[t_d,t_d],[0,1.5],linestyle=3,thick=5.
	;plot the e-folding power
	plots,[0,max(lagnums(0,*))],[exp(-1),exp(-1)],linestyle=3,thick=5.
	;plot the perfect line
	plots,lagnums(0,*),exp(-lagnums(0,*)/(t_d)),linestyle=0,thick=5.
	
	for i=0,mplgs-1 do begin
		plots,lagnums(0,i),mean(acf_power(*,i)),psym=4,thick=3.
	endfor

	erase

	;*****************************************************
	;************* PLOT THE PHASE STUFF ******************
	;*****************************************************
	plot_colorbar,position=[.73,.35,.76,.7],scale=[0,160],/continuous,legend='Number of measurements',parameter='power'


  plot,findgen(1),findgen(1),/nodata,xrange=[0,max(lagnums(0,*))],yrange=[-!pi,!pi],$
				xstyle=1,ystyle=1,xtitle='ACF Lag, ms',ytitle='ACF Phase',$
				position=[.1,.1,.7,.8],title='ACF Histogram for t_d='+strtrim(t_d,2)+' ms, V=350 m/s, N_avg=50, N=1000',$
				xthick=5.,ythick=5.,charthick=3.,xticklen=-.01,yticklen=-.01


	last_coord = 0.
	bsize = .025
	loadct,34
	for i=0,mplgs-1 do begin
		phahist = histogram(acf_phase(*,i),max=!pi,min=-!pi,binsize=bsize)
		for j=0,n_elements(phahist)-1 do begin
			if(phahist(j) eq 0) then continue
			color = (phahist(j)/160.)*255.
			if(color lt 0) then color = 0
			if(color gt 255) then color = 255
			if(i ne mplgs-1) then next_coord = lagnums(0,i) + (lagnums(0,i+1) - lagnums(0,i))/2. $
			else next_coord = max(lagnums(0,*))
			polyfill,[last_coord,next_coord,next_coord,last_coord],[-!pi+j*bsize,-!pi+j*bsize,-!pi+(j+1)*bsize,-!pi+(j+1)*bsize],col=color,/data
		endfor
		last_coord = next_coord
	endfor

	init_colors
	
	;plot the perfect line
	for i=1,mplgs-1 do begin
		if(2.*!pi*lagnums(0,i-1)*1e-3*dfreq le !pi) then $
			plots,[lagnums(0,i-1),lagnums(0,i)],[2.*!pi*lagnums(0,i-1)*1e-3*dfreq,2.*!pi*lagnums(0,i)*1e-3*dfreq],linestyle=0,thick=5. $
		else $
			plots,[lagnums(0,i-1),lagnums(0,i)],[2.*!pi*lagnums(0,i-1)*1e-3*dfreq-2.*!pi,2.*!pi*lagnums(0,i)*1e-3*dfreq-2.*!pi],linestyle=0,thick=5
	endfor


	;close the postscript file
  device,/close


end


