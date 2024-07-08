; grd.pro
; =======
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
; GridLoadInx          
; GridRead               
; GridWrite             
; GridSeek            
; GridOpen              
; GridClose

; Private Functions
; ---------------
;   
;
; GridMakePrm  
; GridMakeStVec  
; GridMakeGVec
; GridMakeGridInx         
; GridGetTime          
; GridPoint
;
; ---------------------------------------------------------------



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       GridMakePrm
;
; PURPOSE:
;       Create a structure to store grdmap parameter data.
;       
;
; CALLING SEQUENCE:
;       GridMakePrm,prm
;
;       This procedure creates a structure to store the grid 
;       parameters
;
;
;-----------------------------------------------------------------
;

pro GridMakePrm,prm

  prm={GridPrm, stme: {GridTime,yr:0,mo:0,dy:0,hr:0,mt:0,sc:0D}, $
                  etme: {GridTime,yr:0,mo:0,dy:0,hr:0,mt:0,sc:0D}, $
                  stnum: 0L, $
                  vcnum: 0L, $
                  xtd:0 }
end




;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       GridMakeStVec
;
; PURPOSE:
;       Create a structure to store grdmap data.
;       
;
; CALLING SEQUENCE:
;       GridMakeStVec,stvec
;
;       This procedure creates a structure to store the station vector
;
;
;-----------------------------------------------------------------
;

pro GridMakeStVec,stvec

  stvec={GridSVec,st_id:0, $
                  chn:0, $
                  npnt:0, $
                  freq:0.0, $
                  major_revision:0, $
                  minor_revision:0, $
                  prog_id: 0, $
                  gsct: 0, $
                  noise: {GridNoise,mean:0.0,sd:0.0}, $ 
                  vel: {GridLimit,min:0.0,max:0.0}, $
                  pwr: {GridLimit,min:0.0,max:0.0}, $
                  wdt: {GridLimit,min:0.0,max:0.0}, $
                  verr: {GridLimit,min:0.0,max:0.0} $

         }
end


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       GridMakeGVec
;
; PURPOSE:
;       Create a structure to store grdmap data.
;       
;
; CALLING SEQUENCE:
;       GridMakeStVec,stvec
;
;       This procedure creates a structure to store the data vector
;
;
;-----------------------------------------------------------------
;


pro GridMakeGVec,gvec


   gvec={GridGVec, mlat:0.0, $
                   mlon:0.0, $
                   azm: 0.0, $
                   srng: 0.0, $
                   vel: {GridValue,median:0.0,sd:0.0}, $
                   pwr: {GridValue,median:0.0,sd:0.0}, $
                   wdt: {GridValue,median:0.0,sd:0.0}, $
                   st_id: 0, $
                   chn: 0, $
                   index: 0L $
        }
end





;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       GridRead
;
; PURPOSE:
;       Read a record from a grdmap file.
;       
;
; CALLING SEQUENCE:
;       status = GridRead(unit,prm,stvec,gvec)
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

function GridRead,unit,prm,stvec,gvec

  s=DataMapRead(unit,sclvec,arrvec)

  if (s eq -1) then return,s

  GridMakePrm,prm 
  GridMakeStVec,stvec
  GridMakeGVec,gvec

  sclname=['start.year','start.month','start.day', $
          'start.hour','start.minute','start.second', $ 
          'end.year','end.month','end.day', $
          'end.hour','end.minute','end.second']

  scltype=[2,2,2,2,2,8,2,2,2,2,2,8]

  sclid=intarr(n_elements(sclname))
  sclid[*]=-1

  arrname=['stid','channel','nvec', $
           'freq','major.revision','minor.revision', $
           'program.id','noise.mean','noise.sd','gsct', $
           'v.min','v.max','p.min','p.max','w.min','w.max','ve.min', $
           've.max', $
           'vector.mlat','vector.mlon','vector.kvect','vector.srng', $
           'vector.stid','vector.channel','vector.index', $
           'vector.vel.median','vector.vel.sd', $
           'vector.pwr.median','vector.pwr.sd', $
           'vector.wdt.median','vector.wdt.sd']

  arrtype=[2,2,2,4,2,2,2,4,4,2,4,4,4,4,4,4,4,4,4,4,4,4,2,2,3,4,4,4,4,4,4]

  arrid=intarr(n_elements(arrname))  
  arrid[*]=-1
   
  if (n_elements(sclvec) ne 0) then begin
    for n=0,n_elements(sclname)-1 do $
      sclid[n]=DataMapFindScalar(sclname[n],scltype[n],sclvec)
  endif
  

  if (n_elements(arrvec) ne 0) then begin
    for n=0,n_elements(arrname)-1 do $
      arrid[n]=DataMapFindArray(arrname[n],arrtype[n],arrvec)
  endif

  q=where(sclid eq -1,count)
  if (count ne 0) then begin
    print,'File is in the wrong format!'
    return, -2
  endif

  q=where(arrid[0:17] eq -1,count)
  if (count ne 0) then begin
    print,'File is in the wrong format!'
    return, -2  
  endif

  ; populate the structures

  prm.stme.yr=*(sclvec[sclid[0]].ptr)
  prm.stme.mo=*(sclvec[sclid[1]].ptr)
  prm.stme.dy=*(sclvec[sclid[2]].ptr)
  prm.stme.hr=*(sclvec[sclid[3]].ptr)
  prm.stme.mt=*(sclvec[sclid[4]].ptr)
  prm.stme.sc=*(sclvec[sclid[5]].ptr)

  prm.etme.yr=*(sclvec[sclid[6]].ptr)
  prm.etme.mo=*(sclvec[sclid[7]].ptr)
  prm.etme.dy=*(sclvec[sclid[8]].ptr)
  prm.etme.hr=*(sclvec[sclid[9]].ptr)
  prm.etme.mt=*(sclvec[sclid[10]].ptr)
  prm.etme.sc=*(sclvec[sclid[11]].ptr)
 

  prm.stnum=N_ELEMENTS(*(arrvec[arrid[0]].ptr))
  stvec=replicate(stvec,prm.stnum)

  stvec[*].st_id=(*(arrvec[arrid[0]].ptr))[*]
  stvec[*].chn=(*(arrvec[arrid[1]].ptr))[*]
  stvec[*].npnt=(*(arrvec[arrid[2]].ptr))[*]
  stvec[*].freq=(*(arrvec[arrid[3]].ptr))[*]
  stvec[*].major_revision=(*(arrvec[arrid[4]].ptr))[*]
  stvec[*].minor_revision=(*(arrvec[arrid[5]].ptr))[*]
  stvec[*].prog_id=(*(arrvec[arrid[6]].ptr))[*]
  stvec[*].noise.mean=(*(arrvec[arrid[7]].ptr))[*]
  stvec[*].noise.sd=(*(arrvec[arrid[8]].ptr))[*]
  stvec[*].gsct=(*(arrvec[arrid[9]].ptr))[*]
  stvec[*].vel.min=(*(arrvec[arrid[10]].ptr))[*]
  stvec[*].vel.max=(*(arrvec[arrid[11]].ptr))[*]
  stvec[*].pwr.min=(*(arrvec[arrid[12]].ptr))[*]
  stvec[*].pwr.max=(*(arrvec[arrid[13]].ptr))[*]
  stvec[*].wdt.min=(*(arrvec[arrid[14]].ptr))[*]
  stvec[*].wdt.max=(*(arrvec[arrid[15]].ptr))[*]
  stvec[*].verr.min=(*(arrvec[arrid[16]].ptr))[*]
  stvec[*].verr.max=(*(arrvec[arrid[17]].ptr))[*]

  if arrid[18] ne -1 then begin
    prm.vcnum=N_ELEMENTS(*(arrvec[arrid[18]].ptr))
    gvec=replicate(gvec,prm.vcnum)
    gvec[*].mlat=(*(arrvec[arrid[18]].ptr))[*]
    gvec[*].mlon=(*(arrvec[arrid[19]].ptr))[*]
    gvec[*].azm=(*(arrvec[arrid[20]].ptr))[*]
    gvec[*].srng=(*(arrvec[arrid[21]].ptr))[*]
    gvec[*].st_id=(*(arrvec[arrid[22]].ptr))[*]
    gvec[*].chn=(*(arrvec[arrid[23]].ptr))[*]
    gvec[*].index=(*(arrvec[arrid[24]].ptr))[*]
 
    gvec[*].vel.median=(*(arrvec[arrid[25]].ptr))[*]
    gvec[*].vel.sd=(*(arrvec[arrid[26]].ptr))[*]

    if arrid[27] ne -1 then begin
      prm.xtd=1
      gvec[*].pwr.median=(*(arrvec[arrid[27]].ptr))[*]
      gvec[*].pwr.sd=(*(arrvec[arrid[28]].ptr))[*]
      gvec[*].wdt.median=(*(arrvec[arrid[29]].ptr))[*]
      gvec[*].wdt.sd=(*(arrvec[arrid[30]].ptr))[*]
    endif
  endif

  st=DataMapFreeScalar(sclvec)
  st=DataMapFreeArray(arrvec)

  return,s
  
end


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       GridWrite
;
; PURPOSE:
;       Write a record to a grdmap file.
;       
;
; CALLING SEQUENCE:
;       status = GridWrite(unit,prm,stvec,gvec)
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


function GridWrite,unit,prm,stvec,gvec

  s=DataMapMakeScalar('start.year',prm.stme.yr,sclvec,/new)
  s=DataMapMakeScalar('start.month',prm.stme.mo,sclvec)
  s=DataMapMakeScalar('start.day',prm.stme.dy,sclvec)
  s=DataMapMakeScalar('start.hour',prm.stme.hr,sclvec)
  s=DataMapMakeScalar('start.minute',prm.stme.mt,sclvec)
  s=DataMapMakeScalar('start.second',prm.stme.sc,sclvec)
  s=DataMapMakeScalar('end.year',prm.etme.yr,sclvec)
  s=DataMapMakeScalar('end.month',prm.etme.mo,sclvec)
  s=DataMapMakeScalar('end.day',prm.etme.dy,sclvec)
  s=DataMapMakeScalar('end.hour',prm.etme.hr,sclvec)
  s=DataMapMakeScalar('end.minute',prm.etme.mt,sclvec)
  s=DataMapMakeScalar('end.second',prm.etme.sc,sclvec)

  stnum=prm.stnum  
  
  s=DataMapMakeArray('stid',stvec[0:stnum-1].st_id,arrvec,/new)
  s=DataMapMakeArray('channel',stvec[0:stnum-1].chn,arrvec)
  s=DataMapMakeArray('nvec',stvec[0:stnum-1].npnt,arrvec)
  s=DataMapMakeArray('freq',stvec[0:stnum-1].freq,arrvec)
  s=DataMapMakeArray('major.revision',stvec[0:stnum-1].major_revision,arrvec)
  s=DataMapMakeArray('minor.revision',stvec[0:stnum-1].minor_revision,arrvec)
  s=DataMapMakeArray('program.id',stvec[0:stnum-1].prog_id,arrvec)
  s=DataMapMakeArray('noise.mean',stvec[0:stnum-1].noise.mean,arrvec)
  s=DataMapMakeArray('noise.sd',stvec[0:stnum-1].noise.sd,arrvec)
  s=DataMapMakeArray('gsct',stvec[0:stnum-1].gsct,arrvec)
  s=DataMapMakeArray('v.min',stvec[0:stnum-1].vel.min,arrvec)
  s=DataMapMakeArray('v.max',stvec[0:stnum-1].vel.max,arrvec)
  s=DataMapMakeArray('p.min',stvec[0:stnum-1].pwr.min,arrvec)
  s=DataMapMakeArray('p.max',stvec[0:stnum-1].pwr.max,arrvec)
  s=DataMapMakeArray('w.min',stvec[0:stnum-1].wdt.min,arrvec)
  s=DataMapMakeArray('w.max',stvec[0:stnum-1].wdt.max,arrvec)
  s=DataMapMakeArray('ve.min',stvec[0:stnum-1].verr.min,arrvec)
  s=DataMapMakeArray('ve.max',stvec[0:stnum-1].verr.max,arrvec)

  vcnum=prm.vcnum  
  if vcnum ne 0 then begin
    s=DataMapMakeArray('vector.mlat',gvec[0:vcnum-1].mlat,arrvec)
    s=DataMapMakeArray('vector.mlon',gvec[0:vcnum-1].mlon,arrvec)
    s=DataMapMakeArray('vector.kvect',gvec[0:vcnum-1].azm,arrvec)
    s=DataMapMakeArray('vector.srng',gvec[0:vcnum-1].srng,arrvec)
    s=DataMapMakeArray('vector.stid',gvec[0:vcnum-1].st_id,arrvec)
    s=DataMapMakeArray('vector.channel',gvec[0:vcnum-1].chn,arrvec)
    s=DataMapMakeArray('vector.index',gvec[0:vcnum-1].index,arrvec)

    s=DataMapMakeArray('vector.vel.median',gvec[0:vcnum-1].vel.median,arrvec)
    s=DataMapMakeArray('vector.vel.sd',gvec[0:vcnum-1].vel.sd,arrvec)
    if prm.xtd ne 0 then begin
      s=DataMapMakeArray('vector.pwr.median',gvec[0:vcnum-1].pwr.median,arrvec)
      s=DataMapMakeArray('vector.pwr.sd',gvec[0:vcnum-1].pwr.sd,arrvec)
      s=DataMapMakeArray('vector.wdt.median',gvec[0:vcnum-1].wdt.median,arrvec)
      s=DataMapMakeArray('vector.wdt.sd',gvec[0:vcnum-1].wdt.sd,arrvec)
    endif
  endif


  s=DataMapWrite(unit,sclvec,arrvec)
  s=DataMapFreeScalar(sclvec)
  s=DataMapFreeArray(arrvec)
  return,s
end


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       GridMakeGridInx
;
; PURPOSE:
;       Create a structure to store grdmap index.
;
;
; CALLING SEQUENCE:
;       GridMakeGridInx,inx
;
;       This procedure creates a structure to store the grdmap index.
;
;
;-----------------------------------------------------------------
;

pro GridMakeGridIndex,inx,time,offset
  inx={GridInx, time: time, offset: offset}
end

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       GridLoadInx
;
; PURPOSE:
;       Read an index to a grdmap file.
;
;
; CALLING SEQUENCE:
;       status = GridLoadInx(unit,inx)
;
;       This function reads an index to a grdmap file.
;
;       The returned value is size in bytes of the record on success,
;       or  -1 for failure
;
;-----------------------------------------------------------------
;

function GridLoadInx,unit,inx

ON_IOERROR,iofail
    
  time=0.0D
  offset=0L
  s=0
  c=0L
  while (s eq 0) do begin
    readu,unit,time,offset
    GridMakeGridIndex,tinx,time,offset
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
;       Extract the time from a grdmap record.
;
;
; CALLING SEQUENCE:
;       status = GridGetTime(sclvec)
;
;       This function extracts the time infornation from the scalar
;       vector array generated by reading a DataMap file.
;
;
;       The returned value is zero on success,
;       or  -1 for failure
;
;-----------------------------------------------------------------
;

function GridGetTime,sclvec
 
  sclname=['start.year','start.month','start.day','start.hour', $
           'start.minute','start.second']

  scltype=[2,2,2,2,2,8]

  sclid=intarr(n_elements(sclname))
  sclid[*]=-1

  if (n_elements(sclvec) ne 0) then begin
    for n=0,n_elements(sclname)-1 do $
      sclid[n]=DataMapFindScalar(sclname[n],scltype[n],sclvec)
  endif

  q=where(sclid eq -1,count)
  if (count ne 0) then begin
    print,'File is in the wrong format!'
    return, -1
  endif

  yr=*(sclvec[sclid[0]].ptr)
  mo=*(sclvec[sclid[1]].ptr)
  dy=*(sclvec[sclid[2]].ptr)
  hr=*(sclvec[sclid[3]].ptr)
  mt=*(sclvec[sclid[4]].ptr)
  sc=*(sclvec[sclid[5]].ptr)
  us=*(sclvec[sclid[6]].ptr)

  return, TimeYMDHMSToEpoch(yr,mo,dy,hr,mt,sc)
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


function GridSeek,unit,yr,mo,dy,hr,mt,sc,inx,atme=atme

  tval=TimeYMDHMSToEpoch(yr,mo,dy,hr,mt,sc);

  if (n_elements(inx) ne 0) then begin
   
    rec=0L
    prec=-1L
    num=n_elements(inx)
    stime=inx[0].time
    etime=inx[num-1].time
   
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
      if (inx[rec].time eq tval) then break
      if (inx[rec].time lt tval) then begin
         srec=rec
         stime=inx[rec].time
      endif else begin
         erec=rec
         etime=inx[rec].time
      endelse
    endrep until (prec eq rec)
    atme=inx[rec].time
    point_lun,unit,inx[rec].offset
    return,0
  endif else begin
     s=DataMapRead(unit,sclvec,arrvec)
     fptr=0L
     if (n_elements(sclvec) ne 0) then begin
       tfile=GridGetTime(sclvec)
       st=DataMapFreeScalar(sclvec)
       st=DataMapFreeArray(arrvec)
       if (tfile gt tval) then point_lun,unit,0 $
       else begin
          stat=fstat(unit)
          fptr=stat.cur_ptr
      endelse
     endif else point_lun,unit,0
     atme=tfile
     repeat begin
       stat=fstat(unit)
       s=DataMapRead(unit,sclvec,arrvec)
       if (s eq -1) then break
       tfile=GridGetTime(sclvec)
      
       st=DataMapFreeScalar(sclvec)
       st=DataMapFreeArray(arrvec)
       if (tval gt tfile) then begin
         fptr=stat.cur_ptr
         atme=tfile
       endif
     endrep until (tval le tfile)
     if (tval gt tfile) then return,-1
     point_lun,unit,fptr
    endelse

 
  return,0 
end




;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       GridPoint
;
; PURPOSE:
;       Sets the file pointer position in a grdmap file.
;
;
; CALLING SEQUENCE:
;       status = GridPoint(unit,ptr)
;
;       This function sets the file pointer in a grdmap file.
;
;
;       The returned value is the offset on success,
;       or  -1 for failure
;
;-----------------------------------------------------------------
;

function GridPoint,unit,off

ON_IOERROR,iofail

  point_lun,unit,off
  return,off

iofail:
  return, -1

end

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       GridOpen
;
; PURPOSE:
;       Open a grdmap file.
;
;
; CALLING SEQUENCE:
;       unit = GridOpen(fname,/read,/write,/update)
;
;       This function opens a grdmap file.
;
;       The returned value is the logical unit number on success,
;       or  -1 for failure
;
;
;       This function is provided as a convenience and opens
;       the grdmap file using the appropriate flags to to open.
;
;-----------------------------------------------------------------
;

function GridOpen,fname,read=read,write=write,update=update
  if KEYWORD_SET(read) then openr,unit,fname,/GET_LUN,/SWAP_IF_BIG_ENDIAN
  if KEYWORD_SET(write) then openw,unit,fname,/GET_LUN, /SWAP_IF_BIG_ENDIAN
  if KEYWORD_SET(update) then openu,unit,fname,/GET_LUN, /SWAP_IF_BIG_ENDIAN
  return, unit
end

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       GridClose
;
; PURPOSE:
;       Close a fitacf file.
;
;
; CALLING SEQUENCE:
;       s = GridClose(unit)
;
;       This function closes a grdmap file.
;
;       The returned value is zero on success,
;       or  -1 for failure
;
;-----------------------------------------------------------------
;

function GridClose,unit

  free_lun,unit
  return,0

end


