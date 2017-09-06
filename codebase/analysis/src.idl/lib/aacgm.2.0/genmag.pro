;------------------------------------------------------------------------------
;
; NAME:
;       AACGM_v2_Dayno
;
; PURPOSE:
;       Determine the day number of the given date.
;   
; CALLING SEQUENCE:
;       AACGM_v2_Dayno, yr,mo,dy, days=days
;
;     Input Arguments:  
;       yr            - 4-digit year
;       mo            - Month: 1-January, 2-February, etc.
;       dy            - Day of month, starting at 1
;
;       date inputs can be an array, but must be the same size and it is
;       assumed that each day is from the same year.
;
;     Keywords:
;       days          - set to a variable that will contain the total number of
;                       days in the given year.
;
;     Return Value:
;       dayno         - day number of the current year.
;
; HISTORY:
;
; Revision 1.0  14/06/10 SGS initial version
; 
;+-----------------------------------------------------------------------------
;

function AACGM_v2_Dayno, yr,mo,dy, days=days
  ; works on an array. assume that all from same day
; WHAT IS THE POINT OF AN ARRAY OF THE SAME DAY?!

  mdays=[0,31,28,31,30,31,30,31,31,30,31,30,31]

  nelem = n_elements(yr)
  if (yr[0] ne yr[nelem-1]) or $
    (mo[0] ne mo[nelem-1]) or $
    (dy[0] ne dy[nelem-1]) then begin
      print, ''
      print, 'Not same day in AACGM_v2_Dayno'
      print, ''
      exit
  endif

  tyr = yr[0]
  ; leap year calculation
  if tyr mod 4 ne 0 then inc=0 $
  else if tyr mod 400 eq 0 then inc=1 $
  else if tyr mod 100 eq 0 then inc=0 $
  else inc=1
  mdays[2]=mdays[2]+inc

  if keyword_set(days) then days = fix(total(mdays))

  if nelem eq 1 then $
    doy = total(mdays[0:mo[0]-1])+dy[0] $
  else $
    doy = intarr(nelem) + total(mdays[0:mo[0]-1])+dy[0]

  return, fix(doy)
end

;------------------------------------------------------------------------------
;
; NAME:
;       AACGM_v2_Date
;
; PURPOSE:
;       Determine the date from the given day number and year.
;   
; CALLING SEQUENCE:
;       AACGM_v2_Dayno, yr, dayno, mo,dy
;
;     Input Arguments:  
;       yr            - 4-digit year
;       dayno         - day number, starting at 1 for Jan 1
;
;     Output Arguments:
;       mo            - Month: 1-January, 2-February, etc.
;       dy            - Day of month, starting at 1
;
; HISTORY:
;
; Revision 1.0  14/06/10 SGS initial version
; 
;+-----------------------------------------------------------------------------
;

pro AACGM_v2_Date, yr,dayno, mo,dy

  err = 0

  mdays=[0,31,28,31,30,31,30,31,31,30,31,30,31]

  ; leap year calculation
  if yr mod 4 ne 0 then inc=0 $
  else if yr mod 400 eq 0 then inc=1 $
  else if yr mod 100 eq 0 then inc=0 $
  else inc=1
  mdays[2]=mdays[2]+inc

  tots = intarr(13)
  for k=0,12 do tots[k] = total(mdays[0:k])

  q = where(tots ge dayno, nq)
  mo = q[0]
  dy = dayno - tots[q[0]-1]

end

