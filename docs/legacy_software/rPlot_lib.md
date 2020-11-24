<!--
(C) copyright 2020 VT SuperDARN, Virginia Polytechnic Institute & State University
author: Kevin Sterne
-->

This information is sourced from the RFC: 0011 previously in the RST RFC documentation that was written by R.J. Barnes.

# rPlot Libraries

## Summary

An overview of how the rPlot graphics specification is used in software.

## Introduction

The rPlot specification defines an XML document structure for describing vector graphics. The purpose of this document is to describe how the specification is used in software to produce a final plot, whether it be a bitmap, PostScript document or another vector graphics format.

### Concepts

__Streams__

The rPlot specification is based on XML, which is from the same family as HTML. XML does not necessarily have to be read from a file stored on a disk, it can just as easily be read from a network data stream. (If the XML is generated dynamically, it may never exist in a file at all). This presents some special problems for the programmer when trying to write software to decode or parse the XML. Firstly the programmer cannot assume that the entire document is available whenthe program starts, secondly the program cannot know the size of the document, and thirdly the program cannot randomly access the document or look ahead from the current position within it.

__Producers and Consumers__

Programs that use the rPlot specification can be divided into producers and consumers. A producer creates rPlot XML data and a consumer converts rPlot XML data into another form. For the rPlot specification, the consumers are programs that render the rplot XML into another form, such as a bitmap or PostScript plot.

At the simplest level, a producer writes rPlot XML to a data file which the consumer can then read back at a later time to produce a plot. Unix pipes can be used to directly pipe producer data into a consumer without having an intermediate file. It's also possible to have a producer running on a remote server and to pipe the data over the internet to a local producer.

Paradoxically its possible for a program to be both a producer and consumer. If a plotting program produces finished plots directly, internally the program will use rplot XML that feeds directly into a parser that produces the output plot.

|-|-|
| `make_plot > plot.rp.xml`
   `rplot -ps  plot.rp.xml > > plot.ps ` | Intemediary file |
| `make_plot > plot.rp.xml /| rplot -ps > plot.ps` | UNIX pipe. |
| `ipclient rhost 1025 | rplot -ps >  > plot.ps` | Remote server. |

__Stream Handlers__

Both producers and consumers can receive data from any source, consequently the rPlot libraries do not implement any routines for reading and writing data and instead expect calling application to implement them. This allows much greater flexibility in designing software.

When reading data the application is responsible for reading the data into a memory buffer and then providing a memory pointer to it and its size in bytes to one of the rPlot libraries.

When writing data, the application provides a pointer to a function that when supplied a memory pointer to a buffer and a size in bytes will write out the buffer to the correct destination.

### Writing a Producer

The `rplot` library is the programming interface for writing a producer. An application using this library calls functions that generate rPlot primitives. Below is a simple example of a program using the library:

```
/* hellow.c
   ============ 
   Author: R.J.Barnes
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rfbuffer.h"
#include "rplot.h"

int stream(char *buf,int sze,void *data) {
  FILE *fp;
  fp=(FILE *) data;
  fwrite(buf,sze,1,stdout);
  return 0;
} 

int main(int argc,char *argv[]) {

  struct Rplot *rplot=NULL;
  
  rplot=RplotMake();
  RplotSetText(rplot,stream,stdout);
  RplotMakePlot(rplot,"hellow.rp.xml",540,540,24);
  RplotEllipse(rplot,NULL,270,270,
            220,150,1,0xffff0000,0x0f,0,NULL);
  RplotText(rplot,NULL,"Helvetica",34.0,
            180,280,12,"Hellow World",0xff00ff00,0x0f,0);          
  RplotEndPlot(rplot);  
  return 0;
}
```

The program first establishes a data structure to store plot parameters in by calling `RplotMake`. Next the program calls `RplotSetText` to tell the `Rplot` library that it should use the application supplied function `stream` to write the output. Then the plot is started by calling `RplotMakePlot`. A simple ellipse and some text are then plotted and then finally a call to `RplotEndPlot` closes the plot.

### Writing a Consumer (Renderer)

Writing a Consumer or Renderer is slightly more complex. The Consumer must parse the XML and then, depending on the desired output, call a function in one of the graphics rendering libraries. The <a href="../../../base/src.lib/xml/xml/">`xml`</a> library is used to parse the XML. The library extract XML tag pairs and any associated content and passes them onto an application supplied function that decodes them. The <a href="../src.lib/base/graphic/rplotin/">`rplotin`</a> library provides such a decoding function for the rPlot graphics specification.

Below is a simple Renderer that is partially implemented for clarity:
```
/* rplot.c
   ======= 
   Author: R.J.Barnes
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rxml.h"
#include "rfbuffer.h"
#include "rplotin.h"

int makeplot(char *name,int depth,float wdt,float hgt,
                         void *data) {
  struct FrameBuffer **ptr;
  ptr=(struct FrameBuffer **) data;
  if (ptr==NULL) return -1;
  if (*ptr !=NULL) FrameBufferFree(*ptr);
  *ptr=FrameBufferMake(name,wdt,hgt,depth);
  if (*ptr==NULL) return -1;
  return 0;
}

int plotshape(char *name,struct RplotInMatrix *matrix,
                          float x,float y,
                          float w,float h,int fill,
                          unsigned int color,unsigned char mask,float width,
                          void *data) {
  struct FrameBuffer *img;
  struct FrameBufferMatrix fbmatrix;
  struct FrameBufferMatrix *mptr=NULL;

  img=*((struct FrameBuffer **) data);
  if (img==NULL) return -1;

  if (matrix !=NULL) {
      fbmatrix.a=matrix->a;
      fbmatrix.b=matrix->b;
      fbmatrix.c=matrix->c;
      fbmatrix.d=matrix->d;
      mptr=&fbmatrix;
  }

  if (strcmp(name,"ellipse")==0)
      FrameBufferEllipse(img,mptr,x,y,
			 w,h,fill,color,mask,width,NULL,NULL);

  if (strcmp(name,"rectangle")==0)
      FrameBufferRectangle(img,mptr,x,y,
			 w,h,fill,color,mask,width,NULL,NULL);

  return 0;
}

int plotline(float x1,float y1,float x2,float y2,
                         unsigned int color,unsigned char mask,float width,
                         void *data) {
  struct FrameBuffer *img;
  img=*((struct FrameBuffer **) data);
  if (img==NULL) return -1;

  FrameBufferLine(img,x1,y1,x2,y2,color,mask,width,NULL,NULL);

  return 0;
}


int main(int argc,char *argv[]) {

  int s=0;
  FILE *fp;
  char lbuf[256];
  struct FrameBuffer *img=NULL;
 
  struct XMLdata *xmldata=NULL;
  struct RplotInPlot *rplotin=NULL;

  xmldata=XMLMake();
  rplotin=RplotInMake(xmldata);

  RplotInSetMake(rplotin,makeplot,&img);
  RplotInSetShape(rplotin,plotshape,&img); 
  RplotInSetLine(rplotin,plotline,&img); 

  XMLSetStart(xmldata,RplotInStart,rplotin);
  XMLSetEnd(xmldata,RplotInEnd,rplotin);
  XMLSetText(xmldata,RplotInText,rplotin);

  if (argc !=1) {
    fp=fopen(argv[argc-1],"r");
    if (fp==NULL) {
      fprintf(stderr,"Error opening file.\n");
      exit(-1);
    }
  } else fp=stdin;


  while(fgets(lbuf,255,fp) !=NULL) {      
    s=XMLDecode(xmldata,lbuf,strlen(lbuf));
    if (s !=0) break;

  }
  if (s !=0) {
    fprintf(stderr,"Error parsing document.\n");
    exit(0);
  }

  if (img !=NULL) FrameBufferSavePPM(img,stdout);
  return 0;
}
```

This simple program first established a data structure for the XML parser by calling `XMLMakeData`. This is followed by a similar call for the rPlot decoder, `RplotInMake`. The next step is to supply the rPlot decoder with functions for implementing graphics commands; this is done using the calls to `RplotInSetMake`, `RplotInSetShape`, and `RplotInSetLine`.

The three functions `makeplot`,`plotshape` and `plotline` will subsequently be called by the rPlot decoder. The functions will receive a pointer to the variable `img` which is istself a pointer to a frame buffer data structure. This pointer allows the three functions to create and modify the frame buffer that will eventually be written out.

The next few lines of code tell the XML parser which functions in the `rplotin` library it should call as it reads the document.

The remainder of the program simply reads in a file line by line and calls the XML parser with `XMLDecode`. When the file has been read a check is made to see if the frame buffer `img` has been created, and if so, it is written to standard out as a Portable PixMap (PPM) file with `FrameBufferSavePPM`.

### Rendering Library

The `fbpsrplot` library implements the the graphics functions required for the `rplotin` decoder for bitmap, PostScript and rPlot output. (an rPlot renderer might seem redundant, but it can be used to "flatten" and rplot document that relies on the `include` tag to incorporate other files).

The example below uses the library to implement an application for plotting both bitmap output:

```
/* rplottoppm.c
   ============ 
   Author: R.J.Barnes
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rxml.h"
#include "rfbuffer.h"
#include "rplotin.h"
#include "fontdb.h"
#include "fbrplot.h"

int main(int argc,char *argv[]) {

  int s=0;
  FILE *fp;
  char lbuf[256];
  struct FrameBufferFontDB *fontdb=NULL;

  struct XMLdata *xmldata=NULL;
  struct RplotInPlot *rplotin=NULL;
  struct FrameBufferRplot *fbplot=NULL;

  xmldata=XMLMake();
  rplotin=RplotInMake(xmldata);

  fp=fopen("fontdb.xml","r");
  if (fp !=NULL) {
    fontdb=FrameBufferFontDBLoad(fp);
    fclose(fp);
  }

  if (fontdb==NULL) {
    fprintf(stderr,"failed to load fonts\n");
    exit(-1);
  }

  fbplot=FrameBufferRplotMake(fontdb,NULL);
  FrameBufferRplotSetAll(rplotin,fbplot);

  XMLSetStart(xmldata,RplotInStart,rplotin);
  XMLSetEnd(xmldata,RplotInEnd,rplotin);
  XMLSetText(xmldata,RplotInText,rplotin);

  if (argc !=1) {
    fp=fopen(argv[argc-1],"r");
    if (fp==NULL) {
      fprintf(stderr,"Error opening file.\n");
      exit(-1);
    }
  } else fp=stdin;


  while(fgets(lbuf,255,fp) !=NULL) {      
    s=XMLDecode(xmldata,lbuf,strlen(lbuf));
    if (s !=0) break;

  }
  if (s !=0) {
    fprintf(stderr,"Error parsing document.\n");
    exit(0);
  }

  if (fbplot->img !=NULL) FrameBufferSavePPM(fbplot->img,stdout);
  return 0;
}
```
 
### Writing a Producer and Consumer

Often an application is required to produce not just an rPlot XML document as output but also a finished rendered plot. One approach would be for the application to determine the type of output required and then for each graphic command call the appropriate function in the rendering library. However this approach is very inefficient and prone to error.

An alternative approach is to write a program that acts as both producer and consumer. The application determines which type of output is required and sets up the `rplotin` library as a consumer. The stream handler for the `rplot` library is then set to point to the `XMLDecode` function.

Subsequent calls to the `rplot` library will then automatically be passed to the `rplotin` decoder for rendering.

Below is a more sophisticated program that produces, rPlot, PostScript and Portable PixMap format output:
```
/* prodcon.c
   ========= 
   Author: R.J.Barnes
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rxml.h"
#include "rfbuffer.h"
#include "rps.h"
#include "rplot.h"
#include "rplotin.h"
#include "fontdb.h"
#include "fbrplot.h"
#include "psrplot.h"


int stream(char *buf,int sze,void *data) {
  FILE *fp;
  fp=(FILE *) data;
  fwrite(buf,sze,1,stdout);
  return 0;
} 

int xmldecode(char *buf,int sze,void *data) {
  struct XMLdata *xmldata;
  xmldata=(struct XMLdata *) data;
  return XMLDecode(xmldata,buf,sze);
} 

int main(int argc,char *argv[]) {

  FILE *fp;
 
  struct Rplot *rplot=NULL;
 
  struct FrameBufferFontDB *fontdb=NULL;
  struct XMLdata *xmldata=NULL;
  struct RplotInPlot *rplotin=NULL;
  struct FrameBufferRplot *fbplot=NULL;
  struct PostScriptRplot *psplot=NULL;
  struct PostScript *psdata=NULL;

  unsigned char ppmflg=0;
  unsigned char psflg=0;
  unsigned char rflg=0;

  int x;

  xmldata=XMLMake();

  if (argc !=1) { 
    if (strcmp(argv[1],"-ppm")==0) ppmflg=1;
    if (strcmp(argv[1],"-ps")==0) psflg=1;
    if ((ppmflg) || (psflg)) rflg=1;
  }

  if (rflg) {
    rplotin=RplotInMake(xmldata);

    fp=fopen("fontdb.xml","r");
    if (fp !=NULL) {
      fontdb=FrameBufferFontDBLoad(fp);
      fclose(fp);
    }

    if (fontdb==NULL) {
      fprintf(stderr,"failed to load fonts\n");
      exit(-1);
    }
    if (ppmflg) {
      fbplot=FrameBufferRplotMake(fontdb,NULL);
      FrameBufferRplotSetAll(rplotin,fbplot);
    } 
    if (psflg) {
       psdata=PostScriptMake();
       PostScriptSetText(psdata,stream,stdout);
       psplot=PostScriptRplotMake(psdata,NULL);
       PostScriptRplotSetAll(rplotin,psplot);
       psplot->land=0;
       psplot->xoff=40;
       psplot->yoff=40;
    }
    XMLSetStart(xmldata,RplotInStart,rplotin);
    XMLSetEnd(xmldata,RplotInEnd,rplotin);
    XMLSetText(xmldata,RplotInText,rplotin);
  }

  rplot=RplotMake();
  if (rflg) RplotSetText(rplot,xmldecode,xmldata);
  else RplotSetText(rplot,stream,stdout);


  RplotMakePlot(rplot,"hellow.rp.xml",540,540,24);

  for (x=10;x<220;x+=10) 
  RplotEllipse(rplot,NULL,270,270,
            x,x,0,0xffff0000,0x0f,0,NULL);

  RplotText(rplot,NULL,"Helvetica",34.0,
            180,280,12,"Hellow World",0xff00ff00,0x0f,0);
           
  RplotEndPlot(rplot);  

  if (ppmflg) {
    if (fbplot->img !=NULL) FrameBufferSavePPM(fbplot->img,stdout);
  }
  return 0;
}
```

## References
[rPlot XML Specification](a href="legacy_software/rPlotXML.md")

## History

2004/11/05  Initial Revision.



