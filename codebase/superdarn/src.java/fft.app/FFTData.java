/* FFTData.java
   ============== 
   Author: R.J.Barnes
*/

/*
 $License$
*/





public class FFTData {
    float buffer[];
    int nsamp=0;
 
    public synchronized void decode(byte buf[])  {
      int n,c,off=0;
      n=buf.length/4;
       
      buffer=new float[n];
      for (c=0;c<n;c++) {
          if (c<n/2) buffer[n/2+c]=MathConvert.flt(buf,off);
          else buffer[c-n/2]=MathConvert.flt(buf,off);  
          off+=4;
      } 
      nsamp=n;
      notifyAll();
    }
    


   public synchronized void poll() {
    try {
      wait(); /* wait until the fitdata arrives */
    } catch (InterruptedException e) {
    }
  }
}
