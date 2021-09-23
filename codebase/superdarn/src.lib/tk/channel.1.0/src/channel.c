/* channel.c
   =========
   Author: Angeline G. Burrell - NRL - 2021
   This is a U.S. government work and not under copyright protection in the U.S.

   This file is part of the Radar Software Toolkit (RST).

   Disclaimer: RST is licensed under GPL v3.0. Please visit 
               <https://www.gnu.org/licenses/> to see the full license

   Modifications:

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

int set_stereo_channel(char *chnstr)
{
  int channel = -1;

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
  int channel_fix = -1;

  if(chnstr_fix != NULL)
    {
      if(tolower(chnstr_fix[0]) == 'a')      channel_fix = 1;
      else if(tolower(chnstr_fix[0]) == 'b') channel_fix = 2;
      else if(tolower(chnstr_fix[0]) == 'c') channel_fix = 3;
      else if(tolower(chnstr_fix[0]) == 'd') channel_fix = 4;
      else
	fprintf(stderr, "Unknown fix channel %s, leaving unset\n", chnstr_fix);
    }

  return(channel_fix);
}
