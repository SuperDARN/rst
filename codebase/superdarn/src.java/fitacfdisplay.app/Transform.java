/* Transform.java
   ==============
   Author R.J.Barnes
*/

/*
 $License$
*/





import java.awt.*;

/* This object plots a set of line vectors on the screen
 * eg. coastlines or grid values */

public abstract class Transform {
  public double lat=0;
  public double lon=0;
  public double sf=0;

  public void setPos(double lat,double lon) {
    this.lat=lat;
    this.lon=lon;
  } 


 public abstract boolean compute(MapPoint p,Cart s);

 public void run(PlotPoly plot) {
   int num;
   boolean flag;
   boolean status;
   int i,j;
   Cart c;
   PolyInfo info;
   MapPoint p;
   num=plot.root.info.size();
   plot.reset();
   for (i=0;i<num;i++) {
     info=(PolyInfo) plot.root.info.elementAt(i); 
     flag=true;
     for (j=0;j<info.num;j++) {
        p=(MapPoint) plot.root.data.elementAt(info.off+j);        
        c=new Cart();
        status=this.compute(p,c);
        if (status==true) {
          if (flag==true) plot.addVector(info.type+1,c);
          else plot.addVector(0,c);
          flag=false;
	}
     }
    
   }
   plot.complete();
 }

}




