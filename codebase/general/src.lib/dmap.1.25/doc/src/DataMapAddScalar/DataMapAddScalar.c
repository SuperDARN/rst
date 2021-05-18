/* DataMapAddScalar.c
   ==================
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

  int t;

  int value=0;

  struct DataMap *dmap;

  dmap=DataMapMake();

  DataMapAddScalar(dmap,"value",DATAINT,&value);

  for (t=0;t<100;t++) {
    value=value+1;
    DataMapFwrite(stdout,dmap);
  }
  DataMapFree(dmap);
  return 0;

}
