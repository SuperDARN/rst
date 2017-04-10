/* plot_logo.h
   =========== 
   Author: R.J.Barnes
*/


/*
   See license.txt
*/






void plot_logo(struct Plot *plot,
               float xoff,float yoff,float wdt,float hgt,
	       unsigned int color,unsigned char mask,
               char *fontname,float fontsize,
               void *txtdata);

void plot_web(struct Plot *plot,
               float xoff,float yoff,float wdt,float hgt,
	       unsigned int color,unsigned char mask,
               char *fontname,float fontsize,
	      void *txtdata);

void plot_credit(struct Plot *plot,
               float xoff,float yoff,float wdt,float hgt,
	       unsigned int color,unsigned char mask,
               char *fontname,float fontsize,
		 void *txtdata);

 
