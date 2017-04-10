/* grplot_key.c
   ============
   Author: R.J.Barnes
*/

/*
   See license.txt
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "rfbuffer.h"
#include "iplot.h"
#include "polygon.h"
#include "grplotraw.h"
#include "grplotstd.h"
#include "grplot.h"





 
int GrplotKey(struct Grplot *plt,int num,
            float xoff,float yoff,
            float wdt,float hgt,
	    double xmin,double xmax,double step,
	    int key_flg,int ttl_num,char *ttl_txt,
            char * (*make_text)(double,double,double,void *),
            void *textdata,
            char *fntname,float fntsze,
	    unsigned int color,unsigned char mask,float width,
            int ksze,unsigned char *a,
            unsigned char *r,unsigned char *g,unsigned char *b) {

  int s=0;
  int xnm,ynm;
  float ox,oy;
  float tx,ty;
  if (plt==NULL) return -1;
 
  xnm=num % plt->xnum;
  ynm=num / plt->xnum;

  ox=plt->xoff+xnm*(plt->box_wdt+plt->lpad+plt->rpad);
  oy=plt->yoff+ynm*(plt->box_hgt+plt->tpad+plt->bpad);
  
  if (key_flg & 0x01) {
    ty=oy+(plt->box_hgt-hgt)/2;  
    s=GrplotStdKey(plt->plot,ox-xoff-wdt,ty+yoff,wdt,hgt,
                      xmin,xmax,step,0,0,1,ttl_num,ttl_txt,
                      plt->text.box,plt->text.data,make_text,textdata,
                      fntname,fntsze,color,mask,width,ksze,a,r,g,b);
  }

  if (key_flg & 0x02) {
    ty=oy+(plt->box_hgt-hgt)/2;  
    s=GrplotStdKey(plt->plot,ox+plt->box_wdt+xoff,ty+yoff,wdt,hgt,
                      xmin,xmax,step,0,0,0,ttl_num,ttl_txt,
                      plt->text.box,plt->text.data,make_text,textdata,
                      fntname,fntsze,color,mask,width,ksze,a,r,g,b);

  }

  if (key_flg & 0x04) {
    tx=ox+(plt->box_wdt-hgt)/2;  
    s=GrplotStdKey(plt->plot,tx-yoff,oy-xoff-wdt,hgt,wdt,
                      xmin,xmax,step,1,0,0,ttl_num,ttl_txt,
                      plt->text.box,plt->text.data,make_text,textdata,
                      fntname,fntsze,color,mask,width,ksze,a,r,g,b);
  }

  if (key_flg & 0x08) {
    tx=ox+(plt->box_wdt-hgt)/2;  
    s=GrplotStdKey(plt->plot,tx-yoff,oy+plt->box_hgt+xoff,hgt,wdt,
                      xmin,xmax,step,1,0,1,ttl_num,ttl_txt,
                      plt->text.box,plt->text.data,make_text,textdata,
                      fntname,fntsze,color,mask,width,ksze,a,r,g,b);
  }

  return 0;
}

  

int GrplotKeyLog(struct Grplot *plt,int num,
            float xoff,float yoff,
            float wdt,float hgt,
	    double xmin,double xmax,double tick_major,double tick_minor,
	    int key_flg,int ttl_num,char *ttl_txt,
            char * (*make_text)(double,double,double,void *),
            void *textdata,
            char *fntname,float fntsze,
	    unsigned int color,unsigned char mask,float width,
            int ksze,unsigned char *a,
            unsigned char *r,unsigned char *g,unsigned char *b) {

  int s=0;
  int xnm,ynm;
  float ox,oy;
  float tx,ty;
  if (plt==NULL) return -1;
 
  xnm=num % plt->xnum;
  ynm=num / plt->xnum;

  ox=plt->xoff+xnm*(plt->box_wdt+plt->lpad+plt->rpad);
  oy=plt->yoff+ynm*(plt->box_hgt+plt->tpad+plt->bpad);
  
  if (key_flg & 0x01) {
    ty=oy+(plt->box_hgt-hgt)/2;  
    s=GrplotStdKeyLog(plt->plot,ox-xoff-wdt,ty+yoff,wdt,hgt,
                      xmin,xmax,tick_major,tick_minor,0,0,1,ttl_num,ttl_txt,
                      plt->text.box,plt->text.data,make_text,textdata,
                      fntname,fntsze,color,mask,width,ksze,a,r,g,b);
  }

  if (key_flg & 0x02) {
    ty=oy+(plt->box_hgt-hgt)/2;  
    s=GrplotStdKeyLog(plt->plot,ox+plt->box_wdt+xoff,ty+yoff,wdt,hgt,
                      xmin,xmax,tick_major,tick_minor,0,0,0,ttl_num,ttl_txt,
                      plt->text.box,plt->text.data,make_text,textdata,
                      fntname,fntsze,color,mask,width,ksze,a,r,g,b);

  }

  if (key_flg & 0x04) {
    tx=ox+(plt->box_wdt-hgt)/2;  
    s=GrplotStdKeyLog(plt->plot,tx-yoff,oy-xoff-wdt,hgt,wdt,
                      xmin,xmax,tick_major,tick_minor,1,0,0,ttl_num,ttl_txt,
                      plt->text.box,plt->text.data,make_text,textdata,
                      fntname,fntsze,color,mask,width,ksze,a,r,g,b);
  }

  if (key_flg & 0x08) {
    tx=ox+(plt->box_wdt-hgt)/2;  
    s=GrplotStdKeyLog(plt->plot,tx-yoff,oy+plt->box_hgt+xoff,hgt,wdt,
                      xmin,xmax,tick_major,tick_minor,1,0,1,ttl_num,ttl_txt,
                      plt->text.box,plt->text.data,make_text,textdata,
                      fntname,fntsze,color,mask,width,ksze,a,r,g,b);
  }

  return 0;
}

  
