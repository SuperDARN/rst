pro fitstat


  ;the file we are reading data from
  file_in = '/rst/output_files/fitcomp.txt'
  set_plot,'PS',/copy
  device,/landscape,/COLOR,BITS_PER_PIXEL=8,filename='/rst/output_plots/fitstat.ps'

  !p.multi = [0,1,1]
  openr,unit,file_in,/get_lun

  errors = dblarr(20,10,3,5)
  nums = dblarr(20,10)

	for i=0,19 do begin
		for j=0,9 do begin
			for k=0,4 do begin
				readf,unit,v,t,k,v1,v2,v3,num
				nums(i,j) = num
				errors(i,j,0,k) = v1
				errors(i,j,1,k) = v2
				errors(i,j,2,k) = v3
			endfor
	  endfor
	endfor

	v_max = 150.

	xlab=['FitACF','FitEX2','LMFit',' ']

	;first, plot V RMS error while keeping t_d constant
  for i=0,9 do begin
		loadct,0
		plot,findgen(2),findgen(2),xrange=[0,2000],yrange=[0,3],pos=[.05,.65,.8,.9],/nodata,/noerase,xticks=20,$
										yticks=3,xstyle=1,ystyle=1,charthick=3.,charsize=.9,xtitle="simulated velocity",$
										ytitle="fitting method",title="RMS Velocity error, t_d = "+strtrim((i+1)*.01,2),ytickname=xlab
		draw_colorbar,v_max,0,"RMS Error","m/s",[.83,.4,.86,.75],ct=34
		loadct,34
		for j=0,19 do begin
			for k=0,2 do begin
				err = errors(j,i,k,0)
				col = (err/v_max) * 255.
				if(col gt 255) then col = 255
				if(col lt 0) then col = 0
				polyfill,[j*100,(j+1)*100,(j+1)*100,j*100],[k,k,k+1,k+1],color=col
			endfor
		endfor

		erase
  endfor


	v_max = 75.
  ;first, plot V "true" error while keeping t_d constant
  for i=0,9 do begin
		loadct,0
		plot,findgen(2),findgen(2),xrange=[0,2000],yrange=[0,3],pos=[.05,.65,.8,.9],/nodata,/noerase,xticks=20,$
										yticks=3,xstyle=1,ystyle=1,charthick=3.,charsize=.9,xtitle="simulated velocity",$
										ytitle="fitting method",title="True Velocity error, t_d = "+strtrim((i+1)*.01,2),ytickname=xlab
		draw_colorbar,v_max,(-1)*v_max,"Vel Error","m/s",[.83,.4,.86,.75],ct=33
		loadct,33
		for j=0,19 do begin
			for k=0,2 do begin
				err = errors(j,i,k,1)
				col = (err/v_max+1)
				if(col gt 2) then col = 2
				if(col lt 0) then col = 0
				col = col/2*255.
				polyfill,[j*100,(j+1)*100,(j+1)*100,j*100],[k,k,k+1,k+1],color=col
			endfor
		endfor

		polyfill,[.83,.86,.86,.83],[.23,.23,.28,.28],color=126,/normal
		loadct,0
		xyouts,.87,.249,'0',/normal,charsize=1,charthick=3.

		erase
  endfor

  e_max = .55
  ;first, plot t_d RMS error while keeping V constant
  for i=0,19 do begin
		loadct,0
		plot,findgen(2),findgen(2),xrange=[.005,.105],yrange=[0,3],pos=[.05,.65,.8,.9],/nodata,/noerase,xticks=10,$
										yticks=4,xstyle=1,ystyle=1,charthick=3.,charsize=.9,xtitle="simulated t_d",$
										ytitle="fitting method",title="RMS decay time error (normalized), V = "+strtrim(fix(i*100.+50.),2),ytickname=xlab
		draw_colorbar,e_max+.15,0+.15,"RMS error",' ',[.83,.4,.86,.75],ct=34
		loadct,34
		print,errors(i,0,0,2)
		for j=0,9 do begin
			for k=0,2 do begin
				err = errors(i,j,k,2)/((j+1)*.01)-.15
				print,i,j,k,err
				col = (err/e_max)*255.
				if(col gt 255) then col = 255
				if(col lt 0) then col = 0
				polyfill,[j*.01+.005,(j+1)*.01+.005,(j+1)*.01+.005,j*.01+.005],[k,k,k+1,k+1],color=col
			endfor
		endfor
		
		erase
	endfor

	e_max = .2
	;first, plot t_d true error while keeping V constant
	for i=0,19 do begin
		loadct,0
		plot,findgen(2),findgen(2),xrange=[.005,.105],yrange=[0,3],pos=[.05,.65,.8,.9],/nodata,/noerase,xticks=10,$
										yticks=3,xstyle=1,ystyle=1,charthick=3.,charsize=.9,xtitle="simulated t_d",$
										ytitle="fitting method",title="True decay time error, V = "+strtrim(fix(i*100.+50.),2),ytickname=xlab
		draw_colorbar,e_max,-1*e_max,"t_d error","s",[.83,.4,.86,.75],ct=33
		loadct,33
		for j=0,9 do begin
			for k=0,2 do begin
				err = errors(i,j,k,3)
				col = (err/e_max+1)
				if(col gt 2) then col = 2
				if(col lt 0) then col = 0
				col = col/2*255.
				polyfill,[j*.01+.005,(j+1)*.01+.005,(j+1)*.01+.005,j*.01+.005],[k,k,k+1,k+1],color=col
			endfor
		endfor

		polyfill,[.83,.86,.86,.83],[.23,.23,.28,.28],color=126,/normal
		loadct,0
		xyouts,.87,.249,'0',/normal,charsize=1,charthick=3.

		erase
	endfor

	e_max = .15
	;first, plot ACFERR while keeping V constant
	for i=0,19 do begin
		loadct,0
		plot,findgen(2),findgen(2),xrange=[.005,.105],yrange=[0,3],pos=[.05,.65,.8,.9],/nodata,/noerase,xticks=10,$
										yticks=3,xstyle=1,ystyle=1,charthick=3.,charsize=.9,xtitle="simulated t_d",$
										ytitle="fitting method",title="ACF error error, V = "+strtrim(fix(i*100.+50.),2),ytickname=xlab
		draw_colorbar,.4,.25,"ACF error","s",[.83,.4,.86,.75],ct=34
		loadct,34
		for j=0,9 do begin
			for k=0,2 do begin
				err = errors(i,j,k,4)-.25
				col = (err/e_max)
				print,err+.25,err,e_max,col
				if(col gt 1) then col = 1
				if(col lt 0) then col = 0
				col = col*255.
				polyfill,[j*.01+.005,(j+1)*.01+.005,(j+1)*.01+.005,j*.01+.005],[k,k,k+1,k+1],color=col
			endfor
		endfor


		erase
	endfor

	e_max = .15
	;first, plot ACFERR while keeping V constant
	for i=0,9 do begin
		loadct,0
		plot,findgen(2),findgen(2),xrange=[0,2000],yrange=[0,3],pos=[.05,.65,.8,.9],/nodata,/noerase,xticks=20,$
										yticks=3,xstyle=1,ystyle=1,charthick=3.,charsize=.9,xtitle="simulated velocity",$
										ytitle="fitting method",title="ACF error error, t_d = "+strtrim((i+1)*.01,2),ytickname=xlab
		draw_colorbar,.4,.25,"ACF error"," ",[.83,.4,.86,.75],ct=34
		loadct,34
		for j=0,19 do begin
			for k=0,2 do begin
				err = errors(j,i,k,4)-.25
				col = (err/e_max)
				print,err+.25,err,e_max,col
				if(col gt 1) then col = 1
				if(col lt 0) then col = 0
				col = col*255.
				polyfill,[j*100,(j+1)*100,(j+1)*100,j*100],[k,k,k+1,k+1],color=col
			endfor
		endfor


		erase
	endfor



  close,unit
  free_lun,unit

  ;close the postscript file
  device,/close

end