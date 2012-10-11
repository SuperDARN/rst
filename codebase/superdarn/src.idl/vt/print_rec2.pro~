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
;
; OPTIONAL INPUTS:
;
; KEYWORD PARAMETERS:
;
; EXAMPLE:
;
; OUTPUT:
; /rst/output_files/rec.dat
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

pro print_rec2


	;open the output file
	openw,rec_unit,'/rst/output_files/rec.dat',/get_lun

	filename = '20111007.bks.fitex'
	;prompt for filename
; 	print,'Directory /data/fit/ is assumed for the file!'
; 	read,filename,prompt='Enter filename (full name+extension)   '

	; Open the fit file
  inp=FitOpen('/data/fit/'+filename,/read)

	;prompt for date
;   read,yr,mo,dy,prompt='Enter date (yyyy mon day)   '
	yr = 2011
	mo = 10
	dy = 7

  ;prompt for start time
; 	read,shr,smt,ssc,prompt='Enter start time (hour min sec)   '
	shr = 5
	smt = 22
	ssc = 0

	;prompt for end time
; 	read,ehr,emt,esc,prompt='Enter end time (hour min sec)   '
	ehr = 5
	emt = 24
	esc = 0

	stime = JULDAY(mo, dy, yr, shr, smt, ssc)
	etime = JULDAY(mo, dy, yr, ehr, emt, esc)

;  Search for a specific time in the file
	while FitRead(inp,prm,fit) ne -1 do begin
		time = JULDAY(prm.time.mo, prm.time.dy, prm.time.yr, prm.time.hr, prm.time.mt, prm.time.sc)
		if(time gt etime) then break
		if(time ge stime) then begin
			print,strtrim(prm.time.yr,2)+'/'+strtrim(prm.time.mo,2)+'/'+strtrim(prm.time.dy,2)+'   '+$
						strtrim(prm.time.hr,2)+':'+strtrim(prm.time.mt,2)+':'+strtrim(fix(prm.time.sc),2)+'   '+$
						'/data/fit/'+filename

			printf,rec_unit,strtrim(prm.time.yr,2)+'/'+strtrim(prm.time.mo,2)+'/'+strtrim(prm.time.dy,2)+'   '+$
						strtrim(prm.time.hr,2)+':'+strtrim(prm.time.mt,2)+':'+strtrim(fix(prm.time.sc),2)+'   '+$
						'/data/fit/'+filename

			print,'bmnum = '+strtrim(prm.bmnum,2)+'   '+'intt = '+strtrim(round(prm.intt.sc),2)+'   '+$
						'tfreq = '+strtrim(prm.tfreq,2)+'   '+'noise_lev = '+strtrim(round(prm.noise.search),2)+'   '+$
						'xcf = '+strtrim(prm.xcf,2)+'   '+'scan = '+strtrim(prm.scan,2)

			printf,rec_unit,'bmnum = '+strtrim(prm.bmnum,2)+'   '+'intt = '+strtrim(round(prm.intt.sc),2)+'   '+$
						'tfreq = '+strtrim(prm.tfreq,2)+'   '+'noise_lev = '+strtrim(round(prm.noise.search),2)+'   '+$
						'xcf = '+strtrim(prm.xcf,2)+'   '+'scan = '+strtrim(prm.scan,2)

			npnts = 0
			for i=0,prm.nrang-1 do begin
				if(fit.qflg[i]) then npnts = npnts + 1
			endfor
			print,'npnts = '+strtrim(npnts,2)+'   nrang = '+strtrim(prm.nrang,2)
			printf,rec_unit,'npnts = '+strtrim(npnts,2)+'   nrang = '+strtrim(prm.nrang,2)

			print,'gate  pwr_lag0/ pwr_l     vel    gscat  vel_err  width_l width_l_err pwr_l_err'
			printf,rec_unit,'gate  pwr_lag0/ pwr_l     vel    gscat  vel_err  width_l width_l_err pwr_l_err'
			for i=prm.nrang-1,0,-1 do begin
				if(fit.qflg[i]) then begin
					print,format='(1x,i2,3x,f6.0,2x,a1,f5.0,4x,1(f6.0,3x),2x,i1,4x,4(f6.0,3x))', $
								i,fit.pwr0[i],'/',fit.p_l[i],fit.v[i],fit.gflg[i],fit.v_e[i], $
								fit.w_l[i],fit.w_l_e[i],fit.p_l_e[i]
					printf,rec_unit,format='(1x,i2,3x,f6.0,2x,a1,f5.0,4x,1(f6.0,3x),2x,i1,4x,4(f6.0,3x))', $
								i,fit.pwr0[i],'/',fit.p_l[i],fit.v[i],fit.gflg[i],fit.v_e[i], $
								fit.w_l[i],fit.w_l_e[i],fit.p_l_e[i]
				endif
			endfor
			printf,rec_unit,' '
		endif
		print,' '
  endwhile

  print,'Output is on rec.dat'





  close,rec_unit
	free_lun,rec_unit
  free_lun,inp

END