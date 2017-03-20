/* plot_imf.h
   ================
   Author: R.J.Barnes
*/

/*
   See license.txt
*/



void plot_imf(struct Plot *plot,
	      float xoff,float yoff,float rad,
	      float bx,float by,float bz,float mxval,
  	      unsigned int color,unsigned char mask,float width,
              char *fontname,float fontsize,
              void *txtdata);
