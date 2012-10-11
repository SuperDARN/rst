/* MathConvert.java
   ================
   Author: R.J.Barnes
*/


/*
  $License$
*/





import java.lang.*;

/* class that handles conversion of byte arrays into numerical values */
  
public class MathConvert {

  static public int bit(byte num) {
    return (num<0) ? (256+num) : num;
  }

  static public short shrt(byte buf[],int off) {
     return (short) (bit(buf[off]) | (bit(buf[off+1])<<8));
  }

  static public short shrt(byte buf[]) {
    return (short) (bit(buf[0]) | (bit(buf[1])<<8));
  }

  static public int lng(byte buf[],int off) {  
    int word=0;

    for(int i=3;i>=0;i--) {
       word=word<<8;
       word=word | bit(buf[i+off]);
     }
     return word;
  }  

  static public int lng(byte buf[]) {  
    int word=0;

    for(int i=3;i>=0;i--) {
       word=word<<8;
       word=word | bit(buf[i]);
     }
     return word;
  }  


  static public float flt(byte buf[],int off) {
    
     int word=0;
     for(int i=3;i>=0;i--) {
       word=word<<8;
       if (i==3) word=word | buf[3+off];
         else word=word | bit(buf[i+off]);
     }
     return Float.intBitsToFloat(word);
  }

  static public float flt(byte buf[]) {
     int word=0;
     for(int i=3;i>=0;i--) {
       word=word<<8;
       if (i==3) word=word | buf[3];
       else word=word | bit(buf[i]);
     }
     return Float.intBitsToFloat(word);
  }

  static public double dbl(byte buf[],int off) {
    
     long word=0;
    
     for(int i=7;i>=0;i--) {
       word=word<<8;
       if (i==7) word=word | buf[7+off];
         else word=word | bit(buf[i+off]);
     }
     return Double.longBitsToDouble(word);
  }

  static public double dbl(byte buf[]) {
    
     long word=0;
    
     for(int i=7;i>=0;i--) {
       word=word<<8;
       if (i==7) word=word | buf[7];
         else word=word | bit(buf[i]);
     }
     return Double.longBitsToDouble(word);
  }
}














