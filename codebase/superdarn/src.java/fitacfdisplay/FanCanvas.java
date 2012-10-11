/*FanCanvas.java
  ==============
  Author R.J.Barnes
*/

/*
 $License$
*/





import java.applet.*;
import java.awt.*;
import java.awt.event.*;
import java.util.*;

public class FanCanvas extends BaseCanvas {

  private FanGrid grid=new FanGrid();
  public byte map[][][]=new byte [RadarData.MAX_BEAM][RadarData.MAX_RANGE][4];
  private boolean gflg=true;
  private int bxcar=0;
  private int prm=0;
  private int mbeam=16;

  private double prng=30.0;
  private double vrng=1000.0;
  private double wrng=500.0;

  private int gskip=0;
  Color gridcolor=Color.black;
  Color backcolor=Color.white;

  private double dval[][][]=
          new double [RadarData.MAX_BEAM][RadarData.MAX_RANGE][3];

  private int nrang[]=new int [RadarData.MAX_BEAM];
  private int frang[]=new int [RadarData.MAX_BEAM];
  private int rsep[]=new int [RadarData.MAX_BEAM];
  private int rxrise[]=new int [RadarData.MAX_BEAM];
  private int tfreq[]=new int [RadarData.MAX_BEAM];
  private float noise[]=new float [RadarData.MAX_BEAM];
  private Time time[]=new Time [RadarData.MAX_BEAM];

 

  private HotBox hotbox;
  private RadarPos rpos;

  private class MousePress extends MouseAdapter {
    public void mousePressed(MouseEvent evt) {
      locatePoint(evt.getX(),evt.getY());
    }
  }

  public FanCanvas(HotBox hotbox,int mbeam) {
    this.hotbox=hotbox;
    this.mbeam=mbeam;
    this.addMouseListener(new MousePress());
  }

  private void locatePoint(int x,int y) {
    String tstr=null;
    RangeBeam rb=grid.locate(s,x,y);
    if (rb==null) return;
    int rkm=frang[rb.bm]+rsep[rb.bm]*rb.rng;
    System.err.println("bm="+rb.bm+", rng="+rb.rng+" ("+rkm+" km.)");

    MapPoint geo=GeoMap.geo(0,rpos,frang[rb.bm],rsep[rb.bm],rxrise[rb.bm],
			    300.0,
                            mbeam,rb.bm,rb.rng);
    if (geo !=null) System.err.println("lat="+geo.lat+", lon="+geo.lon);

    if (map[rb.bm][rb.rng][0]==0) {
       System.err.println("No Data");
    } else {
      if ((prm==0) && (map[rb.bm][rb.rng][0]==2))
        System.err.println("Ground Scatter");
      else System.err.println("Value="+dval[rb.bm][rb.rng][prm]);
    }

    if (time[rb.bm] !=null) {
      tstr=TimeUTC.getString(time[rb.bm].year,time[rb.bm].month,time[rb.bm].day,
                 time[rb.bm].hour,time[rb.bm].minute,(int) time[rb.bm].second);
    } else tstr=new String("Unknown");

    if (geo !=null) 
       hotbox.set(rb.bm,rb.rng,map[rb.bm][rb.rng][0],dval[rb.bm][rb.rng][prm],
               rkm,geo.lat,geo.lon,tfreq[rb.bm],noise[rb.bm],
               tstr);
    else 
      hotbox.set(rb.bm,rb.rng,map[rb.bm][rb.rng][0],dval[rb.bm][rb.rng][prm],
                 rkm,0,0,tfreq[rb.bm],noise[rb.bm],
                 tstr);       
  }
    


  public void remapBeam(int b) {
    for (int r=0;r<nrang[b];r++) {
      byte plot=0;
      if (map[b][r][0] !=0) {
        if (dval[b][r][0]>vrng) plot=127;
        else if (dval[b][r][0]<-vrng) plot=-128;
        else plot=(byte)(127*dval[b][r][0]/vrng);
        map[b][r][1]=plot;
        if (dval[b][r][1]>prng) plot=127;
        else if (dval[b][r][1]<0) plot=-128; 
        else plot=(byte) ((255*dval[b][r][1]/prng)-128);   
        map[b][r][2]=plot;
        if (dval[b][r][2]>wrng) plot=127;
        else if (dval[b][r][2]<0) plot=-128;
        else plot= (byte) ((255*dval[b][r][2]/wrng)-128);
        map[b][r][3]=plot;
      }
    }
  }


  public void plotData(RadarData data) {

    Graphics bg=null;

    if (rpos==null) rpos=new RadarPos(data.prm.stid);
    int b=data.prm.bmnum;

    if (img !=null) bg=img.getGraphics();

    if (nrang[b] !=data.prm.nrang) {
      if ((bg !=null) && (grid.set[b])) eraseBeam(bg,b);
      grid.generate(b,mbeam,data.prm.nrang);
    }
   

    tfreq[b]=data.prm.tfreq;
    noise[b]=data.prm.noise.search;
    nrang[b]=data.prm.nrang;
    frang[b]=data.prm.frang;
    rsep[b]=data.prm.rsep;
    rxrise[b]=data.prm.rxrise;


    if (time[b]==null) time[b]=new Time();
    time[b].year=data.prm.time.yr;
    time[b].month=data.prm.time.mo;
    time[b].day=data.prm.time.dy;
    time[b].hour=data.prm.time.hr;
    time[b].minute=data.prm.time.mt;
    time[b].second=data.prm.time.sc;
    
    for (int r=0;r<data.prm.nrang;r++) {
      if ((data.fit.rng[r].qflg !=0) && (data.fit.rng[r].p_l >1.0)) {
        dval[b][r][0]=data.fit.rng[r].v;
        dval[b][r][1]=data.fit.rng[r].p_l;
        dval[b][r][2]=data.fit.rng[r].w_l;
        if (data.fit.rng[r].gsct==0) map[b][r][0]=1;
        else map[b][r][0]=2;
      } else map[b][r][0]=0;
    }
    remapBeam(b);

    if (bg !=null) {
      eraseBeam(bg,b);
      drawBeam(bg,b);
      bg.dispose();
      repaint();
    }

  }

  public void render() {
    Polygon p;
    int b;
    if (img==null) return;
    Graphics bg=img.getGraphics();
    if (bg==null) return;
  
    for (b=0;b<mbeam;b++) {
        eraseBeam(bg,b);
        drawBeam(bg,b);
        bg.setColor(gridcolor);
        grid.drawGrid(bg,s,b,gskip);
    }
    
    bg.dispose();

  }


 public void eraseBeam(Graphics bg,int b) {
    Polygon p;
    p=grid.clipBeam(bg,s,b);
    if (p==null) return;
    bg.setColor(backcolor);
    bg.fillPolygon(p);
  }

  public void drawBeam(Graphics bg,int b) {
    Polygon p;  
    for (int r=0;r<nrang[b];r++) {
      if (map[b][r][0] !=0) { 
        if (prm==0) {
          if ((map[b][r][0]==2) && (gflg==true)) 
            grid.drawCell(bg,s,b,r,Color.gray);
          else grid.drawCell(bg,s,b,r,ColorKey.color(0,map[b][r][1],bxcar));
	} else grid.drawCell(bg,s,b,r,
                             ColorKey.color(1,map[b][r][prm+1],bxcar));
      }
    }       
    bg.setColor(gridcolor);
    grid.drawGrid(bg,s,b,gskip);
  }

  public double getRange() {
    if (prm==0) return vrng;
    if (prm==1) return prng;
    return wrng;
  }

  public void setGflg(boolean gflg) {
    if (this.gflg==gflg) return;
    this.gflg=gflg;
    this.render();
    this.repaint();
  }

  public void setPrm(int prm) {
    if (prm==this.prm) return;
    this.prm=prm;
    this.render();
    this.repaint();
  } 

  public void setBxCar(int bxcar) {
    if (bxcar==this.bxcar) return;
    if (bxcar<0) bxcar=0;
    if (bxcar>50) bxcar=50;
    this.bxcar=bxcar;
    this.render();
    this.repaint();
  }

  public void setGrid(int gskip) {
    if (gskip==this.gskip) return;
    this.gskip=gskip;
    this.render();
    this.repaint();
  }

  public void setGridColor(Color gridcolor) {
    if (gridcolor==this.gridcolor) return;
    this.gridcolor=gridcolor;
    this.render();
    this.repaint();
  }

  public void setBackColor(Color backcolor) {
    if (backcolor==this.backcolor) return;
    this.backcolor=backcolor;
    this.render();
    this.repaint();
  }

  public void setRange(double rng) {
    int i;
    if (prm==0) {
      if (rng<10.0) rng=10.0;
      if (rng>5000.0) rng=5000.0;
      vrng=rng;
      for (i=0;i<mbeam;i++) this.remapBeam(i);
      this.render();
      this.repaint();
    } else if (prm==1) {
      if (rng<1.0) rng=1.0;
      if (rng>100.0) rng=100.0;
      prng=rng;
      for (i=0;i<mbeam;i++) this.remapBeam(i);
      this.render();
      this.repaint();
    } else {
      if (rng<1.0) rng=1.0;
      if (rng>5000.0) rng=5000.0;
      for (i=0;i<mbeam;i++) this.remapBeam(i);
      wrng=rng;
      this.render();
      this.repaint();
    } 
  }



}
