; maplib.pro
; ===========
; Author: R.J.Barnes
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
; map_openr
; map_openw
; map_close
; map_read
; map_write
;
;
; ---------------------------------------------------------------



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;	map_openr
;
; PURPOSE:
;	Open a map file for input
;
; CALLING SEQUENCE:
;
;	  fptr = map_openr(filename)
;
;	arguments:
;	  filename contains the name of the file. The file
;	  pointer,(unit number) is returned.
;
;------------------------------------------------------------------------------
;



function map_openr, fname
   return,OldCnvMapOpen(fname,/read)
end

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;	map_openw
;
; PURPOSE:
;	Open a map file for output
;
; CALLING SEQUENCE:
;
;	  fptr = map_openw(filename)
;
;	arguments:
;	  filename contains the name of the file. The file
;	  pointer,(unit number) is returned.
;
;------------------------------------------------------------------------------
;




function map_openw, fname
  return,OldCnvMapOpen(fname,/write)
end


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;	map_close
;
; PURPOSE:
;	Closes an open map file
;
; CALLING SEQUENCE:
;
;	  fptr = map_close(fptr)
;
;	arguments:
;	  fptr is the file pointer (unit number).
;
;------------------------------------------------------------------------------
;

function map_close, fileptr
  free_lun,fileptr
end


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;	read_map
;
; PURPOSE:
;	Reads a record from a map file
;
; CALLING SEQUENCE:
;
;	s=read_map(fileptr, syr,smo,sdy,shr,smt,ssc, 
;                   eyr,emo,edy,ehr,emt,esc,
;                   source,major_rev,minor_rev,
;                   nste, ndat, xtd,ste_st_id,ste_nvec,ste_freq0, 
;                   ste_major,ste_minor, 
;                   ste_progid,ste_noise_mean,ste_noise_sd, 
;                   ste_limit, 
;                   dat_mlat,dat_mlon,dat_azm,dat_st_id, 
;                   dat_index,data, 
;                   mlt_t1,mlt_t2,mlt_av,B, 
;                   lon_shft,lat_shft,hemisphere,order,latmin, 
;                   error_wt, model_wt, 
;                   doping,imf_flag,imf_delay,imf_BT_model,imf_Bz_model, 
;                   modnp,mpos,mvmag,mvazm, 
;                   bndnp,bnd_lon,bnd_lat, 
;                   nsol,solution, 
;                   chi_sqr,chi_sqr_dat,rms_err, 
;                   limits,ste_chn,dat_chn)

;
;	arguments:
;	  fptr is the file pointer (unit number).
;
;------------------------------------------------------------------------------
;


function read_map, fileptr, syr,smo,sdy,shr,smt,ssc, $
                   eyr,emo,edy,ehr,emt,esc, $
                   source,major_rev,minor_rev, $
                   nste, ndat, xtd,ste_st_id,ste_nvec,ste_freq0, $
                   ste_major,ste_minor, $
                   ste_progid,ste_noise_mean,ste_noise_sd, $
                   ste_limit, $
                   dat_mlat,dat_mlon,dat_azm,dat_st_id, $
                   dat_index,data, $
                   mlt_t1,mlt_t2,mlt_av,B, $
                   lon_shft,lat_shft,hemisphere,order,latmin, $
                   error_wt, model_wt, $
                   doping,imf_flag,imf_delay,imf_BT_model,imf_Bz_model, $
                   modnp,mpos,mvmag,mvazm, $
                   bndnp,bnd_lon,bnd_lat, $
                   nsol,solution, $
                   chi_sqr,chi_sqr_dat,rms_err, $
                   limits,ste_chn,dat_chn
                                           
  s=OldCnvMapRead(fileptr,prm,stvec,gvec,mvec,coef,bvec)

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
  
  modnp=long(prm.modnum)
  nsol=long(prm.coefnum)
  bndnp=long(prm.bndnum)

  nste=long(prm.stnum)
  ndat=long(prm.vcnum)
  xtd=prm.xtd

  source=prm.source
  major_rev=prm.major_rev
  minor_rev=prm.minor_rev
   
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
   
   if (modnp ne 0) then begin
     mvlat=fltarr(modnp)
     mvlon=fltarr(modnp)
     mvazm=fltarr(modnp)
     mvmag=fltarr(modnp) 
     mvlat[*]=float(mvec[*].mlat)
     mvlon[*]=float(mvec[*].mlon)
     mvazm[*]=float(mvec[*].azm)
     mvmag[*]=float(mvec[*].vel.median)

     mpos=fltarr(2,modnp)
     mpos(1,*)=mvlon
     mpos(0,*)=mvlat

   endif

   if (bndnp ne 0) then begin
     bnd_lon=fltarr(bndnp)
     bnd_lat=fltarr(bndnp) 
     bnd_lat[*]=float(bvec[*].lat)
     bnd_lon[*]=float(bvec[*].lon)
   endif

   if (nsol ne 0) then begin
     solution=fltarr(4,nsol)
     solution=transpose(coef)
 endif


  B=fltarr(3)
  B[0]=prm.Bx
  B[1]=prm.By
  B[2]=prm.By

  mlt_t1=prm.mlt.st
  mlt_t2=prm.mlt.ed
  mlt_av=prm.mlt.av
  lon_shft=prm.lon_shft
  lat_shft=prm.lat_shft
  latmin=prm.latmin
  
  hemisphere=prm.hemisphere
  order=prm.fit_order
  error_wt=prm.error_wt
  model_wt=prm.model_wt
           
  doping=prm.doping_level
  imf_flag=prm.imf_flag
  imf_delay=prm.imf_delay
             
  chi_sqr=prm.chi_sqr
  chi_sqr_dat=prm.chi_sqr_dat
  rms_err=prm.rms_err

  imf_BT_model=prm.imf_model[1]
  imf_Bz_model=prm.imf_model[0] 

  limits=fltarr(6)
  limits[0]=prm.pot_drop
  limits[1]=prm.pot_drop_err
  limits[2]=prm.pot_max
  limits[3]=prm.pot_max_err
  limits[0]=prm.pot_min
  limits[1]=prm.pot_min_err
 

   return, s
end


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;	write_map
;
; PURPOSE:
;	writes a record to an open map file
;
; CALLING SEQUENCE:
;
;
;      s=write_map(fileptr, syr,smo,sdy,shr,smt,ssc, 
;                     eyr,emo,edy,ehr,emt,esc, 
;                     source,major_rev,minor_rev, 
;                     nste, ndat, xtd,ste_st_id,ste_nvec,ste_freq0, 
;                     ste_major,ste_minor, 
;                     ste_progid,ste_noise_mean,ste_noise_sd, 
;                     ste_limit, 
;                     dat_mlat,dat_mlon,dat_azm,dat_st_id, 
;                     dat_index,data, 
;                     mlt_t1,mlt_t2,mlt_av,B, 
;                     lon_shft,lat_shft,hemisphere,order,latmin, 
;                     error_wt, model_wt, 
;                     doping,imf_flag,imf_delay,imf_BT_model,imf_Bz_model, 
;                     modnp,mpos,mvmag,mvazm, 
;                     bndnp,bnd_lon,bnd_lat, 
;                     solution, 
;                     chi_sqr,chi_sqr_dat,rms_err, 
;                     limits,ste_chn,dat_chn)
;
;	arguments:
;	  fptr is the file pointer (unit number).
;
;------------------------------------------------------------------------------
;


function write_map, fileptr, syr,smo,sdy,shr,smt,ssc, $
                     eyr,emo,edy,ehr,emt,esc, $
                     source,major_rev,minor_rev, $
                     nste, ndat, xtd,ste_st_id,ste_nvec,ste_freq0, $
                     ste_major,ste_minor, $
                     ste_progid,ste_noise_mean,ste_noise_sd, $
                     ste_limit, $
                     dat_mlat,dat_mlon,dat_azm,dat_st_id, $
                     dat_index,data, $
                     mlt_t1,mlt_t2,mlt_av,B, $
                     lon_shft,lat_shft,hemisphere,order,latmin, $
                     error_wt, model_wt, $
                     doping,imf_flag,imf_delay,imf_BT_model,imf_Bz_model, $
                     modnp,mpos,mvmag,mvazm, $
                     bndnp,bnd_lon,bnd_lat, $
                     solution, $
                     chi_sqr,chi_sqr_dat,rms_err, $
                     limits,ste_chn,dat_chn 

   if nste eq 0 then return, -1
   CnvMapMakePrm, prm
   GridMakeStVec,stvec
   GridMakeGVec,gvec
   GridMakeGVec,mvec
   CnvMapMakeBnd,bvec

   if n_params() gt 54 then nsol=n_elements(solution[2,*])
   stvec=replicate(stvec,nste)
   print, nsol
   if (ndat ne 0) then gvec=replicate(gvec,ndat)
   if (modnp ne 0) then mvec=replicate(mvec,modnp)
   if (nsol ne 0) then coef=dblarr(nsol,4)
   if (bndnp ne 0) then bvec=replicate(bvec,bndnp)
 
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

   prm.modnum=modnp
   prm.coefnum=nsol
   prm.bndnum=bndnp
  
   prm.stnum=nste
   prm.vcnum=ndat
   prm.xtd=xtd


  prm.source=source
  prm.major_rev=major_rev
  prm.minor_rev=minor_rev

   
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

   if (modnp ne 0) then begin
     mvlat=fltarr(modnp)
     mvlon=fltarr(modnp)
     mvlat[*]=mpos[0,*]
     mvlon[*]=mpos[1,*]
     mvec[*].mlat=mvlat
     mvec[*].mlon=mvlon
     mvec[*].azm=mvazm[*]
     mvec[*].vel.median=mvmag[*]
   endif

   if (bndnp ne 0) then begin
     bvec[0:bndnp-1].lat=bnd_lat[0:bndnp-1]
     bvec[0:bndnp-1].lon=bnd_lon[0:bndnp-1]
   endif

   if (nsol ne 0) then coef=transpose(solution)

   prm.Bx=b[0]
   prm.By=b[1]
   prm.Bz=b[2]


   prm.mlt.st=mlt_t1
   prm.mlt.ed=mlt_t2
   prm.mlt.av=mlt_av   
   prm.lon_shft=lon_shft
   prm.lat_shft=lat_shft
   prm.latmin=latmin
  
  prm.hemisphere=hemisphere
  prm.fit_order=order
  prm.error_wt=error_wt
  prm.model_wt=model_wt
           
  prm.doping_level=doping
  prm.imf_flag=imf_flag
  prm.imf_delay=imf_delay
             
  prm.chi_sqr=chi_sqr
  prm.chi_sqr_dat=chi_sqr_dat
  prm.rms_err=rms_err

  prm.imf_model[1]=imf_BT_model
  prm.imf_model[0]=imf_Bz_model

  prm.pot_drop=limits[0]
  prm.pot_drop_err=limits[1]
  prm.pot_max=limits[2]
  prm.pot_max_err=limits[3]
  prm.pot_min=limits[4]
  prm.pot_min_err=limits[5]


   s=OldCnvMapWrite(fileptr,prm,stvec,gvec,mvec,coef,bvec)
   return, s


end
