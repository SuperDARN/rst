;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
;NAME:
; acf_plot_power_panel
;
; PURPOSE:
; plots the actual and fitted power variation of an ACF
;
; CATEGORY:
; Graphics
;
; CALLING SEQUENCE:
;		acf_plot_power_panel,powers,fpowers,mplgs,lagnums,badlags,snr,wid,position
;
;
;	INPUTS:
;		powers: 		an array of size mplgs with the powers of the ACF
;		fpowers: 		an array of size mplgs with the fitted powers of the ACF
;		mplgs:			number of lags in the acf
;		lagnums:		an array of size mplgs containing the lag numbers of the ACF
;		badlags:		an array of size mplgs with flags indicating bad lags (2=good, 1=more_badlags,0=badlags)
;		snr:				the final fitted power of the ACF
;		wid:  			the final fitted spectral width of the ACF
;		position:		position to put the panel
;
; OPTIONAL INPUTS:
;
; KEYWORD PARAMETERS:
;
; EXAMPLE:
;
; OUTPUT:
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
; Written by AJ Ribeiro 02/06/2012
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
pro acf_plot_power_panel,powers,fpowers,mplgs,lagnums,badlags,snr,wid,position

	line = 3
	x1 = position(0)
	y1 = position(1)
	x2 = position(2)
	y2 = position(3)

	ptitle= 'Power: '+strtrim(round(snr),2)+' dB  Width: '+strtrim(round(wid),2)+' m/s'

	ymin = min([min(powers(where(badlags eq 0))),min(fpowers)])
	ymax = max([max(powers(where(badlags eq 0))),max(fpowers)])

	plot,findgen(1),findgen(1),/nodata,xrange=[0,max(lagnums)],yrange=[0,ymax*1.2],$
				xstyle=1,ystyle=1,xthick=4,ythick=4,pos=position,/noerase,thick=3.,$
				yticklen=-.01,title=ptitle,charthick=3,charsize=.6,xtitle='lag',ytitle='power'

	S = findgen(17)*(!PI*2./16.)
	usersym,cos(S),sin(S),/FILL

	xyouts,x2+.04,y2-.03,'ACF',/normal,charsize=.7
	xyouts,x2+.04,y2-.06,'Fit',/normal,charsize=.7
	loadct,34

	if(lagnums(mplgs-1) eq 0) then nlags = mplgs-1 $
	else nlags = mplgs
	
	for j=0,nlags-1 do begin
		;plot the actual powers
		if(badlags(j) eq 1) then p = 6 $
		else if(badlags(j) eq 11) then p = 4 $
		else if(badlags(j) eq 0) then p = 8 $
		else p = 5
		if(powers(j) gt ymax*1.2) then ycoord = ymax*1.2 $
		else ycoord = powers(j)
		plots,lagnums(j),ycoord,psym=p,col=250,symsize=.75,thick=3.

	endfor

	plots,lagnums(0:nlags-1),fpowers(0:nlags-1),linestyle=line,col=150,thick=5
	usersym,cos(S),sin(S),/FILL
	plots,x2+.02,y2-.025,psym=8,col=250,/normal
	usersym,cos(S),sin(S)
	plots,[x2+.01,x2+.03],[y2-.055,y2-.055],/normal,linestyle=line,col=150,thick=5

	;go back to davit ct
	init_colors

end