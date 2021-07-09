/* get_mult_scan_tfreq_band.c
   =============
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

/**
 * @brief Determine appropriate frequency bands for a data set
 *
 * @params[in] band_width - Desired width for frequency bands in kHz (300)
 *             mult_scan  - Backscatter data structure
 *
 * @params[out] fbands - frequency bands, with the first array dimension
 *                       corresponding to a given band and the second dimension
 *                       holding the minimum (0) and maximum (1).
 *              num    - Returned value, number of frequency bands or -1 if
 *                       `band_width` resulted in more than 90 frequency bands
 **/
int get_radar_tfreq_bands(int band_width, struct MultRadarScan *mult_scan,
			  int fbands[90][2])
{
  int num, ibm, freq, band_tol, local_max, min_freq=-1, max_freq=-1;

  struct RadarScanCycl *scan_cycl;
  struct RadarScan *scan;
  struct RadarBeam *bm;
  struct RadarPrm *prm;

  scan_cycle = mult_scan->scan_ptr;

  /* Get the minimum and maximum frequencies for the loaded scans */
  for(num = 0; num < mult_scan->num_scans; num++)
    {
      scan = scan_cycle->scan_data;

      for(ibm = 0; ibm < scan->num; ibm++)
	{
	  bm  = scan->bm[ibm];
	  prm = bm->prm;

	  if(min_freq < 0 || min_freq > prm->tfreq) min_freq = prm->tfreq;
	  if(max_freq < prm->tfreq) max_freq = prm->tfreq;
	}
    }

  /* Use the data frequency range and desired bandwidth to specify frequency */
  /* bands. Allow a small tolerance for the frequency band of 10%.           */
  band_tol = band_width / 10;
  num  = 0;
  freq = min_freq;

  while(freq <= max_freq && num < 90)
    {
      /* Define the upper limit of this frequency band */
      local_max = freq + band_width;

      /* If the observed upper limit is close, extend it */
      if(local_max + band_tol > max_freq) local_max = max_freq + 1;

      /* Save the result */
      fbands[num][0] = freq;
      fbands[num][1] = local_max;

      /* Cycle to the next possible frequency band */
      num++;
      freq = local_max;
    }

  /* The band width was too small to split into 90 bands or less */
  if(num == 90 && fbands[89][1] <= max_freq) num = -1;

  /* Return the number of frequency bands */
  return(num);
}
