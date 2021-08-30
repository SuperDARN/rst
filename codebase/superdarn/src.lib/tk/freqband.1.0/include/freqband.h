/* freqband.h
   ==========
   Author: A.G. Burrell (NRL)
*/

/*

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

#ifndef MAX_FREQ_KHZ
#define MAX_FREQ_KHZ 30000
#endif

int get_fit_tfreq_bands(int fnum, int channel, int channel_fix, int old,
			int tlen, double stime, double sdate, double etime,
			double edate, double extime, unsigned char fitflg,
			unsigned char nsflg, unsigned char vb, char *vbuf,
			char *iname, char **dnames,int band_width,
			int fbands[90][2], int all_freq[MAX_FREQ_KHZ]);
