/* f2c.h
   =====
   Author R.J.Barnes
*/

/*
 LICENSE AND DISCLAIMER
 
 Copyright (c) 2012 The Johns Hopkins University/Applied Physics Laboratory
 
 This file is part of the Radar Software Toolkit (RST).
 
 RST is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with RST.  If not, see <http://www.gnu.org/licenses/>.
 
 
 
*/


/* implements portions of the f2c library to allow compilation on 
   systems without f2c 
*/


typedef int integer;
typedef float real;
typedef double doublereal;

typedef int flag;
typedef int ftnlen;
typedef int ftnint;

typedef struct
{	flag cierr;
	ftnint ciunit;
	flag ciend;
	char *cifmt;
	ftnint cirec;
} cilist;


typedef int (*S_fp)();

#define dabs(x) (doublereal)abs(x)

integer f2c_s_wsle(cilist *a);
integer f2c_s_wsfe(cilist *a);
integer f2c_e_wsle(void);
integer f2c_e_wsfe(void);
integer f2c_do_fio(ftnint *type,char *ptr,ftnlen len);
integer f2c_do_lio(ftnint *type,ftnint *number,char *ptr,ftnlen len);
double f2c_d_mod (doublereal * x, doublereal * y);
double f2c_pow_dd(doublereal *ap,doublereal *bp);

