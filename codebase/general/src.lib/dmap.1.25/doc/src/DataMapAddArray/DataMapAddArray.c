/* DataMapAddArray.c
   =================
   Author: R.J.Barnes
 Copyright (c) 2012 The Johns Hopkins University/Applied Physics Laboratory

This file is part of the Radar Software Toolkit (RST).

RST is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with this program. If not, see <https://www.gnu.org/licenses/>.

Modifications:

*/


#include <stdio.h>
#include <stdlib.h>

#include "rtypes.h"
#include "dmap.h"



int main(int argc,char *argv[]) {

  int t,n;

  float set[3]={0.1,0.2,10.2};

  int32 dim=1;
  int32 rng[1]={3};

  struct DataMap *dmap;

  dmap=DataMapMake();

  DataMapAddArray(dmap,"set",DATAFLOAT,dim,rng,&set);

  for (t=0;t<100;t++) {
    for (n=0;n<3;n++) set[n]=set[n]+0.1;
    DataMapFwrite(stdout,dmap);
  }
  DataMapFree(dmap);
  return 0;

}
