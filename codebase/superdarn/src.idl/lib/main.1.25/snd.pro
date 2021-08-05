; snd.pro
; =======
; Author: E.G.Thomas
;
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
; SndRead
; SndWrite
; SndSeek
; SndOpen
; SndClose

; Private Functions
; ---------------
;
; SndMakeSndData
; SndGetTime
;
; ---------------------------------------------------------------


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       SndMakeSndData
;
; PURPOSE:
;       Create a structure to store sounding data.
;
;
; CALLING SEQUENCE:
;       SndMakeSndData,snd
;
;       This procedure creates a structure to store the sounding data,
;       the structure is returns in snddata.
;
;
;-----------------------------------------------------------------
;

pro SndMakeSndData,snd

  MAX_RANGE=300

  snd={SndData, $
         radar_revision: {rdstr, major: 0B, minor: 0B}, $
         origin: {ogstr, code: 0B, time: '', command: ''}, $
         cp: 0, $
         stid: 0, $
         time: {tmstr, yr:0, $
                      mo:0, $
                      dy:0, $
                      hr:0, $
                      mt:0, $
                      sc:0, $
                      us:0L  $
               }, $
         nave: 0, $
         lagfr: 0, $
         smsep: 0, $
         noise: {nsstr, search:0.0, mean:0.0}, $
         channel: 0, $
         bmnum: 0, $
         bmazm: 0.0, $
         scan: 0, $
         rxrise: 0, $
         intt: {itstr, sc:0, us:0L}, $
         nrang: 0, $
         frang: 0, $
         rsep: 0, $
         xcf: 0, $
         tfreq: 0, $
         sky_noise: 0.0, $
         combf: '', $
         fit_revision: {rlstr, major: 0L, minor: 0L}, $
         snd_revision: {sdstr, major: 0, minor: 0}, $
         qflg: bytarr(MAX_RANGE), $
         gflg: bytarr(MAX_RANGE), $
         v: fltarr(MAX_RANGE), $
         v_e: fltarr(MAX_RANGE), $
         p_l:  fltarr(MAX_RANGE), $ 
         w_l: fltarr(MAX_RANGE), $
         x_qflg: bytarr(MAX_RANGE), $
         phi0: fltarr(MAX_RANGE), $
         phi0_e: fltarr(MAX_RANGE) $
      }

end


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       SndRead
;
; PURPOSE:
;       Read a record from a sounding file.
;
;
; CALLING SEQUENCE:
;       status = SndRead(unit,snd)
;
;       This function reads a single record from the open file with
;       logical unit number, unit. The snd structure is
;       populated accordingly.
;
;
;       The returned value is size in bytes of the record on success,
;       or  -1 for failure
;
;-----------------------------------------------------------------
;

function SndRead,unit,snd

  s=DataMapRead(unit,sclvec,arrvec)
  if (s eq -1) then return,s

  SndMakeSndData,snd

  sclname=['radar.revision.major','radar.revision.minor', $
           'origin.code','origin.time','origin.command','cp','stid', $
           'time.yr','time.mo','time.dy','time.hr','time.mt','time.sc', $
           'time.us','nave','lagfr','smsep','noise.search','noise.mean', $
           'channel','bmnum','bmazm','scan','rxrise','intt.sc','intt.us', $
           'nrang','frang','rsep','xcf','tfreq','noise.sky', $
           'combf','fitacf.revision.major','fitacf.revision.minor', $
           'snd.revision.major','snd.revision.minor']

  scltype=[1,1, $
           1,9,9,2,2, $
           2,2,2,2,2,2, $
           3,2,2,2,4,4, $
           2,2,4,2,2,2,3, $
           2,2,2,2,2,4, $
           9,3,3, $
           2,2]

  sclid=intarr(n_elements(sclname))
  sclid[*]=-1

  arrname=['slist','qflg','gflg', $
           'v','v_e','p_l','w_l', $
           'x_qflg','phi0','phi0_e']

  arrtype=[2,1,1, $
           4,4,4,4, $
           1,4,4]

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

  ;q=where(sclid eq -1,count)

  ;if (count ne 0) then begin
  ;  print,'File is in the wrong format!'
  ;  st=DataMapFreeScalar(sclvec)
  ;  st=DataMapFreeArray(arrvec)
  ;  return, -2
  ;endif

  ; populate the structures

  if (sclid[0] ne -1) then snd.radar_revision.major=*(sclvec[sclid[0]].ptr)
  if (sclid[1] ne -1) then snd.radar_revision.minor=*(sclvec[sclid[1]].ptr)
  if (sclid[2] ne -1) then snd.origin.code=*(sclvec[sclid[2]].ptr)
  if (sclid[3] ne -1) then snd.origin.time=*(sclvec[sclid[3]].ptr)
  if (sclid[4] ne -1) then snd.origin.command=*(sclvec[sclid[4]].ptr)
  if (sclid[5] ne -1) then snd.cp=*(sclvec[sclid[5]].ptr)
  if (sclid[6] ne -1) then snd.stid=*(sclvec[sclid[6]].ptr)
  if (sclid[7] ne -1) then snd.time.yr=*(sclvec[sclid[7]].ptr)
  if (sclid[8] ne -1) then snd.time.mo=*(sclvec[sclid[8]].ptr)
  if (sclid[9] ne -1) then snd.time.dy=*(sclvec[sclid[9]].ptr)
  if (sclid[10] ne -1) then snd.time.hr=*(sclvec[sclid[10]].ptr)
  if (sclid[11] ne -1) then snd.time.mt=*(sclvec[sclid[11]].ptr)
  if (sclid[12] ne -1) then snd.time.sc=*(sclvec[sclid[12]].ptr)
  if (sclid[13] ne -1) then snd.time.us=*(sclvec[sclid[13]].ptr)
  if (sclid[14] ne -1) then snd.nave=*(sclvec[sclid[14]].ptr)
  if (sclid[15] ne -1) then snd.lagfr=*(sclvec[sclid[15]].ptr)
  if (sclid[16] ne -1) then snd.smsep=*(sclvec[sclid[16]].ptr)
  if (sclid[17] ne -1) then snd.noise.search=*(sclvec[sclid[17]].ptr)
  if (sclid[18] ne -1) then snd.noise.mean=*(sclvec[sclid[18]].ptr)
  if (sclid[19] ne -1) then snd.channel=*(sclvec[sclid[19]].ptr)
  if (sclid[20] ne -1) then snd.bmnum=*(sclvec[sclid[20]].ptr)
  if (sclid[21] ne -1) then snd.bmazm=*(sclvec[sclid[21]].ptr)
  if (sclid[22] ne -1) then snd.scan=*(sclvec[sclid[22]].ptr)
  if (sclid[23] ne -1) then snd.rxrise=*(sclvec[sclid[23]].ptr)
  if (sclid[24] ne -1) then snd.intt.sc=*(sclvec[sclid[24]].ptr)
  if (sclid[25] ne -1) then snd.intt.us=*(sclvec[sclid[25]].ptr)
  if (sclid[26] ne -1) then snd.nrang=*(sclvec[sclid[26]].ptr)
  if (sclid[27] ne -1) then snd.frang=*(sclvec[sclid[27]].ptr)
  if (sclid[28] ne -1) then snd.rsep=*(sclvec[sclid[28]].ptr)
  if (sclid[29] ne -1) then snd.xcf=*(sclvec[sclid[29]].ptr)
  if (sclid[30] ne -1) then snd.tfreq=*(sclvec[sclid[30]].ptr)
  if (sclid[31] ne -1) then snd.sky_noise=*(sclvec[sclid[31]].ptr)
  if (sclid[32] ne -1) then snd.combf=*(sclvec[sclid[32]].ptr)
  if (sclid[33] ne -1) then snd.fit_revision.major=*(sclvec[sclid[33]].ptr)
  if (sclid[34] ne -1) then snd.fit_revision.minor=*(sclvec[sclid[34]].ptr)
  if (sclid[35] ne -1) then snd.snd_revision.major=*(sclvec[sclid[35]].ptr)
  if (sclid[36] ne -1) then snd.snd_revision.minor=*(sclvec[sclid[36]].ptr)

  if (arrid[0] eq -1) then begin
    st=DataMapFreeScalar(sclvec)
    st=DataMapFreeArray(arrvec)
    return, s
  endif

  if ~(ptr_valid(arrvec[arrid[0]].ptr)) then begin
    st=DataMapFreeScalar(sclvec)
    st=DataMapFreeArray(arrvec)
    return, s
  endif

  slist=*(arrvec[arrid[0]].ptr)
  if (n_elements(slist) eq 0) then begin
    st=DataMapFreeScalar(sclvec)
    st=DataMapFreeArray(arrvec)
    return,s
  endif

  if (arrid[1] ne -1) then snd.qflg[slist]= (*(arrvec[arrid[1]].ptr))[*]
  if (arrid[2] ne -1) then snd.gflg[slist]= (*(arrvec[arrid[2]].ptr))[*]
  if (arrid[3] ne -1) then snd.v[slist]= (*(arrvec[arrid[3]].ptr))[*]
  if (arrid[4] ne -1) then snd.v_e[slist]= (*(arrvec[arrid[4]].ptr))[*]
  if (arrid[5] ne -1) then snd.p_l[slist]= (*(arrvec[arrid[5]].ptr))[*]
  if (arrid[6] ne -1) then snd.w_l[slist]= (*(arrvec[arrid[6]].ptr))[*]
  if (arrid[7] ne -1) then snd.x_qflg[slist]= (*(arrvec[arrid[7]].ptr))[*]
  if (arrid[8] ne -1) then snd.phi0[slist]= (*(arrvec[arrid[8]].ptr))[*]
  if (arrid[9] ne -1) then snd.phi0_e[slist]= (*(arrvec[arrid[9]].ptr))[*]

  st=DataMapFreeScalar(sclvec)
  st=DataMapFreeArray(arrvec)

  return,s
end


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       SndWrite
;
; PURPOSE:
;       Write a record from a sounding file.
;
;
; CALLING SEQUENCE:
;       status = SndWrite(unit,snd)
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

function SndWrite,unit,snd

  s=DataMapMakeScalar('radar.revision.major',snd.radar_revision.major,sclvec)
  s=DataMapMakeScalar('radar.revision.minor',snd.radar_revision.minor,sclvec)
  s=DataMapMakeScalar('origin.code',snd.origin.code,sclvec)
  s=DataMapMakeScalar('origin.time',snd.origin.time,sclvec)
  s=DataMapMakeScalar('origin.command',snd.origin.command,sclvec)
  s=DataMapMakeScalar('cp',snd.cp,sclvec)
  s=DataMapMakeScalar('stid',snd.stid,sclvec)
  s=DataMapMakeScalar('time.yr',snd.time.yr,sclvec)
  s=DataMapMakeScalar('time.mo',snd.time.mo,sclvec)
  s=DataMapMakeScalar('time.dy',snd.time.dy,sclvec)
  s=DataMapMakeScalar('time.hr',snd.time.hr,sclvec)
  s=DataMapMakeScalar('time.mt',snd.time.mt,sclvec)
  s=DataMapMakeScalar('time.sc',snd.time.sc,sclvec)
  s=DataMapMakeScalar('time.us',snd.time.us,sclvec)
  s=DataMapMakeScalar('nave',snd.nave,sclvec)
  s=DataMapMakeScalar('lagfr',snd.lagfr,sclvec)
  s=DataMapMakeScalar('smsep',smd.smsep,sclvec)
  s=DataMapMakeScalar('noise.search',smd.noise.search,sclvec)
  s=DataMapMakeScalar('noise.mean',snd.noise.mean,sclvec)
  s=DataMapMakeScalar('channel',snd.channel,sclvec)
  s=DataMapMakeScalar('bmnum',snd.bmnum,sclvec)
  s=DataMapMakeScalar('bmazm',snd.bmazm,sclvec)
  s=DataMapMakeScalar('scan',snd.scan,sclvec)
  s=DataMapMakeScalar('rxrise',snd.rxrise,sclvec)
  s=DataMapMakeScalar('intt.sc',snd.intt.sc,sclvec)
  s=DataMapMakeScalar('intt.us',snd.intt.us,sclvec)
  s=DataMapMakeScalar('nrang',snd.nrang,sclvec)
  s=DataMapMakeScalar('frang',snd.frang,sclvec)
  s=DataMapMakeScalar('rsep',snd.rsep,sclvec)
  s=DataMapMakeScalar('xcf',snd.xcf,sclvec)
  s=DataMapMakeScalar('tfreq',snd.tfreq,sclvec)
  s=DataMapMakeScalar('noise.sky',snd.sky_noise,sclvec)
  s=DataMapMakeScalar('combf',snd.combf,sclvec)
  s=DataMapMakeScalar('snd.revision.major',snd.snd_revision.major,sclvec)
  s=DataMapMakeScalar('snd.revision.minor',snd.snd_revision.minor,sclvec)

  q=snd.qflg[0:snd.nrang-1]+snd.x_qflg[0:snd.nrang-1]

  slist=fix(where(q gt 0,count))

  if (count ne 0) then begin
    s=DataMapMakeArray('slist',slist,arrvec)
    s=DataMapMakeArray('qflg',snd.qflg[slist],arrvec)
    s=DataMapMakeArray('gflg',snd.gflg[slist],arrvec)
    s=DataMapMakeArray('v',snd.v[slist],arrvec)
    s=DataMapMakeArray('v_e',snd.v_e[slist],arrvec)
    s=DataMapMakeArray('p_l',snd.p_l[slist],arrvec)
    s=DataMapMakeArray('w_l',snd.w_l[slist],arrvec)
    s=DataMapMakeArray('x_qflg',snd.x_qflg[slist],arrvec)
    s=DataMapMakeArray('phi0',snd.phi0[slist],arrvec)
    s=DataMapMakeArray('phi0_e',snd.phi0_e[slist],arrvec)
endif

  s=DataMapWrite(unit,sclvec,arrvec)

  s=DataMapFreeScalar(sclvec)
  s=DataMapFreeArray(arrvec)

  return,s
end


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       SndGetTime
;
; PURPOSE:
;       Extract the time from a sounding record.
;
;
; CALLING SEQUENCE:
;       status = SndGetTime(sclvec)
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

function SndGetTime,sclvec
 
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
;       SndSeek
;
; PURPOSE:
;       Find a record in a sounding file.
;
;
; CALLING SEQUENCE:
;       status = SndSeek(unit,yr,mo,dy,hr,mt,sc,atme=atime)
;
;       This function searchs for a record in a sounding file.
;
;       The returned value is zero on success,
;       or  -1 for failure
;
;-----------------------------------------------------------------
;

function SndSeek,unit,yr,mo,dy,hr,mt,sc,atme=atme

  tval=TimeYMDHMSToEpoch(yr,mo,dy,hr,mt,sc);

  s=DataMapRead(unit,sclvec,arrvec)
  fptr=0L
  if (n_elements(sclvec) ne 0) then begin
    tfile=SndGetTime(sclvec)
    st=DataMapFreeScalar(sclvec)
    st=DataMapFreeArray(arrvec)
    if (tfile gt tval) then point_lun,unit,0 $
    else begin
       stat=fstat(unit)
       fptr=long(stat.cur_ptr)
   endelse
  endif else point_lun,unit,0
  atme=tfile
  repeat begin
    stat=fstat(unit)
    s=DataMapRead(unit,sclvec,arrvec)
    if (s eq -1) then break
    tfile=SndGetTime(sclvec)
   
    st=DataMapFreeScalar(sclvec)
    st=DataMapFreeArray(arrvec)
    if (tval ge tfile) then begin
      fptr=stat.cur_ptr
      atme=tfile
    endif
  endrep until (tval lt tfile)
  if (tval gt tfile) then return,-1
  point_lun,unit,fptr

  return,0 
end


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       SndOpen
;
; PURPOSE:
;       Open a sounding file.
;
;
; CALLING SEQUENCE:
;       unit = SndOpen(fname,/read,/write,/update)
;
;       This function opens a sounding file.
;
;       The returned value is the logical unit number on success,
;       or  -1 for failure
;
;
;
;-----------------------------------------------------------------
;

function SndOpen,fname,read=read,write=write,update=update
  if KEYWORD_SET(read) then $
    openr,unit,fname,/GET_LUN,/SWAP_IF_BIG_ENDIAN
  if KEYWORD_SET(write) then $
     openw,unit,fname,/GET_LUN,/SWAP_IF_BIG_ENDIAN
  if KEYWORD_SET(update) then $
     openu,unit,fname,/GET_LUN,/SWAP_IF_BIG_ENDIAN
  return, unit
end


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       SndClose
;
; PURPOSE:
;       Close a sounding file.
;
;
; CALLING SEQUENCE:
;       s = SndClose(unit)
;
;       This function closes a sounding file.
;
;       The returned value is zero on success,
;       or  -1 for failure
;
;-----------------------------------------------------------------
;

function SndClose,unit

  free_lun,unit
  return,0

end
