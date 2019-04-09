/* PostScriptRotateMatrix
   ======================
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
  struct PostScriptMatrix m;

  float wdt=400,hgt=400;
  float xoff=50,yoff=50;

  unsigned int fgcol=0xff000000;
  int a;

  ps=PostScriptMake();
  PostScriptSetText(ps,stream,stdout);   
  PostScriptMakeDocument(ps,xoff,yoff,wdt,hgt,0);
  PostScriptMakePlot(ps);

  m.a=1.0;
  m.b=0.0;
  m.c=0.0;
  m.d=1.0;

 for (a=0;a<360;a++) {
    PostScriptRotateMatrix(&m,10.0);
    PostScriptRectangle(ps,&m,wdt/2,hgt/2,wdt/4,hgt/4,
                   0,fgcol,0,NULL,NULL);
  }


  PostScriptEndPlot(ps);
  PostScriptEndDocument(ps);
  return 0;
}

