; noiseacf.pro
; ============
; Author: R.J.Barnes - Based on C code by K.Baker
; 
; See license.txt
; 
; 
; 
;
;
; PublicFunctions
; ---------------
;
; FitACFNoiseACF 


; ---------------------------------------------------------------



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       FitACFNoiseACF
;
; PURPOSE:
;       Unknown.
;       
;
; CALLING SEQUENCE:
;        omega=FitACFNoiseACF(mnpwr,prm,pwrd,nbad,badsmp,acfd,ncfd)
;
;
;
;-----------------------------------------------------------------
;



function FitACFNoiseACF,mnpwr,prm,pwrd,nbad,badsmp,acfd,ncfd

  LAG_SIZE=48
  PLIMC=1.6D

  ncfd=dcomplexarr(LAG_SIZE)
  np=intarr(LAG_SIZE)
 
  plim=PLIMC*mnpwr

  q=where((pwrd lt plim) and $
          ((abs(double(acfd[*,0]))+abs(imaginary(acfd[*,0]))) gt 0.0D) and $
          ((abs(double(acfd[*,0]))) lt plim) and $
          ((abs(imaginary(acfd[*,0]))) lt plim) ,c)

  for i=0,c-1 do begin

     FitACFRngOverLap,prm.mppul,prm.mpinc,prm.smsep, $
                      prm.pulse,roverlap

     FitACFCkRng,q[i],prm.mplgs,prm.mpinc,prm.smsep,prm.lag, $
                        nbad,badsmp,bdlag

     FitACFLagOverLap,q[i],prm.nave,prm.nrang, $
                   prm.mppul,prm.mplgs,roverlap, $
                   prm.pwr0,prm.lag, $
                   prm.pulse,bdlag



     ql=where((bdlag[0:prm.mplgs-1] eq 0) and $
              (abs(double(acfd[q[i],0:prm.mplgs-1])) lt plim) and $
              (abs(imaginary(acfd[q[i],0:prm.mplgs-1])) lt plim),cl)
     if (cl gt 0) then begin
       ncfd[ql]=ncfd[ql]+acfd[q[i],ql]
       np[ql]=np[ql]+1
     endif
  endfor
  
   if (np[0] le 2) then begin
      ncfd[*]=dcomplex(0.0D,0.0D)
      return, 0.0D
   endif

   ql=where(np[0:prm.mplgs-1] gt 2,cl)

   if (cl gt 0) then ncfd[ql]=ncfd[ql]/np[ql]
 
   ql=where(np[0:prm.mplgs-1] le 2,cl)
 
   if (cl gt 0) then ncfd[ql]=dcomplex(0.0D,0.0D)
 
   P=total(abs(ncfd[1:prm.mplgs-1]))

   P=P/(prm.mplgs-1)
  
  return, P
end

