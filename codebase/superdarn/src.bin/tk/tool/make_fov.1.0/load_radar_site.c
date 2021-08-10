/* load_radar_site.c
   =================
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

#include "radar.h"

/**
 * Load in radar parameter data
 **/

struct RadarSite *load_radar_site(int yr, int mo, int dy, int hr, int mt,
				  int sc, int stid)
{
  char *envstr;
  
  FILE *fp;

  struct RadarNetwork *network;
  struct Radar *radar;

  /* Make sure the SD_RADAR environment variable is set */
  envstr = getenv("SD_RADAR");
  if(envstr==NULL)
    {
      fprintf(stderr, "Environment variable 'SD_RADAR' must be defined.\n");
      exit(-1);
    }

  /* Open the radar information file */
  fp = fopen(envstr, "r");
  if(fp == NULL)
    {
      fprintf(stderr, "Could not locate radar information file.\n");
      exit(-1);
    }
  
  /* Load the radar network information */
  network = RadarLoad(fp);
  fclose(fp);
  if(network == NULL)
    {
      fprintf(stderr,"Failed to read radar information.\n");
      exit(-1);
    }

  /* Make sure the SD_HDWPATH environment variable is set */
  envstr = getenv("SD_HDWPATH");
  if(envstr == NULL)
    {
      fprintf(stderr, "Environment variable 'SD_HDWPATH' must be defined.\n");
      exit(-1);
    }

  /* Load the hardware information for the radar network */
  RadarLoadHardware(envstr, network);

  /* Load the appropriate radar hardware information for the day
     and time of the radar scan (only done once) */
  radar = RadarGetRadar(network, stid);

  if(radar == NULL)
    {
      fprintf(stderr,"Failed to get radar information for stid %d.\n", stid);
      exit(-1);
    }

  /* Get the radar site */
  return RadarYMDHMSGetSite(radar, yr, mo, dy, hr, mt, sc);

}
