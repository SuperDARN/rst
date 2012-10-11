/* Meridian.java
   =============
  Author R.J.Barnes
*/

/*
 $License$
*/





import java.awt.*;
import java.net.*;

public class Meridian  {


 static public void make(PolyVector store,float latspc,float lonspc) {
    float lat,lon;
    store.reset();

    for (lat=90-latspc;lat>0;lat-=latspc) {
      for (lon=0;lon<360;lon+=5) {
         if (lon==0) store.addVector(1,new MapPoint(lat,lon));
         else store.addVector(0,new MapPoint(lat,lon));
      }
      store.addVector(0,new MapPoint(lat,lon));
    } 

   for (lon=0;lon<360;lon+=lonspc) {
     for (lat=0;lat<=(90-latspc);lat+=2) {
       if (lat==0) store.addVector(1,new MapPoint(lat,lon));
       else store.addVector(0,new MapPoint(lat,lon));
     }
     for (lat=90-latspc;lat>0;lat-=2) store.addVector(0,new MapPoint(lat,lon));
   }

   store.complete();
  }
}













