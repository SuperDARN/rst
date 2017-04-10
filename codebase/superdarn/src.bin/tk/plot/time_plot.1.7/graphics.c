/* graphic.c
   =========
   Author: R.J.Barnes
*/

/*
   See license.txt
*/


#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "rfbuffer.h"
#include "fontdb.h"

#include "rtime.h"
#include "iplot.h"
#include "grplot.h"




int txtbox(char *fntname,float sze,int num,char *txt,float *box,void *data) {

  struct FrameBufferFontDB *fontdb;
  struct FrameBufferFont *fnt;
  int tbox[3];

  fontdb=(struct FrameBufferFontDB *)data;
  if (fontdb==NULL) return -1;
  fnt=FrameBufferFontDBFind(fontdb,fntname,sze);
  
  FrameBufferTextBox(fnt,num,txt,tbox);
  
  box[0]=tbox[0];
  box[1]=tbox[1];
  box[2]=tbox[2];
 
  return 0;
}

