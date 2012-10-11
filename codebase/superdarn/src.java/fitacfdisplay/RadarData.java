/* RadarData.java
   ============== 
   Author: R.J.Barnes
*/

/*
 $License$
*/




public class RadarData {
    public static final int MAX_RANGE=300;
    public static final int PULSE_SIZE=64;
    public static final int LAG_SIZE=96;
    public static final int COMBF_SIZE=80;
    public static final int MAX_BEAM=255;


    DataMap dmap;
    int channel;
    RadarParm prm;
    FitData fit;
    

    public RadarData(DataMap dmap,int channel) {
        this.channel=channel;
        this.prm=new RadarParm();
        this.fit=new FitData();
	this.dmap=dmap;
    }
 
    public synchronized void decode(byte buf[],int off)  {
        boolean s;
        int c;
       

	s=dmap.decode(buf,off);
        if (s==false) return;
        
        c=prm.getChannel(dmap);
        System.err.println("Channel:"+c+" "+channel);
        if (channel !=0) {
	  if ((channel==1) && (c==2)) return;
          if ((channel==2) && (c==1)) return;
        }
        s=prm.decode(dmap);
        if (s==false) return;
        s=fit.decode(dmap);
        if (s==false) return;
        notifyAll();
   }
    
   public synchronized void poll() {
     try {
        wait(); /* wait until the data arrives */
      } catch (InterruptedException e) {
    }
  }
}
