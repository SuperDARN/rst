; astalg.pro
; ==========
; Author: R.J.Barnes (Based on C routines by Kile Baker)
;
; Copyright (c) 2012 The Johns Hopkins University/Applied Physics Laboratory
;
; This file is part of the Radar Software Toolkit (RST).
;
; RST is free software: you can redistribute it and/or modify
; it under the terms of the GNU General Public License as published by
; the Free Software Foundation, either version 3 of the License, or
; any later version.
;
; This program is distributed in the hope that it will be useful,
; but WITHOUT ANY WARRANTY; without even the implied warranty of
; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
; GNU General Public License for more details.
;
; You should have received a copy of the GNU General Public License
; along with RST.  If not, see <http://www.gnu.org/licenses/>.
;

; Public Functions:
; -----------------

; #AstAlg_apparent_obliquity
; AstAlg_apparent_solar_longitude
; #AstAlg_dday
; AstAlg_equation_of_time
; AstAlg_geometric_solar_longitude
; #AstAlg_jde
; #AstAlg_jde2calendar
; #AstAlg_lunar_ascending_node
; #AstAlg_mean_lunar_longitude
; #AstAlg_mean_obliquity
; #AstAlg_mean_solar_anomaly
; AstAlg_mean_solar_longitude
; AstAlg_nutation_corr
; AstAlg_solar_declination
; AstAlg_solar_right_ascension

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       AstAlg_dday
;
; PURPOSE:
;       Convert DHMS to decimal day.
;       
;
; CALLING SEQUENCE:
;       dday = AstAlg_dday(day,hour,minute,second)
;
;       All the arguments must be given. 
;
;       Day is the day of month ranging from 0-31.
;       The returned value is the decimal day of month.
;
;-----------------------------------------------------------------

function AstAlg_dday,day,hour,minute,second, dday_bug=dday_bug

  if keyword_set(dday_bug) then $
    return, double(day+hour/24.0D + minute/60.0D +second/3600.0D)

  ; 20150204 SGS : this is an error found by Nathaniel Frissel
  ;                which introduces up to one hour of error in
  ;                the actual time.
  return, double(day + (hour + minute/60.d +second/3600.d)/24.d)
end

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       AstAlg_jde
;
; PURPOSE:
;       Convert year,month, decimal day to Julian date.
;       
;
; CALLING SEQUENCE:
;       jde = AstAlg_jde(year,month,day)
;
;       All the arguments must be given. 
;
;       The returned value is the julian date.
;
;-----------------------------------------------------------------

function AstAlg_jde,year,month,day

  ; 20150209 SGS : use local variables for year and month to prevent
  ;                modification

  yr = year
  mo = month
  if (mo le 2) then begin
    yr = year-1
    mo=mo+12
  endif

  a=fix(yr/100.0D)
  b=double(2-a + a/4)

  return, double( long(365.25D*(yr+4716.0D)) + $
                 double(long(30.6001D*(mo+1.0D)))) + day+b-1524.5D
 
end


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       AstAlg_jde2calendar
;
; PURPOSE:
;       Convert Julian date to YMDHMS
;       
;
; CALLING SEQUENCE:
;       jde = AstAlg_jde2calendar(jd,year,month,day,hour,minute,second)
;
;       All the arguments must be given. 
;
;       The returned value is zero on success.
;
;-----------------------------------------------------------------

function AstAlg_jde2calendar,jd,year,month,day,hour,minute,second


    jd = jd + 0.5D;

    z = long(jd);
    f = jd - z;

    if (z lt 2299161) then a = z $
    else begin
     alpha = long((z - 1867216.25D)/36524.25D)
     a = z + 1 + alpha - long(alpha/4.0D)
    endelse

    b = a + 1524
    c = long((b - 122.1D)/365.25D)
    d = long(365.25D * c)
    e = long((b - d)/30.6001D)

    if (e lt 14) then month = e-1 $
    else month = e-13

    if (month gt 2) then year = c - 4716 $
    else year = c - 4715

    dday = b - d - double(long(30.6001D * e)) + f

    day = fix(dday);
    resid = (dday - day) * 24.0D;

    hour = fix(resid);
    resid = (resid - hour)*60.0D;

    minute = fix(resid);
    resid = (resid - minute)*60.0D;

    second = fix(resid + 0.5)

  return, 0


end

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       AstAlg_mean_solar_anomaly
;
; PURPOSE:
;       Calculate mean solar anomaly.
;       
;
; CALLING SEQUENCE:
;       asl = AstAlg_mean_solar_anomaly(jd)
;
;       All the arguments must be given. 
;
;       The time is given as the julian date
;       The returned value is the mean solar anomaly.
;
;-----------------------------------------------------------------

function AstAlg_mean_solar_anomaly,jd


   J2000=2451545.0D

   tau=(jd-J2000)/36525.0D;

   sma = 357.5291130D + 35999.05029D * tau $
           - 0.0001537D * (tau*tau)
  
   sma = sma mod 360.0D

   if (sma lt 0.0) then sma = sma + 360.0

   return, sma
end

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       AstAlg_mean_lunar_longitude
;
; PURPOSE:
;       Calculate mean lunar longitude
;       
;
; CALLING SEQUENCE:
;       asl = AstAlg_mean_lunar_longitude(jd)
;
;       All the arguments must be given. 
;
;       The time is given as the julian date
;       The returned value is the lunar ascending node.
;
;-----------------------------------------------------------------

function AstAlg_mean_lunar_longitude,jd

   J2000=2451545.0D

   tau=(jd-J2000)/36525.0D

   llong=218.3165D + 481267.8813D * tau

   llong= llong mod 360.0D

   if (llong lt 0.0) then llong = llong + 360.0D

   return, llong
end


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       AstAlg_lunar_ascending_node
;
; PURPOSE:
;       Calculate lunar ascending node
;       
;
; CALLING SEQUENCE:
;       asl = AstAlg_lunar_ascending_node(jd)
;
;       All the arguments must be given. 
;
;       The time is given as the julian date
;       The returned value is the lunar ascending node.
;
;-----------------------------------------------------------------

function AstAlg_lunar_ascending_node,jd


   J2000=2451545.0D

   tau=(jd-J2000)/36525.0D
  
   omega = (((tau/4.50D5 + 2.0708D-3)*tau - 1.934136261D3)*tau) + $
           125.04452D

   omega = omega mod 360.0D

   if (omega lt 0.0) then omega = omega + 360.0D

   return, omega
end

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       AstAlg_mean_solar_longitude
;
; PURPOSE:
;       Calculate mean solar longitude.
;       
;
; CALLING SEQUENCE:
;       asl = AstAlg_mean_solar_longitude(jd)
;
;       All the arguments must be given. 
;
;       The time is given as the julian date
;       The returned value is the mean solar longitude.
;
;-----------------------------------------------------------------

function AstAlg_mean_solar_longitude,jd

   J2000=2451545.0D

   coefs=[280.4664567D, 360007.6982779D, $
          0.03032028D, 2.00276381406D-5, $
          -6.53594771242D-5, -0.50D-6]
 
   tau=(jd-J2000)/365250.0D

   sl = 0.0D;

   for i=5, 0, -1 do sl = tau * sl + coefs[i]   
   sl = sl mod 360.0D

   if (sl lt 0.0) then sl = sl + 360.0

   return, sl

end


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       AstAlg_mean_obliquity
;
; PURPOSE:
;       Calculate mean obliquity
;       
;
; CALLING SEQUENCE:
;       asl = AstAlg_mean_obliquity(jd)
;
;       All the arguments must be given. 
;
;       The time is given as the julian date
;       The returned value is the mean obliquity.
;
;-----------------------------------------------------------------

function AstAlg_mean_obliquity,jd

   J2000=2451545.0D

   coefs =[23.439291111111D, $
           -0.0130041666667D, $
           -1.638888889D-7, $
           5.036111111D-7] 

   tau=(jd-J2000)/36525.0D

   e0 = ((((coefs[3]*tau) + coefs[2]) $
                * tau) + coefs[1]) * tau + coefs[0]

   return,e0
end




;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       AstAlg_apparent_obliquity
;
; PURPOSE:
;       Calculate apparent obliquity
;       
;
; CALLING SEQUENCE:
;       eps = AstAlg_apparent_obliquity(jd)
;
;       All the arguments must be given. 
;
;       The time is given as the julian date
;       The returned value is the apparent obliquity.
;
;-----------------------------------------------------------------

function AstAlg_apparent_obliquity,jd
  AstAlg_DTOR = !dpi/180.d
  eps = AstAlg_mean_obliquity(jd)+ $
            0.00256D*cos(AstAlg_DTOR*AstAlg_lunar_ascending_node(jd))
   return, eps
end


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       AstAlg_geometric_solar_longitude
;
; PURPOSE:
;       Calculate geometric solar longitude.
;       
;
; CALLING SEQUENCE:
;       asl = AstAlg_geometric_solar_longitude(jd)
;
;       All the arguments must be given. 
;
;       The time is given as the julian date
;       The returned value is the geometric solar longitude.
;
;-----------------------------------------------------------------

function AstAlg_geometric_solar_longitude,jd

  AstAlg_DTOR = !dpi/180.d

  J2000 = 2451545.0D

  tau = (jd-J2000)/36525.0D
  sml = AstAlg_mean_solar_longitude(jd)
   
  sma = AstAlg_DTOR*AstAlg_mean_solar_anomaly(jd)
   
  gc = (1.914602D - 0.004817D*tau - 0.000014D*(tau*tau)) * sin(sma) $
        + (0.019993D - 0.000101D*tau) * sin(2.0D*sma) $
        + 0.000289D * sin(3.0D*sma)

  sml += gc

  sml = sml mod 360.0D

  if (sml lt 0.0) then sml += 360.0

   return, sml
end

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       AstAlg_apparent_solar_longitude
;
; PURPOSE:
;       Calculate apparent solar longitude.
;       
;
; CALLING SEQUENCE:
;       asl = AstAlg_apparent_solar_longitude(jd)
;
;       All the arguments must be given. 
;
;       The time is given as the julian date
;       The returned value is the apparent solar longitude.
;
;-----------------------------------------------------------------

function AstAlg_apparent_solar_longitude,jd

  AstAlg_DTOR = !dpi/180.d

  asl = AstAlg_geometric_solar_longitude(jd) - 0.00569D - $
            0.00478D*sin(AstAlg_DTOR*AstAlg_lunar_ascending_node(jd));

   return, asl
end

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       AstAlg_nutation_corr
;
; PURPOSE:
;       Calculate correction factors for the nutation of the Earth's
;       spin.
;       
;
; CALLING SEQUENCE:
;       asl = AstAlg_nutation_corr(jd,slong_corr,obliq_corr)
;
;       All the arguments must be given. 
;
;       The time is given as the julian date
;       The returned values are the correction factors
;
;-----------------------------------------------------------------

function AstAlg_nutation_corr,jd,slong_corr,obliq_corr

  AstAlg_DTOR = !dpi/180.d

  slong = AstAlg_DTOR * AstAlg_mean_solar_longitude(jd)
  lunlong = AstAlg_DTOR * AstAlg_mean_lunar_longitude(jd)
  omega = AstAlg_DTOR * AstAlg_lunar_ascending_node(jd)

  slong_corr = -17.20D * sin(omega) - 1.32D * sin(2.0D*slong) - $
        0.23D * sin(2.0D*lunlong) + 0.21D * sin(2.0D*omega)

  slong_corr = slong_corr/3600.0D


  obliq_corr = 9.20D * cos(omega) + 0.57D * cos(2.0D*slong) + $
         0.10D * cos(2.0D*lunlong) - 0.09D * cos(2.0D*omega)

  obliq_corr = obliq_corr/3600.0D

  return,0

end

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       AstAlg_solar_declination
;
; PURPOSE:
;       Calculate solar declination.
;       
;
; CALLING SEQUENCE:
;       asl = AstAlg_solar_declination(jd)
;
;       All the arguments must be given. 
;
;       The time is given as the julian date
;       The returned value is the solar declination.
;
;-----------------------------------------------------------------

function AstAlg_solar_declination,jd
 
  AstAlg_DTOR = !dpi/180.d

  sindec = sin(AstAlg_DTOR * AstAlg_apparent_obliquity(jd)) * $
          sin(AstAlg_DTOR * AstAlg_apparent_solar_longitude(jd))

  sdec = asin(sindec)/AstAlg_DTOR

  return, sdec

end


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       AstAlg_solar_right_ascension
;
; PURPOSE:
;       Calculate solar right_ascension.
;       
;
; CALLING SEQUENCE:
;       asl = AstAlg_solar_right_ascension(jd)
;
;       All the arguments must be given. 
;
;       The time is given as the julian date
;       The returned value is the solar right ascension.
;
;-----------------------------------------------------------------

function AstAlg_solar_right_ascension,jd
 
  AstAlg_DTOR = !dpi/180.d

  slong = AstAlg_DTOR * AstAlg_apparent_solar_longitude(jd)
   
  eps = AstAlg_DTOR * AstAlg_apparent_obliquity(jd)

  alpha = atan(cos(eps)*sin(slong),cos(slong))

  ra = alpha/AstAlg_DTOR
   
  return, ra

end

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       AstAlg_equation_of_time
;
; PURPOSE:
;       Calculate solar equation of time.
;       
;
; CALLING SEQUENCE:
;       asl = AstAlg_equation_of_time(jd)
;
;       All the arguments must be given. 
;
;       The time is given as the julian date
;       The returned value is the equation of time.
;
;-----------------------------------------------------------------

function AstAlg_equation_of_time,jd

  AstAlg_DTOR = !dpi/180.d

  sml   = AstAlg_mean_solar_longitude(jd)
  sra   = AstAlg_solar_right_ascension(jd)
  obliq = AstAlg_mean_obliquity(jd)
  s     = AstAlg_nutation_corr(jd, dpsi, deps)

  eqt = sml - 0.0057183D - sra + dpsi*cos(AstAlg_DTOR*(obliq + deps))

  eqt = eqt mod 360.0D

  eqt = 4.0D * eqt

  if (eqt gt 20.0D) then eqt = eqt - 24.0D*60.0D
  if (eqt lt -20.0D) then eqt = 24.0D*60.0D + eqt

  return, eqt

end

