/* Polar.java
   ==========
   R.J.Barnes
*/

/*
 $License$
*/





import java.lang.*;

public class Polar {
    double rho;
    double lat;
    double lon;

 public Polar() {
    this.rho=0;
    this.lat=0;
    this.lon=0;
  }

  public Polar(double rho,double lat,double lon) {
    this.rho=(float) rho;
    this.lat=(float) lat;
    this.lon=(float) lon;
  }

  public Polar(Polar c) {
    this.rho=c.rho;
    this.lat=c.lat;
    this.lon=c.lon;
  }

     
}


