/* plot_time.h
   ===========
   Author: R.J.Barnes
*/

/*
   See license.txt
*/



void plot_time(struct Plot *plot,
               float xoff,float yoff,float wdt,float hgt,int flg,
               double stime,double etime,
               unsigned int color,unsigned char mask,
               char *fontname,float fontsize,
               void *txtdata);
