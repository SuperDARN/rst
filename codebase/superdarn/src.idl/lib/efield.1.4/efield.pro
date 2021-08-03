; efield.pro
; ==========
; Author: R.J.Barnes
; 
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
;
; Public Functions
; ----------------
;
; EFieldRead        
; EFieldWrite        
; EFieldMakeEFieldInx         
; EFieldLoadInx          
; EFieldSeek     
; EFieldOpen
; EFieldClose
;
;
; Private Functions
; -----------------
;
; EFieldMakePrm
; EFieldGetTime
; EFieldPoint

;
; ---------------------------------------------------------------
;

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       EFieldMakePrm
;
; PURPOSE:
;       PotCreate a structure to store potgrd parameter data.
;       
;
; CALLING SEQUENCE:
;       EFieldMakePrm,prm
;
;       This procedure creates a structure to store the potgrd 
;       parameters
;
;
;-----------------------------------------------------------------
;

pro EFieldMakePrm,prm

  prm={EFieldPrm, stme: {EfieldTime,yr:0,mo:0,dy:0,hr:0,mt:0,sc:0D}, $
                  etme: {EfieldTime,yr:0,mo:0,dy:0,hr:0,mt:0,sc:0D}, $
                  mlt:0.0, $
                  mltflg:0B, $
                  pflg:0B, $
                  vflg:0B}
end



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       EFieldRead
;
; PURPOSE:
;       Read a record from a electric field data  file.
;       
;
; CALLING SEQUENCE:
;       status = EFieldRead(unit,prm,count,pos,pot,E,V)
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

function EFieldRead,unit,prm,count,pos,P,E,V

  s=DataMapRead(unit,sclvec,arrvec)

  if (s eq -1) then return,s

  EFieldMakePrm,prm 
 
  sclname=['start.year','start.month','start.day', $
          'start.hour','start.minute','start.second', $ 
          'end.year','end.month','end.day', $
          'end.hour','end.minute','end.second','MLT']

  scltype=[2,2,2,2,2,8,2,2,2,2,2,8,4]

  sclid=intarr(n_elements(sclname))
  sclid[*]=-1

  arrname=['vector.data','vector.mlat','vector.mlon', $
           'vector.mlt','vector.E.north','vector.E.east', $
           'vector.potential','vector.V.mag','vector.V.azm']
  arrtype=[3,4,4,4,4,4,4,4,4]

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

  q=where(arrid[0:1] eq -1,count)
  if (count ne 0) then begin
    print,'File is in the wrong format!'
    return, -2  
  endif
  q=where(arrid[4:5] eq -1,count)
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

  prm.mlt=*(sclvec[sclid[12]].ptr)
  
  count=transpose(*(arrvec[arrid[0]].ptr))
  sze=size(*(arrvec[arrid[1]].ptr))
  pos=fltarr(sze[2],sze[1],2)
  pos[*,*,0]=transpose(*(arrvec[arrid[1]].ptr))
  if (arrid[2] ne -1) then begin
    pos[*,*,1]=transpose(*(arrvec[arrid[2]].ptr))
  endif

  if (arrid[3] ne -1) then begin
    pos[*,*,1]=transpose(*(arrvec[arrid[3]].ptr))
    prm.mltflg=1
  endif

  if (arrid[4] ne -1) then begin
    sze=size(*(arrvec[arrid[4]].ptr))
    E=fltarr(sze[2],sze[1],2)
    E[*,*,0]=transpose(*(arrvec[arrid[4]].ptr))
    E[*,*,1]=transpose(*(arrvec[arrid[5]].ptr))
  endif

  if (arrid[6] ne -1) then begin
    P=transpose(*(arrvec[arrid[6]].ptr))
    prm.pflg=1
  endif

  if (arrid[7] ne -1) then begin
    sze=size(*(arrvec[arrid[7]].ptr))
    V=fltarr(sze[2],sze[1],2)
    V[*,*,0]=transpose(*(arrvec[arrid[7]].ptr))
    V[*,*,1]=transpose(*(arrvec[arrid[8]].ptr))
    prm.vflg=1
  endif

  st=DataMapFreeScalar(sclvec)
  st=DataMapFreeArray(arrvec)
 
  return,s
  
end


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       EFieldWrite
;
; PURPOSE:
;       Read a record from a potgrd file.
;       
;
; CALLING SEQUENCE:
;       status = EFieldWrite(unit,prm,count,pos,P,E,V)
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

function EFieldWrite,unit,prm,count,pos,P,E,V
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
  s=DataMapMakeScalar('MLT',prm.mlt,sclvec)
  
  mlat=transpose(pos[*,*,0])
  s=DataMapMakeArray('vector.mlat',mlat,arrvec,/new)
 
  if (prm.mltflg ne 0) then begin
    mlt=transpose(pos[*,*,1])
    s=DataMapMakeArray('vector.mlt',mlt,arrvec)
  endif else begin
    mlon=transpose(pos[*,*,1])
    s=DataMapMakeArray('vector.mlon',mlon,arrvec)
  endelse
  
  if (n_elements(E) ne 0) then begin
    s=DataMapMakeArray('vector.E.north',transpose(E[*,*,0]),arrvec)
    s=DataMapMakeArray('vector.E.east',transpose(E[*,*,1]),arrvec)
  endif

  if ((n_elements(P) ne 0) and (prm.pflg ne 0)) then begin
    s=DataMapMakeArray('vector.potential',transpose(P),arrvec)
  endif

  if ((n_elements(V) ne 0) and (prm.vflg ne 0)) then begin
    s=DataMapMakeArray('vector.V.mag',transpose(V[*,*,0]),arrvec)
    s=DataMapMakeArray('vector.V.azm',transpose(V[*,*,1]),arrvec)
  endif

  s=DataMapMakeArray('vector.data',transpose(count),arrvec)

  s=DataMapWrite(unit,sclvec,arrvec)
  s=DataMapFreeScalar(sclvec)
  s=DataMapFreeArray(arrvec)
  return,s
end




;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       EFieldMakeEFieldInx
;
; PURPOSE:
;       Create a structure to store electric field index.
;
;
; CALLING SEQUENCE:
;       EFieldMakeEFieldInx,inx
;
;       This procedure creates a structure to store the electric
;       field  index.
;
;
;-----------------------------------------------------------------
;

pro EFieldMakeEFieldIndex,inx,time,offset
  inx={EFieldInx, time: time, offset: offset}
end

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       EFieldLoadInx
;
; PURPOSE:
;       Read an index to an electric field file.
;
;
; CALLING SEQUENCE:
;       status = EFieldLoadInx(unit,inx)
;
;       This function reads an index to an electric field file.
;
;       The returned value is size in bytes of the record on success,
;       or  -1 for failure
;
;-----------------------------------------------------------------
;

function EFieldLoadInx,unit,inx

ON_IOERROR,iofail
    
  time=0.0D
  offset=0L
  s=0
  c=0L
  while (s eq 0) do begin
    readu,unit,time,offset
    EFieldMakeEFieldIndex,tinx,time,offset
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
;       EFieldGetTime
;
; PURPOSE:
;       Extract the time from a electric field record.
;
;
; CALLING SEQUENCE:
;       status = EFieldGetTime(sclvec)
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

function EFieldGetTime,sclvec
 
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

  return, TimeYMDHMSToEpoch(yr,mo,dy,hr,mt,sc)
end



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       EFieldSeek
;
; PURPOSE:
;       Find a record in an electric field file.
;
;
; CALLING SEQUENCE:
;       status = EFieldSeek(unit,yr,mo,dy,hr,mt,sc,inx,atme=atime)
;
;       This function searchs for a record in an electric field file.
;
;       The returned value is zero on success,
;       or  -1 for failure
;
;-----------------------------------------------------------------
;


function EFieldSeek,unit,yr,mo,dy,hr,mt,sc,inx,atme=atme

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
       tfile=EFieldGetTime(sclvec)
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
       tfile=EFieldGetTime(sclvec)
      
       st=DataMapFreeScalar(sclvec)
       st=DataMapFreeArray(arrvec)
       if (tval ge tfile) then begin
         fptr=stat.cur_ptr
         atme=tfile
       endif
     endrep until (tval lt tfile)
     if (tval gt tfile) then return,-1
     point_lun,unit,fptr
    endelse 
  return,0 
end




;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       EFieldOpen
;
; PURPOSE:
;       Open a cnvmap file.
;
;
; CALLING SEQUENCE:
;       unit = EFieldOpen(fname,/read,/write,/update)
;
;       This function opens a cnvmap file.
;
;       The returned value is the logical unit number on success,
;       or  -1 for failure
;
;       This function is provided as a convenience and opens
;       the cnvmap file using the appropriate flags to to open.
;
;-----------------------------------------------------------------
;

function EFieldOpen,fname,read=read,write=write,update=update

    if KEYWORD_SET(read) then openr,unit,fname,/GET_LUN,/SWAP_IF_BIG_ENDIAN
    if KEYWORD_SET(write) then openw,unit,fname,/GET_LUN, /SWAP_IF_BIG_ENDIAN
    if KEYWORD_SET(update) then openu,unit,fname,/GET_LUN, /SWAP_IF_BIG_ENDIAN
    return, unit
end

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       EFieldClose
;
; PURPOSE:
;       Close a fitacf file.
;
;
; CALLING SEQUENCE:
;       s = EFieldClose(unit)
;
;       This function closes a grdmap file.
;
;       The returned value is zero on success,
;       or  -1 for failure
;
;-----------------------------------------------------------------
;

function EFieldClose,unit

  free_lun,unit
  return,0

end


