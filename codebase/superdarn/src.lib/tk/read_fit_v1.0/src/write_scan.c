/* write_scan.c
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
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <ctype.h>
#include "rtypes.h"
#include "dmap.h"
#include "option.h"
#include "rtime.h"
#include "radar.h"
#include "scandata.h"
#include "cfitscan.h"
#include "fitscan.h"

/*
 * Cycle through and write multiple scans of radar data
 */

void write_mult_scan(FILE *fp, struct MultRadarScan *mult_scan,
		     unsigned char vb, char *vbuf)
{
  int iscan;

  struct RadarScanCycl *scan;

  void write_scan(FILE *fp, struct RadarScan *scan, unsigned char vb,
		  char *vbuf);

  /* Write the header */
  write_scan(fp, NULL, vp, vbuf);

  scan = mult_scan->scan_ptr;

  for(iscan = 0; iscan < mult_scan->num_scans; iscan++)
    {
      /* Write all the beams from this scan */
      write_scan(fp, &scan->scan_data, vb, vbuf);

      /* Cycle to the next scan */
      scan = scan->next_scan;
    }

  return;
}

/* 
 * Write a scan of radar data
 */

void write_scan(FILE *fp, struct RadarScan *scan, unsigned char vb, char *vbuf)
{
  int snum, rg, yr, mo, dy, hr, mt;
  double sc;
  
  char scan_info[10], bm_info[1000], rng_info[1000];

  struct RadarBeam *bm;
  struct RadarCell rng;

  if(scan == NULL)
    {
      sprintf(scan_info, "#STID");
      sprintf(bm_info, "DATE TIME SCAN BMNUM BMAZM CPID INTT_SC INTT_US NAVE FRANG RSEP RXRISE FREQ NOISE ATTEN CHANNEL NRANG");
      sprintf(rng_info, "RG GFLG P_0 P_0_ERR V V_ERR W_L W_L_ERR P_L P_L_ERR");

      fprintf(fp, "%s %s %s\n", scan_info, bm_info, rng_info);
    }
  else
    {
      for(snum=0; snum<scan->num; snum++)
	{
	  /* Write out the desired info that is the same for this scan */
	  sprintf(scan_info, "%d", scan->stid);

	  /* Write out the desired info that is the same for this beam */
	  bm = scan->bm;
	  TimeEpochToYMDHMS(bm->time, &yr, &mo, &dy, &hr, &mt, &sc);
  
	  sprintf(bm_info, "%04d-%02d-%02d %02d:%02d:%02.0f", yr, mo, dy, hr,
		  mt, sc);
	  sprintf(bm_info, "%s %d %d %0.3f %d %d %d %d %d %d %d %d %d %d %d %d",
		  bm_info, bm->scan, bm->bm, bm->bmazm, bm->cpid, bm->intt.sc,
		  bm->intt.us, bm->nave, bm->frang, bm->rsep, bm->rxrise,
		  bm->freq, bm->noise, bm->atten, bm->channel, bm->nrang);

	  for(rg=0; rg<bm->nrang; rg++)
	    {
	      /* Write out the range info */
	      if(bm->sct[rg] == 1)
		{
		  rng = bm->rng[rg];
		  sprintf(rng_info, "%d %d %f %f %f %f %f %f %f %f", rg,
			  rng.gsct, rng.p_0, rng.p_0_e, rng.v, rng.v_e,
			  rng.w_l, rng.w_l_e, rng.p_l, rng.p_l_e);
	      
		  fprintf(fp, "%s %s %s\n", scan_info, bm_info, rng_info);
		}
	    }
	}
    }


  return;
}
