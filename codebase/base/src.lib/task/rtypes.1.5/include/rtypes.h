/* rtypes.h
   ========
   Author: R.J.Barnes, J.Spaleta
*/

/*
 (c) 2011 The Johns Hopkins University/Applied Physics Laboratory & Others

This file is part of the Radar Software Toolkit (RST).

RST is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.

Modifications:
*/



#ifndef _RTYPES_H
#define _RTYPES_H

#ifdef _QNX4

#include <sys/types.h>

typedef pid_t ipcid_t;
typedef short int16;
typedef int int32;
typedef double int64;

#else
  typedef int ipcid_t;



  /* This defines the int16, int32, and int64 types */

  #include <limits.h>
  #ifdef WORD_BIT
    #if WORD_BIT == 16
      typedef char int8;
      typedef unsigned char uint8;
      typedef int int16;                                                                                                           typedef unsigned int uint16;
      typedef long int32;
      typedef unsigned long uint32;
      typedef double int64;
      typedef unsigned double uint64;
    #endif
    #if WORD_BIT == 32
      typedef char int8;
      typedef unsigned char uint8;
      typedef short int16;
      typedef unsigned short uint16;
      typedef int int32;
      typedef unsigned int uint32;
      typedef long long int64;
      typedef unsigned long long uint64;
    #endif
  #elif defined LONG_BIT
    #if LONG_BIT == 32
      typedef char int8;
      typedef unsigned char uint8;
      typedef short int16;
      typedef unsigned short uint16;
      typedef long int32;
      typedef unsigned long uint32;
      typedef long long int64;
      typedef unsigned long long uint64;
    #endif
    #if LONG_BIT == 64
      typedef char int8;
      typedef unsigned char uint8;
      typedef short int16;
      typedef unsigned short uint16;
      typedef int int32;
      typedef unsigned int uint32;
      typedef long int64;
      typedef unsigned long uint64;
    #endif
  #elif defined __INT_BITS__
    #if __INT_BITS__== 16
      typedef char int8;
      typedef unsigned char uint8;
      typedef int int16;
      typedef unsigned int uint16;
      typedef long int32;
      typedef unsigned long uint32;
      typedef double int64;
      typedef unsigned double uint64;
    #endif
    #if __INT_BITS__== 32
      typedef char int8;
      typedef unsigned char uint8;
      typedef short int16;
      typedef unsigned short uint16;
      typedef int int32;
      typedef unsigned int uint32;
      typedef long long int64;
      typedef unsigned long long uint64;
    #endif
  #endif




#endif

#endif
