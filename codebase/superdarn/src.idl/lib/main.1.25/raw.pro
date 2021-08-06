; raw.pro
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
; RawLoadInx              
; RawRead   
; RawWrite          
; RawSeek
; RawOpen
; RawClose


; Private Functions
; ---------------
;

; RawMakeRawData        
; RawMakeRawInx   
; RawPoint
; RawGetTime

;
; ---------------------------------------------------------------



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       RawMakeRawData
;
; PURPOSE:
;       Create a structure to store rawACF data.
;       
;
; CALLING SEQUENCE:
;       RawMakeRawData,raw
;
;       This procedure creates a structure to store the rawACF data,
;       the structure is returns in rawdata.
;
;
;-----------------------------------------------------------------
;


pro RawMakeRawData,raw

  LAG_SIZE=150
  MAX_RANGE=800

  raw={RawData, $
         revision: {rlstr, major: 0L, minor: 0L}, $ 
         thr: 0.0, $
         pwr0: fltarr(MAX_RANGE), $
         acfd: fltarr(MAX_RANGE,LAG_SIZE,2), $
         xcfd: fltarr(MAX_RANGE,LAG_SIZE,2) $
      }

end
   
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       RawRead
;
; PURPOSE:
;       Read a record from a rawacf file.
;       
;
; CALLING SEQUENCE:
;       status = RawRead(unit,prm,raw)
;
;       This function reads a single record from the open file with
;       logical unit number, unit. The scalar and array vectors are
;       returned in sclvec and arrvec respectively.
;
;
;       The returned value is size in bytes of the record on success, 
;       or  -1 for failure
;
;-----------------------------------------------------------------
;

function RawRead,unit,prm,raw

  s=DataMapRead(unit,sclvec,arrvec)
  if (s eq -1) then return,s
  
  RadarMakeRadarPrm,prm
  RawMakeRawData,raw
  
  s=RadarDecodeRadarPrm(prm,sclvec,arrvec)

  if (s eq -1) then begin 
   print,'File is in the wrong format!'
   st=DataMapFreeScalar(sclvec)
   st=DataMapFreeArray(arrvec)
   return,s
  endif

  sclname=['rawacf.revision.major', 'rawacf.revision.minor','thr']

  scltype=[3,3,4]
  
  sclid=intarr(n_elements(sclname))
  sclid[*]=-1

  arrname=['slist','pwr0','acfd','xcfd']
  arrtype=[2,4,4,4]

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
    st=DataMapFreeScalar(sclvec)
    st=DataMapFreeArray(arrvec)
    return, -2
  endif
 
  ; populate the structures

  raw.revision.major=*(sclvec[sclid[0]].ptr)
  raw.revision.minor=*(sclvec[sclid[1]].ptr)
  raw.thr=*(sclvec[sclid[2]].ptr)

  if (prm.nrang gt 0) then raw.pwr0[0:prm.nrang-1]=*(arrvec[arrid[1]].ptr)

  if (arrid[0] eq -1) then begin
     st=DataMapFreeScalar(sclvec)
     st=DataMapFreeArray(arrvec)
     return,s
  endif

  if ~(ptr_valid(arrvec[arrid[0]].ptr)) then begin
     st=DataMapFreeScalar(sclvec)
     st=DataMapFreeArray(arrvec)
     return,s
  endif

  slist=*(arrvec[arrid[0]].ptr)

  if (n_elements(slist) eq 0) then begin
    st=DataMapFreeScalar(sclvec)
    st=DataMapFreeArray(arrvec)
    return,s 
  endif

  raw.acfd[slist,0:prm.mplgs-1,*]= (*(arrvec[arrid[2]].ptr))[*,*,*]
 
  if (prm.xcf ne 0) and (arrid[3] ne -1) then $
    raw.xcfd[slist,0:prm.mplgs-1,*]= (*(arrvec[arrid[3]].ptr))[*,*,*]

  st=DataMapFreeScalar(sclvec)
  st=DataMapFreeArray(arrvec)

  return,s
end


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       RawWrite
;
; PURPOSE:
;       Write a record from a rawacf file.
;       
;
; CALLING SEQUENCE:
;       status = RawWrite(unit,prm,raw)
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

function RawWrite,unit,prm,raw

  s=RadarEncodeRadarPrm(prm,sclvec,arrvec,/new)

  s=DataMapMakeScalar('rawacf.revision.major',raw.revision.major,sclvec)
  s=DataMapMakeScalar('rawacf.revision.minor',raw.revision.minor,sclvec)
  s=DataMapMakeScalar('thr',raw.thr,sclvec)

  ;tx=floor((raw.thr*prm.noise.search)/2.0)
  tx=0

  slist=fix(where(raw.pwr0[0:prm.nrang-1] ge tx,count))

  if (count ne 0) then s=DataMapMakeArray('slist',slist,arrvec)
  
  s=DataMapMakeArray('pwr0',raw.pwr0[0:prm.nrang-1],arrvec)
  
  if (count ne 0) then begin 
    s=DataMapMakeArray('acfd',raw.acfd[slist,0:prm.mplgs-1,*],arrvec)
 
    if (prm.xcf eq 1) then $
      s=DataMapMakeArray('xcfd',raw.xcfd[slist,0:prm.mplgs-1,*],arrvec)
  endif
  s=DataMapWrite(unit,sclvec,arrvec)
  s=DataMapFreeScalar(sclvec)
  s=DataMapFreeArray(arrvec)
  return,s
end


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       RawMakeRawInx
;
; PURPOSE:
;       Create a structure to store rawACF index.
;
;
; CALLING SEQUENCE:
;       RawMakeRawInx,inx
;
;       This procedure creates a structure to store the rawacf index.
;
;
;-----------------------------------------------------------------
;

pro RawMakeRawIndex,inx,time,offset
  inx={RawInx, time: time, offset: offset}
end


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       RawLoadIndex
;
; PURPOSE:
;       Reads an index to a rawacf file..
;
;
; CALLING SEQUENCE:
;       status = RawLoadIndex(unit,inx)
;
;       This function reads an index to a rawacf file.
;
;
;       The returned value is size in bytes of the record on success,
;       or  -1 for failure
;
;-----------------------------------------------------------------
;

function RawLoadInx,unit,inx

ON_IOERROR,iofail

  time=0.0D
  offset=0L
  s=0
  c=0L
  while (s eq 0) do begin
    readu,unit,time,offset
    RawMakeRawIndex,tinx,time,offset
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
;       RawGetTime
;
; PURPOSE:
;       Extract the time from a fitacf record.
;
;
; CALLING SEQUENCE:
;       tval = RawGetTime(sclvec)
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

function RawGetTime,sclvec

  sclname=['time.yr','time.mo','time.dy','time.hr', $
           'time.mt','time.sc','time.us']

  scltype=[2,2,2,2,2,2,3]

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

  return, TimeYMDHMSToEpoch(yr,mo,dy,hr,mt,sc+us/1.0e6)
end


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       RawSeek
;
; PURPOSE:
;       Find a record in a rawacf file.
;
;
; CALLING SEQUENCE:
;       status = RawSeek(unit,yr,mo,dy,hr,mt,sc,inx,atme=atime)
;
;       This function searchs for a record in a rawacf file.
;
;
;       The returned value is zero on success,
;       or  -1 for failure
;
;-----------------------------------------------------------------
;

function RawSeek,unit,yr,mo,dy,hr,mt,sc,inx,atme=atme

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
       tfile=RawGetTime(sclvec)
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
       tfile=RawGetTime(sclvec)      
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
;       RawPoint
;
; PURPOSE:
;       Sets the file pointer position in a raw file.
;
;
; CALLING SEQUENCE:
;       status = RawPoint(unit,ptr)
;
;       This function sets the file pointer in a raw file.
;
;
;       The returned value is the offset on success,
;       or  -1 for failure
;
;-----------------------------------------------------------------
;

function RawPoint,unit,off

ON_IOERROR,iofail

  point_lun,unit,off
  return,off

iofail:
  return, -1

end

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       RawOpen
;
; PURPOSE:
;       Open a rawacf file.
;
;
; CALLING SEQUENCE:
;       unit = RawOpen(fname,/read,/write,/update)
;
;       This function opens a rawacf file.
;
;       The returned value is the logical unit number on success,
;       or  -1 for failure
;
;
;       This function is provided as a convenience and opens
;       the rawacf file using the appropriate flags to to open.
;
;-----------------------------------------------------------------
;

function RawOpen,fname,read=read,write=write,update=update
  if KEYWORD_SET(read) then openr,unit,fname,/GET_LUN,/SWAP_IF_BIG_ENDIAN
  if KEYWORD_SET(write) then openw,unit,fname,/GET_LUN,/SWAP_IF_BIG_ENDIAN
  if KEYWORD_SET(update) then openu,unit,fname,/GET_LUN,/SWAP_IF_BIG_ENDIAN
  return, unit
end


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       RawClose
;
; PURPOSE:
;       Close a fitacf file.
;
;
; CALLING SEQUENCE:
;       s = RawClose(unit)
;
;       This function closes a rawacf file.
;
;       The returned value is zero on success,
;       or  -1 for failure
;
;-----------------------------------------------------------------
;

function RawClose,unit

  free_lun,unit
  return,0

end
