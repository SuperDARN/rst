/* read_fit_funcs.c
   ===================
   Author: Angeline G. Burrell - NRL - 2019
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

int set_stereo_channel(char *chnstr)
{
  int channel = 0;

  if(chnstr != NULL)
    {
      if(tolower(chnstr[0]) == 'a')      channel = 1;
      else if(tolower(chnstr[0]) == 'b') channel = 2;
      else
	fprintf(stderr, "Unknown Stereo channel %s, leaving unset\n", chnstr);
    }

  return(channel);
}

int set_fix_channel(char *chnstr_fix)
{
  int channel_fix=-1;

  if(chnstr_fix != NULL)
    {
      if(tolower(chnstr_fix[0]) == 'a') channel_fix = 1;
      else if(tolower(chnstr_fix[0]) == 'b') channel_fix = 2;
      else if(tolower(chnstr_fix[0]) == 'c') channel_fix = 3;
      else if(tolower(chnstr_fix[0]) == 'd') channel_fix = 4;
      else
	fprintf(stderr, "Unknown fix channel %s, leaving unset\n", chnstr_fix);
    }

  return(channel_fix);
}
