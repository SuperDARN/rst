; iq.pro
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
; IQOpen
; IQClose   
; IQExtractIQ     
; IQRead
; IQWrite          
; IQLoadInx
; IQSeek              

; Private Functions
; ----------------
;
; IQMakeIQData   
; IQMakeIQInx          
; IQPoint
; IQGetTime    



; ---------------------------------------------------------------



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       IQMakeIQData
;
; PURPOSE:
;       Create a structure to store iqdata data.
;       
;
; CALLING SEQUENCE:
;       IQMakeIQData,raw
;
;       This procedure creates a structure to store the iqdata data,
;       the structure is returns in iq
;
;
;-----------------------------------------------------------------
;


pro IQMakeIQData,iq

  MAX_NAVE=300

  iq={IQData, $
         revision: {rlstr, major: 0L, minor: 0L}, $ 
         chnnum: 0L, $
         smpnum: 0L, $
         skpnum: 0L, $
         seqnum: 0L, $
         tbadtr: 0L, $
         tval: make_array(MAX_NAVE,VALUE={unxtmstr, sec:0L, nsec:0L}), $
         atten: intarr(MAX_NAVE), $
         noise: fltarr(MAX_NAVE), $
         offset: lonarr(MAX_NAVE), $
         size: lonarr(MAX_NAVE), $ 
         badtr: lonarr(MAX_NAVE) $
      } 

end

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       IQExtractIQ
;
; PURPOSE:
;       Extract the IQ data for particular integration and channel 
;       from the raw samples 
;       
;
; CALLING SEQUENCE:
;       IQMakeIQData,iq,samples,IQ
;
;       Extracts a set of I&Q data from a particular integration from
;       the raw sample data. The IQ data block is passed in as iq
;       and the raw samples as samples.
;       the complex array of I&Q measurements is returned in iqc
;
;-----------------------------------------------------------------
;


function IQExtractIQ,iq,seqnum,chnnum,samples,iqc


  if (iq.seqnum eq 0) then return,0

  I=intarr(iq.smpnum)
  Q=intarr(iq.smpnum)

  if (seqnum lt 0) or (seqnum ge iq.seqnum) then return, -1
  if (chnnum lt 0) or (chnnum ge iq.chnnum) then return, -2

  offset=iq.offset[seqnum]+chnnum*iq.smpnum*2
  I=samples[offset:offset+2*iq.smpnum-1:2]
  Q=samples[offset+1:offset+2*iq.smpnum-1:2]
  iqc=complex(I,Q)

return, iq.smpnum
end

   
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       IQRead
;
; PURPOSE:
;       Read a record from a iqdat file.
;       
;
; CALLING SEQUENCE:
;       status = IQRead(unit,prm,iq,badtr,samples)
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

function IQRead,unit,prm,iq,badtr,samples

  s=DataMapRead(unit,sclvec,arrvec)
  if (s eq -1) then return,s
  
  RadarMakeRadarPrm,prm
  IQMakeIQData,iq
  
  s=RadarDecodeRadarPrm(prm,sclvec,arrvec)

  if (s eq -1) then begin 
    print,'File is in the wrong format!'
    st=DataMapFreeScalar(sclvec)
    st=DataMapFreeArray(arrvec)
    return,s
  endif

  sclname=['iqdata.revision.major', 'iqdata.revision.minor','seqnum',$
           'chnnum','smpnum','skpnum','btnum']

  scltype=[3,3,3,3,3,3,3]
  
  sclid=intarr(n_elements(sclname))
  sclid[*]=-1

  arrname=['tsc','tus','tatten','tnoise','toff','tsze', $
           'tbadtr','badtr','data']
  arrtype=[3,3,2,4,3,3,3,3,2]

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
    
  q=where(sclid[0:5] eq -1,count)

  if (count ne 0) then begin
    print,'File is in the wrong format!'
    return, -2
  endif

  ; populate the structures

  iq.revision.major=*(sclvec[sclid[0]].ptr)
  iq.revision.minor=*(sclvec[sclid[1]].ptr)
  iq.seqnum=*(sclvec[sclid[2]].ptr)
  iq.chnnum=*(sclvec[sclid[3]].ptr)
  iq.smpnum=*(sclvec[sclid[4]].ptr)
  iq.skpnum=*(sclvec[sclid[5]].ptr)
  if (sclid[6] ne -1) then iq.tbadtr=*(sclvec[sclid[6]].ptr)
 

  if (iq.seqnum gt 0) then begin
     iq.tval[0:iq.seqnum-1].sec=*(arrvec[arrid[0]].ptr)
     iq.tval[0:iq.seqnum-1].nsec=*(arrvec[arrid[1]].ptr)*1000
     iq.atten[0:iq.seqnum-1]=*(arrvec[arrid[2]].ptr)
     iq.noise[0:iq.seqnum-1]=*(arrvec[arrid[3]].ptr)
     iq.offset[0:iq.seqnum-1]=*(arrvec[arrid[4]].ptr)
     iq.size[0:iq.seqnum-1]=*(arrvec[arrid[5]].ptr)
     if (arrid[6] ne -1) then iq.badtr[0:iq.seqnum-1]=*(arrvec[arrid[6]].ptr)
     if (arrid[7] ne -1) then badtr=*(arrvec[arrid[7]].ptr)
     samples=*(arrvec[arrid[8]].ptr)
   endif
   st=DataMapFreeScalar(sclvec)
   st=DataMapFreeArray(arrvec)
   return,s
end




;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       IQWrite
;
; PURPOSE:
;       Write a record from a rawacf file.
;       
;
; CALLING SEQUENCE:
;       status = IQWrite(unit,prm,iq,samples)
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

function IQWrite,unit,prm,iq,badtr,samples

  s=RadarEncodeRadarPrm(prm,sclvec,arrvec,/new)

  s=DataMapMakeScalar('iqdata.revision.major',iq.revision.major,sclvec)
  s=DataMapMakeScalar('iqdata.revision.minor',iq.revision.minor,sclvec)
  s=DataMapMakeScalar('seqnum',iq.seqnum,sclvec)
  s=DataMapMakeScalar('chnnum',iq.chnnum,sclvec)
  s=DataMapMakeScalar('smpnum',iq.smpnum,sclvec)
  s=DataMapMakeScalar('skpnum',iq.skpnum,sclvec)
  s=DataMapMakeScalar('btnum',iq.tbadtr,sclvec)

  if (iq.seqnum gt 0) then begin  
    s=DataMapMakeArray('tsc',iq.tval[0:iq.seqnum-1].sec,arrvec)
    s=DataMapMakeArray('tus',iq.tval[0:iq.seqnum-1].nsec/1000L,arrvec)
    s=DataMapMakeArray('tatten',iq.atten[0:iq.seqnum-1],arrvec)
    s=DataMapMakeArray('tnoise',iq.noise[0:iq.seqnum-1],arrvec)
    s=DataMapMakeArray('toff',iq.offset[0:iq.seqnum-1],arrvec)
    s=DataMapMakeArray('tsze',iq.size[0:iq.seqnum-1],arrvec)
    s=DataMapMakeArray('tbadtr',iq.badtr[0:iq.seqnum-1],arrvec)
    s=DataMapMakeArray('badtr',badtr,arrvec)
    s=DataMapMakeArray('data',fix(samples),arrvec)
  endif

  s=DataMapWrite(unit,sclvec,arrvec)
  s=DataMapFreeScalar(sclvec)
  s=DataMapFreeArray(arrvec)
  return,s
end




;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       IQMakeIQInx
;
; PURPOSE:
;       Create a structure to store rawACF index.
;
;
; CALLING SEQUENCE:
;       IQMakeIQInx,inx
;
;       This procedure creates a structure to store the rawacf index.
;
;
;-----------------------------------------------------------------
;

pro IQMakeIQIndex,inx,time,offset
  inx={IQInx, time: time, offset: offset}
end


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       IQLoadIndex
;
; PURPOSE:
;       Reads an index to a rawacf file..
;
;
; CALLING SEQUENCE:
;       status = IQLoadIndex(unit,inx)
;
;       This function reads an index to a rawacf file.
;
;
;       The returned value is size in bytes of the record on success,
;       or  -1 for failure
;
;-----------------------------------------------------------------
;

function IQLoadInx,unit,inx

ON_IOERROR,iofail

  time=0.0D
  offset=0L
  s=0
  c=0L
  while (s eq 0) do begin
    readu,unit,time,offset
    IQMakeIQIndex,tinx,time,offset
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
;       IQGetTime
;
; PURPOSE:
;       Extract the time from a fitacf record.
;
;
; CALLING SEQUENCE:
;       tval = IQGetTime(sclvec)
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

function IQGetTime,sclvec

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
;       IQSeek
;
; PURPOSE:
;       Find a record in a rawacf file.
;
;
; CALLING SEQUENCE:
;       status = IQSeek(unit,yr,mo,dy,hr,mt,sc,inx,atme=atime)
;
;       This function searchs for a record in a rawacf file.
;
;
;       The returned value is zero on success,
;       or  -1 for failure
;
;-----------------------------------------------------------------
;

function IQSeek,unit,yr,mo,dy,hr,mt,sc,inx,atme=atme

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
       tfile=IQGetTime(sclvec)
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
       tfile=IQGetTime(sclvec)      
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
;       IQPoint
;
; PURPOSE:
;       Sets the file pointer position in a raw file.
;
;
; CALLING SEQUENCE:
;       status = IQPoint(unit,ptr)
;
;       This function sets the file pointer in a raw file.
;
;
;       The returned value is the offset on success,
;       or  -1 for failure
;
;-----------------------------------------------------------------
;

function IQPoint,unit,off

ON_IOERROR,iofail

  point_lun,unit,off
  return,off

iofail:
  return, -1

end

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       IQOpen
;
; PURPOSE:
;       Open an iqdat file.
;
;
; CALLING SEQUENCE:
;       unit = IQOpen(fname,/read,/write,/update)
;
;       This function opens an iqdat file.
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

function IQOpen,fname,read=read,write=write,update=update
  if KEYWORD_SET(read) then openr,unit,fname,/GET_LUN,/SWAP_IF_BIG_ENDIAN
  if KEYWORD_SET(write) then openw,unit,fname,/GET_LUN,/SWAP_IF_BIG_ENDIAN
  if KEYWORD_SET(update) then openu,unit,fname,/GET_LUN,/SWAP_IF_BIG_ENDIAN
  return, unit
end




;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       IQClose
;
; PURPOSE:
;       Close a fitacf file.
;
;
; CALLING SEQUENCE:
;       s = IQClose(unit)
;
;       This function closes an IQ file.
;
;       The returned value is zero on success,
;       or  -1 for failure
;
;-----------------------------------------------------------------
;

function IQClose,unit

  free_lun,unit
  return,0

end

