;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
;	NAME:
; time_series_plot
;
; PURPOSE:
; makes an RTP plot
;
; CATEGORY:
; Graphics
;
; CALLING SEQUENCE:
; time_series_plot,yr,mo,dy,sthr,stmn,edhr,edmn,rad,filtflg,tgtbeam,ytype,ctype,$
;                     scale,grayflg=GRAYFLG,graynum=GRAYNUM,gsflg=GSFLG,rtflg=RTFLG
;
;	INPUTS:
;	yr 							-- year
; mo 							-- month
;	dy 							-- day
;	sthr 						-- start hour for the plot
;	stmin 					-- start minute for the plot
; edhr 						-- end hour for the plot
; edmin					 	-- end minute for the plot
;	rad 						-- a string with the 3 letter radar abbreviation, e.g. 'bks'
; filtflg 				-- 0 or 1 indicating whether to use filtered file
; tgtbeam         -- beam number
; ytype           -- 0 for gate, 1 for geo lat, 2 for mag lat
;	ctype           -- a string or vector of strings with the param to be plotted, e.g. ['vel','pow','wid','elv']
; scale           -- a scalar or vector with the scale increments for the color scale, must have same number of elements as ctype
;
; OPTIONAL INPUTS:
;	/grayflg        -- plot low vels in gray
; /graynum        -- threshold for grayscale plotting
; /gsflg          -- plot ground scatter in gray
;	/rtflg          -- plotting ray tracing, change filename
;	/rtflg          -- use gs flag from AJ's search
;
; KEYWORD PARAMETERS:
;
; EXAMPLE:
; 
;
; OUTPUT:
; /rst/output_plots/merge.ps (standard)
;	/rst/output_plots/merge.2rad.ps (/ORIG)
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
; Written by AJ Ribeiro 08/24/2011
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

pro time_series_plot,yr,mo,dy,sthr,stmn,edhr,edmn,rad,filtflg,tgtbeam,ytype,ctype,$
                      scale,grayflg=GRAYFLG,graynum=GRAYNUM,gsflg=GSFLG,rtflg=RTFLG,pscat=PSCAT

  if(mo gt 9) then mostr = strtrim(fix(mo),2) $
  else mostr = '0'+strtrim(fix(mo),2)
  if(dy gt 9) then dystr = strtrim(fix(dy),2) $
  else dystr = '0'+strtrim(fix(dy),2)
  date = strtrim(fix(yr),2)+mostr+dystr
  if(keyword_set(rtflg)) then begin
		filename = '/data/fit/'+date+'.'+rad+'.rt.fitex'
  endif else begin
		if(filtflg eq 1) then filename = '/data/fit/'+date+'.'+rad+'.fitexbfnc' $
		else if(keyword_set(pscat)) then filename = '/data/pscat/'+date+'.'+rad+'.pscat' $
		else filename = '/data/fit/'+date+'.'+rad+'.fitex'
	endelse

  ;define a usersym
  S         =  findgen(17)*(!PI*2./16.)
  usersym,cos(S),sin(S),/FILL

  set_plot,'PS',/copy
  if(keyword_set(rtflg)) then begin
		device,/landscape,filename='/rst/output_plots/'+date+'.'+rad+'.rt.ps',/COLOR,BITS_PER_PIXEL=8
  endif else begin
		if(filtflg eq 1) then device,/landscape,filename='/rst/output_plots/'+date+'.'+rad+'.f.ps',/COLOR,BITS_PER_PIXEL=8 $
		else device,/landscape,filename='/rst/output_plots/'+date+'.'+rad+'.ps',/COLOR,BITS_PER_PIXEL=8
	endelse


  jul_start = julday(mo,dy,yr,sthr,stmn,0)
  jul_end = julday(mo,dy,yr,edhr,edmn,0)
  myformat = LABEL_DATE(date_format= '%H:%I:%S')


  RadarMakeRadarPrm,prm
  prm_arr = replicate(prm,20000)
  FitMakeFitData,fit
  fit_arr = replicate(fit,20000)
  dind = 0L
  ;open the fit file
  inp  = FitOpen(filename,/read)
  ;start reading the fit file
  while FitRead(inp,prm,fit) ne -1 do begin
    if(prm.bmnum ne tgtbeam OR prm.channel eq 2) then continue
    jTime = julday(prm.time.mo,prm.time.dy,prm.time.yr,prm.time.hr,prm.time.mt,0)
    ;check if we are in the time window, AND have a cpid change
    if(jTime lt jul_start) then continue
    if(jTime ge jul_end) then break
    if(dind eq 0) then begin
      radar_info,stid,glat,glon,mlat,mlon,oneletter,threeletter,name,prm.stid
      plottitle = name+'               '+strtrim(fix(yr),2)+'/'+mostr+'/'+dystr+$
                    '               Beam: '+strtrim(fix(tgtbeam),2)
    endif
    ;save the contents
    prm_arr(dind) = prm
    fit_arr(dind) = fit
    dind = dind + 1
  endwhile
  free_lun,inp

  prm_arr = reform(prm_arr(0:dind-1))
  fit_arr = reform(fit_arr(0:dind-1))
  ;allow plotting space for last integration period
  jul_end = jul_end+(double(prm_arr(dind-1).intt.sc)/86400.+double(prm_arr(dind-1).intt.us)/86400000000.)
  loadct,0
  for m=0,n_elements(ctype)-1 do begin
    ;set up the color scales
    col = ctype(m)
    s = scale(m)
    ;non-velocity plot
    if(col ne 'vel') then begin
      colors = [0,50,100,150,200,225,250]
      cscale = [0,fix(1.*s),fix(2.*s),fix(3.*s),fix(4.*s),fix(5.*s),fix(6.*s)]
    ;velocity plot
    endif else begin
      ;low vels in gray
      if(keyword_set(grayflg)) then begin
        colors = [150,200,225,250,150,0,50,75,100]
        cscale = [fix(-3.*s),fix(-2.*s),fix(-1.*s),fix(-1.*graynum),$
                  fix(1.*graynum),fix(1.*s),fix(2.*s),fix(3.*s)]
      endif else begin
        colors = [150,200,225,250,0,50,75,100]
        cscale = [fix(-3.*s),fix(-2.*s),fix(-1.*s),0,fix(1.*s),fix(2.*s),fix(3.*s)]
      endelse
    endelse


    ;***********************************
    ;*******FIRST, DO CPID PLOT*********
    ;***********************************
    cpid_old = -99999999
    ;set up the cpid plot
    PLOT,findgen(1),findgen(1),xrange=[jul_start,jul_end],yrange=[0,1],xticks=4,xminor=6,/nodata,title=plottitle,$
      XSTYLE=1,xtickname=REPLICATE(' ', 8),yticks=1,yminor=0,yticklen=-.01,position=[.08,.92,.87,.96],$
      xticklen=-.025,xthick=4,ythick=4,charthick=4,charsize=1.25,ystyle=1,ytickname=REPLICATE(' ', 8),/noerase
    xyouts,.04,.935,"CPID",charthick=4,charsize=.75,/normal
    ;read the file, plotting cpids as it goes along
    for i=0,dind-1 do begin
      ;check for a new cpid
      if(prm_arr(i).cp eq cpid_old) then continue
      if(prm_arr(i).cp eq 153 AND prm_arr(i).channel ne 2) then cpid_str = strtrim(prm_arr(i).cp,2) + " Ch A" $
      else cpid_str = strtrim(prm_arr(i).cp,2)
      jTime = julday(prm_arr(i).time.mo,prm_arr(i).time.dy,prm_arr(i).time.yr,prm_arr(i).time.hr,prm_arr(i).time.mt,prm_arr(i).time.sc)
      if(prm_arr(i).ifmode eq 0) then cpid_str = cpid_str + " / HF"
      if(prm_arr(i).ifmode eq 1) then cpid_str = cpid_str + " / IF"
      plots,[jTime,jTime],[0,1],/data,thick=4
      xyouts,jTime,.25,cpid_str,charthick=4,/data
      cpid_old = prm_arr(i).cp
    endfor

    ;***********************************
    ;*******NEXT, DO Noise PLOT*********
    ;***********************************
    PLOT,findgen(1),findgen(1),xrange=[jul_start,jul_end],yrange=[0,5],xticks=4,xminor=6,/noerase,$
      XSTYLE=1,xtickname=REPLICATE(' ', 8),yticks=1,yminor=5,yticklen=-.01,position=[.08,.84,.87,.92],$
      ytitle="Noise",xticklen=-.025,xthick=4,ythick=4,charthick=4,charsize=1.25,ystyle=1,ytickname=REPLICATE(' ', 8)
    xyouts,.07,.84,'10^0',charsize=.75,charthick=4,align=1,/normal
    xyouts,.07,.91,'10^5',charsize=.75,charthick=4,align=1,/normal
    ;read the file, plotting cpids as it goes along
    for i=0,dind-1 do begin
      if(prm_arr(i).channel eq 2) then continue
      jTime = julday(prm_arr(i).time.mo,prm_arr(i).time.dy,prm_arr(i).time.yr,prm_arr(i).time.hr,prm_arr(i).time.mt,prm_arr(i).time.sc)
      plots,jTime,alog10(prm_arr(i).noise.search),/data,thick=4,psym=8,symsize=.4
    endfor


    ;***********************************
    ;*******NEXT, DO FREQ PLOT*********
    ;***********************************
    ;set up the freq plot
    PLOT,findgen(1),findgen(1),xrange=[jul_start,jul_end],yrange=[8000,18000],xticks=4,xminor=6,ytitle='Freq',$
      XSTYLE=1,xtickname=REPLICATE(' ', 8),yticks=1,yminor=2,yticklen=-.01,position=[.08,.76,.87,.84],$
      xticklen=-.025,xthick=4,ythick=4,charthick=4,charsize=1.25,ystyle=1,/noerase,/nodata,ytickname=REPLICATE(' ', 8)
    xyouts,.07,.76,'8000',charsize=.75,charthick=4,align=1,/normal
    xyouts,.07,.825,'18000',charsize=.75,charthick=4,align=1,/normal
    ;plot the frequencies
    for i=0,dind-1 do begin
      if(prm_arr(i).channel eq 2) then continue
      jTime = julday(prm_arr(i).time.mo,prm_arr(i).time.dy,prm_arr(i).time.yr,prm_arr(i).time.hr,prm_arr(i).time.mt,prm_arr(i).time.sc)
      plots,jTime,prm_arr(i).tfreq,/data,thick=4,psym=8,symsize=.4
    endfor

    ;***********************************
    ;*******NEXT, DO DATA PLOT*********
    ;***********************************
    ymax=-99999.
    ;plot in range gate
    if(ytype eq 0) then begin
      ymin = 0
      for i=0,dind-1 do $
        if(prm_arr(i).nrang gt ymax) then $
          ymax = prm_arr(i).nrang
      yname = 'Range Gate'
    endif
    ;plot in geo coords
    if(ytype eq 1) then begin
      yrsc=TimeYMDHMSToYrSec(prm_arr(0).time.yr,prm_arr(0).time.mo,prm_arr(0).time.dy,$
                              prm_arr(0).time.hr,prm_arr(0).time.mt,prm_arr(0).time.sc)
      pos = rbpos(1,height=300,beam=tgtbeam,lagfr=prm_arr(0).lagfr,smsep=prm_arr(0).smsep, $
							rxrise=prm_arr(0).rxrise,station=prm_arr(0).stid, $
							year=prm_arr(0).time.yr,yrsec=yrsc,/GEO)
      ymin = pos(0,0,0)
      for i=0,dind-1 do begin
        yrsc=TimeYMDHMSToYrSec(prm_arr(i).time.yr,prm_arr(i).time.mo,prm_arr(i).time.dy,$
                                prm_arr(i).time.hr,prm_arr(i).time.mt,prm_arr(i).time.sc)
        pos = rbpos(prm_arr(i).nrang+1,height=300,beam=tgtbeam,lagfr=prm_arr(i).lagfr,smsep=prm_arr(i).smsep, $
          rxrise=prm_arr(i).rxrise,station=prm_arr(i).stid, $
          year=prm_arr(i).time.yr,yrsec=yrsc,/GEO)
        if(abs(pos(0,0,1)) gt abs(ymax)) then $
          ymax = pos(0,0,1)
      endfor
      yname = 'Geographic Latitude'
    endif
    ;plot in mag coords
    if(ytype eq 2) then begin
      yrsc=TimeYMDHMSToYrSec(prm_arr(0).time.yr,prm_arr(0).time.mo,prm_arr(0).time.dy,$
                              prm_arr(0).time.hr,prm_arr(0).time.mt,prm_arr(0).time.sc)
      pos = rbpos(1,height=300,beam=tgtbeam,lagfr=prm_arr(0).lagfr,smsep=prm_arr(0).smsep, $
						rxrise=prm_arr(0).rxrise,station=prm_arr(0).stid, $
						year=prm_arr(0).time.yr,yrsec=yrsc)
      ymin = pos(0,0,0)
      for i=0,dind-1 do begin
        yrsc=TimeYMDHMSToYrSec(prm_arr(i).time.yr,prm_arr(i).time.mo,prm_arr(i).time.dy,$
                                prm_arr(i).time.hr,prm_arr(i).time.mt,prm_arr(i).time.sc)
        for j=0,prm_arr(i).nrang-1 do begin
          pos = rbpos(j+1,height=300,beam=tgtbeam,lagfr=prm_arr(i).lagfr,smsep=prm_arr(i).smsep, $
            rxrise=prm_arr(i).rxrise,station=prm_arr(i).stid, $
            year=prm_arr(i).time.yr,yrsec=yrsc)
          if(abs(pos(0,0,1)) ge abs(ymax)) then $
            ymax = pos(0,0,1)
        endfor
      endfor
      yname = 'Magnetic Latitude'
    endif
    PLOT,findgen(1),findgen(1),xrange=[jul_start,jul_end],yrange=[ymin,ymax],xtitle="UT",$
      XTICKFORMAT= 'LABEL_DATE',XSTYLE=1,xtickunits='time',position=[.08,.08,.87,.76],$
      ytitle=yname,ystyle=1,/normal,charsize=1.25,charthick=4,xthick=4,ythick=4,/noerase
    loadct,34

    if(col ne 'vel') then begin
      draw_other_colorbar,colors,cscale,.92,.3,col
    endif else begin
      flg = 0
      if(keyword_set(grayflg)) then $
        flg = 1

      gflg = 0
      if(keyword_set(gsflg)) then $
        gflg = 1
      draw_vel_colorbar,colors,cscale,.92,.3,flg,gflg
    endelse

    ;do the actual plotting
    for i=0,dind-1 do begin
      ;only plot channel A
      if(prm_arr(i).channel eq 2) then continue
      for j=0,prm.nrang-1 do begin
        ;check for backscatter in the RB cell
        if(fit_arr(i).qflg(j) eq 0) then continue
        ;get x coords
        jTime = julday(prm_arr(i).time.mo,prm_arr(i).time.dy,prm_arr(i).time.yr,$
                        prm_arr(i).time.hr,prm_arr(i).time.mt,prm_arr(i).time.sc)
        if(i lt dind-1) then begin
          xnext = julday(prm_arr(i+1).time.mo,prm_arr(i+1).time.dy,prm_arr(i+1).time.yr,$
                          prm_arr(i+1).time.hr,prm_arr(i+1).time.mt,prm_arr(i+1).time.sc)
;           if((xnext - jTime) gt 4./1440.) then xnext = jTime+2./1440.
        endif else begin
          xnext = jul_end
        endelse
        ;get y coords depending on plot type
        case ytype of
          0: begin
            y1 = j
            y2 = j+1
          end
          1: begin
            yrsc=TimeYMDHMSToYrSec(prm_arr(i).time.yr,prm_arr(i).time.mo,prm_arr(i).time.dy,$
                                    prm_arr(i).time.hr,prm_arr(i).time.mt,prm_arr(i).time.sc)
            pos = rbpos(j+1,height=300,beam=tgtbeam,lagfr=prm_arr(i).lagfr,smsep=prm_arr(i).smsep, $
              rxrise=prm_arr(i).rxrise,station=prm_arr(i).stid, $
              year=prm_arr(i).time.yr,yrsec=yrsc,/GEO)
            y1 = pos(0,0,0)
            y2 = pos(0,0,1)
          end
          2: begin
            yrsc=TimeYMDHMSToYrSec(prm_arr(i).time.yr,prm_arr(i).time.mo,prm_arr(i).time.dy,$
                                    prm_arr(i).time.hr,prm_arr(i).time.mt,prm_arr(i).time.sc)
            pos = rbpos(j+1,height=300,beam=tgtbeam,lagfr=prm_arr(i).lagfr,smsep=prm_arr(i).smsep, $
              rxrise=prm_arr(i).rxrise,station=prm_arr(i).stid, $
              year=prm_arr(i).time.yr,yrsec=yrsc)
            y1 = pos(0,0,0)
            y2 = pos(0,0,1)
          end
        endcase
        ;get point color for non-velocity plot
        if(col ne 'vel') then begin
          case col of
            'pow': begin
              z = fit_arr(i).p_l(j)
            end
            'wid': begin
              z = fit_arr(i).w_l(j)
            end
            'elv': begin
              z = fit_arr(i).elv(j)
            end
          endcase
          for k=n_elements(colors)-1,0,-1 do begin
            if(z ge cscale(k)) then begin
              mycol = colors(k)
              break
            endif
          endfor
          loadct,34
        ;get point color for velocity plot
        endif else begin
          z = fit_arr(i).v(j)
          if(z lt cscale(0)) then begin
            mycol = colors(0)
          endif else begin
          for k=0,n_elements(cscale)-2 do begin
            if(z ge cscale(k) AND z lt cscale(k+1)) then begin
              mycol = colors(k+1)
              break
            endif
          endfor
          endelse
          if(z ge cscale(n_elements(cscale)-1)) then $
            mycol = colors(n_elements(colors)-1)
          loadct,34
          if(keyword_set(grayflg)) then begin
            if(abs(z) le abs(graynum)) then loadct,0
          endif
          if(keyword_set(gsflg) AND fit_arr(i).gflg(j) eq 1) then begin
            mycol = 150
            loadct,0
          endif
        endelse
        polyfill,[jTime,xnext,xnext,jTime],[y1,y1,y2,y2],color=mycol
      endfor
    endfor
    erase
    loadct,0
  endfor

  ;close the postscript file
  if(!d.name eq 'PS') then  device,/close

end
