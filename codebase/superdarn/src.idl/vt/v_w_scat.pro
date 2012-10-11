pro v_w_scat

  set_plot,'PS',/copy
	device,/landscape,filename='/rst/output_plots/corot_factor.ps',/COLOR,BITS_PER_PIXEL=8

	openr,in,'/rst/output_files/v_w_scat.txt',/get_lun

	Re = 6378.e3
	f_arr = dblarr(2,24,50000)
	f_num = dblarr(2,24)
	avg_f = dblarr(2,24)
	median_f = dblarr(2,24)
	stddev_f = dblarr(2,24)

	loadct,0
	plot,(findgen(24)+12) MOD 24,avg_f(0,*),linestyle=0,yrange=[.7,1.3],xstyle=1,ystyle=1,xrange=[0,23],pos=[.08,.08,.88,.95],$
			xtickname=strtrim(fix((findgen(24)+12) MOD 24),2),xticks=23,xtitle='MLT',ytitle='Corotation Factor',$
			title='Corotation Factor versus MLT Scatter Plot',thick=3.,charthick=3.,xthick=3.,ythick=3.,/nodata

	plots,[0,23],[1,1],/data,linestyle=2,thick=3.
	
	xyouts,.925,.755,'L = 3',/normal,charthick=3.
	xyouts,.925,.705,'L = 3.5',/normal,charthick=3.
	loadct,34
	polyfill,[.9,.92,.92,.9],[.75,.75,.77,.77],/normal,col=0
	polyfill,[.9,.92,.92,.9],[.70,.70,.72,.72],/normal,col=255
			
	while(~EOF(in)) do begin
		;read the next line in the file
		readf,in,time,velm,mlat,mlon,mazm,velg,glat,glon,gazm
		stat = TimeEpochToYMDHMS(yr,mo,dy,hr,mt,sc,time)
		;get mlt of the point
		mlt=MLTConvertEpoch(time,mlon)
		;get L-shell of the point
		L = 1./(cos(mlat*!pi/180.)^2)
		;find east-west velocity of the point
		v_zonal = velg*sin(gazm*!pi/180.)
		;distance from spin axis to backscatter point (assuming 300 km)
		r_lat = (Re+300e3)*sin((90.-glat)*!pi/180.)
		;corotation velocity at the scatter point in m/s
		v_corot = 2.*!pi*r_lat/86400.
		;corotation factor
		f_corot = (v_zonal+v_corot)/v_corot

		;get l-shell array index
		if(L gt 2.75 AND L le 3.25) then l_index = 0 $
		else if(L gt 3.25 AND L lt 3.75) then l_index = 1 $
		else continue

		plots,(mlt+12) MOD 24,f_corot,psym=3.,col=l_index*255.,/data

		;get mlt array index
		mlt_index = round(mlt)
		if(mlt_index eq 24) then mlt_index = 0

		f_arr(l_index,mlt_index,long(f_num(l_index,mlt_index))) = f_corot
		f_num(l_index,mlt_index) = f_num(l_index,mlt_index) + 1.
	endwhile

	;find the means and standard deviations in each bin
	for i=0,1 do begin
		for j=0,23 do begin
			if(f_num(i,j) lt 2) then continue
			avg_f(i,j) = mean(f_arr(i,j,0:f_num(i,j)-1),/double)
			median_f(i,j) = median(f_arr(i,j,0:f_num(i,j)-1),/double)
			stddev_f(i,j) = stddev(f_arr(i,j,0:f_num(i,j)-1),/double)
		endfor
	endfor

	for i=0,23 do begin
		print,f_num(0,i),avg_f(0,i),f_num(1,i),avg_f(1,i)
	endfor

	

	
	loadct,0
	plot,(findgen(24)+12) MOD 24,avg_f(0,*),linestyle=0,yrange=[.7,1.3],xstyle=1,ystyle=1,xrange=[0,23],pos=[.08,.08,.88,.95],$
					xtickname=strtrim(fix((findgen(24)+12) MOD 24),2),xticks=23,xtitle='MLT',ytitle='Corotation Factor',$
					title='Mean Corotation Factor versus MLT',thick=3.,charthick=3.,xthick=3.,ythick=3.,/nodata
	plots,[0,23],[1,1],/data,linestyle=2,thick=3.
	
	xyouts,.925,.755,'L = 3',/normal,charthick=3.
	xyouts,.925,.705,'L = 3.5',/normal,charthick=3.
	loadct,34
	polyfill,[.9,.92,.92,.9],[.75,.75,.77,.77],/normal,col=0
	polyfill,[.9,.92,.92,.9],[.70,.70,.72,.72],/normal,col=255
	
	for j=0,1 do begin
	;uncomment this for 2 separate plots
; 		plot,(findgen(24)+12) MOD 24,avg_f(j,*),linestyle=0,yrange=[.8,1.1],xstyle=1,xrange=[0,23],$
; 					xtickname=strtrim(fix((findgen(24)+12) MOD 24),2),xticks=23,xtitle='MLT',ytitle='Corotation Factor',$
; 					title='Corotation Factor versus MLT  L = '+strmid(strtrim(j*.5+3.,2),0,3),thick=3.,charthick=3.,xthick=3.,ythick=3.
		plots,(findgen(24)+12) MOD 24,avg_f(j,*),linestyle=0,/data,col=j*255.,thick=3.
		plots,[11,12],[avg_f(j,23),avg_f(j,0)],/data,col=j*255,thick=3.
		for i=0,23 do begin
			plots,([i,i]+12) MOD 24,[avg_f(j,i)+stddev_f(j,i),avg_f(j,i)-stddev_f(j,i)],/data,thick=3.,col=j*255
			plots,([i-.1,i+.1]+12) MOD 24,[avg_f(j,i)+stddev_f(j,i),avg_f(j,i)+stddev_f(j,i)],/data,thick=3.,col=j*255
			plots,([i-.1,i+.1]+12) MOD 24,[avg_f(j,i)-stddev_f(j,i),avg_f(j,i)-stddev_f(j,i)],/data,thick=3.,col=j*255
		endfor
	endfor

	loadct,0
	plot,(findgen(24)+12) MOD 24,avg_f(0,*),linestyle=0,yrange=[.7,1.3],xstyle=1,ystyle=1,xrange=[0,23],pos=[.08,.08,.88,.95],$
					xtickname=strtrim(fix((findgen(24)+12) MOD 24),2),xticks=23,xtitle='MLT',ytitle='Corotation Factor',$
					title='Median Corotation Factor versus MLT',thick=3.,charthick=3.,xthick=3.,ythick=3.,/nodata
	plots,[0,23],[1,1],/data,linestyle=2,thick=3.

	xyouts,.925,.755,'L = 3',/normal,charthick=3.
	xyouts,.925,.705,'L = 3.5',/normal,charthick=3.
	loadct,34
	polyfill,[.9,.92,.92,.9],[.75,.75,.77,.77],/normal,col=0
	polyfill,[.9,.92,.92,.9],[.70,.70,.72,.72],/normal,col=255
	
	for j=0,1 do begin
	;uncomment this for 2 separate plots
; 		plot,(findgen(24)+12) MOD 24,avg_f(j,*),linestyle=0,yrange=[.8,1.1],xstyle=1,xrange=[0,23],$
; 					xtickname=strtrim(fix((findgen(24)+12) MOD 24),2),xticks=23,xtitle='MLT',ytitle='Corotation Factor',$
; 					title='Corotation Factor versus MLT  L = '+strmid(strtrim(j*.5+3.,2),0,3),thick=3.,charthick=3.,xthick=3.,ythick=3.
		plots,(findgen(24)+12) MOD 24,median_f(j,*),linestyle=0,/data,col=j*255.,thick=3.
		plots,[11,12],[avg_f(j,23),median_f(j,0)],/data,col=j*255,thick=3.
		for i=0,23 do begin
			plots,([i,i]+12) MOD 24,[median_f(j,i)+stddev_f(j,i),median_f(j,i)-stddev_f(j,i)],/data,thick=3.,col=j*255
			plots,([i-.1,i+.1]+12) MOD 24,[median_f(j,i)+stddev_f(j,i),median_f(j,i)+stddev_f(j,i)],/data,thick=3.,col=j*255
			plots,([i-.1,i+.1]+12) MOD 24,[median_f(j,i)-stddev_f(j,i),median_f(j,i)-stddev_f(j,i)],/data,thick=3.,col=j*255
		endfor
	endfor


	close,in
  free_lun,in

  ;close the postscript file
  if(!d.name eq 'PS') then  device,/close

end