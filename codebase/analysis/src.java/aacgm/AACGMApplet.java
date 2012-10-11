/* AACGMApplet.java
   ================  
   By R.J.Barnes   */


/*
 $License$
*/




import java.applet.*;
import java.awt.*;
import java.awt.event.*;
import java.net.*;
import java.io.*;


public class AACGMApplet extends Applet  {

    AACGM aacgm;
    MLT mlt;
    TimeValue tval;

    TextField height;
    TextField glat,glon;
    TextField mlat,mlon;
   
    TextField yr,mo,dy,hr,mt,sc;
    TextField lon;
    TextField mltval;
 
    boolean geo=true;

    private class GeoHandler implements ActionListener {
      public void actionPerformed(ActionEvent evt) {
	double glatv=0,glonv=0,heightv=0;
        try {
           glatv=(new Double(glat.getText().trim())).doubleValue();
           glonv=(new Double(glon.getText().trim())).doubleValue();
           heightv=(new Double(height.getText().trim())).doubleValue();

         } catch (NumberFormatException ex) {
           System.err.println("Invalid entry in field");
         }
	 if (glatv<-90) glatv=-90;
         if (glatv>90) glatv=90;
         if (glonv<0) glonv=0;
         if (glonv>360) glonv=360;
         glon.setText(Double.toString(glonv));
         glat.setText(Double.toString(glatv));
         

         try {
	     aacgm.setGeo(glatv,glonv,heightv);
             geo=true;
             mlat.setText(Double.toString(aacgm.getMagLat()));
             mlon.setText(Double.toString(aacgm.getMagLon()));
        } catch (AACGMException x) {
        }
      }
    }

   private class MagHandler implements ActionListener {
      public void actionPerformed(ActionEvent evt) {
	double mlatv=0,mlonv=0,heightv=0;
        try {
           mlatv=(new Double(mlat.getText().trim())).doubleValue();
           mlonv=(new Double(mlon.getText().trim())).doubleValue();
           heightv=(new Double(height.getText().trim())).doubleValue();

         } catch (NumberFormatException ex) {
           System.err.println("Invalid entry in field");
         }
	 if (mlatv<-90) mlatv=-90;
         if (mlatv>90) mlatv=90;
         if (mlonv<0) mlonv=0;
         if (mlonv>360) mlonv=360;
         mlon.setText(Double.toString(mlonv));
         mlat.setText(Double.toString(mlatv));
        

         try {
	     aacgm.setMag(mlatv,mlonv,heightv);
             geo=false;
             glat.setText(Double.toString(aacgm.getGeoLat()));
             glon.setText(Double.toString(aacgm.getGeoLon()));
        } catch (AACGMException x) {
        }
      }
    }

    private class HgtHandler implements ActionListener {
      public void actionPerformed(ActionEvent evt) {
	double latv=0,lonv=0,heightv=0;
        try {
          heightv=(new Double(height.getText().trim())).doubleValue();
        } catch (NumberFormatException ex) {
          System.err.println("Invalid entry in field");
        }

        if (heightv<0) heightv=0;
        if (heightv>7200) heightv=7200;
        height.setText(Double.toString(heightv));
        
        try {
	    if (geo==true) {
		latv=aacgm.getGeoLat();
     	        lonv=aacgm.getGeoLon();
	        aacgm.setGeo(latv,lonv,heightv);
                mlat.setText(Double.toString(aacgm.getMagLat()));
                mlon.setText(Double.toString(aacgm.getMagLon()));
	    } else {
                latv=aacgm.getMagLat();
     	        lonv=aacgm.getMagLon();
	        aacgm.setMag(latv,lonv,heightv);
                glat.setText(Double.toString(aacgm.getGeoLat()));
                glon.setText(Double.toString(aacgm.getGeoLon()));
	    }
          } catch (AACGMException x) {
        }
      }
    }
   
    private class MLTHandler implements ActionListener {
      public void actionPerformed(ActionEvent evt) {
	int yrv=2000,mov=8,dyv=30,hrv=12,mtv=30,scv=0;
        double lonv=0;  
        try {
          yrv=(new Integer(yr.getText().trim())).intValue();
          mov=(new Integer(mo.getText().trim())).intValue();
          dyv=(new Integer(dy.getText().trim())).intValue();
          hrv=(new Integer(hr.getText().trim())).intValue();
          mtv=(new Integer(mt.getText().trim())).intValue();
          scv=(new Integer(sc.getText().trim())).intValue();
          lonv=(new Double(lon.getText().trim())).doubleValue();
         } catch (NumberFormatException ex) {
           System.err.println("Invalid entry in field");
         }

	if (yrv<1900) yrv=1900;
        if (yrv>2100) yrv=2100;
	if (mov<1) mov=1;
        if (mov>12) mov=12;
	if (dyv<1) dyv=1;
        if (dyv>31) dyv=31;
        if (hrv<0) hrv=0;
        if (hrv>23) hrv=23;
        if (mtv<0) hrv=0;
        if (mtv>59) hrv=59;
        if (scv<0) scv=0;
        if (scv>59) scv=59;
        if (lonv<0) lonv=0;
        if (lonv>360) lonv=360;

        yr.setText(Integer.toString(yrv));
        mo.setText(Integer.toString(mov));
        dy.setText(Integer.toString(dyv));
        hr.setText(Integer.toString(hrv));
        mt.setText(Integer.toString(mtv));
        sc.setText(Integer.toString(scv));
        lon.setText(Double.toString(lonv));


       tval.setTime(yrv,mov,dyv,hrv,mtv,scv);
       try {
          mlt.eval(tval,lonv);
          mltval.setText(Double.toString(mlt.getMLT()));
       } catch (AACGMException x) {
       }
      }
    }


  public void init() {

      MLTHandler mlthand=new MLTHandler();
      GeoHandler geohand=new GeoHandler();
      MagHandler maghand=new MagHandler();
      HgtHandler hgthand=new HgtHandler();

      aacgm=new AACGM();
      mlt=new MLT();

      glat=new TextField("80.0",20);
      glon=new TextField("0.0",20);
      mlat=new TextField("80.0",20);
      mlon=new TextField("0.0",20);

      yr=new TextField("2000",5);
      mo=new TextField("08",3);
      dy=new TextField("30",3);
   
      hr=new TextField("12",3);
      mt=new TextField("30",3);
      sc=new TextField("00",3);

      lon=new TextField("0.0",6);
      mltval=new TextField("0.0",20);

      height=new TextField("100.0",12);
  
     
      yr.addActionListener(mlthand);
      mo.addActionListener(mlthand);
      dy.addActionListener(mlthand);
      hr.addActionListener(mlthand);
      mt.addActionListener(mlthand);
      sc.addActionListener(mlthand);
      lon.addActionListener(mlthand);

      mlat.addActionListener(maghand);
      mlon.addActionListener(maghand);
      glat.addActionListener(geohand);
      glon.addActionListener(geohand);

      height.addActionListener(hgthand);

      Panel a=new Panel();
      a.setLayout(new FlowLayout(FlowLayout.LEFT,0,0));
      a.add(new Label("Altitude:"));
      a.add(height);
      a.add(new Label("km"));
  
      Panel b=new Panel();
      b.setLayout(new FlowLayout(FlowLayout.LEFT,0,0));
      b.add(new Label("Geographic:"));
      b.add(new Label("lat."));
      b.add(glat);
      b.add(new Label("lon."));
      b.add(glon);
    
      Panel c=new Panel();
      c.setLayout(new FlowLayout(FlowLayout.LEFT,0,0));
      c.add(new Label("Geomagnetic:"));
      c.add(new Label("lat."));
      c.add(mlat);
      c.add(new Label("lon."));
      c.add(mlon);
    
      Panel d=new Panel(); 
      d.setLayout(new FlowLayout(FlowLayout.LEFT,0,0));
      d.add(new Label("date."));
      d.add(yr);
      d.add(new Label("-"));
      d.add(mo);
      d.add(new Label("-"));
      d.add(dy);
      d.add(new Label("time."));
      d.add(hr);
      d.add(new Label(":"));
      d.add(mt);
      d.add(new Label(":"));
      d.add(sc);
      d.add(new Label("lon."));
      d.add(lon);

      Panel e=new Panel();
      e.setLayout(new FlowLayout(FlowLayout.LEFT,0,0));
      e.add(new Label("MLT:"));
      e.add(mltval);
  
      mltval.setEditable(false);


      this.setLayout(new GridLayout(5,1,1,1));
      this.add(a);
      this.add(b);
      this.add(c);
      this.add(d);   
      this.add(e);

      tval=new TimeValue(2000,8,30,12,30,0);
      try {
        mlt.eval(tval,0.0);
        aacgm.setGeo(80.0,0.0,100.0);
        mlat.setText(String.valueOf(aacgm.getMagLat()));
        mlon.setText(String.valueOf(aacgm.getMagLon()));
        mltval.setText(String.valueOf(mlt.getMLT()));
      } catch (AACGMException x) {
      }
      

  }
      
       

      



  }


