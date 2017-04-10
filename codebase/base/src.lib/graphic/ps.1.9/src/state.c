/* state.c
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





int PostScriptState(struct PostScript *ptr,
                    unsigned int color,float width,
                    struct PostScriptDash *dash,
                    struct PostScriptClip *clip) {
  int flg=0;
  int s=0;
  char buf[4096];
  if (ptr==NULL) return -1;
  if (color !=ptr->color) flg=1;
  if (width !=ptr->width) flg=1;
  if (dash !=ptr->dash) flg=1;
  if (clip !=ptr->clip) flg=1;
 
  if (flg==0) return 0;
  if (ptr->cnt >0) {
    sprintf(buf,"\ncurrentpoint d m ");
    if (ptr->text.func !=NULL) s=(ptr->text.func)(buf,strlen(buf),
                                                  ptr->text.data);
    ptr->cnt=0;
  }
  if (s !=0) return s;
  
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
  if (width !=ptr->width) {   
    sprintf(buf,"%g w\n", width);
    if (ptr->text.func !=NULL) s=(ptr->text.func)(buf,strlen(buf),
                                                  ptr->text.data);
    if (s !=0) return s;
    ptr->width=width;
  }
 
  if (dash !=ptr->dash) {
    int i;
    if (dash !=NULL) {
      sprintf(buf,"[");
      if (ptr->text.func !=NULL) s=(ptr->text.func)(buf,strlen(buf),
                                                  ptr->text.data);
      if (s !=0) return s;
      for (i=0;i<dash->sze;i++) {
        sprintf(buf,"%g ",dash->p[i]);
        if (ptr->text.func !=NULL) s=(ptr->text.func)(buf,strlen(buf),
                                                  ptr->text.data);
        if (s !=0) break;
      }
      if (i !=dash->sze) return -1;
      sprintf(buf,"] %g setdash\n",dash->phase);
      if (ptr->text.func !=NULL) s=(ptr->text.func)(buf,strlen(buf),
                                                  ptr->text.data);
      if (s !=0) return s;
    } else {
      sprintf(buf,"[] 0 setdash\n");
      if (ptr->text.func !=NULL) s=(ptr->text.func)(buf,strlen(buf),
                                                  ptr->text.data);
      if (s !=0) return s;

    }
    ptr->dash=dash;
  }
  if (clip !=ptr->clip) {
    s=PostScriptClip(ptr,clip);
    if (s !=0) return s;
    ptr->clip=clip;
  }

  return 0;
}

