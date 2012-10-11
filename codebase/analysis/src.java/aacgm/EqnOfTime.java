/* EqnOfTime.java
   ============== 
   By R.J.Barnes */


/*
 $License$
*/




import java.util.*;
import java.lang.*;
import java.io.*;

public class EqnOfTime {
   
  int max=12;
    double coeff[][]={
     {-105.8,596.2,4.4,-12.7,-429.0,-2.1,19.3},
     {-105.9,596.2,4.4,-12.7,-429.0,-2.1,19.3},
     {-106.1,596.2,4.4,-12.7,-428.9,-2.1,19.3},
     {-106.2,596.2,4.4,-12.7,-428.9,-2.1,19.3},
     {-106.4,596.1,4.4,-12.7,-428.9,-2.1,19.3},
     {-106.5,596.1,4.4,-12.7,-428.8,-2.1,19.3},
     {-106.6,596.1,4.4,-12.7,-428.8,-2.1,19.3},
     {-106.7,596.1,4.4,-12.7,-428.7,-2.1,19.3},
     {-106.8,596.1,4.4,-12.7,-428.7,-2.1,19.3},
     {-107.0,596.1,4.4,-12.7,-428.7,-2.1,19.3},
     {-107.2,596.1,4.4,-12.7,-428.6,-2.1,19.3},
     {-107.3,596.1,4.4,-12.7,-428.6,-2.1,19.3},
  };

  public EqnOfTime() {
    
  }

  public EqnOfTime(DataInputStream in) throws IOException {
     int i,j;
     max=in.readInt();
 
     coeff=new double[max][7];
     for (i=0;i<max;i++) {
	 for (j=0;j<7;j++) {
	     coeff[i][j]=in.readDouble();
	 }
     }
  }



    public double eval(double lon,int yr) {
       
      int index;
 
      if (yr<88) index = yr + 2000 - 1988;
      if ((yr>=88) && (yr<100)) index = yr - 88;
      else if ((yr>=100) && (yr<1900)) index=yr-88;
      else index=yr-1988;

   
      if (index<1) index = 1;
      if (index>max) index = max;

      return coeff[index-1][0]*Math.sin(lon*Math.PI/180.0)+
             coeff[index-1][1]*Math.sin(2.0*lon*Math.PI/180.0)+
             coeff[index-1][2]*Math.sin(3.0*lon*Math.PI/180.0)+
             coeff[index-1][3]*Math.sin(4.0*lon*Math.PI/180.0)+
             coeff[index-1][4]*Math.cos(lon*Math.PI/180.0)+
             coeff[index-1][5]*Math.cos(2.0*lon*Math.PI/180.0)+
             coeff[index-1][6]*Math.cos(3.0*lon*Math.PI/180.0);
    }
}
