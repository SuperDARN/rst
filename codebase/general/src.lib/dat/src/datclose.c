/* 
 Copyright (c) 2021 University of Saskatchewan
 Author: Marina Schmidt
 
 Copyright (c) 2012 The Johns Hopkins University/Applied Physics Laboratory
 Copied code from raw_close.c in cmpraw modified for dat files


 This file is part of the Radar Software Toolkit (RST).

 RST is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <https://www.gnu.org/licenses/>.

 Modifications:
 
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <zlib.h>
#include <fcntl.h>
#include "rtypes.h"
#include "rtime.h"
#include "lmt.h"

#include "dmap.h"
#include "dat.h"
#include "datread.h"


/* DatClose
 * Closes the file pointer 
 */
void DatClose(struct Datfp *fp) {
  if (fp==NULL) return;
  if (fp->datfp !=-1) close(fp->datfp);
}
