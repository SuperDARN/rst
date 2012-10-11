pro simdat
  ;the file we are reading data from
  file_in = '/data/fit/aj/simdat/simdata.data'
  set_plot,'PS',/copy
  device,/landscape,/COLOR,BITS_PER_PIXEL=8,filename='simdat.ps'
  S = findgen(17)*(!PI*2./16.)
  !p.multi = [0,1,1]
  acf = dblarr(30,5)
  lmacf = dblarr(30,5)
  exacf = dblarr(30,9)
  openr,aj_unit,file_in,/get_lun

  allacf = dblarr(30,5)

  readf,aj_unit,nrang,nlags,mpinc,freq

  errors = dblarr(nrang,2)

  for i=0,nrang-1 do begin

    readf,aj_unit,v_i,f_i,w_i,t_i,lag0pwr
    n_good = 0
    maxy = -999
    miny = 999
    for j=0,nlags-1 do begin
      readf,aj_unit,lag,tau,re,im,qflg
      allacf(n_good,0) = lag
      allacf(n_good,1) = tau
      allacf(n_good,2) = re
      allacf(n_good,3) = im
      if(qflg) then begin
        acf(n_good,0) = lag
        acf(n_good,1) = tau
        acf(n_good,2) = re
        acf(n_good,3) = im
        acf(n_good,4) = qflg
        n_good = n_good+1
        if(re gt maxy) then maxy = re
        if(im gt maxy) then maxy = im
        if(re lt miny) then miny = re
        if(im lt miny) then miny = im
      endif
    endfor

    ;FIRST, DO A LARGE PLOT OF THE SIMULATED ACF
    loadct,0
    plot,findgen(1),findgen(1),xrange=[0,nlags],yrange=[miny-1,maxy+1],$
          xstyle=1,ystyle=1,xticks=lastlag,yticks=6,position=[0.05,0.8,0.9,0.95],/nodata,/noerase,$
          title='Simulated ACF    vel = '+strmid(strtrim(v_i,2),0,7)+'   width = '+strmid(strtrim(w_i,2),0,7),$
          charsize=0.7,xtickname=replicate(' ',nlags+1)
    xyouts,0.95,0.935,'Re',charsize=0.8,/normal,alignment=0.5
    xyouts,0.95,0.905,'Im',charsize=0.8,/normal,alignment=0.5
    loadct,34
    plots,0.93,0.93,/normal,psym=2,col=255
    plots,[0.93,0.97],[0.93,0.93],col=255,/normal
    plots,0.93,0.9,/normal,psym=7,col=50
    plots,[0.93,0.97],[0.9,0.9],col=50,/normal
    for j=1,n_good-1 do begin
      n_xy_s = convert_coord(acf(j-1,0),acf(j-1,2),/data,/to_normal)
      n_xy_e = convert_coord(acf(j,0),acf(j,2),/data,/to_normal)
      plots,n_xy_s(0),n_xy_s(1),/normal,psym=2,col=255
      plots,n_xy_e(0),n_xy_e(1),/normal,psym=2,col=255
      plots,[n_xy_s(0),n_xy_e(0)],[n_xy_s(1),n_xy_e(1)],col=255,/normal

      n_xy_s = convert_coord(acf(j-1,0),acf(j-1,3),/data,/to_normal)
      n_xy_e = convert_coord(acf(j,0),acf(j,3),/data,/to_normal)
      plots,n_xy_s(0),n_xy_s(1),/normal,psym=7,col=50
      plots,n_xy_e(0),n_xy_e(1),/normal,psym=7,col=50
      plots,[n_xy_s(0),n_xy_e(0)],[n_xy_s(1),n_xy_e(1)],col=50,/normal
    endfor

    readf,aj_unit,stat,v_if,w_if,t_if,f_if,lag0pwrf

    ;NOW, OVERPLOT HOW WELL L-M APPROXIMATES THE ACF
    error = 0.
    for j=0,nlags-1 do begin
      tau = j*mpinc
      re = lag0pwr*exp(-1.0*tau/t_i)*cos(tau*f_i)
      im = lag0pwr*exp(-1.0*tau/t_i)*sin(tau*f_i)
      tau = j*mpinc
      ref = lag0pwrf*exp(-1.0*tau/t_if)*cos(tau*f_if)
      imf = lag0pwrf*exp(-1.0*tau/t_if)*sin(tau*f_if)
      error = error + ((re-ref)^2 + (im-imf)^2)
    endfor
    error = error / nlags
    error = sqrt(error)
    loadct,0
    plot,findgen(1),findgen(1),xrange=[0,nlags],yrange=[miny-1,maxy+1],$
          xstyle=1,ystyle=1,xticks=lastlag,yticks=6,position=[0.05,0.6,0.42,0.75],/nodata,/noerase,$
          title='Simulated ACF (no noise) and LM-Fit -- MSE = '+strmid(strtrim(error,2),0,6),$
          charsize=0.7,xtickname=replicate(' ',nlags+1)
    xyouts,0.47,0.735,'Re',charsize=0.8,/normal,alignment=0.5
    xyouts,0.47,0.705,'Im',charsize=0.8,/normal,alignment=0.5
    xyouts,0.47,0.675,'LM Re',charsize=0.8,/normal,alignment=0.5
    xyouts,0.47,0.645,'LM Im',charsize=0.8,/normal,alignment=0.5
    loadct,34
    plots,0.45,0.73,/normal,psym=2,col=255
    plots,[0.45,0.49],[0.73,0.73],col=255,/normal
    plots,0.45,0.7,/normal,psym=7,col=50
    plots,[0.45,0.49],[0.7,0.7],col=50,/normal

    plots,0.45,0.67,/normal,psym=6,col=225
    plots,[0.45,0.49],[0.67,0.67],col=225,/normal,linestyle=2
    plots,0.45,0.64,/normal,psym=5,col=75
    plots,[0.45,0.49],[0.64,0.64],col=75,/normal,linestyle=2

    error = 0.

    for j=1,nlags-1 do begin

      tau = j*mpinc
      re = lag0pwr*exp(-1.0*tau/t_i)*cos(tau*f_i)
      im = lag0pwr*exp(-1.0*tau/t_i)*sin(tau*f_i)

      tau = (j-1)*mpinc
      rep = lag0pwr*exp(-1.0*tau/t_i)*cos(tau*f_i)
      imp = lag0pwr*exp(-1.0*tau/t_i)*sin(tau*f_i)

      n_xy_s = convert_coord(j-1,rep,/data,/to_normal)
      n_xy_e = convert_coord(j,re,/data,/to_normal)
      plots,n_xy_s(0),n_xy_s(1),/normal,psym=2,col=255
      plots,n_xy_e(0),n_xy_e(1),/normal,psym=2,col=255
      plots,[n_xy_s(0),n_xy_e(0)],[n_xy_s(1),n_xy_e(1)],col=255,/normal

      n_xy_s = convert_coord(j-1,imp,/data,/to_normal)
      n_xy_e = convert_coord(j,im,/data,/to_normal)
      plots,n_xy_s(0),n_xy_s(1),/normal,psym=7,col=50
      plots,n_xy_e(0),n_xy_e(1),/normal,psym=7,col=50
      plots,[n_xy_s(0),n_xy_e(0)],[n_xy_s(1),n_xy_e(1)],col=50,/normal

      tau = j*mpinc
      re = lag0pwrf*exp(-1.0*tau/t_if)*cos(tau*f_if)
      im = lag0pwrf*exp(-1.0*tau/t_if)*sin(tau*f_if)

      tau = (j-1)*mpinc
      rep = lag0pwrf*exp(-1.0*tau/t_if)*cos(tau*f_if)
      imp = lag0pwrf*exp(-1.0*tau/t_if)*sin(tau*f_if)

      n_xy_s = convert_coord(j-1,rep,/data,/to_normal)
      n_xy_e = convert_coord(j,re,/data,/to_normal)
      plots,n_xy_s(0),n_xy_s(1),/normal,psym=6,col=225,symsize=1.5
      plots,n_xy_e(0),n_xy_e(1),/normal,psym=6,col=225,symsize=1.5
      plots,[n_xy_s(0),n_xy_e(0)],[n_xy_s(1),n_xy_e(1)],col=225,/normal,linestyle=2

      n_xy_s = convert_coord(j-1,imp,/data,/to_normal)
      n_xy_e = convert_coord(j,im,/data,/to_normal)
      plots,n_xy_s(0),n_xy_s(1),/normal,psym=5,col=75,symsize=1.5
      plots,n_xy_e(0),n_xy_e(1),/normal,psym=5,col=75,symsize=1.5
      plots,[n_xy_s(0),n_xy_e(0)],[n_xy_s(1),n_xy_e(1)],col=75,/normal,linestyle=2

    endfor

    ;NOW, PLOT LM PHASE FIT
    LOADCT,0
    plot,findgen(1),findgen(1),xrange=[0,nlags],yrange=[-1.*!pi,!pi],$
          xstyle=1,ystyle=1,xticks=lastlag,yticks=6,position=[0.53,0.6,0.9,0.75],/nodata,/noerase,$
          title='Sim vel = '+strmid(strtrim(v_i,2),0,6)+' m/s L-M Fit vel ='+strmid(strtrim(v_if,2),0,6)+$
          'm/s err = '+strmid(strtrim(abs(v_i-v_if),2),0,4)+'m/s',charsize=0.7,xtickname=replicate(' ',nlags+1)
    xyouts,0.95,0.735,'Phi',charsize=0.8,/normal,alignment=0.5
    xyouts,0.95,0.705,'LM phi',charsize=0.8,/normal,alignment=0.5
    loadct,34
    usersym,cos(S),sin(S),/FILL
    plots,0.93,0.73,/normal,psym=8,col=0
    plots,[0.93,0.97],[0.53,0.53],col=0,/normal
        usersym,cos(S),sin(S)
    plots,0.93,0.7,/normal,psym=8,col=150
    plots,[0.93,0.97],[0.7,0.7],col=150,/normal,linestyle=2


    for j=1,nlags-1 do begin

        usersym,cos(S),sin(S),/FILL

        tau = j*mpinc
        re = lag0pwr*exp(-1.0*tau/t_i)*cos(tau*f_i)
        im = lag0pwr*exp(-1.0*tau/t_i)*sin(tau*f_i)

        tau = (j-1)*mpinc
        rep = lag0pwr*exp(-1.0*tau/t_i)*cos(tau*f_i)
        imp = lag0pwr*exp(-1.0*tau/t_i)*sin(tau*f_i)

        phi = atan(im,re)
        phip = atan(imp,rep)
        n_xy_s = convert_coord(j-1,phip,/data,/to_normal)
        n_xy_e = convert_coord(j,phi,/data,/to_normal)
        plots,n_xy_s(0),n_xy_s(1),/normal,psym=8,col=0
        plots,n_xy_e(0),n_xy_e(1),/normal,psym=8,col=0
        plots,[n_xy_s(0),n_xy_e(0)],[n_xy_s(1),n_xy_e(1)],col=0,/normal

        usersym,cos(S),sin(S)
        tau = j*mpinc
        re = lag0pwrf*exp(-1.0*tau/t_if)*cos(tau*f_if)
        im = lag0pwrf*exp(-1.0*tau/t_if)*sin(tau*f_if)

        tau = (j-1)*mpinc
        rep = lag0pwrf*exp(-1.0*tau/t_if)*cos(tau*f_if)
        imp = lag0pwrf*exp(-1.0*tau/t_if)*sin(tau*f_if)

        phi = atan(im,re)
        phip = atan(imp,rep)
        n_xy_s = convert_coord(j-1,phip,/data,/to_normal)
        n_xy_e = convert_coord(j,phi,/data,/to_normal)
        plots,n_xy_s(0),n_xy_s(1),/normal,psym=8,col=150,symsize=1.5
        plots,n_xy_e(0),n_xy_e(1),/normal,psym=8,col=150,symsize=1.5
        plots,[n_xy_s(0),n_xy_e(0)],[n_xy_s(1),n_xy_e(1)],col=150,/normal,linestyle=2
    endfor

    ;READ FITEX PARAMETERS
    readf,aj_unit,stat,v_if,w_if,t_if,f_if,lag0pwrf

    ;NOW, OVERPLOT HOW WELL FITEX APPROXIMATES THE ACF
    error = 0.
    for j=0,nlags-1 do begin
      tau = j*mpinc
      re = lag0pwr*exp(-1.0*tau/t_i)*cos(tau*f_i)
      im = lag0pwr*exp(-1.0*tau/t_i)*sin(tau*f_i)
      tau = j*mpinc
      ref = lag0pwrf*exp(-1.0*tau/t_if)*cos(tau*f_if)
      imf = lag0pwrf*exp(-1.0*tau/t_if)*sin(tau*f_if)
      error = error + ((re-ref)^2 + (im-imf)^2)
    endfor
    error = error / nlags
    error = sqrt(error)
    loadct,0
    plot,findgen(1),findgen(1),xrange=[0,nlags],yrange=[miny-1,maxy+1],$
          xstyle=1,ystyle=1,xticks=lastlag,yticks=6,position=[0.05,0.4,0.42,0.55],/nodata,/noerase,$
          title='Simulated ACF (no noise) and Fitex -- MSE = '+strmid(strtrim(error,2),0,6),$
          charsize=0.7,xtickname=replicate(' ',nlags+1)
    xyouts,0.47,0.535,'Re',charsize=0.8,/normal,alignment=0.5
    xyouts,0.47,0.505,'Im',charsize=0.8,/normal,alignment=0.5
    xyouts,0.47,0.475,'Ex Re',charsize=0.8,/normal,alignment=0.5
    xyouts,0.47,0.445,'Ex Im',charsize=0.8,/normal,alignment=0.5
    loadct,34
    plots,0.45,0.53,/normal,psym=2,col=255
    plots,[0.45,0.49],[0.53,0.53],col=255,/normal
    plots,0.45,0.7,/normal,psym=7,col=50
    plots,[0.45,0.49],[0.5,0.5],col=50,/normal

    plots,0.45,0.47,/normal,psym=6,col=225
    plots,[0.45,0.49],[0.47,0.47],col=225,/normal,linestyle=2
    plots,0.45,0.44,/normal,psym=5,col=75
    plots,[0.45,0.49],[0.44,0.44],col=75,/normal,linestyle=2

    error = 0.

    for j=1,nlags-1 do begin

      tau = j*mpinc
      re = lag0pwr*exp(-1.0*tau/t_i)*cos(tau*f_i)
      im = lag0pwr*exp(-1.0*tau/t_i)*sin(tau*f_i)

      tau = (j-1)*mpinc
      rep = lag0pwr*exp(-1.0*tau/t_i)*cos(tau*f_i)
      imp = lag0pwr*exp(-1.0*tau/t_i)*sin(tau*f_i)

      n_xy_s = convert_coord(j-1,rep,/data,/to_normal)
      n_xy_e = convert_coord(j,re,/data,/to_normal)
      plots,n_xy_s(0),n_xy_s(1),/normal,psym=2,col=255
      plots,n_xy_e(0),n_xy_e(1),/normal,psym=2,col=255
      plots,[n_xy_s(0),n_xy_e(0)],[n_xy_s(1),n_xy_e(1)],col=255,/normal

      n_xy_s = convert_coord(j-1,imp,/data,/to_normal)
      n_xy_e = convert_coord(j,im,/data,/to_normal)
      plots,n_xy_s(0),n_xy_s(1),/normal,psym=7,col=50
      plots,n_xy_e(0),n_xy_e(1),/normal,psym=7,col=50
      plots,[n_xy_s(0),n_xy_e(0)],[n_xy_s(1),n_xy_e(1)],col=50,/normal

      tau = j*mpinc
      re = lag0pwrf*exp(-1.0*tau/t_if)*cos(tau*f_if)
      im = lag0pwrf*exp(-1.0*tau/t_if)*sin(tau*f_if)

      tau = (j-1)*mpinc
      rep = lag0pwrf*exp(-1.0*tau/t_if)*cos(tau*f_if)
      imp = lag0pwrf*exp(-1.0*tau/t_if)*sin(tau*f_if)

      n_xy_s = convert_coord(j-1,rep,/data,/to_normal)
      n_xy_e = convert_coord(j,re,/data,/to_normal)
      plots,n_xy_s(0),n_xy_s(1),/normal,psym=6,col=225,symsize=1.5
      plots,n_xy_e(0),n_xy_e(1),/normal,psym=6,col=225,symsize=1.5
      plots,[n_xy_s(0),n_xy_e(0)],[n_xy_s(1),n_xy_e(1)],col=225,/normal,linestyle=2

      n_xy_s = convert_coord(j-1,imp,/data,/to_normal)
      n_xy_e = convert_coord(j,im,/data,/to_normal)
      plots,n_xy_s(0),n_xy_s(1),/normal,psym=5,col=75,symsize=1.5
      plots,n_xy_e(0),n_xy_e(1),/normal,psym=5,col=75,symsize=1.5
      plots,[n_xy_s(0),n_xy_e(0)],[n_xy_s(1),n_xy_e(1)],col=75,/normal,linestyle=2

    endfor

    ;NOW, PLOT EX PHASE FIT
    LOADCT,0
    plot,findgen(1),findgen(1),xrange=[0,nlags],yrange=[-1.*!pi,!pi],$
          xstyle=1,ystyle=1,xticks=lastlag,yticks=6,position=[0.53,0.4,0.9,0.55],/nodata,/noerase,$
          title='Sim vel = '+strmid(strtrim(v_i,2),0,6)+' m/s Fitex vel ='+strmid(strtrim(v_if,2),0,6)+$
          'm/s err = '+strmid(strtrim(abs(v_i-v_if),2),0,4)+'m/s',charsize=0.7,xtickname=replicate(' ',nlags+1)
    xyouts,0.95,0.535,'Phi',charsize=0.8,/normal,alignment=0.5
    xyouts,0.95,0.505,'Ex phi',charsize=0.8,/normal,alignment=0.5
    loadct,34
    usersym,cos(S),sin(S),/FILL
    plots,0.93,0.53,/normal,psym=8,col=0
    plots,[0.93,0.97],[0.53,0.53],col=0,/normal
        usersym,cos(S),sin(S)
    plots,0.93,0.5,/normal,psym=8,col=150
    plots,[0.93,0.97],[0.5,0.5],col=150,/normal,linestyle=2


    for j=1,nlags-1 do begin

        usersym,cos(S),sin(S),/FILL

        tau = j*mpinc
        re = lag0pwr*exp(-1.0*tau/t_i)*cos(tau*f_i)
        im = lag0pwr*exp(-1.0*tau/t_i)*sin(tau*f_i)

        tau = (j-1)*mpinc
        rep = lag0pwr*exp(-1.0*tau/t_i)*cos(tau*f_i)
        imp = lag0pwr*exp(-1.0*tau/t_i)*sin(tau*f_i)

        phi = atan(im,re)
        phip = atan(imp,rep)
        n_xy_s = convert_coord(j-1,phip,/data,/to_normal)
        n_xy_e = convert_coord(j,phi,/data,/to_normal)
        plots,n_xy_s(0),n_xy_s(1),/normal,psym=8,col=0
        plots,n_xy_e(0),n_xy_e(1),/normal,psym=8,col=0
        plots,[n_xy_s(0),n_xy_e(0)],[n_xy_s(1),n_xy_e(1)],col=0,/normal


        usersym,cos(S),sin(S)
        tau = j*mpinc
        re = lag0pwrf*exp(-1.0*tau/t_if)*cos(tau*f_if)
        im = lag0pwrf*exp(-1.0*tau/t_if)*sin(tau*f_if)

        tau = (j-1)*mpinc
        rep = lag0pwrf*exp(-1.0*tau/t_if)*cos(tau*f_if)
        imp = lag0pwrf*exp(-1.0*tau/t_if)*sin(tau*f_if)

        phi = atan(im,re)
        phip = atan(imp,rep)
        n_xy_s = convert_coord(j-1,phip,/data,/to_normal)
        n_xy_e = convert_coord(j,phi,/data,/to_normal)
        plots,n_xy_s(0),n_xy_s(1),/normal,psym=8,col=150,symsize=1.5
        plots,n_xy_e(0),n_xy_e(1),/normal,psym=8,col=150,symsize=1.5
        plots,[n_xy_s(0),n_xy_e(0)],[n_xy_s(1),n_xy_e(1)],col=150,/normal,linestyle=2
    endfor


    ;READ FITACF PARAMETERS
    readf,aj_unit,stat,v_if,w_if,t_if,f_if,lag0pwrf

    ;NOW, OVERPLOT HOW WELL FITEX APPROXIMATES THE ACF
    error = 0.
    for j=0,nlags-1 do begin
      tau = j*mpinc
      re = lag0pwr*exp(-1.0*tau/t_i)*cos(tau*f_i)
      im = lag0pwr*exp(-1.0*tau/t_i)*sin(tau*f_i)
      tau = j*mpinc
      ref = lag0pwrf*exp(-1.0*tau/t_if)*cos(tau*f_if)
      imf = lag0pwrf*exp(-1.0*tau/t_if)*sin(tau*f_if)
      error = error + ((re-ref)^2 + (im-imf)^2)
    endfor
    error = error / nlags
    error = sqrt(error)
    loadct,0
    plot,findgen(1),findgen(1),xrange=[0,nlags],yrange=[miny-1,maxy+1],$
          xstyle=1,ystyle=1,xticks=lastlag,yticks=6,position=[0.05,0.2,0.42,0.35],/nodata,/noerase,$
          title='Simulated ACF (no noise) and Fitacf -- MSE = '+strmid(strtrim(error,2),0,6),$
          charsize=0.7,xtickname=replicate(' ',nlags+1)
    xyouts,0.47,0.335,'Re',charsize=0.8,/normal,alignment=0.5
    xyouts,0.47,0.305,'Im',charsize=0.8,/normal,alignment=0.5
    xyouts,0.47,0.275,'Fit Re',charsize=0.8,/normal,alignment=0.5
    xyouts,0.47,0.245,'Fit Im',charsize=0.8,/normal,alignment=0.5
    loadct,34
    plots,0.45,0.33,/normal,psym=2,col=255
    plots,[0.45,0.49],[0.33,0.33],col=255,/normal
    plots,0.45,0.3,/normal,psym=7,col=50
    plots,[0.45,0.49],[0.3,0.3],col=50,/normal

    plots,0.45,0.27,/normal,psym=6,col=225
    plots,[0.45,0.49],[0.27,0.27],col=225,/normal,linestyle=2
    plots,0.45,0.24,/normal,psym=5,col=75
    plots,[0.45,0.49],[0.24,0.24],col=75,/normal,linestyle=2

    error = 0.

    for j=1,nlags-1 do begin

      tau = j*mpinc
      re = lag0pwr*exp(-1.0*tau/t_i)*cos(tau*f_i)
      im = lag0pwr*exp(-1.0*tau/t_i)*sin(tau*f_i)

      tau = (j-1)*mpinc
      rep = lag0pwr*exp(-1.0*tau/t_i)*cos(tau*f_i)
      imp = lag0pwr*exp(-1.0*tau/t_i)*sin(tau*f_i)

      n_xy_s = convert_coord(j-1,rep,/data,/to_normal)
      n_xy_e = convert_coord(j,re,/data,/to_normal)
      plots,n_xy_s(0),n_xy_s(1),/normal,psym=2,col=255
      plots,n_xy_e(0),n_xy_e(1),/normal,psym=2,col=255
      plots,[n_xy_s(0),n_xy_e(0)],[n_xy_s(1),n_xy_e(1)],col=255,/normal

      n_xy_s = convert_coord(j-1,imp,/data,/to_normal)
      n_xy_e = convert_coord(j,im,/data,/to_normal)
      plots,n_xy_s(0),n_xy_s(1),/normal,psym=7,col=50
      plots,n_xy_e(0),n_xy_e(1),/normal,psym=7,col=50
      plots,[n_xy_s(0),n_xy_e(0)],[n_xy_s(1),n_xy_e(1)],col=50,/normal

      tau = j*mpinc
      re = lag0pwrf*exp(-1.0*tau/t_if)*cos(tau*f_if)
      im = lag0pwrf*exp(-1.0*tau/t_if)*sin(tau*f_if)

      tau = (j-1)*mpinc
      rep = lag0pwrf*exp(-1.0*tau/t_if)*cos(tau*f_if)
      imp = lag0pwrf*exp(-1.0*tau/t_if)*sin(tau*f_if)

      n_xy_s = convert_coord(j-1,rep,/data,/to_normal)
      n_xy_e = convert_coord(j,re,/data,/to_normal)
      plots,n_xy_s(0),n_xy_s(1),/normal,psym=6,col=225,symsize=1.5
      plots,n_xy_e(0),n_xy_e(1),/normal,psym=6,col=225,symsize=1.5
      plots,[n_xy_s(0),n_xy_e(0)],[n_xy_s(1),n_xy_e(1)],col=225,/normal,linestyle=2

      n_xy_s = convert_coord(j-1,imp,/data,/to_normal)
      n_xy_e = convert_coord(j,im,/data,/to_normal)
      plots,n_xy_s(0),n_xy_s(1),/normal,psym=5,col=75,symsize=1.5
      plots,n_xy_e(0),n_xy_e(1),/normal,psym=5,col=75,symsize=1.5
      plots,[n_xy_s(0),n_xy_e(0)],[n_xy_s(1),n_xy_e(1)],col=75,/normal,linestyle=2

    endfor

    ;NOW, PLOT PHASE FIT
    LOADCT,0
    plot,findgen(1),findgen(1),xrange=[0,nlags],yrange=[-1.*!pi,!pi],$
          xstyle=1,ystyle=1,xticks=lastlag,yticks=6,position=[0.53,0.2,0.9,0.35],/nodata,/noerase,$
          title='Sim vel = '+strmid(strtrim(v_i,2),0,6)+' m/s Fitacf vel ='+strmid(strtrim(v_if,2),0,6)+$
          'm/s err = '+strmid(strtrim(abs(v_i-v_if),2),0,4)+'m/s',charsize=0.7,xtickname=replicate(' ',nlags+1)
    xyouts,0.95,0.335,'Phi',charsize=0.8,/normal,alignment=0.5
    xyouts,0.95,0.305,'Fit phi',charsize=0.8,/normal,alignment=0.5
    loadct,34
    usersym,cos(S),sin(S),/FILL
    plots,0.93,0.33,/normal,psym=8,col=0
    plots,[0.93,0.97],[0.33,0.33],col=0,/normal
        usersym,cos(S),sin(S)
    plots,0.93,0.3,/normal,psym=8,col=150
    plots,[0.93,0.97],[0.3,0.3],col=150,/normal,linestyle=2


    for j=1,nlags-1 do begin

        usersym,cos(S),sin(S),/FILL

        tau = j*mpinc
        re = lag0pwr*exp(-1.0*tau/t_i)*cos(tau*f_i)
        im = lag0pwr*exp(-1.0*tau/t_i)*sin(tau*f_i)

        tau = (j-1)*mpinc
        rep = lag0pwr*exp(-1.0*tau/t_i)*cos(tau*f_i)
        imp = lag0pwr*exp(-1.0*tau/t_i)*sin(tau*f_i)

        phi = atan(im,re)
        phip = atan(imp,rep)
        n_xy_s = convert_coord(j-1,phip,/data,/to_normal)
        n_xy_e = convert_coord(j,phi,/data,/to_normal)
        plots,n_xy_s(0),n_xy_s(1),/normal,psym=8,col=0
        plots,n_xy_e(0),n_xy_e(1),/normal,psym=8,col=0
        plots,[n_xy_s(0),n_xy_e(0)],[n_xy_s(1),n_xy_e(1)],col=0,/normal


        usersym,cos(S),sin(S)
        tau = j*mpinc
        re = lag0pwrf*exp(-1.0*tau/t_if)*cos(tau*f_if)
        im = lag0pwrf*exp(-1.0*tau/t_if)*sin(tau*f_if)

        tau = (j-1)*mpinc
        rep = lag0pwrf*exp(-1.0*tau/t_if)*cos(tau*f_if)
        imp = lag0pwrf*exp(-1.0*tau/t_if)*sin(tau*f_if)

        phi = atan(im,re)
        phip = atan(imp,rep)
        n_xy_s = convert_coord(j-1,phip,/data,/to_normal)
        n_xy_e = convert_coord(j,phi,/data,/to_normal)
        plots,n_xy_s(0),n_xy_s(1),/normal,psym=8,col=150,symsize=1.5
        plots,n_xy_e(0),n_xy_e(1),/normal,psym=8,col=150,symsize=1.5
        plots,[n_xy_s(0),n_xy_e(0)],[n_xy_s(1),n_xy_e(1)],col=150,/normal,linestyle=2
    endfor



    erase
  endfor
  loadct,0

;   n_arr = lonarr(11)
;   err = dblarr(11)
;   for i=0,nrang-1 do begin
;     err(fix(errors(i,0)/.1)) = err(round(errors(i,0)/.1)) + errors(i,1)
;     n_arr(fix(errors(i,0)/.1)) = n_arr(round(errors(i,0)/.1)) + 1
;   endfor
; ;   usersym,cos(S),sin(S),/FILL
; ;   plot,errors(*,0),errors(*,1),psym=8
; ;   !p.multi=[0,1,2]
; ;   plot,histogram(errors(*,1)/errors(*,0))
; ;   plot,histogram(errors(*,0),binsize=.1,max=1.,min=0.)
;   plot,findgen(1),findgen(1),xrange=[0,1],xticks=10,/nodata,/normal,yrange=[0,max(err(*)/n_arr(*))]
;   for i=0,10 do begin
;     n_xy_s = convert_coord(double(i)/10.,err(i)/n_arr(i),/data,/to_normal)
;     plots,n_xy_s(0),n_xy_s(1),psym=2,/normal
;   endfor

  print,'done'

  close,aj_unit
  free_lun,aj_unit

  ;close the postscript file
  device,/close


end