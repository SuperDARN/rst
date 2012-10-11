;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
;NAME:
; acf_plot_phase_panel
;
; PURPOSE:
; plots the actual and fitted phase variation of an ACF 
;
; CATEGORY:
; Graphics
;
; CALLING SEQUENCE:
;		acf_plot_phase_panel,phases,fphases,mplgs,lagnums,badlags,vel,omega_loc,err,position
;
;
;	INPUTS:
;		phases: 		an array of size mplgs with the phases of the ACF
;		fphases: 		an array of size mplgs with the fitted phases of the ACF
;		mplgs:			number of lags in the acf
;		lagnums:		an array of size mplgs containing the lag numbers of the ACF
;		badlags:		an array of size mplgs with flags indicating bad lags (2=good, 1=more_badlags, 0=badlags)
;		vel:				the final fitted velocity of the ACF
;		omega_loc:	the omega_loc value from FITACF (set to -1 for non-FITACF)
;		err:  			fitted velocity error
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

pro acf_plot_phase_panel,phases,fphases,mplgs,lagnums,badlags,vel,omega_loc,err,position

	line = 3
	x1 = position(0)
	y1 = position(1)
	x2 = position(2)
	y2 = position(3)

	if(omega_loc ne -1) then $
		ptitle= 'Vel: '+strtrim(round(vel),2)+' m/s  V_err: '+strtrim(round(err),2)+$
						'  O_loc: '+strtrim(round(omega_loc),2) $
	else $
		ptitle= 'Vel: '+strtrim(round(vel),2)+' m/s  V_err: '+strtrim(round(err),2)


	plot,findgen(1),findgen(1),/nodata,xrange=[0,max(lagnums)],yrange=[-!pi,!pi],$
				xstyle=1,ystyle=1,xthick=4,ythick=4,pos=position,/noerase,thick=3.,$
				yticklen=-.01,title=ptitle,charthick=3,charsize=.9,xtitle='lag',ytitle='phase'

	S = findgen(17)*(!PI*2./16.)
	usersym,cos(S),sin(S),/FILL

	xyouts,x2+.04,y2-.03,'ACF',/normal,charsize=1.125
	xyouts,x2+.04,y2-.06,'Fit',/normal,charsize=1.125
	loadct,34
	if(lagnums(mplgs-1) eq 0) then nlags = mplgs-1 $
	else nlags = mplgs
	
	for j=0,nlags-1 do begin
		;plot the actual ACF
; 		if(badlags(j) eq 0) then begin
; 			usersym,cos(S),sin(S),/FILL
		if(badlags(j) eq 1) then p = 6 $
		else if(badlags(j) eq 11) then p = 4 $
		else if(badlags(j) eq 0) then p = 8 $
		else p = 5
		plots,lagnums(j),phases(j),psym=p,col=0,symsize=1.13,thick=3.
; 			usersym,cos(S),sin(S)
; 		endif
; 		plots,lagnums(j),fphases(j),psym=8,col=150
	endfor

	plots,lagnums(0:nlags-1),fphases(0:nlags-1),linestyle=line,col=150,thick=5.
	usersym,cos(S),sin(S),/FILL
	plots,x2+.02,y2-.025,psym=8,col=0,/normal
	usersym,cos(S),sin(S)
	plots,[x2+.01,x2+.03],[y2-.055,y2-.055],/normal,linestyle=line,col=150,thick=5.

	;go back to davit ct
	init_colors
	
end