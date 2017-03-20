/* plot_source.h
   ============= 
   Author: R.J.Barnes
*/


/*
   See license.txt
*/






void plot_source(struct Rplot *rplot,
		 float xoff,float yoff,
                 char *source,int major,int minor,
  	         unsigned int color,unsigned char mask,
                 char *fontname,float fontsize,
                 void *txtdata);
