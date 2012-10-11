pro	rad_fit_calc_velocity, force=force

common rad_data_blk
common rt_data_blk

; Find data index
data_index = rad_fit_get_data_index()
nrecs = (*rad_fit_info[data_index]).nrecs


; Find run date and time
caldat, (*rad_fit_info[data_index]).sjul, mm1, dd1, yy1, hh1, mn1
caldat, (*rad_fit_info[data_index]).fjul, mm2, dd2, yy2, hh2, mn2
date = yy1*10000L + mm1*100L + dd1
time = [hh1*100L + mn1, hh2*100L + mn2]


; run ray tracing
rt_run, date, (*rad_fit_info[data_index]).code, time=[0000,2400], force=force,$
				freq=(*rad_fit_data[data_index]).tfreq[fix(nrecs/4)]/1000.,outdir='/data/rt'

stop

; Apply correction to velocity (mask)
njuls = n_elements((*rad_fit_data[data_index]).juls)
novelinds = where((*rad_fit_data[data_index]).velocity eq 10000.)
for it=0,n_elements(rt_data.juls[*,0])-2 do begin
	for ib=0,n_elements(rt_data.beam[0,*])-1 do begin
		for j=0,rt_info.ngates-2 do begin
			julinds = where((*rad_fit_data[data_index]).juls ge rt_data.juls[it,0] and $
							(*rad_fit_data[data_index]).juls lt rt_data.juls[it+1,0] and $
							(*rad_fit_data[data_index]).beam eq rt_data.beam[it,ib], cc)
			; If there is a correction to apply, do it
			if cc gt 0 and rt_data.nr[it,ib,j] gt 0 then begin
				(*rad_fit_data[data_index]).velocity[julinds + j*njuls] = (*rad_fit_data[data_index]).velocity[julinds + j*njuls] * 1./rt_data.nr[it,ib,j]
			endif
		endfor
	endfor
endfor
; Enforce no velocity value (the correction overwrote this, which is why it needs to be enforced here)
(*rad_fit_data[data_index]).velocity[novelinds] = 10000.

openw,unit,'/data/corr/'+strtrim(date(0),2)+'.'+(*rad_fit_info[data_index]).code+'.corr',/get_lun

for i=0L,(*rad_fit_info[data_index]).nrecs-1 do begin
	caldat, (*rad_fit_data[data_index]).juls[i],mo,dy,yr,hr,mn,sc
	printf,unit,yr,mo,dy,hr,mn,round(sc),(*rad_fit_data[data_index]).beam[i],(*rad_fit_info[data_index]).ngates
	for j=0,(*rad_fit_info[data_index]).ngates-1 do begin
 		printf,unit,j,(*rad_fit_data[data_index]).velocity[i,j]
	endfor
endfor

close,unit
free_lun,unit



end