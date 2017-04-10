/* plot_mmaxmin.h
   ============== 
   Author: R.J.Barnes
*/


/*
   See license.txt
*/




void plot_maxmin(struct Plot *plot,
                 struct CnvGrid *ptr,int magflg,
		 float xoff,float yoff,float wdt,float hgt,float sze,
                 int (*trnf)(int,void *,int,void *,void *data),void *data,
                 unsigned int color,char mask,float width,
                 struct PlotDash *dash);



