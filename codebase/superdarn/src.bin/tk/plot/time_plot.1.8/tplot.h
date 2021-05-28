/* tplot.h
   =======
   Author: R.J.Barnes
*/

/*
  Copyright (c) 2012 The Johns Hopkins University/Applied Physics Laboratory
 
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
along with this program. If not, see <https://www.gnu.org/licenses/>.

Modifications:
*/ 




struct tplot {
  int bmnum;
  int channel;
  int cpid;
  int scan;
  int nrang;
  int nave;
  struct {
    int sc;
    int us;
  } intt;
  int frang;
  int rsep;
  int rxrise;
  int noise;
  int tfreq;
  int atten;
  int *qflg;
  int *gsct;
  double *p_0;
  double *p_l;
  double *p_l_e;
  double *v;
  double *v_e;
  double *w_l;
  double *w_l_e;
  double *phi0;
  double *elv;
}; 

int tplotset(struct tplot *ptr,int nrang);

