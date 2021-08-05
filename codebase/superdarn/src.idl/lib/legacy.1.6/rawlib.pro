; rawlib.pro
; ==========
; Author: K.Baker & R.J.Barnes
; 
;  Copyright (c) 2012 The Johns Hopkins University/Applied Physics Laboratory
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
; along with this program. If not, see <https://www.gnu.org/licenses/>.
; 
; Modifications:
;
; Public Functions
; ----------------
; rawropen
; raw_close
; pack_raw
; read_raw
; find_raw_rec
; rawdef
;
; ---------------------------------------------------------------



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;	rawropen
;
; PURPOSE:
;	Open a raw radar data file
;
; Calling Sequence:
;	rawfile = rawropen(filename)
;
; ARGUMENTS:
;	filename contains a string giving the file name.  It is not 
;	necessary to specify either the directory or the extension if
;	the file is a standard raw data file in the path specified by
;	the environment variable SD_RAWROPEN_PATH.
;	
;	If the file is a Data General/Compressed format file (.cmp), you
;	do have to specify the full name of the file, but not the extension.
;
;	If the file does not reside in the SD_RAWROPEN_PATH you must specify
;	the full path of the file.
;
;
; Return Value:
;	A pointer to the RAW_FILE structure
;
; NOTE:
;	If there is an associated raw index file (.rin), then both the
;	raw data file and the index file will be opened.  
;-------------------------------------------------------------------------
;


function rawropen, fname

  common rawdata_com, rawfileptr, raw_data, badlag, $
                      more_badrange, lags, rd_byte

  common rawfp_com, rfp
  if (n_params() EQ 0) then name = dialog_pickfile() else name = fname

  splitpath,fname,path,name,ext

  rawpath=getenv('SD_RAWROPEN_PATH')
  if (strlen(rawpath) ne 0) then begin
    search=strsplit(rawpath,PATH_SEP(/SEARCH_PATH),/EXTRACT)
    if (strlen(path) ne 0) then search=[search,path]
  endif else search=path

  if (strlen(ext) eq 0) then ext='dat'
  for n=0,n_elements(search)-1 do begin
    rawname=search[n]+PATH_SEP()+name+'.'+ext
    print, rawname
    rawfp=OldRawOpen(rawname)
    if (n_tags(rawfp) gt 0) then break
  endfor


  if (n_tags(rawfp) gt 0) then begin
    if (n_elements(rfp) eq 0) then begin
       rfp=replicate(rawfp,32)
       rawfileptr=1
     endif else begin
       rfp[rawfileptr]=rawfp
       rawfileptr++
    endelse
  endif else return, -1 
  return, rawfileptr


end

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;	raw_close
;
; PURPOSE:
;	Close the raw (.dat) and raw index (.rin) files
;
; Calling Sequence:
;	status = raw_close(fileptr)
;
; Arguments:
;	the file pointer is optional.  If no file pointer is specified,
;	the file pointer in the common block rawdata_com will be used.
;
;-----------------------------------------------------------------------
;

function raw_close,fileptr

  common rawdata_com, rawfileptr, raw_data, badlag, $
                      more_badrange, lags, rd_byte

  common rawfp_com, rfp
  if (n_params() GE 1) then rawfp = rfp[fileptr-1] $
  else rawfp = rfp[rawfileptr-1]
  
  s=OldRawClose(rawfp)
  return, s
end

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;	pack_raw
;
; PURPOSE:
;	This packs the raw data structure into a byte array that can
;	be transferred back to C routines thru the call_external
;	mechanism.
;
;-----------------------------------------------------------------------

function pack_raw, rd
  common rawdata_com, rawfileptr, raw_data, badlag, $
         more_badrange, lags, rd_byte
 
  if n_params() GE 1 then raw_data = rd

  SIZEOF_RAWDATA = 58300
  PULSE_PAT_LEN = 16
  LAG_TAB_LEN = 48
  MAX_RANGE = 75
  COMBF_SIZE = 80
  
  rd_byte(0:SIZEOF_RAWDATA-1) = 0
  
  offset = 0L

  rd_byte(offset) = raw_data.p.rev.major
  offset = offset +1
  rd_byte(offset) = raw_data.p.rev.minor
  offset = offset + 1
  rd_byte(offset:offset+1) = byte(raw_data.p.nparm,0,2)
  offset = offset + 2
  rd_byte(offset:offset+1) = byte(raw_data.p.st_id,0,2)
  offset = offset + 2
  rd_byte(offset:offset+1) = byte(raw_data.p.year,0,2)
  offset = offset + 2
  rd_byte(offset:offset+1) = byte(raw_data.p.month,0,2)
  offset = offset + 2
  rd_byte(offset:offset+1) = byte(raw_data.p.day,0,2)
  offset = offset + 2
  rd_byte(offset:offset+1) = byte(raw_data.p.hour,0,2)
  offset = offset + 2
  rd_byte(offset:offset+1) = byte(raw_data.p.minut,0,2)
  offset = offset + 2
  rd_byte(offset:offset+1) = byte(raw_data.p.sec,0,2)
  offset = offset + 2
  rd_byte(offset:offset+1) = byte(raw_data.p.txpow,0,2)
  offset = offset + 2
  rd_byte(offset:offset+1) = byte(raw_data.p.nave,0,2)
  offset = offset + 2
  rd_byte(offset:offset+1) = byte(raw_data.p.atten,0,2)
  offset = offset + 2
  rd_byte(offset:offset+1) = byte(raw_data.p.lagfr,0,2)
  offset = offset + 2
  rd_byte(offset:offset+1) = byte(raw_data.p.smsep,0,2)
  offset = offset + 2
  rd_byte(offset:offset+1) = byte(raw_data.p.ercod,0,2)
  offset = offset + 2
  rd_byte(offset:offset+1) = byte(raw_data.p.agc_stat,0,2)
  offset = offset + 2
  rd_byte(offset:offset+1) = byte(raw_data.p.lopwr_stat,0,2)
  offset = offset + 2
  rd_byte(offset:offset+1) = byte(raw_data.p.radops_sys_resS,0,2)
  offset = offset + 2
  rd_byte(offset:offset+3) = byte(raw_data.p.noise,0,4)
  offset = offset + 4
  rd_byte(offset:offset+3) = byte(raw_data.p.radops_sys_resL(0),0,4)
  offset = offset + 4
  rd_byte(offset:offset+3) = byte(raw_data.p.radops_sys_resL(1),0,4)
  offset = offset + 4
  rd_byte(offset:offset+1) = byte(raw_data.p.intt,0,2)
  offset = offset + 2
  rd_byte(offset:offset+1) = byte(raw_data.p.txpl,0,2)
  offset = offset + 2
  rd_byte(offset:offset+1) = byte(raw_data.p.mpinc,0,2)
  offset = offset + 2
  rd_byte(offset:offset+1) = byte(raw_data.p.mppul,0,2)
  offset = offset + 2
  rd_byte(offset:offset+1) = byte(raw_data.p.mplgs,0,2)
  offset = offset + 2
  rd_byte(offset:offset+1) = byte(raw_data.p.nrang,0,2)
  offset = offset + 2
  rd_byte(offset:offset+1) = byte(raw_data.p.frang,0,2)
  offset = offset + 2
  rd_byte(offset:offset+1) = byte(raw_data.p.rsep,0,2)
  offset = offset + 2
  rd_byte(offset:offset+1) = byte(raw_data.p.bmnum,0,2)
  offset = offset + 2
  rd_byte(offset:offset+1) = byte(raw_data.p.xcf,0,2)
  offset = offset + 2
  rd_byte(offset:offset+1) = byte(raw_data.p.tfreq,0,2)
  offset = offset + 2
  rd_byte(offset:offset+1) = byte(raw_data.p.scan,0,2)
  offset = offset + 2
  rd_byte(offset:offset+3) = byte(raw_data.p.mxpwr,0,4)
  offset = offset + 4
  rd_byte(offset:offset+3) = byte(raw_data.p.lvmax,0,4)
  offset = offset + 4
  rd_byte(offset:offset+3) = byte(raw_data.p.usr_resL1,0,4)
  offset = offset + 4
  rd_byte(offset:offset+3) = byte(raw_data.p.usr_resL2,0,4)
  offset = offset + 4
  rd_byte(offset:offset+1) = byte(raw_data.p.cp,0,2)
  offset = offset + 2
  rd_byte(offset:offset+1) = byte(raw_data.p.usr_resS1,0,2)
  offset = offset + 2
  rd_byte(offset:offset+1) = byte(raw_data.p.usr_resS2,0,2)
  offset = offset + 2
  rd_byte(offset:offset+1) = byte(raw_data.p.usr_resS3,0,2)
  offset = offset + 2

;
; now pack the pulse table and the lag table
;
  for i=0, pulse_pat_len - 1 do begin
    rd_byte(offset:offset+1) = byte(raw_data.pulse_pattern(i),0,2)
    offset = offset + 2
  endfor

  for j=0, 1 do $
   for i=0, lag_tab_len - 1 do begin
    rd_byte(offset:offset+1) = byte(raw_data.lag_table(j,i),0,2)
    offset = offset + 2
  endfor
;
; pack in the comment buffer
;
  for i=0, combf_size - 1 do begin
    rd_byte(offset) = raw_data.combf(i)
    offset = offset + 1
  endfor
;

; now pack in the lag 0 powers
;
  for i=0,max_range-1 do begin
    rd_byte(offset:offset+3) = byte(raw_data.pwr0(i),0,4) 
    offset = offset + 4
  endfor
;
; now pack the ACFs
;
for i=0,max_range-1 do $
  for j = 0, lag_tab_len-1 do $
    for k=0,1 do begin
      rd_byte(offset:offset+3) = byte(raw_data.acfd(k,j,i),0,4)
      offset = offset + 4
  endfor
;
; now pack the XCFs
;

for i=0,max_range-1 do $
  for j = 0, lag_tab_len-1 do $
    for k=0,1 do begin
      rd_byte(offset:offset+3) = byte(raw_data.xcfd(k,j,i),0,4)
      offset = offset + 4
  endfor

;
; OK, were all done packing up the data
;
  return, rd_byte
end

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;	read_raw
;
; PURPOSE:
;	Read a single record from a raw data file
;
; Calling Sequence:
;	status = read_raw([fp], [fdata])
;
; ARGUMENTS:
;	both arguments are optional.  If neither is specified, the file
;	pointed to by rawfileptr in the common block rawdata_com will be
;	used as the input file.  The data will be place in raw_data in the 
;	same common block.
;
;	fp  is  a rawfile pointer specifying which file to read
;
;	fdata  is a raw data strucutre where the data is to be stored
;
;------------------------------------------------------------------------
;

function read_raw, fp, rdata, raw_offset
  common rawdata_com, rawfileptr, raw_data, badlag, more_badrange, $
                      lags, rd_byte

  common rawfp_com, rfp

  if n_params() GE 1 then ifileptr = fp else ifileptr = rawfileptr
  if (n_params() GE 3) then rawoff = long(raw_offset) else rawoff = 0L
  rawdef,rd

  if (rawoff lt 0) then begin
    s=OldRawPoint(rfp[ifileptr-1],rfp[ifileptr-1].frec)      
  endif else if (rawoff gt 0) then begin
    s=OldRawPoint(rfp[ifileptr-1],rawoff)
  endif 

  rawfp=rfp[ifileptr-1]  

  status=OldRawRead(rawfp,prm,raw)
  rfp[ifileptr-1]=rawfp
  if (status eq -1) then return, status

  pulse_pat_len = 16
  lag_tab_len = 48
  combf_size = 80
  max_range = 75

  rd.p.rev.major=prm.revision.major
  rd.p.rev.minor=prm.revision.minor
  rd.p.nparm=48
  rd.p.st_id=prm.stid
  rd.p.year=prm.time.yr
  rd.p.month=prm.time.mo
  rd.p.day=prm.time.dy
  rd.p.hour=prm.time.hr
  rd.p.minut=prm.time.mt
  rd.p.sec=prm.time.sc 
  rd.p.txpow=prm.txpow
  rd.p.nave=prm.nave
  rd.p.atten=prm.atten 
  rd.p.lagfr=prm.lagfr
  rd.p.smsep=prm.smsep
  rd.p.ercod=prm.ercod
  rd.p.agc_stat=prm.stat.agc
  rd.p.lopwr_stat=prm.stat.lopwr 
  rd.p.nbaud=0
  rd.p.noise=prm.noise.search
  rd.p.noise_mean =prm.noise.mean
  rd.p.rxrise=prm.rxrise
  rd.p.intt=prm.intt.sc
  rd.p.txpl=prm.txpl
  rd.p.mpinc=prm.mpinc
  rd.p.mppul=prm.mppul
  rd.p.mplgs=prm.mplgs
  rd.p.nrang=prm.nrang
  rd.p.frang=prm.frang
  rd.p.rsep=prm.rsep
  rd.p.bmnum=prm.bmnum
  rd.p.xcf=prm.xcf
  rd.p.tfreq=prm.tfreq
  rd.p.scan=prm.scan
  rd.p.mxpwr=prm.mxpwr
  rd.p.lvmax=prm.lvmax
  rd.p.usr_resL1=0
  rd.p.usr_resL2=0
  rd.p.cp=prm.cp
  rd.p.usr_resS1=0
  rd.p.usr_resS2=0
  rd.p.usr_resS3=0

  rd.pulse_pattern[0:pulse_pat_len-1]=prm.pulse[0:pulse_pat_len-1]
  rd.lag_table[0,0:lag_tab_len-1]=prm.lag[0:lag_tab_len-1,0]
  rd.lag_table[1,0:lag_tab_len-1]=prm.lag[0:lag_tab_len-1,1]

  l=strlen(prm.combf)
  if (l gt combf_size) then l=combf_size
  tmp=byte(prm.combf)
  rd.combf[0:l-1]=tmp[0:l-1]

  rd.pwr0[0:max_range-1]=raw.pwr0[0:max_range-1]
  
  for n=0,max_range-1 do begin
    rd.acfd[0,0:lag_tab_len-1,n]=raw.acfd[n,0:lag_tab_len-1,0]
    rd.acfd[1,0:lag_tab_len-1,n]=raw.acfd[n,0:lag_tab_len-1,1]
    rd.xcfd[0,0:lag_tab_len-1,n]=raw.acfd[n,0:lag_tab_len-1,0]
    rd.xcfd[1,0:lag_tab_len-1,n]=raw.acfd[n,0:lag_tab_len-1,1]
  endfor

  if (n_params() GE 2) then rdata = rd else raw_data = rd

end

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;	find_raw_rec
;
; PURPOSE:
;	find a record in a raw data file according to time
;
; Calling Sequence:
;	status = find_raw_rec(t_req, [t_found], [fileptr])
;	
; ARGUMENTS:
;	t_req is the time in seconds from the beginning of the year of
;		the data to be located.
;	t_found is optional and will contain the actual time found.
;
;	fileptr is optional and specifies the raw data file to be used.
;		if fileptr is not specified, the file pointed to by
;		rawfileptr in the common block rawdata_com will be used
;
;-----------------------------------------------------------------------
;

function find_raw_rec, sttim, time_found, fileptr
  common rawdata_com, rawfileptr, raw_data, badlag, $
         more_badrange, lags, rd_byte

  common rawfp_com, rfp

  if n_params() EQ 0 then begin
    print,'USE status=find_raw_rec(st_time, [time_found], [fp])'
    return,0
  endif

  if n_params() EQ 3 then fp = fileptr else fp = rawfileptr
  if n_params() GE 2 then time_found = sttim
  if (sttim GT 6) then ttemp = long(sttim) - 6 else ttemp = long(sttim)

  atme=0.0D
  s=TimeEpochToYMDHMS(yr,mo,dy,hr,mt,sc,rfp[fp-1].stime)
  s=TimeYrsecToYMDHMS(yr,mo,dy,hr,mt,sc,ttemp)
  rawfp=rfp[fp-1]
  s=OldRawSeek(rawfp,yr,mo,dy,hr,mt,sc,atme=atme)         

  rfp[fp-1]=rawfp
  offset=rfp[fp-1].ptr
  
  if (offset GT 0) then status = read_raw(fp, raw_data, offset)
     
  if n_params() GE 2 then time_found = TimeYMDHMSToYrsec(raw_data.p.year, $
    raw_data.p.month, raw_data.p.day, raw_data.p.hour, $
    raw_data.p.minut, raw_data.p.sec)

  return, status

end

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       rawdef
;
; PURPOSE:
;
;	define a variable to be of type 'rawdata_str'
;
;	This procedure is used to define a variable for the storage of 
;	  one raw data record.  Normally, this routine does not need to
;	  be used, since the raw data is usually read into a pre-defined
;	  variable, "raw_data" in the common block "rawdata_com"
;
;	NOTE:  the raw data variable is a structure
;
;-----------------------------------------------------------------------------
;	

pro rawdef, r
  PULSE_PAT_LEN = 16
  LAG_TAB_LEN = 48
  COMBF_SIZE = 80
  MAX_RANGE = 75
  r = {oldrawdata, $
	p: {parms, rev: {rev_no, major: byte(0), minor: byte(0)}, $
		nparm: 0, $
		st_id: 0, $
		year: 0, $
		month: 0, $
		day: 0, $
		hour: 0, $
		minut: 0, $
		sec: 0, $
		txpow: 0, $
		nave: 0, $
		atten: 0, $
		lagfr: 0, $
		smsep: 0, $
		ercod: 0, $
		agc_stat: 0, $
		lopwr_stat: 0, $
		nbaud: 0, $
		noise: long(0), $
		noise_mean: long(0), $
	        channel: 0, $
	        rxrise: 0, $
		intt: 0, $
		txpl: 0, $
		mpinc: 0, $
		mppul: 0, $
		mplgs: 0, $
		nrang: 0, $
		frang: 0, $
		rsep: 0, $
		bmnum: 0, $
		xcf: 0, $
		tfreq: 0, $
		scan: 0, $
		mxpwr: long(0), $
		lvmax: long(0), $
		usr_resL1: long(0), $
		usr_resL2: long(0), $
		cp: 0, $
		usr_resS1: 0, $
		usr_resS2: 0, $
		usr_resS3: 0}, $
	pulse_pattern: intarr(PULSE_PAT_LEN), $
	lag_table: intarr(2, LAG_TAB_LEN), $
	combf: bytarr(COMBF_SIZE), $
	pwr0: lonarr(MAX_RANGE), $
	acfd: lonarr(2, LAG_TAB_LEN, MAX_RANGE), $
	xcfd: lonarr(2, LAG_TAB_LEN, MAX_RANGE) $
	}
return
end
