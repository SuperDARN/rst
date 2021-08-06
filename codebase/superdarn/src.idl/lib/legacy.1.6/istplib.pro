; istplib.pro
; ============
; Author: K.Baker, S.Shepherd & R.J.Barnes
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
; istp_tm_hr
;
; ---------------------------------------------------------------


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       istp_tm_hr
;
; PURPOSE:
;
;       Convert CDF Epoch to decimal hour of day.
;       
;----------------------------------------------------------------------------
;

function istp_tm_hr,epoch

  cdf_epoch,epoch(0),yr,month,day,/breakdown_epoch
  cdf_epoch,t0,yr,month,day,/compute_epoch
  dt = epoch - t0
  hr = dt/3600000.
  return,hr
end

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       istp_file_name
;
; PURPOSE:
;
;       Construct a filename for an ISTP data file.
;       
;----------------------------------------------------------------------------
;



function istp_file_name,dir,sat,inst,date,vno

  datelen=strlen(date)
  if (datelen eq 6) then begin
    yy = strmid(date, 0, 2)
    if (yy GT 80) then yy = 1900+yy else yy = 2000 + yy
    new_date = strcompress((string(yy) + strmid(date,2,4)),/remove_all)
  endif $
  else begin
    if (datelen eq 8) then new_date = date else begin
      print,"Invalid date string: ",date
      print,"USE:  yyyymmdd"
      return,""
      endelse
      yy=strmid(date,0,4);
  endelse


  ; correct to use the HTR ace data

   case sat of
     "ac": level="_k1_"
     else: level="_k0_"
   endcase

  return, dir + strcompress(string(yy),/remove_all) + "/" + sat + level + inst + "_" + new_date + "_v0" + $
     strcompress(string(vno),/remove_all)+ ".cdf"

end

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       decompose_istp_file_name
;
; PURPOSE:
;
;       Break an ISTP filename down 
;       
;----------------------------------------------------------------------------
;



pro decompose_istp_file_name,fname,dir,sat,inst,datestr,vno

  pos = 0

; locate the last '/'

  while (pos GE 0) do begin
    pos_last = pos
    pos = strpos(fname,'/',pos_last+1)
  endwhile
  if (pos_last EQ 0) then dir = '' else dir = strmid(fname,0,pos_last)
  
  if (pos_last EQ 0) then current = 0 else current = pos_last + 1
  sat = strmid(fname,current,2)

  ; skip over the '_k0_'
  
  current = current+2 + strlen('_k0_')
  pos = strpos(fname,'_',current)
  if (pos EQ -1) then begin
    print,"%%decompose_file_name:  invalid name = ",fname
    return
  endif

  inst = strmid(fname,current,pos-current)
  current = pos+1
  pos = strpos(fname,'_',current)
  if (pos EQ -1) then begin

 ; if there is no underscore it means the file name is given
; without a version number.  Just return the datestring and skip
; the version number.
;
    datestr = strmid(fname,current,99)
    return
  endif

  datestr = strmid(fname,current,pos-current)
  current = pos+1
  pos = strpos(fname,'.',current)
  if (pos EQ -1) then vno = strmid(fname,current,99) else $
    vno = strmid(fname,current,pos-current)
  return
end

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       open_istp_cdf
;
; PURPOSE:
;
;       Open an ISTP CDF file.
;       
;----------------------------------------------------------------------------
;





function open_istp_cdf,fname,sat=sat,dir=dir,inst=inst,datestr=datestr

  flag =0
  dir_top = getenv("SD_DATA")
  dir_top = "/share/"


  if (n_params() EQ 0) then begin
    if (keyword_set(sat)) then sat=sat else sat="wi"
    if (keyword_set(inst)) then inst=inst else inst="mfi"
    if (keyword_set(dir)) then dir=dir else begin
      case sat of
        "ac": dir=dir_top+"ace/"
        "wi": dir=dir_top+"wind/"
        "dn": dir=dir_top+"pace/"
        "i8": dir=dir_top+"imp8/"
        "ge": dir=dir_top+"geotail/"
       ;"ge": dir=dir_top+"goes/"
        "sn": dir=dir_top+"sonde/"
        else: dir=dir_top
      endcase
    endelse
  echar = strmid(dir,strlen(dir)-1,1)
  if echar NE '/' then dir=dir+'/'

  if (keyword_set(datestr)) then date1 = datestr else begin
    flag = 0
    print,"enter a date string (e.g. 960421) for the file you want"
    print,"or hit the RETURN key to bring up the Pickfile menu"
    date1 = ' '
    read,date1
    if (strlen(date1) Eq 0) then begin
      ;fname = dialog_pickfile(filter="*.cdf", path=dir)
      flag = 1
    endif
  endelse
  endif else flag=1

;
; initialize the version number of the file to 1 
;
; If the variable "flag" is 0 it means that we have to
; generate a file name from a date string
;

  vno = 0
  if (flag EQ 0) then begin
;
; Now try to open the filename we've generated
;
    oflag = 0
    err = 1
    get_lun,unit
    while (err ne 0 AND oflag eq 0 and vno le 9) do begin
      vno = vno + 1
      fname = istp_file_name(dir,sat,inst,date1,vno)
      print, fname
      openr,unit,fname,error=err
      free_lun,unit
      if (err eq 0) then begin
        oflag = 1
;      print,"Successfully opened file ",fname
      endif else begin
;        print,"failed to open ",fname,"  error = ",err
      endelse
    endwhile
    if (oflag EQ 0) then begin
      print,"CDF file not found"
      return,0
    endif
;
; OK, we've been able to open a file, but is it really the most
; recent version?
;
    while (err eq 0) do begin
      vno = vno + 1
      fname = istp_file_name(dir,sat,inst,date1,vno)
      openr,unit,fname,error=err
      free_lun,unit
;    print,"tried to open ",fname," error condition = ",err
    endwhile
;
    vno = vno -1
    fname = istp_file_name(dir,sat,inst,date1,vno)
    if (vno eq 0) then begin
      print,"Unable to open the cdf file"
      return,0
    endif
  endif
;
; OK, now we can really open the cdf file
;
  id = 0
  print,"opening cdf file ",fname
  id = cdf_open(fname)
  return,id
end


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       read_istp_cdf
;
; PURPOSE:
;
;       Read data from an ISTP CDF file
;       
;----------------------------------------------------------------------------
;


function read_istp_cdf,id,varname

  if (n_params() EQ 0) then begin
    print,"READ_ISTP_CDF: use result=read_istp_cdf,cdf_id,varname"
    return,0
  endif
  if (n_params() EQ 1) then begin
    print,"READ_ISTP_CDF: reading epoch"
    var = "Epoch"
    endif $
  else var = varname


; we first have to look at the file and find out how long it is


  info = cdf_inquire(id)
  imax = info.maxrec
  zflag = 0


  if (imax lt 0) then begin
;  
;  the number of records is negative, which means we've hit the
;  idl bug in dealing with these files.  So we have to work 
;  around it.
;
    cdf_control,id,variable=0,/zvariable,get_var_info=info
    imax = info.maxrec
  endif
;
; Now get the information on the variables
;

  var_info = cdf_varinq(id,var)
  cdf_varget,id,var,x,rec_count = imax, zvariable=var_info.is_zvar
;
; OK, we've got the data
;
  return,reform(x)
end
 
pro istp_cdf_help
  print,"id = OPEN_ISTP_CDF([filename],[sat=sat],[inst=inst],[datestr=datestr])"
  print,"'sat' must be a valid 2-letter designation for a satellite or"
  print,"ground-based instrument, such as 'wi' for wind, 'se' for sesame, etc."
  print
  print,"data = read_istp_cdf(id, varname)"
  print,"id must be the id number of an already opened cdf file"
  print,"varname must be the full variable name of a variable within the file"
  print
  print,"hr = istp_tm_hr(epoch)"
  print,"epoch must be a vector containing the epoch variable from a cdf"
  print
  print,"name = istp_file_name(dir,sat,inst,date,vno)"
  print,"dir = directory path, sat = satellite designator, inst=instrument"
  print,"designator, date = date (yyyymmdd), and vno is the version number (1-9)"
  print,"data = concat_wind_imf(d1,d2)"
  print,"d1 = structure containing wind imf data"
  print
return
end

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       istp_cdf
;
; PURPOSE:
;
;       ISTP Interface tool
;       
;----------------------------------------------------------------------------
;

function istp_cdf

  print,"This procedure is designed to help you work with data"
  print,"contained in CDF files from the ISTP/GGS mission."
  print
  print,"You can call the individual routines yourself, or you can"
  print,"use the pop-up menu to navigate through the routines."
  print,"Click on the HELP button to get a printout of the names"
  print,"and options of the various routines."
  print
  select = -1
  while select NE 4 do begin
    select = wmenu(['HELP','Open cdf file','Read cdf file',$
                'convert ISTP Epoch to hour','EXIT'])
    case select of
      0: istp_cdf_help
      1: begin
          print,"Select the satellite/ground-based"
          inst = ""
          datestr = ""
          satnum=wmenu(['cn','dn','ge','i8','po','wi','se','sn'])
          case satnum of
            0: sat='cn'
            1: sat='dn'
            2: sat='ge'
            3: sat='i8'
            4: sat='po'
            5: sat='wi'
            6: sat='se'
            7: sat='sn'
          endcase
          print,"Enter an instrument designation (e.g.'mfi')"
          read,inst
          print,"Enter a date string in the form: yyyymmdd"
          read,datestr
          id = open_istp_cdf(sat=sat,inst=inst,datestr=datestr)
        end
      2: begin
          if (id EQ 0) then begin
            print,"You haven't opened a file yet"
          endif else begin
            print,"Enter the name of the variable to be read"
            print,"If you don't know the names of the variables"
            print,"Exit from this routine and execute 'cdf_browse'"
            print,"The variable 'Epoch' will be read automatically"
            print,"When you exit, the variable you selected and Epoch"
            print,"will be returned in a structure"
            varname=""
            read,varname
            Epoch = read_istp_cdf(id,"Epoch")
            var = read_istp_cdf(id,varname)
            x = {Epoch: Epoch, var: var}
          endelse
        end
      3: begin
        hr = istp_tm_hr(x.Epoch)
          x = {Epoch: Epoch, hr: hr, var: var}
          print,"The hour of the day will be added to the returned structure"
        end
      4: return,x
    endcase
  endwhile
end


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       istp_cnv_gse_gsm 
;
; PURPOSE:
;
;       Convert from GSE to GSM
;       
;----------------------------------------------------------------------------
;

pro istp_cnv_gse_gsm,epoch,gse,gsm,stat

  dhrs=istp_tm_hr(epoch)		; array of decimal hours
  hrs= fix(dhrs)			; integer hours
  dmin=(dhrs-hrs)*60.		; decimal minutes
  minu=fix(dmin)			; integer minutes
  sec=fix((dmin-minu)*60.)	; integer seconds

  nelems=n_elements(epoch)
  for i=0, nelems-1 do begin
    cdf_epoch,epoch(i),yr,mn,dy,/breakdown_epoch
    yr=fix(yr)
    mn=fix(mn)
    dy=fix(dy)
    tgse=double(gse(*,i))
    tgsm=double(gsm(*,i))

    status=GEOPGseGsm(yr,mn,dy,hrs(i),minu(i),sec(i),tgse,tgsm)

    if status eq 0L then begin
      if nelems eq 1 then gsm=tgsm $
      else gsm(*,i)=tgsm
    endif else $
      print, 'ERROR returned by cnv_gse_gsm_idl'
  endfor

  stat=status

  return
end


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       istp_cnv_gse_gsm 
;
; PURPOSE:
;
;       Convert from GSM to GSE
;       
;----------------------------------------------------------------------------
;

pro istp_cnv_gsm_gse,epoch,gsm,gse,stat

  dhrs=istp_tm_hr(epoch)		; array of decimal hours
  hrs= fix(dhrs)			; integer hours
  dmin=(dhrs-hrs)*60.		; decimal minutes
  minu=fix(dmin)			; integer minutes
  sec=fix((dmin-minu)*60.)	; integer seconds

  nelems=n_elements(epoch)
  for i=0, nelems-1 do begin
    cdf_epoch,epoch(i),yr,mn,dy,/breakdown_epoch
    yr=fix(yr)
    mn=fix(mn)
    dy=fix(dy)
    tgsm=double(gsm(*,i))
    tgse=double(gse(*,i))
		
   
    status=GEOPGsmGse(yr,mn,dy,hrs(i),minu(i),sec(i),tgse,tgsm)
			
    if status eq 0L then begin
      if nelems eq 1 then gse=tgse $
      else gse(*,i)=tgse
    endif else $
      print, 'ERROR returned by cnv_gsm_gse_idl'
  endfor

  stat=status
  return

end






