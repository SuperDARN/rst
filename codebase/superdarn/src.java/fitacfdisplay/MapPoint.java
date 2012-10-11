/* MapPoint.java 
   ============= 
   Author R.J.Barnes
*/

/*
 $License$
*/





import java.util.*;

public class MapPoint {
  float lat,lon;

  public MapPoint(float lat,float lon) {
    this.lat=lat;
    this.lon=lon;
  } 

  public MapPoint(double lat,double lon) {
    this.lat=(float) lat;
    this.lon=(float) lon;
  } 

}
