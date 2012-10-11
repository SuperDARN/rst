/* ScopeData.java
   ============== 
   Author: R.J.Barnes
*/

/*
 $License$
*/





public class ScopeData {
    double time;
    int chn;
    int nsamp;

    float pwr[];
    short buffer[];
   
    public synchronized void decode(byte buf[])  {
      int n,c,off=0;

      time=MathConvert.dbl(buf,off);off+=8;
      chn=MathConvert.lng(buf,off); off+=4;
      nsamp=MathConvert.lng(buf,off); off+=4;
      System.err.println(time);
      System.err.println(chn);
      System.err.println(nsamp);
      pwr=new float[nsamp];
      for (c=0;c<nsamp;c++) {
        pwr[c]=MathConvert.flt(buf,off);
        off+=4;      
      }
      buffer=new short[nsamp*chn*2];
      n=nsamp*chn*2;
      for (c=0;c<n;c++) {;
	  buffer[c]=MathConvert.shrt(buf,off);  
          off+=2;
      } 
      notifyAll();
    }
    


   public synchronized void poll() {
    try {
      wait(); /* wait until the fitdata arrives */
    } catch (InterruptedException e) {
    }
  }
}
