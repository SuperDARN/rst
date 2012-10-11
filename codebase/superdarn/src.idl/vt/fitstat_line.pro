pro fitstat_line


  ;the file we are reading data from
  file_in = '/rst/output_files/fitcomp.txt'
  set_plot,'PS',/copy
  device,/landscape,/COLOR,BITS_PER_PIXEL=8,filename='/rst/output_plots/fitstat_line.ps'

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

	xlab=[' ',' ',' ',' ']

	;first, plot V RMS error while keeping t_d constant
  for i=0,9 do begin
		loadct,0
		plot,findgen(2),findgen(2),xrange=[0,2000],yrange=[1,10000],pos=[.05,.6,.5,.8],/nodata,/noerase,xticks=10,$
										yticks=4,xstyle=1,ystyle=1,charthick=3.,charsize=.9,xtitle="simulated velocity",$
										ytitle="error",title="RMS Velocity error, t_d = "+strtrim((i+1)*.01,2),/ylog,xthick=4,ythick=4

		loadct,34
		plots,findgen(20)*100.+50.,errors(*,i,0,0),col=250,thick=15

		plots,findgen(20)*100.+50.,errors(*,i,1,0),col=0,thick=15,linestyle=5

		plots,findgen(20)*100.+50.,errors(*,i,2,0),col=150,thick=15,linestyle=3

		plots,[.82,.87],[.87,.87],col=250,thick=15,/normal
		loadct,0
		xyouts,.88,.865,'FITACF',/normal,charthick=4
		loadct,34
		plots,[.82,.87],[.83,.83],col=0,thick=15,linestyle=5,/normal
		loadct,0
		xyouts,.88,.825,'FITEX2',/normal,charthick=4
		loadct,34
		plots,[.82,.87],[.79,.79],col=150,thick=15,linestyle=3,/normal
		loadct,0
		xyouts,.88,.785,'LMFIT',/normal,charthick=4
		loadct,34

		erase
  endfor


	v_max = 75.
  ;first, plot V "true" error while keeping t_d constant
  for i=0,9 do begin
		loadct,0
		plot,findgen(2),findgen(2),xrange=[0,2000],yrange=[-600,600],pos=[.05,.6,.5,.8],/nodata,/noerase,xticks=10,$
										yticks=6,xstyle=1,ystyle=1,charthick=3.,charsize=.9,xtitle="simulated velocity",$
										ytitle="error",title="True Velocity error, t_d = "+strtrim((i+1)*.01,2),xthick=4,ythick=4

		loadct,34
		plots,findgen(20)*100.+50.,errors(*,i,0,1),col=250,thick=15

		plots,findgen(20)*100.+50.,errors(*,i,1,1),col=0,thick=15,linestyle=5

		plots,findgen(20)*100.+50.,errors(*,i,2,1),col=150,thick=15,linestyle=3

		plots,[.82,.87],[.87,.87],col=250,thick=15,/normal
		loadct,0
		xyouts,.88,.865,'FITACF',/normal,charthick=4
		loadct,34
		plots,[.82,.87],[.83,.83],col=0,thick=15,linestyle=5,/normal
		loadct,0
		xyouts,.88,.825,'FITEX2',/normal,charthick=4
		loadct,34
		plots,[.82,.87],[.79,.79],col=150,thick=15,linestyle=3,/normal
		loadct,0
		xyouts,.88,.785,'LMFIT',/normal,charthick=4
		loadct,34

		erase
  endfor

  e_max = .55
  ;first, plot t_d RMS error while keeping V constant
  for i=0,19 do begin
		loadct,0
		plot,findgen(2),findgen(2),xrange=[.01,.1],yrange=[0,1],pos=[.05,.6,.5,.8],/nodata,/noerase,$
										yticks=3,xstyle=1,ystyle=1,charthick=3.,charsize=.9,xtitle="simulated t_d",xticks=9,$
										ytitle="fitting method",title="RMS decay time error (normalized), V = "+strtrim(fix(i*100.+50.),2),xthick=4,ythick=4

		loadct,34
		print,errors(i,0,0,2)
		plots,findgen(10)*.01+.01,errors(i,*,0,2)/((findgen(10)+1)*.01),col=250,thick=15

		plots,findgen(10)*.01+.01,errors(i,*,1,2)/((findgen(10)+1)*.01),col=0,thick=15,linestyle=5

		plots,findgen(10)*.01+.01,errors(i,*,2,2)/((findgen(10)+1)*.01),col=150,thick=15,linestyle=3

				plots,[.82,.87],[.87,.87],col=250,thick=15,/normal
		loadct,0
		xyouts,.88,.865,'FITACF',/normal,charthick=4
		loadct,34
		plots,[.82,.87],[.83,.83],col=0,thick=15,linestyle=5,/normal
		loadct,0
		xyouts,.88,.825,'FITEX2',/normal,charthick=4
		loadct,34
		plots,[.82,.87],[.79,.79],col=150,thick=15,linestyle=3,/normal
		loadct,0
		xyouts,.88,.785,'LMFIT',/normal,charthick=4
		loadct,34
		
		erase
	endfor



	e_max = .2
	;first, plot t_d true error while keeping V constant
	for i=0,19 do begin
		loadct,0
		plot,findgen(2),findgen(2),xrange=[.01,.1],yrange=[0,3],pos=[.05,.6,.5,.8],/nodata,/noerase,$
										yticks=3,xstyle=1,ystyle=1,charthick=3.,charsize=.9,xtitle="simulated t_d",xticks=9,$
										ytitle="fitting method",title="True decay time error, V = "+strtrim(fix(i*100.+50.),2),ytickname=xlab,xthick=4,ythick=4
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
	;first, plot ACFERR while keepfing V constant
	for i=0,19 do begin
		loadct,0
		plot,findgen(2),findgen(2),xrange=[.01,.1],yrange=[.25,.5],pos=[.05,.6,.5,.8],/nodata,/noerase,$
										yticks=5,xstyle=1,ystyle=1,charthick=3.,charsize=.9,xtitle="simulated t_d",$
										ytitle="error",title="ACF error, V = "+strtrim(fix(i*100.+50.),2),xticks=9,xthick=4,ythick=4

		loadct,34
		plots,findgen(10)*.01+.01,errors(i,*,0,4),col=250,thick=15

		plots,findgen(10)*.01+.01,errors(i,*,1,4),col=0,thick=15,linestyle=5

		plots,findgen(10)*.01+.01,errors(i,*,2,4),col=150,thick=15,linestyle=3


				plots,[.82,.87],[.87,.87],col=250,thick=15,/normal
		loadct,0
		xyouts,.88,.865,'FITACF',/normal,charthick=4
		loadct,34
		plots,[.82,.87],[.83,.83],col=0,thick=15,linestyle=5,/normal
		loadct,0
		xyouts,.88,.825,'FITEX2',/normal,charthick=4
		loadct,34
		plots,[.82,.87],[.79,.79],col=150,thick=15,linestyle=3,/normal
		loadct,0
		xyouts,.88,.785,'LMFIT',/normal,charthick=4
		loadct,34
		
		erase
	endfor

	e_max = .15
	;first, plot ACFERR while keeping V constant
	for i=0,9 do begin
		loadct,0
		plot,findgen(2),findgen(2),xrange=[0,2000],yrange=[.25,.5],pos=[.05,.6,.5,.8],/nodata,/noerase,xticks=10,$
										yticks=5,xstyle=1,ystyle=1,charthick=3.,charsize=.9,xtitle="simulated velocity",$
										ytitle="error",title="ACF error, t_d = "+strtrim((i+1)*.01,2),xthick=4,ythick=4

		loadct,34
		plots,findgen(20)*100.+50.,errors(*,i,0,4),col=250,thick=15

		plots,findgen(20)*100.+50.,errors(*,i,1,4),col=0,thick=15,linestyle=5

		plots,findgen(20)*100.+50.,errors(*,i,2,4),col=150,thick=15,linestyle=3

				plots,[.82,.87],[.87,.87],col=250,thick=15,/normal
		loadct,0
		xyouts,.88,.865,'FITACF',/normal,charthick=4
		loadct,34
		plots,[.82,.87],[.83,.83],col=0,thick=15,linestyle=5,/normal
		loadct,0
		xyouts,.88,.825,'FITEX2',/normal,charthick=4
		loadct,34
		plots,[.82,.87],[.79,.79],col=150,thick=15,linestyle=3,/normal
		loadct,0
		xyouts,.88,.785,'LMFIT',/normal,charthick=4
		loadct,34


		erase
	endfor



  close,unit
  free_lun,unit

  ;close the postscript file
  device,/close

end