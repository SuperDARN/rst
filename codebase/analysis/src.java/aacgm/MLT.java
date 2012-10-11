/* MLT.java
   ============= 
   By R.J.Barnes */


/*
 $License$
*/





import java.util.*;
import java.lang.*;
import java.io.*;

public class MLT {

  AACGM aacgm;
  SolarLoc solarLoc;
  EqnOfTime eqnOfTime;
 
  public double mslong;
  public double mlt;  

  static double sol_dec_old=0;
  static double told=1e12;
  static double mslon1=0;
  static double mslon2=0;

  public MLT(AACGM aacgm) {
      this.aacgm=new AACGM(aacgm);
      this.solarLoc=new SolarLoc();
      this.eqnOfTime=new EqnOfTime();   
  }

  public MLT() {
      this.aacgm=new AACGM();
      this.solarLoc=new SolarLoc();
      this.eqnOfTime=new EqnOfTime();   
  }

  public MLT(AACGM aacgm,DataInputStream in) throws IOException {
      this.aacgm=new AACGM(aacgm);
      this.solarLoc=new SolarLoc(in);
      this.eqnOfTime=new EqnOfTime(in);   
  }

  public MLT(DataInputStream in) throws IOException {
      this.aacgm=new AACGM(in);
      this.solarLoc=new SolarLoc(in);
      this.eqnOfTime=new EqnOfTime(in);   
  }


  void mlt1(int t0,double solar_dec,double mlon) throws AACGMException {
 
    double ret_val;
    double mslat1,mslat2,slon1,slon2,height;
     int err;
 
    if ((Math.abs(solar_dec-sol_dec_old)>0.1) || (sol_dec_old==0)) told=1e12;
    if (Math.abs(mslon2-mslon1)>10) told=1e12;
    
     if ((t0>=told) && (t0<(told+600))) {
       mslong=mslon1+(t0-told)*(mslon2-mslon1)/600.0;
     } else {
      told=t0;
      sol_dec_old=solar_dec;

      slon1 = (43200.0-t0)*15.0/3600.0;
      slon2 = (43200.0-t0-600)*15.0/3600.0;

      height = 450;
      
      aacgm.convert_geo_coord(solar_dec,slon1,height,0,4);
      mslat1=aacgm.getMagLat();
      mslon1=aacgm.getMagLon();
      aacgm.convert_geo_coord(solar_dec,slon2,height,0,4);
      mslat2=aacgm.getMagLat();
      mslon2=aacgm.getMagLon();
    
      mslong=mslon1;
    }

  
    mlt = (mlon - mslong) /15.0 + 12.0;
    if (mlt >=24) mlt -=24;
    if (mlt <0) mlt+=24;  
  } 

  public void eval(TimeValue time,double mlong) throws AACGMException {

    
    double apparent_time;
    double et;
    double ut;
 
    int yr,t0;
     
    yr=time.getYear();
    t0=time.getYrSec();
 
    if (yr > 1900) yr-=1900;
    solarLoc.eval(yr, t0);

    et = eqnOfTime.eval(solarLoc.getLon(), yr);

    ut=(float) (t0 % (24*3600));
    apparent_time = ut + et;
    mlt1( (int) apparent_time, solarLoc.getDec(), mlong);
   }


    public double getMLT() {
	return this.mlt;
    }
 
    public double getMSlong() {
	return this.mslong;
    }


   

}
