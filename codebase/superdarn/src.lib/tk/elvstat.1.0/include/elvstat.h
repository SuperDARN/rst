/* freqband.h
   ==========
   Author: Angeline G. Burrell - NRL - 2023
   This is a U.S. government work and not under copyright protection in the U.S.

   This file is part of the Radar Software Toolkit (RST).

   Disclaimer: RST is licensed under GPL v3.0. Please visit 
               <https://www.gnu.org/licenses/> to see the full license

   Modifications:

*/

int get_fit_elv_stat(int fnum, int channel, int channel_fix, int old,
		     double stime, double sdate, double etime, double edate,
		     double extime, unsigned char fitflg, unsigned char nsflg,
		     unsigned char vb, char *vbuf, char *iname, char **dnames);
