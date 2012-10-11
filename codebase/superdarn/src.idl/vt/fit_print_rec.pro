;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
;NAME:
; plot_fitacf
;
; PURPOSE:
; outputs the records of a fitacf file into ascii format
;
; CATEGORY:
; misc
;
; CALLING SEQUENCE:
; print_rec2
;
;	INPUTS:
;		yr   				: year
;		mo   				: month
;		dy   				: day
;		shr   			: start hour
;		smt   			: start minute
;		ehr   			: end hour
;		emt   			: end minute
;		rad   			: radar abbreviation, eg 'bks'
;		outfile   	: file to output to
;
; OPTIONAL INPUTS:
;
; KEYWORD PARAMETERS:
;		/fitacf			: use fitacf file instead of fitex file
;
; EXAMPLE:
;
; OUTPUT:
; 	outfile
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
; Written by AJ Ribeiro 10/11/2011
;	based on print_rec written by Mike Ruohoniemi
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

pro fit_print_rec,yr,mo,dy,shr,smt,ehr,emt,rad,outfile,FITACF=fitacf

	common rad_data_blk
	common rt_data_blk


	;open the output file
	openw,rec_unit,outfile,/get_lun

	;date of file
	yr_str = strtrim(round(yr),2)
	if(mo lt 10) then mo_str = '0'+strtrim(round(mo),2) $
	else mo_str = strtrim(round(mo),2)
	if(dy lt 10) then dy_str = '0'+strtrim(round(dy),2) $
	else dy_str = strtrim(round(dy),2)
	date_str = yr_str+mo_str+dy_str

	;file type
	if(keyword_set(fitacf)) then rad_fit_read,date_str,rad,time=[shr*100L+smt,ehr*100L+emt],fitacf=1 $
	else rad_fit_read,date_str,rad,time=[shr*100L+smt,ehr*100L+emt],fitex=1

	ssc = 0
	esc = 0

	data_index = rad_fit_get_data_index()
	nrecs = (*rad_fit_info[data_index]).nrecs


	;get start and end times
	stime = JULDAY(mo, dy, yr, shr, smt, ssc)
	etime = JULDAY(mo, dy, yr, ehr, emt, esc)

;  Search for a specific time in the file
	for i=0L,(*rad_fit_info[data_index]).nrecs-1 do begin
		time = (*rad_fit_data[data_index]).juls[i]
		if(time gt etime) then break
		if(time ge stime) then begin
			caldat,(*rad_fit_data[data_index]).juls[i],mo,dy,yr,hr,mt,sc
			
			print,strtrim(yr,2)+'/'+strtrim(mo,2)+'/'+strtrim(dy,2)+'   '+$
						strtrim(hr,2)+':'+strtrim(mt,2)+':'+strtrim(fix(sc),2)+'   '+$
						(*rad_fit_info[data_index]).name

			printf,rec_unit,strtrim(yr,2)+'/'+strtrim(mo,2)+'/'+strtrim(dy,2)+'   '+$
						strtrim(hr,2)+':'+strtrim(mt,2)+':'+strtrim(fix(sc),2)+'   '+$
						(*rad_fit_info[data_index]).name

			print,'bmnum = '+strtrim((*rad_fit_data[data_index]).beam[i],2)+'   '+$
						'tfreq = '+strtrim(round((*rad_fit_data[data_index]).tfreq[i]),2)+'   '+$
						'sky_noise_lev = '+strtrim(round((*rad_fit_data[data_index]).noise.sky[i]),2)+'   '+$
						'search_noise_lev = '+strtrim(round((*rad_fit_data[data_index]).noise.search[i]),2)+'   '+$
						'xcf = '+strtrim(((*rad_fit_data[data_index]).xcf[i]+1)-1,2)+'   '+$
						'scan = '+strtrim((*rad_fit_data[data_index]).scan_mark[i],2)

			printf,rec_unit,'bmnum = '+strtrim((*rad_fit_data[data_index]).beam[i],2)+'   '+$
						'tfreq = '+strtrim(round((*rad_fit_data[data_index]).tfreq[i]),2)+'   '+$
						'sky_noise_lev = '+strtrim(round((*rad_fit_data[data_index]).noise.sky[i]),2)+'   '+$
						'search_noise_lev = '+strtrim(round((*rad_fit_data[data_index]).noise.search[i]),2)+'   '+$
						'xcf = '+strtrim(((*rad_fit_data[data_index]).xcf[i]+1)-1,2)+'   '+$
						'scan = '+strtrim((*rad_fit_data[data_index]).scan_mark[i],2)

			npnts = 0L
			for j=0,(*rad_fit_info[data_index]).ngates-1 do begin
				if((*rad_fit_data[data_index]).velocity[i,j] ne 10000.) then npnts = npnts + 1
			endfor


			if((*rad_fit_data[data_index]).channel[i] eq 0 OR (*rad_fit_data[data_index]).channel[i] eq 1) then chn = 'A' $
			else chn = 'B'

			print,'npnts = '+strtrim(npnts,2)+$
			'   nrang = '+strtrim((*rad_fit_info[data_index]).ngates,2)+$
			'   channel = '+chn+$
			'   cpid = '+strtrim((*rad_fit_data[data_index]).scan_id[i],2)
	
			printf,rec_unit,'npnts = '+strtrim(npnts,2)+$
			'   nrang = '+strtrim((*rad_fit_info[data_index]).ngates,2)+$
			'   channel = '+chn+$
			'   cpid = '+strtrim((*rad_fit_data[data_index]).scan_id[i],2)
			
			print,					'gate  pwr_lag0 / pwr_l     vel    gscat  vel_err  width_l   geo_lat   geo_lon'
			printf,rec_unit,'gate  pwr_lag0 / pwr_l     vel    gscat  vel_err  width_l   geo_lat   geo_lon'
			for j=(*rad_fit_info[data_index]).ngates-1,0,-1 do begin
				if((*rad_fit_data[data_index]).velocity[i,j] ne 10000.) then begin
					yrsec = TimeYMDHMSToYrsec(yr,mo,dy,hr,mt,sc)
					pos = rbpos(j, BEAM=(*rad_fit_data[data_index]).beam[i], CENTER=1, GEO=1, HEIGHT=300,$
											LAGFR=(*rad_fit_data[data_index]).lagfr[i], $
											SMSEP=(*rad_fit_data[data_index]).smsep[i], STATION=(*rad_fit_info[data_index]).id, YEAR=yr, YRSEC=yrsec)

					print,format='(1x,i3,3x,f6.0,2x,a1,f5.0,4x,1(f6.0,3x),2x,i1,4x,2(f6.0,3x),2(f7.2,3x))', $
								j,(*rad_fit_data[data_index]).lag0power[i,j],'/',$
								(*rad_fit_data[data_index]).power[i,j],$
								(*rad_fit_data[data_index]).velocity[i,j],$
								(*rad_fit_data[data_index]).gscatter[i,j],$
								(*rad_fit_data[data_index]).velocity_error[i,j],$
								(*rad_fit_data[data_index]).width[i,j],pos(0),pos(1)
					printf,rec_unit,format='(1x,i3,3x,f6.0,2x,a1,f5.0,4x,1(f6.0,3x),2x,i1,4x,2(f6.0,3x),2(f7.2,3x))', $
								j,(*rad_fit_data[data_index]).lag0power[i,j],'/',$
								(*rad_fit_data[data_index]).power[i,j],$
								(*rad_fit_data[data_index]).velocity[i,j],$
								(*rad_fit_data[data_index]).gscatter[i,j],$
								(*rad_fit_data[data_index]).velocity_error[i,j],$
								(*rad_fit_data[data_index]).width[i,j],pos(0),pos(1)
				endif
			endfor
			printf,rec_unit,' '
		endif
		print,' '
  endfor

  print,'Output is on '+outfile


  close,rec_unit
	free_lun,rec_unit

END