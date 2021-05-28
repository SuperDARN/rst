; aacgmlib.pro
; ============
; Author: K.Baker, S.Shepherd & R.J.Barnes
; 
; Copyright (c) 2012 The Johns Hopkins University/Applied Physics Laboratory
; 
; This file is part of the Radar Software Toolkit (RST).
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
; along with this program.  If not, see <https://www.gnu.org/licenses/>.
; 
; Modifications:
;
; PublicFunctions
; ---------------
; cnv_geo_gsm
; cnv_gsm_geo
; cnv_gse_gsm
; cnv_gsm_gse
; cnv_sm_gsm
; cnv_gsm_sm
;
; ---------------------------------------------------------------



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; NAME:
;       cnv_geo_gsm
;
; PURPOSE:
;
;         Converts the GEO to GSM coordinate or GSM to GEO.
;
; CALLING SEQUENCE:
;
;	  cnv_geo_gsm,syear,smonth,sday,shour,smin,ssec,bgeo,bgsm,s
;
; OR: 
;
;         cnv_geo_gsm,syear,smonth,sday,shour,smin,ssec,bgsm,bgeo,s,/rev
;
;         Converts from GEO to GSM or GSM to GEO for the specified
;         time. The arguments bgsm and bgeo are three element, double arrays
;         containing the coordinates of the point.
;         If the keyword /rev is specified the inverse transform is
;         performed.
;
;      
;         s  is 0 if everything is ok and a non-zero error code otherwise
;-------------------------------------------------------------------------



pro cnv_geo_gsm,syear,smonth,sday,shour,smin,ssec,bgeo,bgsm,s,rev=rev

  syear = fix(syear)
  smonth = fix(smonth)
  sday = fix(sday)
  shour = fix(shour)
  smin = fix(smin)
  ssec = fix(ssec)
  bgeo = double(bgeo)
  bgsm = double(bgsm)


  if not keyword_set(rev) then $
    s=GEOPGeoGsm(syear,smonth,sday,shour,smin,ssec,bgeo,bgsm) $
   else  $
    s=GEOPGsmGeo(syear,smonth,sday,shour,smin,ssec,bgsm,bgeo) 

  return
end

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; NAME:
;       cnv_gsm_geo
;
; PURPOSE:
;
;         Converts the GSM to GEO coordinate or GEO to GSM.
;
; CALLING SEQUENCE:
;
;	  cnv_gsm_geo,syear,smonth,sday,shour,smin,ssec,bgsm,bgeo,s
;
; OR: 
;
;         cnv_gsm_geo,syear,smonth,sday,shour,smin,ssec,bgeo,bgsm,s,/rev
;
;         Converts from GSM to GEO or GEO to GSM for the specified
;         time. The arguments bgsm and bgeo are three element, double arrays
;         containing the coordinates of the point.
;         If the keyword /rev is specified the inverse transform is
;         performed.
;
;      
;         s  is 0 if everything is ok and a non-zero error code otherwise
;-------------------------------------------------------------------------



pro cnv_gsm_geo,syear,smonth,sday,shour,smin,ssec,bgsm,geo,s,rev=rev

  syear = fix(syear)
  smonth = fix(smonth)
  sday = fix(sday)
  shour = fix(shour)
  smin = fix(smin)
  ssec = fix(ssec)
  bgeo = double(bgeo)
  bgsm = double(bgsm)


  if not keyword_set(rev) then $
    s=GEOPGsmGeo(syear,smonth,sday,shour,smin,ssec,bgsm,bgeo) $
   else  $
    s=GEOPGeoGso(syear,smonth,sday,shour,smin,ssec,bgeo,bgsm) 

  return
end


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; NAME:
;       cnv_gse_gsm
;
; PURPOSE:
;
;         Converts the GSE to GSM coordinate or GSM to GSE.
;
; CALLING SEQUENCE:
;
;	  cnv_gse_gsm,syear,smonth,sday,shour,smin,ssec,bgse,bgsm,s
;
; OR: 
;
;         cnv_gse_gsm,syear,smonth,sday,shour,smin,ssec,bgsm,bgse,s,/rev
;
;         Converts from GSE to GSM or GSM to GSE for the specified
;         time. The arguments bgse and bgsm are three element, double arrays
;         containing the coordinates of the point.
;         If the keyword /rev is specified the inverse transform is
;         performed.
;
;      
;         s  is 0 if everything is ok and a non-zero error code otherwise
;-------------------------------------------------------------------------





pro cnv_gse_gsm,syear,smonth,sday,shour,smin,ssec,bgse,bgsm,s,rev=rev

  syear = fix(syear)
  smonth = fix(smonth)
  sday = fix(sday)
  shour = fix(shour)
  smin = fix(smin)
  ssec = fix(ssec)
  bgse = double(bgse)
  bgsm = double(bgsm)


  if not keyword_set(rev) then $
    s=GEOPGseGsm(syear,smonth,sday,shour,smin,ssec,bgse,bgsm) $
   else  $
    s=GEOPGsmGse(syear,smonth,sday,shour,smin,ssec,bgsm,bgse) 

  return
end

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; NAME:
;       cnv_gsm_gse
;
; PURPOSE:
;
;         Converts the GSM to GSE coordinate or GSE to GSM.
;
; CALLING SEQUENCE:
;
;	  cnv_gsm_gse,syear,smonth,sday,shour,smin,ssec,bgsm,bgse,s
;
; OR: 
;
;         cnv_gsm_gse,syear,smonth,sday,shour,smin,ssec,bgse,bgsm,s,/rev
;
;         Converts from GSM to GSE or GSE to GSM for the specified
;         time. The arguments bgsm and bgse are three element, double arrays
;         containing the coordinates of the point.
;         If the keyword /rev is specified the inverse transform is
;         performed.
;
;      
;         s  is 0 if everything is ok and a non-zero error code otherwise
;-------------------------------------------------------------------------





pro cnv_gsm_gse,syear,smonth,sday,shour,smin,ssec,bgsm,bgse,s,rev=rev

  syear = fix(syear)
  smonth = fix(smonth)
  sday = fix(sday)
  shour = fix(shour)
  smin = fix(smin)
  ssec = fix(ssec)
  bgse = double(bgse)
  bgsm = double(bgsm)


  if not keyword_set(rev) then $
    s=GEOPGsmGse(syear,smonth,sday,shour,smin,ssec,bgsm,bgse) $
   else  $
    s=GEOPGseGsm(syear,smonth,sday,shour,smin,ssec,bgse,bgsm) 

  return
end

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; NAME:
;       cnv_gsm_sm
;
; PURPOSE:
;
;         Converts the GSM to SM coordinate or SM to GSM.
;
; CALLING SEQUENCE:
;
;	  cnv_gsm_sm,syear,smonth,sday,shour,smin,ssec,bgsm,bsm,s
;
; OR: 
;
;         cnv_gsm_sm,syear,smonth,sday,shour,smin,ssec,bsm,bgsm,s,/rev
;
;         Converts from GSM to SM or SM to GSM for the specified
;         time. The arguments bgsm and bsm are three element, double arrays
;         containing the coordinates of the point.
;         If the keyword /rev is specified the inverse transform is
;         performed.
;
;      
;         s  is 0 if everything is ok and a non-zero error code otherwise
;-------------------------------------------------------------------------



pro cnv_gsm_sm,syear,smonth,sday,shour,smin,ssec,bgsm,bsm,s,rev=rev

  syear = fix(syear)
  smonth = fix(smonth)
  sday = fix(sday)
  shour = fix(shour)
  smin = fix(smin)
  ssec = fix(ssec)
  bsm = double(bsm)
  bgsm = double(bgsm)

  if not keyword_set(rev) then $
    s=GEOPGsmSm(syear,smonth,sday,shour,smin,ssec,bgsm,bsm) $
   else  $
    s=GEOPSmGsm(syear,smonth,sday,shour,smin,ssec,bsm,bgsm) 
  return
end

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; NAME:
;       cnv_sm_gsm
;
; PURPOSE:
;
;         Converts the SM to GSM coordinate or GSM to SM.
;
; CALLING SEQUENCE:
;
;	  cnv_sm_gsm,syear,smonth,sday,shour,smin,ssec,bsm,bgsm,s
;
; OR: 
;
;         cnv_sm_gsm,syear,smonth,sday,shour,smin,ssec,bgsm,bsm,s,/rev
;
;         Converts from SM to GSM or GSM to SM for the specified
;         time. The arguments bsm and bgsm are three element, double arrays
;         containing the coordinates of the point.
;         If the keyword /rev is specified the inverse transform is
;         performed.
;
;      
;         s  is 0 if everything is ok and a non-zero error code otherwise
;-------------------------------------------------------------------------



pro cnv_sm_gsm,syear,smonth,sday,shour,smin,ssec,bsm,bgsm,s,rev=rev

  syear = fix(syear)
  smonth = fix(smonth)
  sday = fix(sday)
  shour = fix(shour)
  smin = fix(smin)
  ssec = fix(ssec)
  bsm = double(bsm)
  bgsm = double(bgsm)

  if not keyword_set(rev) then $
    s=GEOPSmGsm(syear,smonth,sday,shour,smin,ssec,bsm,bgsm) $
   else  $
    s=GEOPGsmSm(syear,smonth,sday,shour,smin,ssec,bgsm,bsm) 

  return
end
