; groundscatter.pro
; =================
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
; FitACFGroundScatter


; ---------------------------------------------------------------





;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       FitACFGroundScatter
;
; PURPOSE:
;       Unknown.
;       
;
; CALLING SEQUENCE:
;        gflg=FitACFGroundScatter(ptr)
;
;
;
;-----------------------------------------------------------------
;



; This version of the ground-scatter detection algorithm is based
; on a statistical analysis of ground-scatter vs ionospheric scatter
; done by Stefan Sundeen, Gerard Blanchard and Kile Baker.
;
; The analysis inluded all data from 12 days (with large amounts of both
; ionospheric scatter and ground scatter) distributed over all seasons of
; the year.  Two radars were used in the analysis, Saskatoon and Kapuskasing.
;
; The result of the initial statistical analysis showed that scatter was 
; most likely ground scatter if
;
; abs(v) < GS_VMAX - (GS_VMAX/GS_WMAX)*abs(width)
;
; Let g(vabs,wabs) = (abs(v) - GS_VMAX - (GS_VMAX/GS_WMAX)*abs(width))
;
; Then, if was assume the errors in v and width are independent,
; we can estimate the error in the function g to be:
;
; g_err/g = sqrt((v_err/vabs)^2 + (width_err/width)^2)
;
; Then the final condition for flagging data as ground scatter is:
;
;if g - g_err <= 0 then flag as ground scatter.


function FitACFGroundScatter,ptr


  GS_VMAX=30.0d
  GS_WMAX=90.0d


; This is the old version.  No one seems to remember why
;  it was done this way.
;
;  vabs = ((fabs(ptr->v) - ptr->v_err) > 0.0) ? 
;          (fabs(ptr->v) - ptr->v_err) : 0.0;
;
;  wabs = ((fabs(ptr->w_l) - ptr->w_l_err) > 0.0) ? 
;          (fabs(ptr->w_l) - ptr->w_l_err) : 0.0;
;
;  if ((vabs < GS_VMAX) && (wabs < GS_WMAX)) return 1;

; Here is the Sundeen-Blanchard-Baker version */

  
  vabs = abs(ptr.v)
  wabs = abs(ptr.w_l)

; Trap to make sure that we don't have zero values

  if (vabs eq 0.0d) then return, 1
  if (wabs eq 0.0d) then return, 1

  g = (vabs - (GS_VMAX - (GS_VMAX/GS_WMAX)*wabs))

; don't allow the error to get too extreme.  If v_err/v or
; w_err/w is greater than 1 (100% error) just use 1.0 

  if ((ptr.v_err/vabs) gt 1.0) then dv=1.0d $
  else dv=ptr.v_err/vabs

  if ((ptr.w_l_err/wabs) gt 1.0) then dw=1.0d $
  else dw=ptr.w_l_err/wabs



;  g_err = abs(g)*sqrt(dv*dv + dw*dw)

  g_err = 0 ; turn off any correction for the error 

  if ((g - g_err) lt 0.0) then return,1 $
  else return,0

end
