/* sndseek.h
   =========
   Author: E.G.Thomas
*/


#ifndef _SNDSEEK_H
#define _SNDSEEK_H

int SndSeek(int fid, int yr, int mo, int dy,
            int hr, int mt, int sc, double *atme);
int SndFseek(FILE *fp, int yr, int mo, int dy,
             int hr, int mt, int sc, double *atme);

#endif
