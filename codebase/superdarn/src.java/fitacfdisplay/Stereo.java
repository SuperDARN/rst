/* Stereo.java
   ===========
   Author R.J.Barnes
*/

/*
 $License$
*/





import java.awt.*;
import java.lang.*;

public class Stereo extends Transform {
 
  public Stereo(double sf,double lat,double lon) {
    this.lat=lat;
    this.lon=lon;
    this.sf=sf;
  } 


 public boolean compute(MapPoint p,Cart s) {
    double z=0,k=0;
    double r=1.0;
   
    z=Math.sin(lat*Math.PI/180)*Math.sin(p.lat*Math.PI/180)+
      Math.cos(lat*Math.PI/180)*Math.cos(p.lat*Math.PI/180)*
      Math.cos(Math.PI*(p.lon-lon)/180);

    if (Math.abs(p.lat-this.lat)>90) return false;
    if (z==-1) return false;

    k=sf*2/(1+z);

    s.x=(float) ((1.0+k*r*Math.cos(Math.PI*p.lat/180)*
	  Math.sin(Math.PI*(p.lon-lon)/180))/2.0);
    s.y=(float) ((1.0-k*r*(Math.cos(Math.PI*lat/180)*
			   Math.sin(Math.PI*p.lat/180)-
	  Math.sin(Math.PI*lat/180)*Math.cos(Math.PI*p.lat/180)*
	  Math.cos(Math.PI*(p.lon-lon)/180)))/2.0);
    return true;
  }


}






