/* CSDData.java
   ============
   Author R.J.Barnes
*/

/*
 $License$
*/




public class CSDData {

  /* this is the header information */

  double pmax;
  double vmax;
  double wmax;
  short stid;
  int stored;
  int num=0;

  /* this is the information for a frame */

    double time[]=new double[32];
    short bmnum[]=new short[32]; 
    short channel[]=new short[32];
    short scan[]=new short[32];
    short cp[]=new short[32];
    short inttsc[]=new short[32];
    int inttus[]=new int[32];
    short frang[]=new short[32]; 
    short rsep[]=new short[32];
    short rxrise[]=new short[32];
    short tfreq[]=new short[32];
    int noise[]=new int[32];
    short atten[]=new short[32];
    short nave[]=new short[32];
    short nrang[]=new short[32];
    byte map[][][]=new byte[32][300][4];
}







