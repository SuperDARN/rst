; oldgrd.pro
; ==========
; Author: R.J.Barnes
; 
; Copyright (c) 2012 The Johns Hopkins University/Applied Physics Laboratory
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
;
; OldGridLoadInx          
; OldGridRead               
; OldGridWrite             
; OldGridSeek            
; OldGridOpen              
; OldGridClose

; Private Functions
; ----------------
;
; OldGridMakeGridInx         
; OldGridGetTime          
; OldGridPoint
;



; ---------------------------------------------------------------


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       OldGridRead
;
; PURPOSE:
;       Read a record from a grid file.
;       
;
; CALLING SEQUENCE:
;       status = OldGridRead(unit,prm,stvec,gvec)
;
;       This function reads a single record from the open file with
;       logical unit number, unit. 
;
;
;       The returned value is size in bytes of the record on success, 
;       or  -1 for failure
;
;-----------------------------------------------------------------
;

function OldGridRead,unit,prm,stvec,gvec

  block=strarr(3,18)
  block[0,0:17]=['st_id','chn','nvec','freq0','major_rev','minor_rev', $
                 'prog_id','noise_mean','noise_sd','gsct','v_min','v_max', $
                 'p_min','p_max','w_min','w_max','ve_min','ve_max']

  block[1,0:11]=['gmlong','gmlat','kvect','st_id','chn','grid_index', $
              'vlos','vlos_sd','pwr','pwr_sd','wdt','wdt_sd']
  block[2,0:7]=['gmlong','gmlat','kvect','st_id','chn','grid_index', $
              'vlos','vlos_sd']


  ON_IOERROR,iofail

  GridMakePrm,prm 
  GridMakeStVec,stvec
  GridMakeGVec,gvec

  syr=0
  smo=0
  sdy=0
  shr=0
  smt=0
  ssc=0
 
  eyr=0
  emo=0
  edy=0
  ehr=0
  emt=0
  esc=0

  point_lun,-unit,sptr
 
  readf,unit,syr,smo,sdy,shr,smt,ssc,eyr,emo,edy,ehr,emt,esc
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

  nblk=0
  nprm=0
  npnt=0

 readf,unit,nblk
  for blk=0,nblk-1 do begin
    readf,unit,npnt,nprm
    names=''
    units=''
    types=''
    readf,unit,names 
    readf,unit,units
    readf,unit,types
    str=strsplit(names,' ',/EXTRACT)
    x=strjoin(str)
    y=strjoin(transpose(block[*,*]))
    q=where(strcmp(x,y) ne 0,cnt)
    if cnt eq 0 then begin
       tmp=''
       for n=0,npnt-1 do readf,tmp
       continue
    endif
    case q[0] of

    0: BEGIN
        prm.stnum=npnt
        stvec=replicate(stvec,prm.stnum)
        for n=0,npnt-1 do begin
          readf,unit,stid,chn,nvec,freq,major_rev,minor_rev,prog_id, $
            noise_mean,noise_sd,gsct, $
            v_min,v_max,p_min,p_max,w_min,w_max,ve_min,ve_max
          stvec[n].st_id=stid
          stvec[n].chn=chn
          stvec[n].npnt=nvec
          stvec[n].freq=freq
          stvec[n].major_revision=major_rev
          stvec[n].minor_revision=minor_rev
          stvec[n].prog_id=prog_id
          stvec[n].noise.mean=noise_mean
          stvec[n].noise.sd=noise_sd
          stvec[n].gsct=gsct
          stvec[n].vel.min=v_min
          stvec[n].vel.max=v_max
          stvec[n].pwr.min=p_min
          stvec[n].pwr.max=p_max
          stvec[n].wdt.min=w_min
          stvec[n].wdt.max=w_max
          stvec[n].verr.min=ve_min
          stvec[n].verr.max=ve_max
        endfor

      END
      1: BEGIN
        prm.vcnum=npnt
        prm.xtd=1
        if prm.vcnum eq 0 then break
        gvec=replicate(gvec,prm.vcnum)
        for n=0,npnt-1 do begin
          readf,unit,gmlon,gmlat,kvect,stid,chn,index,vlos,vlos_sd, $
               pwr,pwr_sd,wdt,wdt_sd
          gvec[n].mlon=gmlon
          gvec[n].mlat=gmlat
          gvec[n].azm=kvect
          gvec[n].st_id=stid
          gvec[n].chn=chn
          gvec[n].index=index
          gvec[n].vel.median=vlos
          gvec[n].vel.sd=vlos_sd
          gvec[n].pwr.median=pwr
          gvec[n].pwr.sd=pwr_sd
          gvec[n].wdt.median=wdt
          gvec[n].wdt.sd=wdt_sd
        endfor

      END
      2: BEGIN
        prm.vcnum=npnt
        prm.xtd=0
        if prm.vcnum eq 0 then break
        gvec=replicate(gvec,prm.vcnum)
        for n=0,npnt-1 do begin
          readf,unit,gmlon,gmlat,kvect,stid,chn,index,vlos,vlos_sd
          gvec[n].mlon=gmlon
          gvec[n].mlat=gmlat
          gvec[n].azm=kvect
          gvec[n].st_id=stid
          gvec[n].chn=chn
          gvec[n].index=index
          gvec[n].vel.median=vlos
          gvec[n].vel.sd=vlos_sd
        endfor
      END
  endcase
  endfor

  point_lun,-unit,eptr

  return,eptr-sptr  

iofail:
  return,-1 
end

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       OldGridWrite
;
; PURPOSE:
;       Write a record to a grdmap file.
;       
;
; CALLING SEQUENCE:
;       status = OlcGridWrite(unit,prm,stvec,gvec)
;
;       This function writes a single record to the open file with
;       logical unit number, unit. 
;
;
;       The returned value is size in bytes of the record on success, 
;       or  -1 for failure
;
;-----------------------------------------------------------------
;


function OldGridWrite,unit,prm,stvec,gvec

ON_IOERROR,iofail

  point_lun,-unit,sptr

  printf,unit,prm.stme.yr,prm.stme.mo,prm.stme.dy,prm.stme.hr,prm.stme.mt,$
              fix(prm.stme.sc),prm.etme.yr,prm.etme.mo,prm.etme.dy,$
              prm.etme.hr,prm.etme.mt,fix(prm.etme.sc),$
              format='(1I5,5I3.2,1I5,5I3.2)'
  printf,unit,2,format='(1I0)'
  printf,unit,prm.stnum,18,format='(1I0," ",1I0)'
  printf,unit,'st_id','chn','nvec','freq0','major_rev','minor_rev', $
              'prog_id','noise_mean','noise_sd','gsct','v_min','v_max', $
              'p_min','p_max','w_min','w_max','ve_min','ve_max', $
              format='(18A15)'

  printf,unit,'count','count','count','KHz','count','count', $
              'count','count','count','count','m/sec','m/sec', $
              'dB','dB','m/sec','m/sec','m/sec','m/sec', $
               format='(18A15)'

  printf,unit,'int','int','int','float','int','int', $
              'int','float','float','int','float','float', $
              'float','float','float','float','float','float', $
               format='(18A15)'

  for n=0,prm.stnum-1 do begin
    printf,unit,stvec[n].st_id,stvec[n].chn,stvec[n].npnt,stvec[n].freq, $
      stvec[n].major_revision,stvec[n].minor_revision, $
      stvec[n].prog_id,float(stvec[n].noise.mean),float(stvec[n].noise.sd), $
      stvec[n].gsct,float(stvec[n].vel.min),float(stvec[n].vel.max), $
      float(stvec[n].pwr.min),float(stvec[n].pwr.max),$
      float(stvec[n].wdt.min), $
      float(stvec[n].wdt.max),float(stvec[n].verr.min), $
      float(stvec[n].verr.max), $
      format='(3I15,1G15,3I15,2G15,1I15,8G15)' 

  endfor

  if prm.xtd eq 1 then begin
    printf,unit,prm.vcnum,12,format='(1I0," ",1I0)'
    printf,unit,'gmlong','gmlat','kvect','st_id','chn','grid_index', $
              'vlos','vlos_sd','pwr','pwr_sd','wdt','wdt_sd', $
              format='(12A15)'
    printf,unit,'degrees','degrees','degrees','count','count','count', $
              'm/sec','m/sec','dB','dB','m/sec','m/sec', $
              format='(12A15)'
    printf,unit,'float','float','float','int','int','int', $
              'float','float','float','float','float','float', $
              format='(12A15)'

    for n=0,prm.vcnum-1 do begin
      printf,unit,float(gvec[n].mlon),float(gvec[n].mlat),float(gvec[n].azm), $
        gvec[n].st_id,gvec[n].chn, $
        gvec[n].index,float(gvec[n].vel.median),float(gvec[n].vel.sd), $
        float(gvec[n].pwr.median),float(gvec[n].pwr.sd),$
        float(gvec[n].wdt.median), $
        float(gvec[n].wdt.sd),format='(3G15,3I15,6G15)'
    endfor

  endif else begin
    printf,unit,prm.vcnum,12,format='(1I0," ",1I0)'
    printf,unit,'gmlong','gmlat','kvect','st_id','chn','grid_index', $
              'vlos','vlos_sd', $
              format='(8A15)'
    printf,unit,'degrees','degrees','degrees','count','count','count', $
              'm/sec','m/sec', $
              format='(8A15)'
    printf,unit,'float','float','float','int','int','int', $
              'float','float', $
              format='(8A15)'
    for n=0,prm.vcnum-1 do begin
      printf,unit,gvec[n].mlon,gvec[n].mlat,gvec[n].azm, $
        gvec[n].st_id,gvec[n].chn, $
        gvec[n].index,gvec[n].vel.median,gvec[n].vel.sd, $
        format='(3G15,3I15,2G15)'
     endfor

 endelse


  point_lun,-unit,eptr

  return,eptr-sptr  

iofail:
  return,-1 
end


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       OldGridMakeGridInx
;
; PURPOSE:
;       Create a structure to store grdmap index.
;
;
; CALLING SEQUENCE:
;       OldGridMakeGridInx,inx
;
;       This procedure creates a structure to store the grid index.
;
;
;-----------------------------------------------------------------
;

pro OldGridMakeGridIndex,inx,st_time,ed_time,offset
  inx={OldGridInx, st_time: st_time, ed_time: ed_time, offset: offset}
end

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       OldGridLoadInx
;
; PURPOSE:
;       Read an index to a grdmap file.
;
;
; CALLING SEQUENCE:
;       status = OldGridLoadInx(unit,inx)
;
;       This function reads an index to a grid file.
;
;       The returned value is size in bytes of the record on success,
;       or  -1 for failure
;
;-----------------------------------------------------------------
;

function OldGridLoadInx,unit,inx

ON_IOERROR,iofail
   
  syr=0
  smo=0
  sdy=0
  shr=0
  smt=0
  ssc=0
 
  eyr=0
  emo=0
  edy=0
  ehr=0
  emt=0
  esc=0
  offset=0L
  s=0
  c=0L

  st_time=0.0D
  ed_time=0.0D

  while (s eq 0) do begin
    readf,unit,syr,smo,sdy,shr,smt,ssc,eyr,emo,edy,ehr,emt,esc,offset
    st_time=TimeYMDHMSToEpoch(syr,smo,sdy,shr,smt,ssc)
    ed_time=TimeYMDHMSToEpoch(eyr,emo,edy,ehr,emt,esc)
    OldGridMakeGridIndex,tinx,st_time,ed_time,offset
    if (n_elements(inx) eq 0) then inx=tinx $
    else inx=[inx,tinx] 
  endwhile
 
  return, 0

iofail:
  return,0 


end


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       GridGetTime
;
; PURPOSE:
;       Read a record from a grid file.
;       
;
; CALLING SEQUENCE:
;       status = GridGetTime(unit,prm,st_time,ed_time)
;
;       This function reads a single record from the open file with
;       logical unit number, unit. 
;
;
;       The returned value is size in bytes of the record on success, 
;       or  -1 for failure
;
;-----------------------------------------------------------------
;

function OldGridGetTime,unit,st_time,ed_time

  ON_IOERROR,iofail

  syr=0
  smo=0
  sdy=0
  shr=0
  smt=0
  ssc=0
 
  eyr=0
  emo=0
  edy=0
  ehr=0
  emt=0
  esc=0

  point_lun,-unit,sptr
 
  readf,unit,syr,smo,sdy,shr,smt,ssc,eyr,emo,edy,ehr,emt,esc
  st_time=TimeYMDHMSToEpoch(syr,smo,sdy,shr,smt,ssc)
  ed_time=TimeYMDHMSToEpoch(eyr,emo,edy,ehr,emt,esc)

  nblk=0
  nprm=0
  npnt=0

  readf,unit,nblk
  tmp=''
  for n=0,nblk-1 do begin 
    readf,unit,npnt,nprm
    for x=0,npnt+2 do begin
      readf,unit,tmp  
    endfor
  endfor
 
  point_lun,-unit,eptr
  return,eptr-sptr  

iofail:
  return,-1 
end

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       GridSeek
;
; PURPOSE:
;       Find a record in a grdmap file.
;
;
; CALLING SEQUENCE:
;       status = GridSeek(unit,yr,mo,dy,hr,mt,sc,inx,atme=atime)
;
;       This function searchs for a record in a grdmap file.
;
;       The returned value is zero on success,
;       or  -1 for failure
;
;-----------------------------------------------------------------
;


function OldGridSeek,unit,yr,mo,dy,hr,mt,sc,inx,atme=atme

  tval=TimeYMDHMSToEpoch(yr,mo,dy,hr,mt,sc);

  if (n_elements(inx) ne 0) then begin
   
    rec=0L
    prec=-1L
    num=n_elements(inx)
    stime=inx[0].st_time
    etime=inx[num-1].ed_time
   
    srec=0L
    erec=num; 
    if (tval lt stime) then begin
      atme=stime
      point_lun,unit,inx[srec].offset
      return, 0
    endif else if (tval gt etime) then begin
      atme=stime
      point_lun,unit,inx[erec-1].offset
      return, -1
    endif 

   repeat begin
      prec=rec;
      rec=srec+fix(((tval-stime)*(erec-srec))/(etime-stime))
      if (inx[rec].st_time eq tval) then break
      if (inx[rec].st_time lt tval) then begin
         srec=rec
         stime=inx[rec].st_time
      endif else begin
         erec=rec
         etime=inx[rec].ed_time
      endelse
    endrep until (prec eq rec)
    atme=inx[rec].st_time
    point_lun,unit,inx[rec].offset
    return,0
  endif else begin
     st_time=0.0D
     ed_time=0.0D
     s=OldGridGetTime(unit,st_time,ed_time)
     if (s ne -1) then begin
       if (st_time gt tval) then point_lun,unit,0 $
       else begin
          stat=fstat(unit)
          fptr=stat.cur_ptr
      endelse
     endif else point_lun,unit,0
     atme=st_time
     repeat begin
       stat=fstat(unit)
       s=OldGridGetTime(unit,st_time,ed_time)
       if (s eq -1) then break
       if (tval ge st_time) then begin
         fptr=stat.cur_ptr
         atme=st_time
       endif
     endrep until (tval lt st_time)
     if (tval gt st_time) then return,-1
     point_lun,unit,fptr
    endelse
  return,0 
end






;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       OldGridPoint
;
; PURPOSE:
;       Sets the file pointer position in a grdmap file.
;
;
; CALLING SEQUENCE:
;       status = OldGridPoint(unit,ptr)
;
;       This function sets the file pointer in a oldgrdmap file.
;
;
;       The returned value is the offset on success,
;       or  -1 for failure
;
;-----------------------------------------------------------------
;

function OldGridPoint,unit,off

ON_IOERROR,iofail

  point_lun,unit,off
  return,off

iofail:
  return, -1

end


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       OldGridOpen
;
; PURPOSE:
;       Open a grid file.
;
;
; CALLING SEQUENCE:
;       unit = OldGridOpen(fname,/read,/write,/update)
;
;       This function opens a grid file.
;
;       The returned value is the logical unit number on success,
;       or  -1 for failure
;
;
;       This function is provided as a convenience and opens
;       the grid file using the appropriate flags to to open.
;
;-----------------------------------------------------------------
;

function OldGridOpen,fname,read=read,write=write,update=update
  if KEYWORD_SET(read) then openr,unit,fname,/GET_LUN,/SWAP_IF_BIG_ENDIAN
  if KEYWORD_SET(write) then openw,unit,fname,/GET_LUN, /SWAP_IF_BIG_ENDIAN
  if KEYWORD_SET(update) then openu,unit,fname,/GET_LUN, /SWAP_IF_BIG_ENDIAN
  return, unit
end


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       OldGridClose
;
; PURPOSE:
;       Close a grd file.
;
;
; CALLING SEQUENCE:
;       s = OldGridClose(unit)
;
;       This function closes a grd file.
;
;       The returned value is zero on success,
;       or  -1 for failure
;
;-----------------------------------------------------------------
;

function OldGridClose,unit

  free_lun,unit
  return,0

end




