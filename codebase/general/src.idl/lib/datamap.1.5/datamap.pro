; datamap.pro
; ===========
; Author: R.J.Barnes
; 
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
; DataMapMakeBlankArray     Make an empty entry in array vector
; DataMapMakeArray          Make an entry in an array vector and fill
;                           it with the supplied array.
; DataMapFreeArray          Free any heap variables in the array vector
; DataMapMakeBlankScalar    Make an empty entry in scalar vector
; DataMapMakeScalar         Make an entry in an array vector and fill
;                           it with the supplied array. 
; DataMapFreeScalar         Free any heap variables in the scalar vector
; DataMapRead               Read a record from a DataMap file
; DataMapWrite              Write a record to a DataMap file
;
; ---------------------------------------------------------------



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       DataMapMakeBlankArray
;
; PURPOSE:
;       Make an empty entry in the array vector.
;       
;
; CALLING SEQUENCE:
;       status = DataMapMakeBlankArray(name,type,dim,rng,arrvec,[/new])
;
;       All the arguments must be given. name is a text string that
;       is used for the array name. type is the type of the 
;       scalar, acceptable values are:
;           
;         value        element type
;     
;           1          byte
;           2          short integer
;           3          long integer
;           4          float
;           8          double precision floating point
;           9          string
;
;       dim is the number of dimensions of the array and rng 
;       is a vector that defines the range for each dimension.
;       The new array structure is appended to arrvec. 
;       If the keyword new is set, then arrvec is assumed to be 
;       unitialized and is created. 
;
;       The returned value is the current number of entries in
;       the array vector or  -1 for failure
;
;       This function is called when reading a file
;       create the structure to store the data read in.
;
;-----------------------------------------------------------------
;



function DataMapMakeBlankArray,name, type, dim, rng, arrvec,new=new 

;  Allocates a structure to store an array.
;  If the keyword "new" is set then a new vector of arrays is
;  created, otherwise the new array structure is appended to
;  "arrvec".
;  The array data is stored as a heap variable with an associated
;  pointer.

  if (rng[0] ne 0) then begin
    case type of
    1: arr={array, name:name,type:type,ptr:PTR_NEW(BYTARR(rng))}
    2: arr={array, name:name,type:type,ptr:PTR_NEW(INTARR(rng))}
    3: arr={array, name:name,type:type,ptr:PTR_NEW(LONARR(rng))}
    4: arr={array, name:name,type:type,ptr:PTR_NEW(FLTARR(rng))}
    8: arr={array, name:name,type:type,ptr:PTR_NEW(DBLARR(rng))}
    else: arr={array, name:name,type:type, $
             ptr:PTR_NEW(STRARR(rng))}
    endcase 
  endif else begin
     arr={array, name:name,type:type,ptr:PTR_NEW()}
  endelse
  if KEYWORD_SET(new) then arrvec=replicate(arr,1) $
  else arrvec=[arrvec(*),arr] 
  return, n_elements(arrvec)-1
end


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       DataMapMakeArray
;
; PURPOSE:
;       Make an entry in the array vector.
;       
;
; CALLING SEQUENCE:
;       status = DataMapMakeArray(name,obj,arrvec,[/new])
;
;       All the arguments must be given. name is a text string that
;       is used for the array name. obj is the array to add to the
;       table, the type of the elements in the 
;       array must be either byte, integer, long integer, float, 
;       double, or a string. 
;       The new array structure is appendd to arrvec. 
;       If the keyword new is set, then arrvec is assumed to be 
;       unitialized and is created. 
;
;       The returned value is the current number of entries in
;       the array vector or  -1 for failure
;
;       This function is a wrapper to DataMapMakeBlankArray and
;       performs the additional step of copying the input array
;       into the heap variable created for the array entry.
;
;-----------------------------------------------------------------
;



function DataMapMakeArray,name,obj,arrvec,new=new

  type=size(obj,/TYPE)
  case type of 
    1: type=1
    2: type=2
    3: type=3
    4: type=4
    5: type=8
    7: type=9
    else: type=-1
   endcase 
   
   if (type eq -1) then return, -1

   if (size(obj,/N_DIMENSIONS) eq 0) then begin
     tmp=obj
     case type of
       1: obj=bytarr(1)
       2: obj=intarr(1)
       3: obj=lonarr(1)
       4: obj=fltarr(1)
       5: obj=dblarr(1)
       7: obj=strarr(1)
     endcase
     obj[0]=tmp
   endif
   
   if KEYWORD_SET(new) then $
    s=DataMapMakeBlankArray(name,type,size(obj,/N_DIMENSIONS), $
                                      size(obj,/DIMENSIONS),arrvec,/new) $
  else $
    s=DataMapMakeBlankArray(name,type,size(obj,/N_DIMENSIONS), $
                                       size(obj,/DIMENSIONS),arrvec) 
  if (s ne -1) then *arrvec[s].ptr=obj 
  return, s
end


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       DataMapFreeArray
;
; PURPOSE:
;       Free any heap variables associated with an array vector.
;       
;
; CALLING SEQUENCE:
;       status = DataMapFreeArray(arrvec)
;
;       The only argument is arrvec which is the array vector.
;
;
;       The returned value is zero on success, or  -1 for failure
;
;       This function should be called to release memory resources
;       claimed when reading or writing DataMap files.
;
;-----------------------------------------------------------------
;


function DataMapFreeArray,arrvec

; Free the set of heap variables in the vector of
; arrays given by arrvec.


  q=WHERE(PTR_VALID(arrvec.ptr),count)
  if (count ne 0) then begin
    for n=0, count-1 do begin
      PTR_FREE,arrvec[q[n]].ptr
    endfor
  endif
  return ,0
end 


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       DataMapFindArray
;
; PURPOSE:
;       Finds an array/type pair in the array vector.
;       
;
; CALLING SEQUENCE:
;       status = DataMapFindArray(name,type,arrvec)
;
;       The function searches for the name and type in the 
;       array matrix. If an entry that matches both is found
;       the vector index is returned.
;
;
;       The returned value is the matched name/type index in the array
;       vector on success, or  -1 for failure
;
;-----------------------------------------------------------------
;


function DataMapFindArray,name,type,arrvec

; Finds the index of a variable/type pair in the array
;
  q=WHERE((arrvec.name eq name) and (arrvec.type eq type),count)
  if (count ne 0) then return, q[0]
  return, -1
end


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       DataMapMakeBlankScalar
;
; PURPOSE:
;       Make an empty entry in the scalar vector.
;       
;
; CALLING SEQUENCE:
;       status = DataMapMakeBlankScalar(name,type,sclvec,[/new])
;
;       All the arguments must be given. name is a text string that
;       is used for the array name. type is the type of the 
;       scalar, acceptable values are:
;           
;         value        element type
;     
;           1          byte
;           2          short integer
;           3          long integer
;           4          float
;           8          double precision floating point
;           9          string
;
;       The new scalar structure is appended to sclvec. 
;       If the keyword new is set, then arrvec is assumed to be 
;       unitialized and is created. 
;
;       The returned value is the current number of entries in
;       the scalar vector or  -1 for failure
;
;       This function is called when reading a file
;       create the structure to store the data read in.
;
;-----------------------------------------------------------------
;

function DataMapMakeBlankScalar,name, type, sclvec,new=new 

;  Allocates a structure to store a scalar.
;  If the keyword "new" is set then a new vector of scalars is
;  created, otherwise the new scalar structure is appended to
;  "sclvec".
;  The scalar data is stored as a heap variable with an associated
;  pointer.


  case type of
  1: scl={scalar, name:name,type:type,ptr:PTR_NEW(0B)}
  2: scl={scalar, name:name,type:type,ptr:PTR_NEW(0S)}
  3: scl={scalar, name:name,type:type,ptr:PTR_NEW(0L)}
  4: scl={scalar, name:name,type:type,ptr:PTR_NEW(0E)}
  8: scl={scalar, name:name,type:type,ptr:PTR_NEW(0D)}
  else: scl={scalar, name:name,type:type,ptr:PTR_NEW('')}
  endcase 
  if KEYWORD_SET(new) then sclvec=replicate(scl,1) $
  else sclvec=[sclvec(*),scl] 
  return, n_elements(sclvec)-1
end

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       DataMapMakeScalar
;
; PURPOSE:
;       Make an entry in the scalar vector.
;       
;
; CALLING SEQUENCE:
;       status = DataMapMakeScalar(name,obj,arrvec,[/new])
;
;       All the arguments must be given. name is a text string that
;       is used for the array name. obj is the scalar to add to the
;       table, the type of the scalar must be either byte,
;       integer, long integer, float, double, or a string. 
;       The new scalar structure is appended to sclvec. 
;       If the keyword new is set, then sclvec is assumed to be 
;       unitialized and is created. 
;
;       The returned value is the current number of entries in
;       the array vector or  -1 for failure
;
;       This function is a wrapper to DataMapMakeScalarArray and
;       performs the additional step of copying the input array
;       into the heap variable created for the array entry.
;
;-----------------------------------------------------------------
;

function DataMapMakeScalar,name,obj,sclvec,new=new

  type=size(obj,/TYPE)
  case type of 
    1: type=1
    2: type=2
    3: type=3
    4: type=4
    5: type=8
    7: type=9
    else: type=-1
  endcase
  if (type eq -1) then return, -1

  if KEYWORD_SET(new) then $
    s=DataMapMakeBlankScalar(name,type,sclvec,/new) $
  else $
    s=DataMapMakeBlankScalar(name,type,sclvec) 
  if (s ne -1) then *sclvec[s].ptr=obj 
  return, s
end

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       DataMapFreeScalar
;
; PURPOSE:
;       Free any heap variables associated with an scalar vector.
;       
;
; CALLING SEQUENCE:
;       status = DataMapFreeScalar(sclvec)
;
;       The only argument is sclvec which is the scalar vector.
;
;
;       The returned value is zero on success, or  -1 for failure
;
;       This function should be called to release memory resources
;       claimed when reading or writing DataMap files.
;
;-----------------------------------------------------------------
;

function DataMapFreeScalar,sclvec

; Free the set of heap variables in the vector of
; scalars given by sclvec.


  q=WHERE(PTR_VALID(sclvec.ptr),count)
  if (count ne 0) then begin
    for n=0, count-1 do begin
      PTR_FREE,sclvec[q[n]].ptr
    endfor
  endif
  return ,0
end 

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       DataMapFindScalar
;
; PURPOSE:
;       Finds an array/type pair in the array vector.
;       
;
; CALLING SEQUENCE:
;       status = DataMapFindScalar(name,type,sclvec)
;
;       The function searches for the name and type in the 
;       scalar vector. If an entry that matches both is found
;       the vector index is returned.
;
;
;       The returned value is the matched name/type index in the scalar
;       vector on success, or  -1 for failure
;
;-----------------------------------------------------------------
;


function DataMapFindScalar,name,type,sclvec

; Finds the index of a variable/type pair in the array
;
  q=WHERE((sclvec.name eq name) and (sclvec.type eq type),count)
  if (count ne 0) then return, q[0]
  return, -1
end



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       DataMapRead
;
; PURPOSE:
;       Read a record from a DataMap file.
;       
;
; CALLING SEQUENCE:
;       status = DataMapRead(unit,sclvec,arrvec)
;
;       This function reads a single record from the open file with
;       logical unit number, unit. The scalar and array vectors are
;       returned in sclvec and arrvec respectively.
;
;
;       The returned value is size in bytes of the record on success, 
;       or  -1 for failure
;
;-----------------------------------------------------------------
;


function DataMapRead,unit,sclvec,arrvec

; Read a record from the datamap file pointed to by "unit".
; The resulting vectors of scalars and arrays are returned
; in sclvec and arrvec respectively.
 
   ON_IOERROR, iofail

   byte=0B
   magic=0L
   size=0L
   snum=0L
   anum=0L
   type=0B 
   dim=0L
  
; Read the magic (ID) byte and the block size.

   readu, unit, magic,size
   
; Read the number of scalars and arrays

   readu, unit, snum,anum 

; Process the scalars

   for n=0, snum-1 do begin

; Read in the 0 terminated array of bytes that comprise the name

     c=0L
     repeat begin
        readu,unit, byte
        if (c eq 0) then bstr=replicate(byte,1) $
        else bstr=[bstr(*),byte]  
        c=c+1
     endrep until (byte eq 0)

; Convert the name to an IDL string

     name=STRING(bstr)

; Read in the type byte

     readu, unit,type

; Add this variable to the scalar vector

     if (n eq 0) then s=DataMapMakeBlankScalar(name,type,sclvec,/new) $
     else s=DataMapMakeBlankScalar(name,type,sclvec)

; Read in the scalar value
  
     case type of
       1: readu,unit,*sclvec[n].ptr
       2: readu,unit,*sclvec[n].ptr
       3: readu,unit,*sclvec[n].ptr
       4: readu,unit,*sclvec[n].ptr
       8: readu,unit,*sclvec[n].ptr
       else: begin
           c=0L
           repeat begin
           readu,unit, byte
           if (c eq 0) then bstr=replicate(byte,1) $
           else bstr=[bstr(*),byte]  
           c=c+1
           endrep until (byte eq 0)
           *sclvec[n].ptr=STRING(bstr)
           end
       endcase
   endfor


; Process the arrays 

   for n=0, anum-1 do begin

; Read in the 0 terminated array of bytes that comprise the name

     c=0L
     repeat begin
        readu,unit, byte
        if (c eq 0) then bstr=replicate(byte,1) $
        else bstr=[bstr(*),byte]  
        c=c+1
     endrep until (byte eq 0)

; Convert the name to an IDL string

     name=STRING(bstr)

; Read the type byte;

     readu, unit,type

; Read the number of dimensions of the array

     readu, unit,dim

; Create a vector to contain the array ranges

     rng=lonarr(dim)

; Read in the ranges

     readu, unit,rng

     rng=reverse(rng)

 ; Add this variable to the array vector 

     if (n eq 0) then s=DataMapMakeBlankArray(name,type,dim,rng,arrvec,/new) $
     else s=DataMapMakeBlankArray(name,type,dim,rng,arrvec)

; Read in the array values
     
     if (ptr_valid(arrvec[n].ptr)) then begin
        tmparr=transpose(*arrvec[n].ptr)  

        case type of
           1: readu,unit,tmparr
           2: readu,unit,tmparr
           3: readu,unit,tmparr
           4: readu,unit,tmparr
           8: readu,unit,tmparr
           else : begin
              inum=n_elements(tmparr)
              tmpstr=strarr(inum)
              for i=0,inum-1 do begin
                c=0L
                repeat begin
                  readu,unit, byte
                  if (c eq 0) then bstr=replicate(byte,1) $
                  else bstr=[bstr(*),byte]
                  c=c+1
                endrep until (byte eq 0)
                tmpstr[i]=STRING(bstr)
           endfor
           tmparr=reform(tmpstr,reverse(rng))
          end
      endcase
      *arrvec[n].ptr=transpose(tmparr)
    endif
   endfor
   return, size
   
   iofail:
      return, -1

  end


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       DataMapWrite
;
; PURPOSE:
;       Write a record from a DataMap file.
;       
;
; CALLING SEQUENCE:
;       status = DataMapWrite(unit,sclvec,arrvec)
;
;       OR
;
;       status = DataMapWrite(unit,sclvec,/scalar)
;
;       OR
;
;       status = DataMapWrite(unit,arrvec,/array)
;
;       This function writes a single record to the open file with
;       logical unit number, unit. The scalar and array vectors to
;       store are give by sclvec and arrvec respectively. If the
;       keyword array is set, then the function assumes that only
;       an array vector has been defined. Similarly if the scalar
;       keyword is set then only a scalar vector has been defined.
;
;       The returned value is size in bytes of the record written
;       on success, or  -1 for failure
;
;       The array vector and scalar vector should be created using
;       the appropriate DataMapMake functions.
;

;-----------------------------------------------------------------
;




function DataMapWrite,unit,vecone,vectwo,array=array,scalar=scalar

; Write a record to the datamap file pointed to by "unit".
; "sclvec" and "arrvec" are the vectors of scalars and arrays
; to store.

   ON_IOERROR, iofail

   byte=0B
   magic=65537L
   size=16L
   snum=0L
   anum=0L
   type=0B 
   dim=0L

; First work out the block size of the record

  if KEYWORD_SET(array) then begin
     arrvec=vecone 
     anum=n_elements(arrvec)
  endif else if KEYWORD_SET(scalar) then begin
     sclvec=vecone
     snum=n_elements(sclvec)
  endif else begin
      sclvec=vecone
      arrvec=vectwo
      snum=n_elements(sclvec)
      anum=n_elements(arrvec)
  endelse

   for n=0, snum-1 do begin
     size+=strlen(sclvec[n].name)+2
     case sclvec[n].type of
       1: size+=1;
       2: size+=2;
       3: size+=4;
       4: size+=4;
       8: size+=8;
       else: size+=strlen(*sclvec[n].ptr)+1
     endcase
   endfor    

   for n=0, anum-1 do begin
     if (ptr_valid(arrvec[n].ptr)) then begin 
       dim=size(*arrvec[n].ptr,/N_DIMENSIONS)
       rng=size(*arrvec[n].ptr,/DIMENSIONS)
      endif else begin
       dim=1
       rng=intarr(1)
     endelse
     size+=strlen(arrvec[n].name)+6+4*dim
     if (ptr_valid(arrvec[n].ptr)) then begin
       case arrvec[n].type of
         1: size+=n_elements(*arrvec[n].ptr)
         2: size+=2*n_elements(*arrvec[n].ptr)
         3: size+=4*n_elements(*arrvec[n].ptr)
         4: size+=4*n_elements(*arrvec[n].ptr)
         8: size+=8*n_elements(*arrvec[n].ptr)
         else: size+=long(1*n_elements(*arrvec[n].ptr)+$
                        total(strlen(*arrvec[n].ptr)))
        endcase
      endif
   endfor  

; write out the magic word and block size

   writeu, unit, magic,size
  
; write out the number of scalars and arrays
   
   writeu, unit, snum,anum 

   for n=0, snum-1 do begin
     tmpbyte=[byte(sclvec[n].name),0B]
     type=byte(sclvec[n].type)
     writeu,unit,tmpbyte,type
     case type of
     1: writeu,unit,*sclvec[n].ptr
     2: writeu,unit,*sclvec[n].ptr
     3: writeu,unit,*sclvec[n].ptr
     4: writeu,unit,*sclvec[n].ptr
     8: writeu,unit,*sclvec[n].ptr
     else: begin
       tmpbyte=[byte(*sclvec[n].ptr),0B] 
       writeu,unit,tmpbyte
       end      
   endcase
   endfor
   for n=0, anum-1 do begin
     tmpbyte=[byte(arrvec[n].name),0B]
     type=byte(arrvec[n].type)
     writeu,unit,tmpbyte,type
     if (ptr_valid(arrvec[n].ptr)) then begin 
       dim=long(size(*arrvec[n].ptr,/N_DIMENSIONS))
       rng=long(reverse(size(*arrvec[n].ptr,/DIMENSIONS)))
     endif else begin
       dim=1
       rng=longarr(1)
     endelse
     writeu,unit,dim,rng
     if (ptr_valid(arrvec[n].ptr)) then begin
       case type of
         1: writeu,unit,transpose(*arrvec[n].ptr)
         2: writeu,unit,transpose(*arrvec[n].ptr)
         3: writeu,unit,transpose(*arrvec[n].ptr)
         4: writeu,unit,transpose(*arrvec[n].ptr)
         8: writeu,unit,transpose(*arrvec[n].ptr)
         else: begin
           inum=n_elements(*arrvec[n].ptr)
           tmpstr=reform(transpose(*arrvec[n].ptr),inum)
           for i=0,inum-1 do begin
              tmpbyte=[byte(tmpstr[i]),0B]
              writeu,unit,tmpbyte
           endfor
          end 
      endcase
    endif
  endfor
   return, size   

   iofail:
      return, -1

end  





