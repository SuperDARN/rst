/* plot_refvec.h
   ============= 
   Author: R.J.Barnes
*/


/*
   See license.txt
*/





void plot_refvec(struct Plot *plot,float px,float py,int or,double max,
              int magflg,
		 float xoff,float yoff,float wdt,float hgt,float sf,
              int (*trnf)(int,void *,int,void *,void *data),void *data,
              unsigned int color,char mask,float width,
              char *fontname,float fontsize,
              void *txtdata,float rad);

 
