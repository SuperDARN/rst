; gridlib.pro
; ===========
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
;
; PublicFunctions
; ---------------
; grid_openr
; grid_openw
; grid_close
; grid_read
; grid_write
;
;
; ---------------------------------------------------------------



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;	grid_openr
;
; PURPOSE:
;	Open a grid file for input
;
; CALLING SEQUENCE:
;
;	  fptr = grid_openr(filename)
;
;	arguments:
;	  filename contains the name of the file. The file
;	  pointer,(unit number) is returned.
;
;------------------------------------------------------------------------------
;



function grid_openr, fname
   return,OldGridOpen(fname,/read)
end

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;	grid_openw
;
; PURPOSE:
;	Open a grid file for output
;
; CALLING SEQUENCE:
;
;	  fptr = grid_openw(filename)
;
;	arguments:
;	  filename contains the name of the file. The file
;	  pointer,(unit number) is returned.
;
;------------------------------------------------------------------------------
;




function grid_openw, fname
  return,OldGridOpen(fname,/write)
end


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;	grid_close
;
; PURPOSE:
;	Closes an open grid file
;
; CALLING SEQUENCE:
;
;	  fptr = grid_close(fptr)
;
;	arguments:
;	  fptr is the file pointer (unit number).
;
;------------------------------------------------------------------------------
;

function grid_close, fileptr
  free_lun,fileptr
end


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;	read_grid
;
; PURPOSE:
;	Reads a record from a grid file
;
; CALLING SEQUENCE:
;
;	s=read_grid(fileptr, 
;                   syr,smo,sdy,shr,smt,ssc,eyr,emo,edy,ehr,emt,esc, 
;                   nste, ndat, xtd, ste_st_id, 
;                   ste_nvec,ste_freq0, 
;                   ste_major,ste_minor, 
;                   ste_progid,ste_noise_mean,ste_noise_sd, 
;                   ste_limit, 
;                   dat_mlat,dat_mlon,dat_azm,dat_st_id, 
;                   dat_index,data,ste_chn,dat_chn) 
;
;	arguments:
;	  fptr is the file pointer (unit number).
;
;------------------------------------------------------------------------------
;




function read_grid, fileptr, syr,smo,sdy,shr,smt,ssc,eyr,emo,edy,ehr,emt,esc, $
                             nste, ndat, xtd, ste_st_id, $
                             ste_nvec,ste_freq0, $
                             ste_major,ste_minor, $
                             ste_progid,ste_noise_mean,ste_noise_sd, $
                             ste_limit, $
                             dat_mlat,dat_mlon,dat_azm,dat_st_id, $
                             dat_index,data,ste_chn,dat_chn
                            
               
  s=OldGridRead(fileptr,prm,stvec,gvec)

  if s eq -1 then return, s 
       
  syr=prm.stme.yr
  smo=prm.stme.mo
  sdy=prm.stme.dy
  shr=prm.stme.hr
  smt=prm.stme.mt
  ssc=prm.stme.sc
  
  eyr=prm.etme.yr
  emo=prm.etme.mo
  edy=prm.etme.dy
  ehr=prm.etme.hr
  emt=prm.etme.mt
  esc=prm.etme.sc
  
  nste=long(prm.stnum)
  ndat=long(prm.vcnum)
  xtd=prm.xtd

  ste_st_id=lonarr(nste)
  ste_chn=lonarr(nste)
  ste_nvec=lonarr(nste)
  ste_freq0=fltarr(nste)
  ste_major=lonarr(nste)
  ste_minor=lonarr(nste)
  ste_progid=lonarr(nste)
  ste_noise_mean=fltarr(nste)
  ste_noise_sd=fltarr(nste)
  ste_limit=fltarr(9,nste)

  ste_st_id[*]=long(stvec[*].st_id)
  ste_chn[*]=long(stvec[*].chn)
  ste_nvec[*]=long(stvec[*].npnt)
  ste_freq0[*]=long(stvec[*].freq)
  ste_major[*]=long(stvec[*].major_revision)
  ste_minor[*]=long(stvec[*].minor_revision)
  ste_progid[*]=long(stvec[*].prog_id)
  ste_noise_mean[*]=float(stvec[*].noise.mean)
  ste_noise_sd[*]=float(stvec[*].noise.sd)
  ste_limit[0,*]=float(stvec[*].vel.min)
  ste_limit[1,*]=float(stvec[*].vel.max)
  ste_limit[2,*]=float(stvec[*].pwr.min)
  ste_limit[3,*]=float(stvec[*].pwr.max)
  ste_limit[4,*]=float(stvec[*].wdt.min)
  ste_limit[5,*]=float(stvec[*].wdt.max)
  ste_limit[6,*]=float(stvec[*].verr.min)
  ste_limit[7,*]=float(stvec[*].verr.max)

  if (ndat ne 0) then begin
     dat_mlat=fltarr(ndat);
     dat_mlon=fltarr(ndat);
     dat_azm=fltarr(ndat);
     dat_st_id=lonarr(ndat); 
     dat_chn=lonarr(ndat); 
     dat_index=lonarr(ndat);
     data=fltarr(6,ndat);

     dat_mlat[*]=float(gvec[*].mlat)
     dat_mlon[*]=float(gvec[*].mlon)
     dat_azm[*]=float(gvec[*].azm)
     dat_st_id[*]=long(gvec[*].st_id)
    dat_chn[*]=long(gvec[*].chn)

     dat_index[*]=long(gvec[*].index)
     data[0,*]=float(gvec[*].vel.median)
     data[1,*]=float(gvec[*].vel.sd)
     data[2,*]=float(gvec[*].pwr.median)
     data[3,*]=float(gvec[*].pwr.sd)
     data[4,*]=float(gvec[*].wdt.median)
     data[5,*]=float(gvec[*].wdt.sd)
   endif
       
   return, s
end


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;	write_grid
;
; PURPOSE:
;	writes a record to an open grid file
;
; CALLING SEQUENCE:
;
;	s=write_grid(fileptr, syr,smo,sdy,shr,smt,ssc,
;                    eyr,emo,edy,ehr,emt,esc, 
;                    nste, ndat, xtd,ste_st_id,ste_nvec,ste_freq0, 
;                    ste_major,ste_minor, 
;                    ste_progid,ste_noise_mean,ste_noise_sd, 
;                    ste_limit, 
;                    dat_mlat,dat_mlon,dat_azm,dat_st_id, 
;                    dat_index,data)
;
;
;	arguments:
;	  fptr is the file pointer (unit number).
;
;------------------------------------------------------------------------------
;


function write_grid, fileptr, syr,smo,sdy,shr,smt,ssc, $
                     eyr,emo,edy,ehr,emt,esc, $
                     nste, ndat, xtd,ste_st_id,ste_nvec,ste_freq0, $
                     ste_major,ste_minor, $
                     ste_progid,ste_noise_mean,ste_noise_sd, $
                     ste_limit, $
                     dat_mlat,dat_mlon,dat_azm,dat_st_id, $
                     dat_index,data,ste_chn,dat_chn


   if nste eq 0 then return, -1
   GridMakePrm, prm
   GridMakeStVec,stvec
   GridMakeGVec,gvec
   stvec=replicate(stvec,nste)
   if (ndat ne 0) then gvec=replicate(gvec,ndat)

   prm.stme.yr=syr
   prm.stme.mo=smo
   prm.stme.dy=sdy
   prm.stme.hr=shr
   prm.stme.mt=smt
   prm.stme.sc=ssc
  
   prm.etme.yr=eyr
   prm.etme.mo=emo
   prm.etme.dy=edy
   prm.etme.hr=ehr
   prm.etme.mt=emt
   prm.etme.sc=esc
  
   prm.stnum=nste
   prm.vcnum=ndat
   prm.xtd=xtd

   
  stvec[*].st_id=ste_st_id[*]
  if n_elements(ste_chn) ne 0 then stvec[*].chn=ste_chn[*]
  stvec[*].npnt=ste_nvec[*]
  stvec[*].freq=ste_freq0[*]
  stvec[*].major_revision=ste_major[*]
  stvec[*].minor_revision=ste_minor[*]
  stvec[*].prog_id=ste_progid[*]
  stvec[*].noise.mean=ste_noise_mean[*]
  stvec[*].noise.sd=ste_noise_sd[*]
  stvec[*].vel.min=transpose(ste_limit[0,*])
  stvec[*].vel.max=transpose(ste_limit[1,*])
  stvec[*].pwr.min=transpose(ste_limit[2,*])
  stvec[*].pwr.max=transpose(ste_limit[3,*])
  stvec[*].wdt.min=transpose(ste_limit[4,*])
  stvec[*].wdt.max=transpose(ste_limit[5,*])
  stvec[*].verr.min=transpose(ste_limit[6,*])
  stvec[*].verr.max=transpose(ste_limit[7,*])

  if (ndat ne 0) then begin
     gvec[*].mlat=dat_mlat[*]
     gvec[*].mlon=dat_mlon[*]
     gvec[*].azm=dat_azm[*]
     gvec[*].st_id=dat_st_id[*]
     if n_elements(dat_chn) ne 0 then gvec[*].chn=dat_chn[*]
     gvec[*].index=dat_index[*]
     gvec[*].vel.median=transpose(data[0,*])
     gvec[*].vel.sd=transpose(data[1,*])
     gvec[*].pwr.median=transpose(data[2,*])
     gvec[*].pwr.sd=transpose(data[3,*])
     gvec[*].wdt.median=transpose(data[4,*])
     gvec[*].wdt.sd=transpose(data[5,*])
   endif

   s=OldGridWrite(fileptr,prm,stvec,gvec)
   return, s


end
