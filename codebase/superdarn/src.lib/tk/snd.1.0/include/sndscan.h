/* sndscan.h
   =========
   Author: E.G.Thomas
*/


#ifndef _SNDSCAN_H
#define _SNDSCAN_H

int SndReadRadarScan(int fid, int *state, struct RadarScan *ptr,
                     struct SndData *snd, int tlen, int lock);

int SndFreadRadarScan(FILE *fp, int *state, struct RadarScan *ptr,
                      struct SndData *snd, int tlen, int lock);

int SndToRadarScan(struct RadarScan *ptr, struct SndData *snd);

#endif
