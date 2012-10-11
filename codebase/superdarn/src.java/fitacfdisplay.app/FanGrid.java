/* FanGrid.java
   ============
   Author R.J.Barnes
*/

/*
 $License$
*/




import java.lang.*;


public class FanGrid  extends Grid {
  public void generate(int bmnum,int mbeam,int nrang) {
    int bm,rng;
    double rad;
    float sep=3;
    this.mbeam=mbeam;
    sep=48/mbeam;

    for (rng=0;rng<nrang+1;rng++) {
        float angle;
        angle=90-(bmnum-mbeam/2)*sep;
        rad=rng*0.9/nrang+0.2;
        if (grid[bmnum][rng][0]==null) grid[bmnum][rng][0]=new Cart(0,0);
        if (grid[bmnum][rng][1]==null) grid[bmnum][rng][1]=new Cart(0,0);
        grid[bmnum][rng][0].x= 
                    (float) (0.5 + (rad*Math.cos(Math.PI*angle/180)));
        grid[bmnum][rng][0].y= (float) (1.0 +(0.1)- 
                             (rad*Math.sin(Math.PI*angle/180)));
        angle=90-(bmnum-mbeam/2-1)*sep;
        grid[bmnum][rng][1].x= 
                      (float) (0.5 + (rad*Math.cos(Math.PI*angle/180)));
        grid[bmnum][rng][1].y= (float) (1.0 +(0.1)- 
                             (rad*Math.sin(Math.PI*angle/180)));

    }
    nrng[bmnum]=nrang;
    set[bmnum]=true;
  }
}








