/* TimeUTC.java
   ============ 
   Author: R.J.Barnes
*/


/*
 $License$
*/




import java.lang.*; 
import java.text.*;

public class TimeUTC {

  static String month[]={"January","February","March","April","May","June",
            "July","August","September","October","November","December"};
	
  public static String getString(int yr,int mo,int dy,int hr,int mt,int sc) {
    DecimalFormat df=new DecimalFormat("00");
  
    return month[mo-1]+" "+df.format(dy)+", "+yr+"   "+df.format(hr)+
           df.format(mt)+":"+df.format(sc)+" UT";
  }	     
}     
	    
	    
