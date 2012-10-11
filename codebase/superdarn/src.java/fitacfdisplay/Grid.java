/* Grid.java
   =========
   Author R.J.Barnes
*/

/*
 $License$
*/





import java.awt.*;

public class Grid  {
  public int mbeam=16;
  public Cart grid[][][]=
         new Cart [RadarData.MAX_BEAM][RadarData.MAX_RANGE+1][2];
  public boolean set[]=new boolean[RadarData.MAX_BEAM];
  public int nrng[]=new int [RadarData.MAX_BEAM]; 

 public RangeBeam locate(Dimension  d,int x,int y) {
    if (d==null) return null;
    int beam,rng;
    int a,b;
  
    for (beam=0;beam<mbeam;beam++) {
      Polygon p=new Polygon();
      if (set[beam]==false) continue;
      for (rng=0;rng<nrng[beam]+1;rng++) {
        a=(int) (grid[beam][rng][0].x*d.width);
        b=(int) (grid[beam][rng][0].y*d.height);
        p.addPoint(a,b);
      }
      for (rng=nrng[beam];rng>=0;rng--) {
        a=(int) (grid[beam][rng][1].x*d.width);
        b=(int) (grid[beam][rng][1].y*d.height);
        p.addPoint(a,b);
      }
      if (p.contains(x,y)==true) break;
    }
    if (beam==mbeam) return null;
    for (rng=0;rng<nrng[beam];rng++) {
        Polygon p=new Polygon();
        a=(int) (grid[beam][rng][0].x*d.width);
        b=(int) (grid[beam][rng][0].y*d.height);
        p.addPoint(a,b);
        a=(int) (grid[beam][rng+1][0].x*d.width);
        b=(int) (grid[beam][rng+1][0].y*d.height);
        p.addPoint(a,b);
        a=(int) (grid[beam][rng+1][1].x*d.width);
        b=(int) (grid[beam][rng+1][1].y*d.height);
        p.addPoint(a,b);
        a=(int) (grid[beam][rng][1].x*d.width);
        b=(int) (grid[beam][rng][1].y*d.height);
        p.addPoint(a,b);
        if (p.contains(x,y)==true) break;
    }
    if (rng==nrng[beam]) return null;
    return new RangeBeam(rng,beam);
  }

  public void drawCell(Graphics g,Dimension d,int beam,int range,Color col) {
  
    if (g==null) return;
    if (set[beam]==false) return;
    if (range>=nrng[beam]) return;
    int xpnt[]=new int[4];
    int ypnt[]=new int[4];
  
    g.setColor(col);   
    xpnt[0]=(int) (grid[beam][range][0].x*d.width);
    ypnt[0]=(int) (grid[beam][range][0].y*d.height);
    xpnt[1]=(int) (grid[beam][range+1][0].x*d.width);
    ypnt[1]=(int) (grid[beam][range+1][0].y*d.height);
    xpnt[2]=(int) (grid[beam][range+1][1].x*d.width);
    ypnt[2]=(int) (grid[beam][range+1][1].y*d.height);
    xpnt[3]=(int) (grid[beam][range][1].x*d.width);
    ypnt[3]=(int) (grid[beam][range][1].y*d.height);
    g.fillPolygon(xpnt,ypnt,4);
  }  

  public Polygon clipBeam(Graphics g,Dimension d,int beam) {
    if (g==null) return null;
    if (set[beam]==false) return null;
    int rng;
    int a,b;
   
    Polygon p=new Polygon();
    for (rng=0;rng<nrng[beam]+1;rng++) {
      a=(int) (grid[beam][rng][0].x*d.width);
      b=(int) (grid[beam][rng][0].y*d.height);
      p.addPoint(a,b);
    }
    for (rng=nrng[beam];rng>=0;rng--) {
      a=(int) (grid[beam][rng][1].x*d.width);
      b=(int) (grid[beam][rng][1].y*d.height);
      p.addPoint(a,b);
    }      
    return p;
  }
  
  public void drawGrid(Graphics g,Dimension dm,int beam,int step) {
    if (g==null) return;
    if (set[beam]==false) return;
    int rng;
    int a,b,c,d;
 
    if (step !=0) {
      for (rng=0;rng<nrng[beam]+1;rng+=step) {
        a=(int) (grid[beam][rng][0].x*dm.width);
        b=(int) (grid[beam][rng][0].y*dm.height);
        c=(int) (grid[beam][rng][1].x*dm.width);
        d=(int) (grid[beam][rng][1].y*dm.height);
      g.drawLine(a,b,c,d);
      }
     } else {
       a=(int) (grid[beam][0][0].x*dm.width);
       b=(int) (grid[beam][0][0].y*dm.height);
       c=(int) (grid[beam][0][1].x*dm.width);
       d=(int) (grid[beam][0][1].y*dm.height);
       g.drawLine(a,b,c,d);
     }
     a=(int) (grid[beam][nrng[beam]][0].x*dm.width);
     b=(int) (grid[beam][nrng[beam]][0].y*dm.height);
     c=(int) (grid[beam][nrng[beam]][1].x*dm.width);
     d=(int) (grid[beam][nrng[beam]][1].y*dm.height);
     g.drawLine(a,b,c,d);

     if ((step !=0) || (beam==0)) {
       for (rng=0;rng<nrng[beam];rng++) {
         a=(int) (grid[beam][rng][0].x*dm.width);
         b=(int) (grid[beam][rng][0].y*dm.height);
         c=(int) (grid[beam][rng+1][0].x*dm.width);
         d=(int) (grid[beam][rng+1][0].y*dm.height);
         g.drawLine(a,b,c,d);
       }
     }
     if ((step !=0) || (beam==mbeam-1)) {
        for (rng=0;rng<nrng[beam];rng++) {
          a=(int) (grid[beam][rng][1].x*dm.width);
          b=(int) (grid[beam][rng][1].y*dm.height);
          c=(int) (grid[beam][rng+1][1].x*dm.width);
          d=(int) (grid[beam][rng+1][1].y*dm.height);
          g.drawLine(a,b,c,d);
        }
     }
  }

 


}






