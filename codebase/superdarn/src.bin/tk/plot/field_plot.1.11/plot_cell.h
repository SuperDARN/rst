/* plot_cell.h
   ===========
   Author: R.J.Barnes
*/


/*
   See license.txt
*/




void plot_cell(struct Plot *plot,struct RadarBeam *sbm,
               struct GeoLocBeam *gbm,float latmin,int magflg,
               float xoff,float yoff,float wdt,float hgt,
               int (*trnf)(int,void *,int,void *,void *data),void *data,
               unsigned int(*cfn)(double,void *),void *cdata,
               int prm,unsigned int gscol,unsigned char gsflg);


