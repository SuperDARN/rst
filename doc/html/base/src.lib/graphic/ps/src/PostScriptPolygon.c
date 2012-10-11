/* PostScriptPolygon
   =================
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

  float x[4],y[4];
  int t[4]={0,0,0,0};


  unsigned int fgcol=0xff000000;

  ps=PostScriptMake();
  PostScriptSetText(ps,stream,stdout);   
  PostScriptMakeDocument(ps,xoff,yoff,wdt,hgt,0);
  PostScriptMakePlot(ps);

  x[0]=50;
  x[1]=300;
  x[2]=350;
  x[3]=100;

  y[0]=100;
  y[1]=100;
  y[2]=300;
  y[3]=300;

  PostScriptPolygon(ps,NULL,0,0,4,x,y,t,
                  1,fgcol,0,NULL,NULL);

  PostScriptEndPlot(ps);
  PostScriptEndDocument(ps);
  return 0;
}

