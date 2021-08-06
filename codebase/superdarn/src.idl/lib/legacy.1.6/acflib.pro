; acflib.pro
; ==========
; Author: K.Baker & R.J.Barnes
; 
; Copyright (c) 2012 The Johns Hopkins University/Applied Physics Laboratory
;
; This file is part of the Radar Software Toolkit (RST).
; 
; RST is free software: you can redistribute it and/or modify
; it under the terms of the GNU General Public License as published by
; the Free Software Foundation, either version 3 of the License, or
; (at your option) any later version.
; 
; This program is distributed in the hope that it will be useful,
; but WITHOUT ANY WARRANTY; without even the implied warranty of
; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
; GNU General Public License for more details.
; 
; You should have received a copy of the GNU General Public License
; along with this program.  If not, see <https://www.gnu.org/licenses/>.
; 
; Modifications:
;
; Public Functions
; ----------------
; badlags
; interp_bad
; fix_acf
; spec_title
; vvect
; pwrspec
; acf_pwrspec
; plot_spec
; pwr_spec_corr
; plot_spec_corr
; spec_width
; ident_badlags
; more_badlags
; plt_acf
; plt_acf2
; plt_acf_pwr
; plt_phase
; plt_pwr
; plt_both
;
; ---------------------------------------------------------------
; 


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;	badlags
;
; PURPOSE:
;	Identify badlags and allow user to manually identify addlitional
;	badlags, or alternatively accept apparently bad lags as OK.
;
;----------------------------------------------------------------------------
;



function badlags, range

  common rawdata_com, rawfileptr, raw_data, badlag, more_badrange, $
                      lags, rd_byte

  if n_params() NE 1 then begin
    print,"No Range specified"
    return,badlag
  endif
     
  if (lags(1) LE 0) then $
    lags = reform(raw_data.lag_table(1,*) $
		- raw_data.lag_table(0,*))
    more_badrange = -1
    ident_badlags, range
    return, badlag
end

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;	interp_bad
;
; PURPOSE:
;	This function fixes bad data values by interpolating from good
;	data values.  
;	
; Calling Sequence:
;	vout = interp_bad(vin, bad)
;
;	Where vin and bad must be vectors with the same dimensions
;	bad is a vector with 1's wherever the data in vin are bad values
;
;----------------------------------------------------------------------------
;



function interp_bad,vin,bad

  if (n_elements(vin) NE n_elements(bad)) then begin
    print,"arguments do not have same length"
    return,vin
  endif
  if (bad(0) GE 1) then begin
    print,"Can't interpolate the first point"
    return,vin
  endif

;
; OK to start the loop
;
	
  vout = vin
  for i=1,n_elements(vout)-2 do begin
    if (bad(i) GE 1) then begin
      j=i+1
      while (bad(j) GE 1) AND (j LT n_elements(vin)-1) do j=j+1
        if (j ge n_elements(vin)) then vout(i)=0 $
        else Begin
          vout(i) = (vout(i-1)*(j-i) + vout(j))/(j-i+1)
          bad(i) = 0
	endelse
      endif
    endfor
  return,vout
end

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; NAME:
;	fix_acf
;
; PURPOSE:
;
;	this function fixes an acf by interpolating thru bad or
;	missing lags
;
; Calling Sequence:
;	acf = fix_acf(range, [rawdat])
;
;	The original ACF is assummed to be in the raw data structure
;	specified by the argument 'rawdat' or if rawdat is not
;	specified, then the orignal ACF is in raw_data.
;
;----------------------------------------------------------------------------
;


function fix_acf,range,interp = method

  common rawdata_com, rawfileptr, raw_data, badlag, more_badrange, $
                      lags, rd_byte

  acf =complex(raw_data.acfd(0,0:raw_data.p.mplgs-1,range), $
	raw_data.acfd(1,0:raw_data.p.mplgs-1,range))
  acf = reform(acf)
  bad = badlag(0:raw_data.p.mplgs-1)
  acf = interp_bad(acf, bad)
  raw_data.acfd(0,0:raw_data.p.mplgs-1,range) = long(float(acf))
  raw_data.acfd(1,0:raw_data.p.mplgs-1,range) = long(imaginary(acf))
  return,acf
end

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; NAME:
;	spectitle
;
;	procedure spectitle sets the axis titles for the spectrum plots
;
;	calling sequence:
;	  spectitle,range,bm
;
;	This procedure defines the the system variable !p.title to be
;	  of the form:
;	  "PWR SPEC: rng: 24  bm: 5  time: 92/08/16  12:22:17"
;
;
;----------------------------------------------------------------------------
;


pro spectitle,range,bm
  common rawdata_com, rawfileptr, raw_data, badlag, more_badrange, $
                      lags, rd_byte

  ptit1 = "PWR SPEC:  "
  ptit2 = string(format='("rng:",i3)',range)
  if (n_params() eq 2) then b=bm else b=raw_data.p.bmnum
  ptit3 = string(format='("  bm:",i3)',b)
  ptit4 = string(format='("  time: ",i4,"/",i2,"/",i2)',$
	raw_data.p.year,raw_data.p.month,raw_data.p.day)
  ptit5 = string(format='("  ",i2.2,i2.2,":",i2.2)',$
	raw_data.p.hour,raw_data.p.minut,raw_data.p.sec)
  !p.title = ptit1+ptit2+ptit3+ptit4+ptit5
  !x.title = 'Doppler Velocity'
  !y.title = "Power"
  return
end

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; NAME:
;	vvect
;
;	sets up the vector defining the x-axis of a Doppler power spectrum.
;
;	Calling sequence:  
;	  v = vvect(spec,f,tau)
;
;	  spec is the output from either acf_pwrspec or pwrspec
;	  f is the frequency (raw_data.p.tfreq)
;	  tau is the lag length (raw_data.p.mpinc)
;
;	  The output is a vector running from -Vmax to + Vmax with the same
;	    length as the spectrum vector
;
;----------------------------------------------------------------------------
;


function vvect,spec,f,tau
  common rawdata_com, rawfileptr, raw_data, badlag, more_badrange, lags, rd_byte
  if (n_params() ge 3) then begin
    freq = f
    t = tau
    end else begin
    freq = raw_data.p.tfreq
    t = raw_data.p.mpinc
  end

  p=size(spec)
  v = (findgen(p(1)) - p(1)/2)*2.0/p(1)
  vmax = 3.0e8/(4.0*freq*t*1.0e-3)
  v=vmax*v
  return,v
end

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; NAME:
;	pwrspec 
;
;	The function pwrspec takes a complex ACF and returns the power
;	spectrum generated from that ACF.  All the lags must be good
;	in order for this to work properly.  
;
;	NOTE:  this routine is not normally called by the user.  The user
;	  should use the routine "acf_pwrspec", which fills in missing and
;	  bad lags and then calls this routine to produce the spectrum
;
;----------------------------------------------------------------------------
;


function pwrspec,acf
  s = size(acf)
  if (s(0) NE 1) then begin
    print,'The argument is not a one dimensional array'
    return,0
    endif
  if (s(2) NE 6) then begin
    print,'The argument is not a COMPLEX array'
    return,0
    endif
  n = s(1)
;
;  make sure the working array is about 4 times as long as the original
;  array and padded with 0's

  m = fix(alog(2*n)/alog(2))+1
  m = 2^m
  temp = complexarr(m)
  mid = m/2
  temp(indgen(n)) = acf(indgen(n))

;
;	now window the final 4 points down to 0
;	using a cosine window
;

  temp(indgen(4)+n-4)=temp(indgen(4)+n-4)*cos(indgen(4)*!pi/6)

;
;	now shift up so that the t=0 is in the middle
;

  temp = shift(temp,mid)
;
;	now reflect the ACF about the t=0 axis
;	This makes sure the spectrum is real and positive
;
  temp(mid-indgen(n)-1) = conj(temp(mid+indgen(n)+1))
;
;	now shift the array so that t=0 is at element 0
;
  temp=shift(temp,-mid)
;
;	OK, now do the fft to get the power spectrum
;
  spec = fft(temp,-1)
;
;	shift the result so that negative frequencies lie on the left
;	and 0 frequency is in the middle
;
  return,abs(shift(spec,mid))
end

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; NAME:
;	acf_pwrspec
;
;	create the Doppler power spectra for a set of ranges.
;	  It uses the function pwrspec.
;
;	calling sequence:
;	  spec_array = acf_pwrspec(first_range, last_range, [acf_array])
;
;	  the ACF data must be in the variable "raw_data" in the common block
;	     "rawdata_com"
;	  the spectra for the ranges defined by the first and second arguments
;	     will be computed and returned in the variable spec_array.
;	  the optional argument "acf_arry"  can be used to receive the 
;	     complete, filled out and corrected complex autocorrelation
;	     functions for the specified ranges.
;
;----------------------------------------------------------------------------
;


function acf_pwrspec,range1,range2,acf_array
  common rawdata_com, rawfileptr, raw_data, badlag, more_badrange, $
                      lags, rd_byte

  if n_params() LT 1 then begin
    print,"Use: spec=acf_pwrspec(first_range,[last_range])"
    return,0
  end
  r1 = range1
  if n_params() GE 2 then r2 = range2 else r2=r1
  nlag = raw_data.p.mplgs - 1
  mlag = raw_data.lag_table(1,nlag) - raw_data.lag_table(0,nlag) + 1

;  now create a complex array to hold the ACFs

  acf=complexarr(mlag,r2-r1+1)

;
;  The vector 'lags' gives the lag numbers for which there is data
;

  if (lags(1) LE 0) then $
  lags=reform(raw_data.lag_table(1,*) - raw_data.lag_table(0,*))

;
;  now put the original integer acf data into the complex array
;

  acf(lags(0:nlag),*) = complex(raw_data.acfd(0,0:nlag,r1:r2),$
  raw_data.acfd(1,0:nlag,r1:r2))

;
;  now identify the bad lags for each of these range gates
;

  for i=0,r2-r1 do begin

;
;  create an array that indicates which lags are missing
;
    missing = replicate(1,mlag)
    missing(lags(0:nlag))=0

;  then identify badlags 

    ident_badlags,r1+i
    bads = where(badlag gt 0)
    sb = size(bads)
    if (sb(0) EQ 1) then begin
      badl = raw_data.lag_table(1,bads) - raw_data.lag_table(0,bads)
      missing(badl) = 1
    endif

;
;  finally, interpolate all the missing and bad data points
;  

    acf(*,i) = interp_bad(acf(*,i),missing)
    if (n_params() GE 3) then acf_array(*,i) = acf(*,i)
  end

;
;we are now ready to do the spectra
;
  for i=0,r2-r1 do begin
    spec1 = pwrspec(acf(*,i))
    if (i eq 0) then spec = fltarr(n_elements(spec1),r2-r1+1)
    spec(*,i)=spec1
  end
  return,spec
end

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; NAME:
;	plot_spec
;
;	plot a sequence of spectra, 4 to a page
;
;       calling sequence:  
;	  plot_spec,v,spec,r1,[bm]
;
;	arguments
;	  v = x axis velocity values
;	  spec = 2-d array of spectra from acf_pwrspec
;	  r1 = range number of the first spectrum
;	  optionally you can also include the beam number
;	    if the beam number is not included it is taken from the
;	    raw_data structure
;
;----------------------------------------------------------------------------
;



pro plot_spec,v,spec,r1,bm

  common ben,fname,yy,mo,dd,hr,mn,ss,bnum,rbin,flag

  if (n_params() lt 3) then begin
    print,'Use:  plot_spec,v_vector,spec_array,first_range'
    return
  end
  p = size(spec)
  if (p(0) EQ 1) then n = 1 else n=p(2)
  ptitle = !p.title
  xtitle = !x.title
  ytitle = !y.title
  linetype = !linetype
  !linetype = 0
  !p.multi = [0, 0, n, 0, 0]
  if !d.name EQ "PS" then begin
    device,/portrait
    device,/inches,yoffset=1.0,ysize=9.0
    !p.multi = [0, 0, n, 0, 0]
  endif
  for i = 0, n-1 do begin
    if (n_params() eq 4) then spectitle,r1+i,bm else spectitle,r1+i
    plot,v,spec(*,i), xrange=[-2000,+2000]
  endfor
  XYOUTS, 0.5,1.03, '!5SuperDARN Power Spectra ' + fname, $
  alignment = 0.5, /normal, size = 1.5

;m=3
;k = 0
;while (k le n-1) do begin
;  if (k+m gt n-1) then m = n-k-1
;  if !d.name EQ "PS" then mplt = 6 else mplt = 4
;  print,"changing p.multi to: ",!p.multi
;  !p.multi = [0, 0, mplt, 0, 0]
;  for i=k,k+m do begin
;    if (n_params() eq 4) then spectitle,r1+i,bm else spectitle,r1+i
;    plot,v,spec(*,i)
;  end
;k = k+m +1
;end
!p.multi = 0
!p.title = ptitle
!x.title = xtitle
!y.title = ytitle
!linetype = linetype

  return
end

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; NAME:
;	pwr_spec_corr,spec
;
;	This function calculates the auto and cross correlations between
;	a set of Doppler power spectra.  The Doppler power spectra must
;	be a 2-d array with the first dimension being the index for
;	the spectrum (for example a range index) and the second index giving
;	the velocity component of each spectrum.  [This is the format that
;	is produced by the function acf_pwrspec.]
;
;	The returned value is an n x n x k array where n is the number of
;	individual power spectra, and k is the size of each spectrum
;	The vector corr(i,j,*) gives the correlation of spectrum i with
;	spectrum j.  The middle value of the vector is the correlation with
;	0 shift.  Values to the right of the center value have the jth spectrum
;	shifted right k/2 times while the values left of center correspond
;	to correlations where the jth spectrum is shifted left.
;
;	Thus, the corr(i,i,*) is the autocorrelation of spectrum i and will
;	have the center element = 1.0 (perfect correlation).
;
;----------------------------------------------------------------------------
;



function pwr_spec_corr,spec
  s = size(spec)
  ss=s(1)
  nspec = s(2)
  corr = fltarr(nspec,nspec,ss)
  for i=0, nspec-1 do for j=0,nspec-1 do for k=0,ss-1 do $
  corr(i,j,k) = correlate(spec(*,i),shift(spec(*,j),k))
  for i=0,nspec-1 do for j=0, nspec-1 do $
  corr(i,j,*)=shift(corr(i,j,*),ss/2)
  return, corr
end

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; NAME:
;	plot_spec_corr,corr,r1
;
;	This procedure plots the auto & cross correlations computed by the
;	function pwr_spec_corr
;
;----------------------------------------------------------------------------
;



pro plot_spec_corr, corr, r1
  ptitle = !p.title
  xtitle = !x.title
  ytitle = !y.title
  linetype = !linetype
  if !d.name EQ "PS" then begin
    device,/portrait
    device,/inches,yoffset=1.0,ysize=9.0
  endif
  s = size(corr)
  n1 = s(1)
  n2 = s(2)
  m = s(3)
  x = indgen(m) - m/2
  if (n1 GT 3) then !p.multi = [0, 0, 3, 0, 0] $
  else !p.multi = [0, 0, n1, 0, 0]
  if (n_params() EQ 2) then r0 = r1 else r0 = 0
  !x.title = "element shift"
  !y.title = "corr"
  for i=0,n1-1 do begin
    r = r0 + i
    !p.title = "Correlation for range (or beam) " + string(r)
    !linetype = 0
    plot,x,corr(i,i,*)
    for j=0, i-2 do begin
      !linetype = j+1
      while !linetype GT 5 do !linetype = !linetype - 4
      oplot,x,corr(i,j,*)
    endfor
    for j=i+1, n2-1 do begin
      !linetype = j+1
      while !linetype GT 5 do !linetype = !linetype - 4
      oplot,x,corr(i,j,*)
    endfor
  endfor
  !p.title = ptitle
  !x.title = xtitle
  !y.title = ytitle
  !linetype = linetype
  !p.multi = [0, 0, 0, 0, 0]
  return
end


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; NAME:
;	spec_width(spec,v)
;	
;	This function locates the 1/2 power point of a spectrum and determines
;	the total width of the spectrum between the 1/2 power points.  Note
;	that if the spectrum has multiple components, the power may drop to
;	values below the 1/2 power point in between the limits determined
;	by this function.
;
;	inputs:  spec is a 1 or 2 dim array containing either a single
;		spectrum (1-d array) or multiple spectra (2-d array)
;		v is a vector of the same length as the spectrum, giving
;		the velocity associated with each spectral bin (see function
;		vvect).
;	
;	output: a single value (for a single spectrum) or a vector containing
;		the spectral widths.
;
;----------------------------------------------------------------------------
;


function spec_width,spec,v

  if (n_params() ne 2) then begin
    print,"spec_width requires 2 parameters: spectra and vel vector"
    return,0
  endif
  s = size(spec)
  if s(0) eq 0 then begin
    print,"The first argument to spec_width must be a single spectrum"
    print,"or an array of spectra."
  return,0
  endif $
  else if s(0) eq 1 then wd = 0.0 $
  else if s(0) eq 2 then begin
    n=s(2)
    wd = fltarr(n)
  endif $
  else begin
    print,"spec width can only have 2 dimensions"
    return,0
  endelse
  if s(0) eq 1 then begin
    w = where(spec gt max(spec)/2.0)
    wd = max(v(w)) - min(v(w))
    return,wd
  endif $
  else begin
    for i=0,n-1 do begin
      w = where(spec(*,i) gt max(spec(*,i))/2.0)
      wd(i) = max(v(w)) - min(v(w))
      endfor
    return,wd
  endelse
end

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; NAME:
;	IDENT_BADLAGS
;
;	This procedure locates all the bad lags in an ACF and creates the
;	array containing a 1 for each bad lag and places the contents
;	of this array into the common block variable badlag.
;
;----------------------------------------------------------------------------
;


pro ident_badlags,range
  common rawdata_com, rawfileptr, raw_data, badlag, more_badrange, $
                      lags, rd_byte


  FitACFBadlags,raw_data.p.mppul,raw_data.p.lagfr,raw_data.p.smsep, $
                raw_data.p.mpinc,raw_data.p.txpl,raw_data.pulse_pattern, $
                nbad,badsmp
   
  FitACFRngOverLap,raw_data.p.mppul,raw_data.p.mpinc,raw_data.p.smsep, $
                   raw_data.pulse_pattern,roverlap

  FitACFCkRng,range,raw_data.p.mplgs,raw_data.p.mpinc, $
              raw_data.p.smsep,transpose(raw_data.lag_table), $
              nbad,badsmp,badlag

  FitACFLagOverLap,range,raw_data.p.nave,raw_data.p.nrang, $
                   raw_data.p.mppul,raw_data.p.mplgs,roverlap, $
                   raw_data.pwr0,transpose(raw_data.lag_table), $
                   raw_data.pulse_pattern,badlag

;
;we have now identified the bad lags that are contaminated by
;power from other ranges.  Next we have to eliminate lags that
;are in the noise or have excess power, etc.
;
  acf = complex(raw_data.acfd(0,*,range),raw_data.acfd(1,*,range))
  w=abs(acf)
  badflag1=0
  badflag2=0
  fluct0=w(0)/sqrt(2.0*raw_data.p.nave)
  if raw_data.p.noise LT 0 then raw_data.p.noise = 20000
  noise_lev = raw_data.p.noise/sqrt(2.0*raw_data.p.nave)
  fluct = w(0) + 2.0*noise_lev
  fluct_old=fluct
  k_old=0

  for k=1, raw_data.p.mplgs-1 do begin
    if (badflag2) then badlag(k)=1
    if (w(k) LT noise_lev) then begin
      badlag(k)=1
      badflag2=badflag1
      badflag1=1
    end
    if (w(k) GT fluct) then begin
      badlag(k) = 1
      if (k LT raw_data.p.mplgs-1) then begin
        if ((w(k) LT fluct_old) AND (w(k+1) GT fluct) AND (w(k+1) LT $
           (w(k)+noise_lev))) then begin 
          badlag(k_old) = 1
          badlag(k)=0
        end
      end
    end
  if (badlag(k) EQ 0) then begin
    fluct_old = fluct
    fluct1=w(k)+fluct0
    fluct2 = (w(k) +w(k_old))/2.0
    if (fluct1 lt fluct2) then fluct1=fluct2
    fluct = 2.0*noise_lev + fluct1
    k_old = k
    badflag2=0
    badflag1=0
    end
  end
return
end

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; NAME:
;	MORE_BADLAGS
;
;	procedure more_badlags allows you to identify additional lags as
;	bad by clicking the left mouse button on a point.
;	Conversly, you can identify a point as good by clicking the 
;	right mouse button.
;
;----------------------------------------------------------------------------
;


pro more_badlags,range
  common rawdata_com, rawfileptr, raw_data, badlag, more_badrange, $
         lags, rd_byte

  if n_params() NE 1 then begin
     print,"No Range specified"
     return
  endif
  plt_both,range
  done=1
  while done do begin
    cursor,cx,cy,/down
    k=fix(cx+.1)
    j = -1
    for j1=0,n_elements(badlag)-1 do if (lags(j1) EQ k) then j=j1 
    if (j ge n_elements(badlag)) then done = 0
    if (j lt 0) then done = 0
    if (done) then if (badlag(j) AND !ERR) then done = 0
    if (!ERR EQ 2) then done = 0
    if (!d.name EQ 'X') then bb = (!ERR AND 1)
    if (!d.name EQ 'TEK') AND (!ERR GE 1) then bb = 1
    if (j ge 0) AND (j lt n_elements(badlag)) then badlag(j) = bb
  endwhile
  more_badrange = range
  plt_both,range
end


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; NAME:
;	PLT_ACF
;
; PURPOSE:
;	PLOT a radar autocorrelation function
;
; Calling Sequence:
;	plt_acf, range, [rawdata]
;
;----------------------------------------------------------------------------
;


pro plt_acf, range, rd
  common rawdata_com, rawfileptr, raw_data, badlag, more_badrange, $ 
         lags, rd_byte

;
;if there is only 1 argument, assume the raw data is in the common
;block.  If there are two arguments, put the data into the common
;block.
;
  if n_params() EQ 2 then raw_data = rd

  mplgs_min_1 = raw_data.p.mplgs - 1


  char_real_good = '*'
  char_real_bad = '-'
  char_imag_good = '+'
  char_imag_bad = '='
  lin1 = 0
  lin2 = 1                           
  LAG_TAB_LEN = 48
  lag_x = reform(raw_data.lag_table(1,0:mplgs_min_1) - $
                 raw_data.lag_table(0,0:mplgs_min_1))
  y_max = max(raw_data.acfd(0:1,0:mplgs_min_1,range))
  y_min = min(raw_data.acfd(0:1,0:mplgs_min_1,range))
;
;   now start plotting
;
  dfmt = '(i4.4,"/",i2.2,"/",i2.2," ",i2.2,":",i2.2,":",i2.2)'
  datestring=string(format=dfmt,raw_data.p.year,$
  raw_data.p.month,raw_data.p.day,raw_data.p.hour,$
  raw_data.p.minut,raw_data.p.sec)
  !p.title='Range ' + string(format='(i3)',range) + ' ACF  ' +$
  datestring
  !x.title = 'Lag Number'
  !y.title = ''
  if more_badrange NE range then begin
    badlag=badlags(range)
  endif  
  !psym = 0
  plot,/nodata, $
  lag_x, raw_data.acfd(0,0:mplgs_min_1,range),yrange=[y_min,y_max]
  oplot, lag_x, raw_data.acfd(0,0:mplgs_min_1,range), $
                 line = lin1

  for i=0,mplgs_min_1 do begin

    if ( badlag(i) eq 1) then begin 
                 char = char_real_bad
    endif else begin
      char = char_real_good
    endelse  
          
    xyouts, lag_x(i), raw_data.acfd(0,i,range),char

  endfor

  !psym = 0
  oplot, lag_x, raw_data.acfd(1,0:mplgs_min_1,range), $
                  line = lin2

  for i=0,mplgs_min_1 do begin

    if ( badlag(i) eq 1) then begin
      char = char_imag_bad
    endif $
      else begin
      char = char_imag_good
    endelse  
          
    xyouts, lag_x(i), raw_data.acfd(1,i,range),char

  endfor
  !p.title =''
  !x.title=''
  return
end


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; NAME:
;	PLT_ACF2
;
; PURPOSE:
;	PLOT a radar autocorrelation function
;
; Calling Sequence:
;	plt_acf, range, [rawdata]
;
;----------------------------------------------------------------------------
;


pro plt_acf2, range, rd
  common rawdata_com, rawfileptr, raw_data, badlag, $

  more_badrange, lags, rd_byte

;
;if there is only 1 argument, assume the raw data is in the common
;block.  If there are two arguments, put the data into the common
;block.
;
  if n_params() EQ 2 then raw_data = rd
;
  mplgs_min_1 = raw_data.p.mplgs - 1

  LAG_TAB_LEN = 48
  lag_x = reform(raw_data.lag_table(1,0:mplgs_min_1) - $
                 raw_data.lag_table(0,0:mplgs_min_1))
  y_max = max(raw_data.acfd(0:1,0:mplgs_min_1,range))
  y_min = min(raw_data.acfd(0:1,0:mplgs_min_1,range))
;
;   now start plotting
;
  dfmt = '(i2.2,"/",i2.2,"/",i2.2," ",i2.2,":",i2.2,":",i2.2)'
  datestring=string(format=dfmt,raw_data.p.year-1900,$
  raw_data.p.month,raw_data.p.day,raw_data.p.hour,$
  raw_data.p.minut,raw_data.p.sec)
  ra_km = range * raw_data.p.rsep + raw_data.p.frang
  !p.title='Range ' + $
  string(format='(i3," (",i4," km), ")',range, ra_km) + $
         ' ACF:  ' + datestring
  !x.title = 'Lag (ms)'
  !y.title = ''

  badlag=badlags(range)

  coef = raw_data.p.mpinc * 1e-03 ; lag in msec
  circle=FINDGEN(16)*(!PI*2/16.)
  all = indgen(mplgs_min_1+1)
  goods = Where(badlag(all) eq 0, cg)
  plot, lag_x(all)*coef, raw_data.acfd(0,all,range),$
              yrange=[y_min,y_max],$
  line = 0, psym = -8, xstyle=1,/nodata
  USERSYM,COS(circle),SIN(circle), /FILL
  If cg gt 1 then begin
    oplot, lag_x(goods)*coef, raw_data.acfd(0,goods,range), $
           linestyle = 0, psym = -8, symsize = 0.4
    oplot, lag_x(goods)*coef, raw_data.acfd(1,goods,range), $
           linestyle = 1, psym = -8, symsize = 0.4
  endIf

  oplot, [0,20]*coef, [0.,0.], linestyle = 0

  !p.title =''
  !x.title=''
  return
end

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; NAME:
;	PLT_ACF_PWR
;
; PURPOSE:
;	Plot both the acf and the power at each lag
;
; Calling Sequence:
;	plt_acf_pwr, range, [rawdata], [/LOGPLOT]
;
;
;----------------------------------------------------------------------------
;


pro plt_acf_pwr, range, rd, logplot = logplot
  common rawdata_com, rawfileptr, raw_data, badlag, more_badrange, $
         lags, rd_byte
;
;if there is only 1 argument, assume the raw data is in the common
;block.  If there are two arguments, put the data into the common
;block.
;
  if n_params() EQ 2 then raw_data = rd
;
;check the logplot keyword
  if (NOT keyword_set(logplot)) then logplot=0
;
;   define constants
;
  mplgs_min_1 = raw_data.p.mplgs - 1
  lag_x = reform(raw_data.lag_table(1,0:mplgs_min_1) - $
                   raw_data.lag_table(0,0:mplgs_min_1))
  LAG_TAB_LEN = 48
;
;if the device uses color then set the proper color values
;
   
  if (!D.N_COLORS GE 255) then begin
    col_pwr_good = 85
    col_pwr_bad =  200
    col_real = 250
    col_imag = 50
  endif    $
    else if (!D.N_COLORS GE 201) then begin
    col_pwr_good = 85
    col_pwr_bad = 200
    col_real = !D.N_COLORS - 5
    col_imag = 50
  end $
  else if (!D.N_COLORS GE 8) then begin
    col_pwr_good = 3
    col_pwr_bad = 2
    col_real = 4
    col_imag = 5
  end $
  else begin
    col_pwr_good = !p.color
    col_pwr_bad = !p.color
    col_real = !p.color
    col_imag = !p.color
  endelse
  if (!D.NAME EQ "PS") then begin
    col_pwr_good = !p.color
    col_pwr_bad = !p.color
    col_real = !p.color 
    col_imag = !p.color
  endif
;
  char_pwr_good = "*"
  char_pwr_bad = '-'
  lin1 = 1
  lin2 = 1
;
;   calculate the power
;

  acf_real = float(raw_data.acfd(0,0:mplgs_min_1,range))
             acf_real_2 = acf_real^2
  acf_imag = float(raw_data.acfd(1,0:mplgs_min_1,range))
             acf_imag_2 = acf_imag^2
  acf_pwr = sqrt(acf_real_2 + acf_imag_2)           

;   max_pwr = max(acf_pwr)
;   y_min = min(raw_data.acfd(0:1,0:mplgs_min_1,range))
;   y_max = max_pwr

  y_max = raw_data.acfd(0,0,range)
  y_min = -y_max

  if more_badrange NE range then $
    badlag = badlags(range+1)
;
;  now plot the acf
;
;  !psym = 6

  !psym = 0
  dfmt = '(i2.2,"/",i2.2,"/",i2.2," ",i2.2,":",i2.2,":",i2.2)'
  datestring=string(format=dfmt,raw_data.p.year-1900,$
       raw_data.p.month,raw_data.p.day,raw_data.p.hour,$
       raw_data.p.minut,raw_data.p.sec)
  !p.title = 'ACF and Power, range ' + string(format='(i3)',range) +$
    '  ' + datestring
  !x.title = 'Lag Number'
  plot, lag_x, raw_data.acfd(0,0:mplgs_min_1,range), $
                 yrange=[y_min,y_max], color=!p.color,/nodata,ytype=logplot
  oplot, lag_x, raw_data.acfd(0,0:mplgs_min_1,range), $
                  color=col_real,line = lin1
;!psym = 4

  !psym = 0
  oplot, lag_x, raw_data.acfd(1,0:mplgs_min_1,range), $
                  color = col_imag,line = lin2

;
;   plot the power
;
  for i=0,mplgs_min_1 do begin

    if ( badlag(i) eq 1) then begin
       col_pwr = col_pwr_bad 
       char = char_pwr_bad
    endif   $
    else begin 
      col_pwr = col_pwr_good
      char = char_pwr_good
    endelse  
          
    xyouts, lag_x(i), acf_pwr(i),char, color=col_pwr

  endfor
  !p.title = ''
  !x.title = ''
  return
end   

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; NAME:
;	PLT_PHASE
;
; PURPOSE:
;	Plot the phase of ACF as function of lag
;
; Calling Sequence:
;	plt_phase, range
;
;----------------------------------------------------------------------------
;


pro plt_phase,range,rd

  common rawdata_com, rawfileptr, raw_data, badlag, more_badrange, $
         lags, rd_byte

;
;if there is only 1 argument, assume the raw data is in the common
;block.  If there are two arguments, put the data into the common
;block.
;
  if n_params() EQ 2 then raw_data = rd

;
;   define constants 
;
   mplgs_min_1 = raw_data.p.mplgs - 1
;
;if the device uses color then set the proper color values
;
  if (!D.N_COLORS GE 255) then begin
    col_phase_good = 250
    col_phase_bad =  175
  endif    $
  else if (!D.N_COLORS GE 176) then begin
    col_phase_good = !D.N_COLORS - 5
    col_phase_bad = 175
   endif $
   else if (!D.N_COLORS GE 8) then begin
    col_phase_good = 3
    col_phase_bad = 2
  endif $
  else begin
    col_phase_good = !p.color
    col_phase_bad = !p.color
  endelse
  if (!D.NAME EQ "PS") then begin
    col_phase_good = !p.color
    col_phase_bad = !p.color
  endif

;
  char_phase_good = '*'
  char_phase_bad = '-'
  lin1 = 1
  lag_x = reform(raw_data.lag_table(1,0:mplgs_min_1) - $
                   raw_data.lag_table(0,0:mplgs_min_1))
  phase = atan(raw_data.acfd(1,0:mplgs_min_1,range), $
                        raw_data.acfd(0,0:mplgs_min_1,range))
  LAG_TAB_LEN = 48
  pi = !pi

  if more_badrange NE range then $
    badlag = badlags(range+1)
;
;   now start plotting
;
  dfmt = '(i2.2,"/",i2.2,"/",i2.2," ",i2.2,":",i2.2,":",i2.2)'
  datestring=string(format=dfmt,raw_data.p.year-1900,$
        raw_data.p.month,raw_data.p.day,raw_data.p.hour,$
        raw_data.p.minut,raw_data.p.sec)
  !p.title = 'Range: ' + string(format='(i3)',range) +$
     ' Residual Phase  ' + datestring
  !x.title = 'Lag Number'
  !psym = 0
  plot, lag_x, phase, color = !p.color, /nodata, $
              yrange=[-pi,pi]
  oplot, lag_x, phase, color = col_phase_good,line=lin1

  for i=0,mplgs_min_1 do begin

    if ( badlag(i) eq 1) then begin
      col_phase = col_phase_bad 
      char = char_phase_bad
    endif  $
      else begin
      col_phase = col_phase_good
      char = char_phase_good
    endelse  
          
    xyouts, lag_x(i), phase(i),char, color=col_phase

    endfor

  return
end

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; NAME:
;	PLT_PWR
;
; PURPOSE:
;	plot the lag-0 power as a function of range gate using
;	the raw data
;
; Calling Sequence:
;	plt_pwr, [rd],[/LOGPLOT]
;
;	procedure plt pwr plots the power
;
;----------------------------------------------------------------------------
;


pro plt_pwr, rd, logplot=logplot
  common rawdata_com, rawfileptr, raw_data, badlag, more_badrange, $
                      lags, rd_byte
;
;  if there is one or more arguments, copy the first argument to the
;  common block raw_data.
;
  if n_params() GT 1  then raw_data = rd
;
;  Check the logplot keyword
;
  if (NOT keyword_set(logplot)) then logplot=0

  pwr_x = indgen(raw_data.p.nrang)
  dfmt = '(i2.2,"/",i2.2,"/",i2.2," ",i2.2,":",i2.2,":",i2.2)'
  datestring = string(format=dfmt,raw_data.p.year-1900,$
    raw_data.p.month,$
    raw_data.p.day,raw_data.p.hour,raw_data.p.minut,raw_data.p.sec)
  !p.title='Lag-0 Power vs Range  ' + datestring
  !x.title='Range Number'
  !y.title='Lag-0 Power'
  plot,pwr_x,raw_data.pwr0,ytype=logplot
  !p.title=''
  !x.title=''
  !y.title=''
  return
end

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; NAME:
;	PLT_BOTH
;
; PURPOSE:
;	Plot the ACF and the PHASE
;
; Calling Sequence:
;	plt_both, range, [rawdata], [/LOGPLOT]
;
;----------------------------------------------------------------------------
;


pro plt_both, range, rd, logplot=logplot
  common rawdata_com, rawfileptr, raw_data, badlag, more_badrange, $
                      lags, rd_byte
;
; if there are two arguments, copy the second argument to the
; common block raw_data.
;
  if n_params() EQ 2 then raw_data = rd
;
;  Check the logplot keyword
;
  if (NOT keyword_set(logplot)) then logplot=0
;
  !p.multi = [0,1,2]
  plt_acf_pwr, range, logplot=logplot
  plt_phase, range
  !p.multi = 0

  return
end







