/* plot_excluded.h
   =============== 
   Author: R.J.Barnes
*/


/*
   See license.txt
*/




void plot_excluded(struct Plot *plot,
              struct CnvGrid *ptr,float latmin,int magflg,
	    float xoff,float yoff,float wdt,float hgt,float sf,float rad,
              int (*trnf)(int,void *,int,void *,void *data),void *data,
	       unsigned int color,float width);

