/* hlpstr.h
   ========
   Author: A. G. Burrell
*/

/*
 LICENSE AND DISCLAIMER

 This file is part of the Radar Software Toolkit (RST).

 RST is free software: you can redistribute it and/or modify
 it under the terms of the GNU Lesser General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 any later version.

 RST is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public License
 along with RST.  If not, see <http://www.gnu.org/licenses/>.
*/


char *hlpstr[]={
"make_fov - Determines the return direction of backscatter.\n",
"make_fov --help\n",
"make_fov [-vb] [-old] [-tl] [-sd yyyymmdd] [-st hr:mn] [-ed yyyymmdd] [-et hr:mn] [-ex hr:mn] [-cn a/b] [-cn_fix c] [-ns +/-1] [-tfmin] [-tfmax] [-gs_strict 0/1] [-update_tdiff 0/1] [-tdiff] [name]\n",

"--help\tprint the help message and exit.\n",
"--version\tprint the RST version number and exit.\n",
"-vb\tverbose. Log information to the console.\n",
"-old\tthe input file is a map format file.\n",
"-tl\tIgnore scan flag and use scan length of `tl` seconds.\n",
"-sd yyyymmdd\textract starting from the date yyyymmdd.\n",
"-st hr:mn\textract starting from the time hr:mn.\n",
"-ed yyyymmdd\tstop extracting at the date yyyymmdd.\n",
"-et hr:mn\tstop extracting at the time hr:mn.\n",
"-ex hr:mn\textract an interval whose extent is hr:mn.\n",
"-cn a/b\tUse Stero channel a or b.\n",
"-cn_fix c\tApply a user-defined channel number.\n",
"-ns +/-1\tApply the scan flag limit (exclude data with scan flag of -1)\n",
"-tfmin\tMinimum of the desired frequency band in kHz\n",
"-tfmax\tMaximum of the desired frequency band in kHz\n",
"-gs_strict 0/1\tRemove or keep indeterminate groundscatter (1 to remove)\n",
"-update_tdiff 0/1\tUpdate hardware tdiff value with user-supplied value\n",
"-tdiff\tUser supplied hardware value in microseconds, only used if -update_tdiff is 1\n",
"name\tfilename of the fitacf (or equivalent) format file(s) to use when determining the origin field of view. If this is omitted, the file is read from standard input.\n",
NULL};
