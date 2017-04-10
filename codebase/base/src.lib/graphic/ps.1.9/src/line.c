/* line.c
   ====== 
   Author: R.J.Barnes
*/

/*
   See license.txt
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rfbuffer.h"
#include "rps.h"



int PostScriptLine(struct PostScript *ptr,
              float ax,float ay,float bx,float by,
              unsigned int color,float width,
              struct PostScriptDash *dash,
              struct PostScriptClip *clip) {
  int s=0;
  char buf[4096];
  if (ptr==NULL) return -1;
  s=PostScriptState(ptr,color,width,dash,clip);
  if (s !=0) return s;

  if ((ax !=ptr->px) || (ay !=ptr->py)) {
    sprintf(buf,"%g %g m ",ptr->x+ax,ptr->y+ptr->hgt-ay);
    if (ptr->text.func !=NULL) s=(ptr->text.func)(buf,strlen(buf),
                                                  ptr->text.data);
    if (s !=0) return s;
  } 

  sprintf(buf,"%g %g l ",ptr->x+bx,ptr->y+ptr->hgt-by);
  if (ptr->text.func !=NULL) s=(ptr->text.func)(buf,strlen(buf),
                                                  ptr->text.data);
    if (s !=0) return s;
  
  ptr->cnt++;
  if (ptr->cnt>STROKE_COUNT) {
  
    sprintf(buf,"d\n %g %g m ",ptr->x+bx,ptr->y+ptr->hgt-by);
    if (ptr->text.func !=NULL) s=(ptr->text.func)(buf,strlen(buf),
                                                  ptr->text.data);
    if (s !=0) return s;
    ptr->cnt=0;
  }

  ptr->px=bx;
  ptr->py=by;
  return 0;
}
