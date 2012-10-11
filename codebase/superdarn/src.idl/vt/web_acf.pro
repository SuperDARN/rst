PRO web_acf,date,hr,mn,rad,beam,nmod,time


  date=strtrim(date,2)
  yr = strmid(date,0,4)
  hr=strtrim(hr,2)
  mn=strtrim(mn,2)
  rad=strtrim(rad,2)
  beam=strtrim(beam,2)
  nmod=strtrim(nmod,2)
  time=strtrim(time,2)

  case rad of
    "rkn": rad_old="i"
    "inv": rad_old="i"
    "ksr": rad_old="c"
    "kod": rad_old="a"
    "pgr": rad_old="b"
    "sas": rad_old="t"
    "kap": rad_old="k"
    "gbr": rad_old="g"
    "sto": rad_old="w"
    "pyk": rad_old="e"
    "han": rad_old="f"
    "wal": rad_old="i"
    "hok": rad_old="i"
    "bks": rad_old="i"
    "hal": rad_old="h"
    "san": rad_old="d"
    "sys": rad_old="j"
    "sye": rad_old="n"
    "tig": rad_old="r"
    "ker": rad_old="p"
    "unw": rad_old="u"
    "fhe": rad_old="x"
    "fhw": rad_old="z"
  endcase

  hrstr = fix(fix(hr)/2)*2
  if(hrstr lt 10) then hstr = '0'+strtrim(hrstr,2)+'??' $
  else hstr = strtrim(hrstr,2)+'??'
  print,hstr

  filedir = '/sd-data/'+yr+'/rawacf/'+rad+'/'+date+'.'+hstr+'*'
  filename= '/data/fit/aj/acfex/temp_files/'

  print,time


  if((fix(strmid(date,0,4)) lt 2006) OR $
      ((fix(strmid(date,0,4)) eq 2006)AND(fix(strmid(date,4,2)) lt 07))) then begin
    spawn,"/data/fit/aj/acfex/./acfex -web -hr "+hr+" -min "+mn+" -beam "+beam+" -nslopes "+nmod +" /data/raw/"+date+"."+rad_old+".dat"
  endif else begin
    spawn,'cp '+filedir+' '+filename
    print,'cp '+filedir+' '+filename
    print,'bunzip2 '+ filename+date+'.'+hstr+'*'
    spawn,'bunzip2 '+ filename+date+'.'+hstr+'*'
;     spawn,'gunzip -f '+ filename+'.out'
;     print,'gunzip -f '+ filename+'.out'
    ;spawn,"bunzip2 /data/raw/"+date+"."+rad+".rawacf.bz2"
    print,"/rst/bin/test_fitacf -new -hr "+hr+" -min "+mn+" -beam "+beam+' '+filename+date+'.'+hstr+'*'+' > /rst/output_files/'+time+'.fitacf.test'
    spawn,"/rst/bin/test_fitacf -new -hr "+hr+" -min "+mn+" -beam "+beam+' '+filename+date+'.'+hstr+'*'+' > /rst/output_files/'+time+'.fitacf.test'
; 		spawn,"/rst/bin/test_fitacf -new -hr "+hr+" -min "+mn+" -beam "+beam+' /data/fit/aj/sim_test.rawacf > /rst/output_files/'+time+'.fitacf.test'
    ;spawn,"bzip2 /data/raw/"+date+"."+rad+".rawacf"
  endelse

  plot_fitacf,time


END