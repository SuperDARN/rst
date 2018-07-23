/*
 * function prototypes
 */

#ifndef _HMB_H
#define _HMB_H

struct hmbtab *load_hmb(FILE *fp);
void make_hmb(void);
void add_hmb_grd(float latmin,int yr,int yrsec,struct CnvMapData *map,
                 int old_aacgm);
void map_addhmb(int yr, int yrsec, struct CnvMapData *map, int bndnp,
                float bndstep, float latref, float latmin, int old_aacgm);
int latcmp(const void *a,const void *b);

#endif
