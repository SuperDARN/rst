/* plot_logo.h
   =========== 
   Author: R.J.Barnes and others
*/


/*
   See license.txt
*/

void plot_logo(struct Plot *plot, float xoff,float yoff,float wdt,float hgt,
               unsigned int color,unsigned char mask, char *fontname,
               float fontsize, void *txtdata);

void plot_aacgm(struct Plot *plot, float xoff,float yoff,float wdt,float hgt,
              unsigned int color,unsigned char mask, char *fontname,
              float fontsize, void *txtdata, int old);

void plot_web(struct Plot *plot, float xoff,float yoff,float wdt,float hgt,
              unsigned int color,unsigned char mask, char *fontname,
              float fontsize, void *txtdata);

void plot_credit(struct Plot *plot, float xoff,float yoff,float wdt,float hgt,
                 unsigned int color,unsigned char mask, char *fontname,
                 float fontsize, void *txtdata);
 
