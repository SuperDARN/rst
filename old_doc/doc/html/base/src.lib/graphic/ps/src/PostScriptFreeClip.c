/* PostScriptFreeClip
   ==================
   Author: R.J.Barnes
*/


#include <stdio.h>
#include <stdlib.h>
#include "rfbuffer.h"
#include "rps.h"

int stream(char *buf,int sze,void *data) {
  FILE *fp;
  fp=(FILE *) data;
  fwrite(buf,sze,1,stdout);
  return 0;
} 


int main(int argc,char *argv[]) {

  struct PostScript *ps=NULL;
  struct PostScriptClip *clip=NULL;

  float wdt=400,hgt=400;
  float xoff=50,yoff=50;

  unsigned int fgcol=0xff000000;

  float x[4]={0,200,400,200};
  float y[4]={200,0,200,400};
  int t[4]={0,0,0,0};


  ps=PostScriptMake();
  PostScriptSetText(ps,stream,stdout);   

  PostScriptMakeDocument(ps,xoff,yoff,wdt,hgt,0);
  PostScriptMakePlot(ps);

  clip=PostScriptMakeClip(50,50,wdt,hgt,4,x,y,t);

  PostScriptRectangle(ps,NULL,10,10,wdt-20,hgt-20,
                  1,fgcol,0,NULL,clip);

  PostScriptFreeClip(clip);
  

  PostScriptEndPlot(ps);
  PostScriptEndDocument(ps);
  return 0;
}

