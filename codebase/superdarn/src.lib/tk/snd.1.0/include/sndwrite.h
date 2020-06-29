/* sndwrite.h
   ==========
   Author: E.G.Thomas
*/


#ifndef _SNDWRITE_H
#define _SNDWRITE_H

int SndFwrite(FILE *fp, struct SndData *);
int SndWrite(int fid, struct SndData *);

#endif
