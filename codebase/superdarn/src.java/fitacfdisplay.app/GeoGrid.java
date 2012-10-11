/* GeoGrid.java
   ============
   Author R.J.Barnes
*/

/*
 $License$
*/




import java.lang.*;

public class GeoGrid  extends Grid {

  public void generate(Transform trf,
		       RadarPos rpos,int bmnum,int mbeam,int nrang,
		       int frang,int rsep,
                       int rxrise) {
    int rng;
    MapPoint p;
    Cart c=new Cart(0,0);
    this.mbeam=mbeam;
    for (rng=0;rng<nrang+1;rng++) {
        p=GeoMap.geo(0,rpos,frang,rsep,rxrise,300,mbeam,bmnum,rng);
        trf.compute(p,c);
        if (grid[bmnum][rng][0]==null) grid[bmnum][rng][0]=new Cart(0,0);
        grid[bmnum][rng][0].x=c.x;
        grid[bmnum][rng][0].y=c.y;

        p=GeoMap.geo(0,rpos,frang,rsep,rxrise,300,mbeam,bmnum+1,rng);
        trf.compute(p,c);
        if (grid[bmnum][rng][1]==null) grid[bmnum][rng][1]=new Cart(0,0);
        grid[bmnum][rng][1].x=c.x;
        grid[bmnum][rng][1].y=c.y;

    }
    nrng[bmnum]=nrang;
    set[bmnum]=true;
  }

   
}









