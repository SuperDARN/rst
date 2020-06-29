/* snd.c
   ========
   Author: E.G.Thomas
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <zlib.h>
#include "rtypes.h"
#include "dmap.h"
#include "snddata.h"


struct SndData *SndMake() {

  struct SndData *ptr=NULL;
  ptr=malloc(sizeof(struct SndData));
  if (ptr==NULL) return NULL;
  memset(ptr,0,sizeof(struct SndData));
  ptr->rng=NULL;
  return ptr;
}


void SndFree(struct SndData *ptr) {

  if (ptr==NULL) return;
  if (ptr->rng !=NULL) free(ptr->rng);
  free(ptr);
  return;
}


int SndSetRng(struct SndData *ptr,int nrang) {
  void *tmp=NULL;

  if (ptr==NULL) return -1;
  if (nrang==0) {
    if (ptr->rng !=NULL) free(ptr->rng);
    ptr->rng=NULL;
    return 0;
  }
  if (ptr->rng==NULL) tmp=malloc(sizeof(struct SndRange)*nrang);
  else tmp=realloc(ptr->rng,sizeof(struct SndRange)*nrang);

  if (tmp==NULL) return -1;
  memset(tmp,0,sizeof(struct SndRange)*nrang);
  ptr->rng=tmp;
  return 0;
}

