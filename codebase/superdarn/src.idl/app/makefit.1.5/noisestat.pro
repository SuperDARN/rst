; noisestat.pro
; =============
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
; FitACFNoiseStat


; ---------------------------------------------------------------



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       FitACFNoiseStat
;
; PURPOSE:
;       Unkown.
;       
;
; CALLING SEQUENCE:
;
;        sigma=FitACFNoiseStat(mnpwr,prm,nbad,badsmp,acfd,signal)
;
;
;-----------------------------------------------------------------
;



function FitACFNoiseStat,mnpwr,prm,nbad,badsmp,acfd,signal

  PLIMC=1.6D
  ROOT_3=1.7D

  plim = PLIMC * mnpwr

  q=where((double(acfd[*,0]) le plim) and (double(acfd[*,0]) gt 0.0D),c)

  npt=0
  np0=0
  P=0.0D
  P2=0.0D
  for i=0,c-1 do begin

     FitACFRngOverLap,prm.mppul,prm.mpinc,prm.smsep, $
                      prm.pulse,roverlap


     FitACFCkRng,q[i],prm.mplgs,prm.mpinc,prm.smsep,prm.lag, $
                        nbad,badsmp,bdlag

     FitACFLagOverLap,q[i],prm.nave,prm.nrang, $
                   prm.mppul,prm.mplgs,roverlap, $
                   prm.pwr0,prm.lag, $
                   prm.pulse,bdlag

     np0++
     fluct=double(acfd[q[i],0])/sqrt(prm.nave)

     low_lim=double(acfd[q[i],0])-2.0*fluct

     if (low_lim lt 0) then low_lim=low_lim+fluct

     high_lim=double(acfd[q[i],0])+fluct    
       
     ql=where((bdlag[1:prm.mplgs] eq 0) and $
              (abs(acfd[q[i],1:prm.mplgs-1]) ge low_lim) and $
              (abs(acfd[q[i],1:prm.mplgs-1]) le high_lim),cl)
     if (cl gt 0) then begin
       P=P+total(abs(acfd[q[i],ql+1]))
       P2=P2+total(abs(acfd[q[i],ql+1])^2)
       npt=npt+cl
     endif
 endfor

  if (npt lt 2) then begin
    signal=0
    return, plim/sqrt(double(prm.nave))
  endif
 

  P = P/npt;
  var = (P2 - P*P*npt)/(double(npt-1))

  sigma=0.0D
  if (var gt 0.0D) then sigma=sqrt(var)

  signal=0.0D

  if ((P ge sigma*ROOT_3) and (sigma gt 0.0)) then signal=P
 

  if (P gt sigma) then  return, P
  return, sigma
end
