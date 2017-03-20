/* plot.c
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





int PostScriptDocumentStart(struct PostScript *ptr,
                          float x,float y,float wdt,float hgt,
                           int land) {
 
  char buf[4096];
  char lbuf[256];
  
  if (ptr==NULL) return -1;
  ptr->x=x;
  ptr->y=y;
  ptr->wdt=wdt;
  ptr->hgt=hgt;
  ptr->land=land;
  ptr->pagenum=0;
  ptr->cnt=0;
  sprintf(lbuf,"%%!PS-Adobe-3.0\n");
  strcpy(buf,lbuf);
  sprintf(lbuf,"%%%%Title:Test Document\n");
  if (land==1) 
    sprintf(lbuf,"%%%%BoundingBox:%g %g %g %g\n",
            ptr->y,ptr->x,ptr->y+ptr->hgt,ptr->x+ptr->wdt);
  else 
     sprintf(lbuf,"%%%%BoundingBox:%g %g %g %g\n",
          ptr->x,ptr->y,
	  ptr->x+ptr->wdt,ptr->y+ptr->hgt);
  strcat(buf,lbuf);
  sprintf(lbuf,"%%%%PageOrder: Ascend\n");
  strcat(buf,lbuf);
  sprintf(lbuf,"%%%%Pages: (atend)\n");
  strcat(buf,lbuf);
  sprintf(lbuf,"%%%%EndComments\n");
  strcat(buf,lbuf);
  sprintf(lbuf,"%%%%BeginProlog\n");
  strcat(buf,lbuf);
  sprintf(lbuf,"/bdef {bind def} bind def\n");
  strcat(buf,lbuf);
  sprintf(lbuf,"/ldef {load def} bdef\n");
  strcat(buf,lbuf);
  sprintf(lbuf,
          "/m /moveto ldef /l /lineto ldef /c /setrgbcolor ldef\n");
  strcat(buf,lbuf);
  sprintf(lbuf,
          "/w /setlinewidth ldef /f /fill ldef /d /stroke ldef\n");
  strcat(buf,lbuf);
  sprintf(lbuf,
          "/n /newpath ldef /e /closepath ldef /s /gsave  ldef\n");
  strcat(buf,lbuf);
  sprintf(lbuf,
          "/r /grestore ldef /o /currentpoint ldef /t /show ldef\n");
  strcat(buf,lbuf);
  sprintf(lbuf,"/b /curveto ldef\n"); 
  strcat(buf,lbuf);
  sprintf(lbuf,"/F {findfont exch scalefont setfont } bdef\n");
  strcat(buf,lbuf);
  sprintf(lbuf,"%%%%EndProlog\n");
  strcat(buf,lbuf);

  ptr->px=0;
  ptr->py=0;
  ptr->width=0.5;
  ptr->color=0;
  ptr->dash=NULL;
  ptr->clip=NULL;
  ptr->pagenum=0;

  if (ptr->text.func !=NULL) return (ptr->text.func)(buf,strlen(buf),
                                                     ptr->text.data);

  return 0;
}




int PostScriptPlotStart(struct PostScript *ptr) {


  char buf[4096];
  char lbuf[256];
  
  if (ptr==NULL) return -1;

  buf[0]=0; 
  if (ptr->land==1) { 
     sprintf(lbuf,"%%%%PageOrientation: Landscape\n");
     strcpy(buf,lbuf);
  }

  sprintf(lbuf,"gsave\n");
  strcat(buf,lbuf);

  if (ptr->land==1) { 
    sprintf(lbuf,"%g %g translate 90 rotate\n",2*ptr->y+
            ptr->hgt,0.0);
    strcat(buf,lbuf);
  }
  ptr->pagenum++;
  if (ptr->text.func !=NULL) return (ptr->text.func)(buf,strlen(buf),
                                                     ptr->text.data);

  return 0;
}


int PostScriptPlotEnd(struct PostScript *ptr) {


  char buf[4096];
  char lbuf[256];

  if (ptr==NULL) return -1;

  sprintf(lbuf,"\nstroke\n");
  strcpy(buf,lbuf);
  sprintf(lbuf,"%%%%PageTrailer\n");
  strcat(buf,lbuf);
  sprintf(lbuf,"grestore\n");
  strcat(buf,lbuf);
  sprintf(lbuf,"showpage\n");
  strcat(buf,lbuf);
  sprintf(lbuf,"%%%%Page: %d %d\n",ptr->pagenum,ptr->pagenum+1);
  strcat(buf,lbuf);
  if (ptr->text.func !=NULL) return (ptr->text.func)(buf,strlen(buf),
                                                     ptr->text.data);

  return 0;
}




int PostScriptDocumentEnd(struct PostScript *ptr) {
 
  char buf[4096];
  char lbuf[256];
  
  if (ptr==NULL) return -1;

  sprintf(lbuf,"%%%%Trailer\n");
  strcpy(buf,lbuf);
  sprintf(lbuf,"%%%%Pages: %d\n",ptr->pagenum);
  strcat(buf,lbuf);
  sprintf(lbuf,"%%%%EOF\n");
  strcat(buf,lbuf);

  if (ptr->text.func !=NULL) return (ptr->text.func)(buf,strlen(buf),
                                                     ptr->text.data);

  return 0;

}
