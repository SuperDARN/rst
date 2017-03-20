/* graphic.h
   ======
   Author: R.J.Barnes
*/


/*
   See license.txt
*/




int txtbox(char *fntname,float sze,int num,char *txt,float *box,void *data);

void plot_frame(struct Grplot *grplot,int pmax,double stime,
                double etime,int dflg,
                unsigned int color,float width,char *fontname,
                float fontsize);

void plot_pos(struct Grplot *grplot,int p,
              struct posdata *pos,double stime,double etime,int mode,
              unsigned int color,float width,char *fontname,
              float fontsize);

void plot_plasma(struct Grplot *grplot,int p,struct plasmadata *plasma,
              double stime,double etime,int mode,int *pflg,
              unsigned int color,float width,char *fontname,
	      float fontsize);

void plot_imf(struct Grplot *grplot,int p,struct imfdata *imf,
              double stime,double etime,int mode,int *pflg,
              unsigned int color,float width,char *fontname,
              float fontsize);

void plot_title(struct Grplot *grplot,double stime,double etime,
                char *sat,char *plot,int mode,
                unsigned int color,float width,char *fontname,
                float fontsize);








