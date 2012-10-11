/* SolarLoc.java
   ============= 
   By R.J.Barnes */


/*
 $License$
*/





import java.util.*;
import java.lang.*;
import java.io.*;


public class SolarLoc {

  public double meanLon=0.0;
  public double dec=0.0; 

  int max=12;

  double L0[]={279.642,279.403,279.165,278.926,279.673,279.434,
		 279.196,278.957,279.704,279.465,279.226,278.982};
  double DL=0.985647;
  double G0[]={356.892984,356.637087,356.381191,356.125295,
                 356.854999,356.599102,356.343206,356.087308,
		 356.817011,356.561113,356.31,356.05};
  double DG=0.98560028;
  double EPS0[]={23.440722,23.440592,23.440462,23.440332,
	           23.440202,23.440072,23.439942,23.439811,
		   23.439682,23.439552,23.439422,23.439292};
  double DE=-0.00000036;

  public SolarLoc() {
    
  }

  public SolarLoc(DataInputStream in) throws IOException {
    int i;  
    max=in.readInt();
    L0=new double[max];
    G0=new double[max];
    EPS0=new double[max];
    
    for (i=0;i<max;i++) L0[i]=in.readDouble();
    DL=in.readDouble();
 
   for (i=0;i<max;i++) G0[i]=in.readDouble();
    DG=in.readDouble();
  
    for (i=0;i<max;i++) EPS0[i]=in.readDouble();
   
    DE=in.readDouble();
  }

   public void eval(int yr,int t1) {

      int index,delta_yr,yr_step,i;
      double d,lambda,g,eps,L;

      d = 0;
      if (yr<1900) index = yr - 88;
      else index = yr - 1988;

      if (index<=0) delta_yr = index - 1;
      else if (index>10) delta_yr = index - 10;
      else  delta_yr = 0;
     
      if (index<=0) index = 1;
      if (index>max) index = max;

     yr_step = (int) AACGMMath.sgn(1,delta_yr);
     delta_yr = Math.abs(delta_yr);

     for (i=1;i<=delta_yr;i++) {
      if (yr_step>0) yr=98+i-1;
      else yr=89-i;

      if (AACGMMath.mod(yr,4)==0) d = d + 366*yr_step;
      else d = d + 365*yr_step;
    }

     
    d = d + t1/86400;
    L = L0[index-1] + DL*d;
    g = G0[index-1] + DG*d;

    while (L<0) L = L + 360;
    while (g<0) g = g + 360;
    
    L = AACGMMath.mod(L,360.0);
    g = AACGMMath.mod(g,360.0);

    lambda = L + 1.915*Math.sin(g*Math.PI/180.0) + 
                 0.020*Math.sin(2*g*Math.PI/180.0);
    eps = EPS0[index-1] + DE*d;
  
    dec=Math.asin(Math.sin(eps*Math.PI/180.0)*
		  Math.sin(lambda*Math.PI/180.0))*180.0/Math.PI;
    meanLon=L;
  
  }

  public double getDec() {
    return this.dec;
  }

  public double getLon() {
    return this.meanLon;
  }

};


