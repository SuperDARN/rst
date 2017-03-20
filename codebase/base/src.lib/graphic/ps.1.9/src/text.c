/* text.c
   ======= 
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



int PostScriptText(struct PostScript *ptr,
	              struct PostScriptMatrix *matrix,
                      char *fname,float fsize,
                      float x,float y,int num,char *txt,
                      unsigned int color,
                      struct PostScriptClip *clip) {
  int s=0;
  float ma=1.0,mb=0.0,mc=0.0,md=1.0;

  char buf[4096];

  if (ptr->cnt >0) {
    sprintf(buf,"d\n ");
    if (ptr->text.func !=NULL) s=(ptr->text.func)(buf,strlen(buf),
                                                  ptr->text.data);
    ptr->cnt=0;
  }
  if (s !=0) return s;

 if (clip !=ptr->clip) {
    s=PostScriptClip(ptr,clip);
    if (s !=0) return s;
    ptr->clip=clip;
  }

  if (matrix !=NULL) {
    ma=matrix->a;
    mb=matrix->b;
    mc=matrix->c;
    md=matrix->d;
  }

  if (color !=ptr->color) {
    unsigned int ri,gi,bi;
    float r,g,b;

    ri=(color>>16) & 0xff;
    gi=(color>>8) & 0xff;
    bi=color & 0xff;
   
    r=(float) ri/255.0;
    g=(float) gi/255.0;
    b=(float) bi/255.0;
    
    sprintf(buf,"%g %g %g c\n",r,g,b); 
    if (ptr->text.func !=NULL) s=(ptr->text.func)(buf,strlen(buf),
                                                  ptr->text.data);
    if (s !=0) return s;
    ptr->color=color;
  }
  sprintf(buf,"s [%g %g %g %g %g %g] concat 0 0 m %g /%s F (",
          ma,mb,mc,md,ptr->x+x,ptr->y+ptr->hgt-y,fsize,fname); 
  if (ptr->text.func !=NULL) s=(ptr->text.func)(buf,strlen(buf),
                                               ptr->text.data);
  if (s !=0) return s;

  if (ptr->text.func !=NULL) s=(ptr->text.func)(txt,num,
                                               ptr->text.data);
  if (s !=0) return s;

  sprintf(buf,") show r\n");
  if (ptr->text.func !=NULL) s=(ptr->text.func)(buf,strlen(buf),
                                               ptr->text.data);

  return s;
}

