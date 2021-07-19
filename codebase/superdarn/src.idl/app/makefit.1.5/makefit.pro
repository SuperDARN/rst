; fitacfwrapper.pro
; =================
; Author: R.J.Barnes - Based on C code by K.Baker
; 
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
; FitACFStart 
; FitACF

; ---------------------------------------------------------------




@fitelv
@fitrange
@removenoise
@noiseacf
@noisestat
@morebadlags
@calcphires
@omegaguess
@dophasefit
@fitacfal
@fitnoise
@elevation
@elevgoose
@groundscatter
@dofit
@fitacfwrapper

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       MakeFit
;
; PURPOSE:
;       Initialize a structure to store parameters used by
;       the fitacf algorithm.
;       
;
; CALLING SEQUENCE:
;
;        MakeFit,input,output,index,/NEW
;
;
;        The filename of the dat or rawacf file is given by input.
;        The filename of the fit or fitacf file is given by output.
;        The filename of the optional index file is given by index.
;        If the keyword /NEW is set, then FitACF will assume that
;        the input files are in rawacf format and the output should
;        be in fitacf format.
;
;-----------------------------------------------------------------
;




pro MakeFit,input,output,index,new=new

  s=size(input)

  if (s[1] ne 7) then begin
    print, 'You must provide an input filename'
    stop
  endif

  s=size(output)

  if (s[1] ne 7) then begin
    print, 'You must provide an output filename'
    stop
  endif

  s=size(index)
  if (s[1] eq 7) then iflg=1 $
  else iflg=0

  
 

; Open the data table, this should be defined in the environment
; variable SD_RADAR

  fname=getenv('SD_RADAR')
  openr,inp,fname,/get_lun

; Load the data tables

  network=RadarLoad(inp)
  free_lun,inp

; Load up the hardware data, the environment variable SD_HDWPATH
; should point to the directory containing the hardware files


  s=RadarLoadHardware(network,path=getenv('SD_HDWPATH'))

  ; Open input file for reading


  n=0
  if KEYWORD_SET(new) then n=1
  
  if (n ne 0) then begin 
   
    inp=RawOpen(input,/read)

    out=FitOpen(output,/write)

    s=RawRead(inp,prm,raw)
  endif else begin  
    rawfp=OldRawOpen(input)
  
    openw,out,output,bufsize=0,/get_lun,/stdio
    if (iflg ne 0) then openw,inx,index,bufsize=0,/get_lun,/stdio

    s=OldRawRead(rawfp,prm,raw)

    irec=1L 
    drec=2L
    dnum=0L

  endelse


  if (s eq -1) then begin
     print, 'Error reading file'
     stop
  endif
  
  radar=RadarGetRadar(network,prm.stid)
 
   if (n_tags(radar) eq 0) then begin
     print, 'Could not get radar information'
     stop
  endif


  site=RadarYMDHMSGetSite(radar,prm.time.yr,prm.time.mo,prm.time.dy, $
                          prm.time.hr,prm.time.mt,prm.time.sc)

  if (n_tags(site) eq 0) then begin
     print, 'Could not get site information'
     stop
  endif
 
  FitACFStart,site,prm.time.yr,fitblk




  while s ne -1 do begin
   print, prm.time.hr,prm.time.mt,prm.time.sc

   s=FitACF(prm,raw,fitblk,fit)

   if (n ne 0) then s=FitWrite(out,prm,fit) $
   else begin
     tprm=prm
     if (drec eq 2) then s=OldFitWriteHeader(out,prm,fit)
     if ((iflg ne 0) and (irec eq 1)) then s=OldFitInxWriteHeader(inx,prm)
     dnum=OldFitWrite(out,prm,fit)
     if (iflg ne 0) then s=OldFitInxWrite(inx,drec,dnum,prm) 
     drec+=dnum
     irec++
   endelse



   if (n ne 0) then s=RawRead(inp,prm,raw) $
   else s=OldRawRead(rawfp,prm,raw)


  endwhile

   if (iflg ne 0) then begin
     s=OldFitInxClose(inx,tprm,irec-1)
     free_lun,inx
   endif

   
   free_lun,inp
   free_lun,out
   


end
