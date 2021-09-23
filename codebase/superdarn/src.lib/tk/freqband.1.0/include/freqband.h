/* freqband.h
   ==========
   Author: Angeline G. Burrell - NRL - 2021
   This is a U.S. government work and not under copyright protection in the U.S.

   This file is part of the Radar Software Toolkit (RST).

   Disclaimer: This code is licensed under GPL v3.0 please LICENSE to see the
               full license

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
