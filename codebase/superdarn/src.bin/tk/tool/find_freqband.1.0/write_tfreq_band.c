/* write_tfreq_band.c
   ==================
   Author: Angeline G. Burrell - NRL - 2021
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

#ifndef MAX_FREQ_KHZ
#define MAX_FREQ_KHZ 30000
#endif

/**
 * @brief Print frequency band information to file
 *
 * @params[in] nfbands  - Number of frequency bands in `fbands`
 *             fbands   - Array of frequency bands with min and max (0, 1)
 *             all_freq - Number of observations per transmission frequency
 *             fout     - File output pointer
 **/

void write_tfreq_band(int nfbands, int fbands[][2], int all_freq[MAX_FREQ_KHZ],
		      FILE *fout)
{
  int i, j;

  /* Print a header */
  fprintf(fout,
	  "# NBANDS BAND_MIN_KHZ BAND_MAX_KHZ TRANS_FREQ_KHZ NUM_TRANS_FREQ\n");

  for(i = 0; i < nfbands; i++)
    {
      for(j = fbands[i][0]; j < fbands[i][1]; j++)
	{
	  if(all_freq[j] > 0)
	    fprintf(fout, "%d\t%5d\t%5d\t%5d\t%d\n", nfbands, fbands[i][0],
		    fbands[i][1], j, all_freq[j]);
	}
    }

  return;
}
      
