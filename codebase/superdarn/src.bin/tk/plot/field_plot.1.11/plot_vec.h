/* plot_vec.h
   ========== 
   Author: R.J.Barnes
*/


/*
   See license.txt
*/





void plot_vec(struct Plot *plot,struct RadarBeam *sbm,
               struct GeoLocBeam *gbm,float latmin,int magflg,
	      float xoff,float yoff,float wdt,float hgt,float sf,
               int (*trnf)(int,void *,int,void *,void *data),void *data,
 	       unsigned int(*cfn)(double,void *),void *cdata,
               unsigned int gscol,unsigned char gsflg,float width,float rad);
