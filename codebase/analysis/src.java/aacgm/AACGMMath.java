/* AACGMMath.java
   ============= 
   By R.J.Barnes */


/*
 $License$
*/




import java.util.*;
import java.lang.*;

public class AACGMMath {

   static double sgn(double a,double b) {
      double x=0;
      x=(double) ((a>=0) ? a : -a);
      return (double) ((b>=0) ? x: -x);
    }


    static double mod(double x,double y) {
      double quotient;
      quotient = x / y;
      if (quotient >= 0) quotient = Math.floor(quotient);
      else quotient = -Math.floor(-quotient);
      return(x - y *quotient);
    }





};
