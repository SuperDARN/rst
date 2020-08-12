/* snd.h
   =====
   Author: E.G.Thomas
*/


double snd_find(FILE *fp,struct SndData *snd,
                double sdate,double stime);

double snd_scan(double ctime,FILE *fp,int rflg,
                struct SndData *snd,
                int bmnum,int cpid,int sflg,int scan);

void snd_tplot(struct SndData *snd,struct tplot *tptr);
