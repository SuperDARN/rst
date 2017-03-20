/* plot_fit.h
   ========== 
   Author: R.J.Barnes
*/


/*
   See license.txt
*/




void plot_fit(struct Plot *plot,
              struct CnvGrid *ptr,float latmin,int magflg,
              float xoff,float yoff,float wdt,float hgt,float sf,float rad,
              int (*trnf)(int,void *,int,void *,void *data),void *data,
              unsigned int(*cfn)(double,void *),void *cdata,
              float width);



