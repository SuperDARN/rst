pro readcfit

  ; Open input file for reading
 

  openr,inp,'data/cfit/test.cfit',/GET_LUN,/SWAP_IF_BIG_ENDIAN,/COMPRESS


  c=0 
  vstore=fltarr(100,1440)
  while  CFitRead(inp,prm,fit) ne -1 do begin
   print, prm.time.hr,prm.time.mt,prm.time.sc
   if (prm.bmnum eq 8) then begin
     vstore[0:prm.nrang-1,c]=fit.v[0:prm.nrang-1]
     c++
     print, c
   endif
 endwhile
 surface,vstore[0:74,0:c-1]
 free_lun,inp
end


