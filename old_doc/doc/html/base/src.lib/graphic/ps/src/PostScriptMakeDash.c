/* PostScriptMakeDash
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
  struct PostScriptDash *dash=NULL;

  float p[3]={10,5,20};

  float wdt=400,hgt=400;
  float xoff=50,yoff=50;

  unsigned int fgcol=0xff000000;

  ps=PostScriptMake();
  PostScriptSetText(ps,stream,stdout);   
  PostScriptMakeDocument(ps,xoff,yoff,wdt,hgt,0);
  PostScriptMakePlot(ps);

  dash=PostScriptMakeDash(p,0,3);

  PostScriptRectangle(ps,NULL,10,10,wdt-20,hgt-20,
                 0,fgcol,1.0,dash,NULL);

  PostScriptFreeDash(dash);

  PostScriptEndPlot(ps);
  PostScriptEndDocument(ps);
  return 0;
}

