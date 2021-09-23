/* write_tfreq_band.c
   ==================
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

#include "freqband.h"

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

  /* Print the data to the specified file pointer */
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
      
