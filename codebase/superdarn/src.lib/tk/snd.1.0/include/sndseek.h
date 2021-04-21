/* sndseek.h
   =========
   Author: E.G.Thomas

   
   Copyright (C) <year>  <name of author>

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


#ifndef _SNDSEEK_H
#define _SNDSEEK_H

int SndSeek(int fid, int yr, int mo, int dy,
            int hr, int mt, int sc, double *atme);
int SndFseek(FILE *fp, int yr, int mo, int dy,
             int hr, int mt, int sc, double *atme);

#endif
