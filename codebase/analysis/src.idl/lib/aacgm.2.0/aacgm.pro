; aacgm.pro
; =========
; Author: R.J.Barnes
;
; LICENSE AND DISCLAIMER
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
; 
; 
;
; Public Functions:
; -----------------
;
; AACGMConvert
; AACGMLoadCoef
; AACGMInit
;
; Private Functions:
; ------------------
;
; AACGMSgn
; AACGMRylm
; AACGMAltitudeToCGM
; AACGMCGMToAltitude
; AACGMConvertGeoCoord

;
; ---------------------------------------------------------------

; Common blocks for the AACGM algorithm

common AACGMCom,coef,cint,height_old,first_coeff_old

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       AACGMSgn
;
; PURPOSE:
;       AACGM Math function
;       
;
; CALLING SEQUENCE:
;       AACGMSgn,a,b
;     
;-----------------------------------------------------------------
;

function AACGMSgn,a,b
 if (a ge 0) then x=a else x=-a
 if (b ge 0) then return, x
 return, -x
end


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       AACGMRylm
;
; PURPOSE:
;       AACGM Math function
;       
;
; CALLING SEQUENCE:
;       AACGMRylm,colat,lon,order,ylmval
;     
;-----------------------------------------------------------------
;
 
pro AACGMRylm,colat,lon,order,ylmval

  cos_theta = cos(colat)
  sin_theta = sin(colat)

  cos_lon = cos(lon)
  sin_lon = sin(lon)

  d1 = -sin_theta;
  z2=complex(cos_lon,sin_lon)
  z1=d1*z2
  q_fac=z1

  ylmval(0)=1;
  ylmval(2)=cos_theta;

  for l=1,order-1 do begin
    la = (l - 1) * l + 1
    lb = l * (l + 1) + 1
    lc = (l + 1) * (l + 2) + 1;

    ca =float(l * 2. + 1.) / (l + 1.)
    cb= float(l)/(l + 1.)
    ylmval(lc-1) = ca * cos_theta * ylmval(lb-1) - cb * ylmval(la-1);
  end

  q_val=q_fac;
  ylmval(3)=float(q_val)
  ylmval(1)=-imaginary(q_val)
  for l=2,order do begin
    d1 = l*2 - 1.
    z2=d1*q_fac
    z1=z2*q_val
    q_val=z1
    
    la = l*l + (2*l) + 1
    lb = l*l + 1;

    ylmval(la-1) = float(q_val)
    ylmval(lb-1) = -imaginary(q_val)

  end

  for l=2,order do begin
     la = l*l
     lb = l*l - 2*(l - 1)
     lc = l*l + (2*l);
     ld = l*l + 2

     fac = l*2 - 1
     ylmval(lc-1) = fac * cos_theta * ylmval(la-1)
     ylmval(ld-1) = fac * cos_theta * ylmval(lb-1)
   end

   for m=1,order-2 do begin
      la = (m+1)*(m+1)
      lb = (m+2)*(m+2)-1
      lc = (m+3)*(m+3)-2
	
      ld = la - (2*m)
      ldd = lb - (2*m)
      lf = lc - (2*m)

      for l=m+2,order do begin
        ca=float(2.*l-1)/(l-m)
        cb=float(l+m-1.)/(l-m)

        ylmval(lc-1) = ca * cos_theta *ylmval(lb-1) - cb *ylmval(la-1);
	ylmval(lf-1) = ca * cos_theta *ylmval(ldd-1) - cb *ylmval(ld-1);

	la = lb
        lb = lc
        lc = lb + (2*l) + 2

        ld = la - (2*m)
	ldd = lb - (2*m)
	lf = lc - (2*m)
      end
   end 
   return
end


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       AACGMAltitudeToCGM
;
; PURPOSE:
;       AACGM Coordinate transform function
;       
;
; CALLING SEQUENCE:
;       AACGMAltitudeToCGM,r_height_in,r_lat_alt,r_lat_adj
;     
;-----------------------------------------------------------------
;

pro AACGMAltitudeToCGM, r_height_in, r_lat_alt, r_lat_adj
   eradius=6371.2
   eps=1e-9
   unim=0.9999999;

  r1 = cos(!PI*r_lat_alt/180.0)
  ra = r1 * r1
  if (ra lt eps) then ra = eps
  r0 = (r_height_in/eradius+1) / ra
  if (r0 lt unim) then r0 = unim
  
  r1 = acos(sqrt(1/r0));
  r_lat_adj= AACGMSgn(r1, r_lat_alt)*180.0/!PI;
  return
end


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       AACGMCGMToAltitude
;
; PURPOSE:
;       AACGM Coordinate transform function
;       
;
; CALLING SEQUENCE:
;       AACGMCGMToAltitude,r_height_in,r_lat_alt,r_lat_adj,error
;     
;-----------------------------------------------------------------
;

pro AACGMCGMToAltitude, r_height_in,r_lat_in,  r_lat_adj, error
  eradius=6371.2
  unim=1
  error=0
  r1 = cos(!PI*r_lat_in/180.0);
  ra = (r_height_in/ eradius+1)*(r1*r1);
  if (ra gt unim) then begin
    ra = unim;
    error=1;
  end

  r1 = acos(sqrt(ra));
  r_lat_adj = AACGMSgn(r1,r_lat_in)*180.0/!PI;
  return
end


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       AACGMConvertGeoCoord
;
; PURPOSE:
;       Convert to and from AACGM and Geographic
;       
;
; CALLING SEQUENCE:
;       AACGMConvertGeoCoord,lat_in,lon_in,height_in,lat_out,lon_out,
;                            order,error,geo=geo
;
;
;       This procedure implements the AACGM coordinate transform
;
;     
;-----------------------------------------------------------------
;

pro AACGMConvertGeoCoord, lat_in,lon_in,height_in,lat_out,lon_out, $
                       order,error, geo=geo

  common AACGMCom,coef,cint,height_old,first_coeff_old
  if (n_elements(coef) eq 0) then AACGMDefault

  flag=keyword_set(GEO);
  
  if lon_in lt 0 then lon_in=lon_in+360
  if (first_coeff_old ne coef(0,0,0,0)) then height_old=[-1.0, -1.0]
  first_coeff_old=coef(0,0,0,0)

  error=-2
  if ((height_in lt 0) or (height_in gt 7200)) then return
  error=-8;
  if (abs(lat_in) gt 90.) then return
  error=-16
  if ((lon_in lt 0) or (lon_in gt 360)) then return
   
  if (height_in ne height_old(flag)) then begin
      alt_var= height_in/7200.0;
      alt_var_sq = alt_var * alt_var;
      alt_var_cu = alt_var * alt_var_sq;
      alt_var_qu = alt_var * alt_var_cu;

      for i=0,2 do begin
        for j=0,120 do begin 
          cint(j,i,flag) =coef(j,i,0,flag)+ $
                coef(j,i,1,flag)*alt_var+ $
                coef(j,i,2,flag)*alt_var_sq+ $
                coef(j,i,3,flag)*alt_var_cu+ $
                coef(j,i,4,flag)*alt_var_qu 
	end
      end
      height_old(flag) = height_in;
  end
  x=0.
  y=0.
  z=0.
 
  lon_input =lon_in*!pi/180.0; 
    
  if (flag eq 0) then colat_input = (90.-lat_in)*!pi/180.0 $
  else begin
   error=-64
    AACGMCGMToAltitude,height_in, lat_in,lat_adj,errflg
    if (errflg ne 0) then return;
    colat_input= (90. - lat_adj)*!pi/180.0;
  end
  ylmval=fltarr(121);
  AACGMRylm,colat_input,lon_input,order,ylmval;

  for l = 0, order do begin
     for m = -l,l do begin
       k = l * (l+1) + m+1;
      
       x=x+cint(k-1,0,flag)*ylmval(k-1)
       y=y+cint(k-1,1,flag)*ylmval(k-1)
       z=z+cint(k-1,2,flag)*ylmval(k-1)
     end
   end
   error=-32
   r = sqrt(x * x + y * y + z * z)
   if ((r lt 0.9) or (r gt 1.1)) then return
   
   z=z / r
   x=x / r
   y=y / r

   if (z ge 1.) then colat_temp=0 $ 
   else if (z lt -1.) then colat_temp =!pi $
   else colat_temp= acos(z)
  
   if ((abs(x) lt 1e-8) and (abs(y) lt 1e-8)) then lon_temp =0 $
   else lon_temp = atan(y,x)

   lon_output = lon_temp

   if (flag eq 0) then begin
     lat_alt =90 - colat_temp*180.0/!pi;
     AACGMAltitudeToCGM,height_in, lat_alt,lat_adj
     colat_output = (90. - lat_adj) * !pi/180.0;
   end else colat_output = colat_temp

   lat_out =90. - colat_output*180.0/!pi
   lon_out  =lon_output*180.0/!pi
   error=0  
  return
end


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       AACGMConvert
;
; PURPOSE:
;       Convert to and from AACGM and Geographic
;       
;
; CALLING SEQUENCE:
;       s=AACGMConvert(lat_in,lon_in,height_in,lat_out,lon_out,geo=geo)
;
;
;       This function implements the AACGM Transform
;
;     
;-----------------------------------------------------------------
;

function AACGMConvert,in_lat,in_lon,height,out_lat,out_lon,r, geo=geo
   
   geo=keyword_set(geo)
   if N_ELEMENTS(in_lat) ne 1 then begin
      n=N_ELEMENTS(in_lat)
      sze=SIZE(in_lat)
      out_lat=dblarr(sze[1:sze[0]])
      out_lon=dblarr(sze[1:sze[0]])
      r=dblarr(sze[1:sze[0]])
      tmp_lat=0.0
      tmp_lon=0.0
      for i=0,n-1 do begin 
        AACGMConvertGeoCoord,in_lat[i],in_lon[i],height[i], $
                             tmp_lat,tmp_lon,10,error,geo=geo
        out_lat[i]=tmp_lat
        out_lon[i]=tmp_lon
        r[i]=1.0
      end
   end else begin
     out_lat=0.
     out_lon=0.
     AACGMConvertGeoCoord,in_lat,in_lon,height,out_lat,out_lon,10,error,geo=geo
     r=1.0
   endelse

   return, 0
end


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       AACGMLoadCoef
;
; PURPOSE:
;       Load a set of coefficients from an open file.
;       
;
; CALLING SEQUENCE:
;       s=AACGMLoadCoef(unit)
;
;
;       This function loads a coefficient file.
;
;     
;-----------------------------------------------------------------
;

function AACGMLoadCoef,unit
ON_IOERROR, iofail

 common AACGMCom,coef,cint,height_old,first_coeff_old  
 coef=fltarr(121,3,5,2)
 readf, unit,coef
 cint=fltarr(121,3,2)
 height_old=[-1.,-1.]
 first_coeff_old=-1.
 sol_dec_old=0
 told=1e12
 mslon1=0
 mslon2=0
 return, 0

iofail:
  return, -1
end


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       AACGMInit
;
; PURPOSE:
;       Load a set of coefficients for the input year.
;       
;
; CALLING SEQUENCE:
;       s=AACGMInit(year)
;
;
;       This function calls AACGMLoadCoef to load a coefficient
;       file for the four-digit input year.
;
;     
;-----------------------------------------------------------------
;

function AACGMInit, year

    prefix = GETENV('AACGM_DAT_PREFIX')
        
    valid_years = [1975,1980,1985,1990,1995,2000,2005,2010]
    test = where(year eq valid_years)
    if test[0] eq -1 then begin
        if (year ge 2010) then year_str = '2010'
        if (year ge 2005) and (year lt 2010) then year_str = '2005'
        if (year ge 2000) and (year lt 2005) then year_str = '2000'
        if (year ge 1995) and (year lt 2000) then year_str = '1995'
        if (year ge 1990) and (year lt 1995) then year_str = '1990'
        if (year ge 1985) and (year lt 1990) then year_str = '1985'
        if (year ge 1980) and (year lt 1985) then year_str = '1980'
        if (year lt 1980) then year_str = '1975'
    endif else $
        year_str = strtrim(year,2)
    
    file_str = prefix + year_str + '.asc'
    
    openr, unit, file_str, /get_lun, /stdio
    c = AACGMLoadCoef(unit)
    free_lun, unit

    return, c
end

