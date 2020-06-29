/* sndread.h
   =========
   Author: E.G.Thomas
*/


#ifndef _SNDREAD_H
#define _SNDREAD_H

int SndDecode(struct DataMap *ptr, struct SndData *);
int SndFread(FILE *fp, struct SndData *);
int SndRead(int fid, struct SndData *);

#endif
