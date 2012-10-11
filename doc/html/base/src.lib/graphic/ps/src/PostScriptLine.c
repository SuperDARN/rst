/* PostScriptLine
   ==============
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

  float wdt=400,hgt=400;
  float xoff=50,yoff=50;

  unsigned int fgcol=0xff000000;
  int x;

  ps=PostScriptMake();
  PostScriptSetText(ps,stream,stdout);  
  PostScriptMakeDocument(ps,xoff,yoff,wdt,hgt,0);
  PostScriptMakePlot(ps);


  for (x=0;x<wdt;x+=10) {
    PostScriptLine(ps,x,0,0,hgt-x,
              fgcol,1.0,NULL,NULL);
  }


  PostScriptEndPlot(ps);
  PostScriptEndDocument(ps);
  return 0;
}

