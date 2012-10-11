/* TimeCanvas.java
   ===============
   Author R.J.Barnes
*/

/*
 $License$
*/





import java.applet.*;
import java.awt.*;
import java.awt.event.*;
import java.util.*;

public class TimeCanvas extends BaseCanvas implements AdjustmentListener,
                                                      ActionListener {
  Image imgPwr=null;
  Image imgVel=null;
  Image imgWdt=null;
  Image imgNse=null;
  Image imgFrq=null;

  public Scrollbar hbar;
  public Panel zoom;

  private Button plus;
  private Button minus;

  public TimeData time;

  private boolean gflg=true;
  private int bxcar=0;
  private int prm=0;
  private int mbeam=0;

  private double prng=30.0;
  private double vrng=1000.0;
  private double wrng=500.0;

  private int offset=0;
  private int flength=24*60*60; /* frame length in seconds */
  private int fstart=0; /* frame start in seconds */

  int rstep=0;
  int tstep=4*60*60;
  int ttstep=60*60;
  int width=0;

  private Font font;
  FontMetrics fm;

  private int fwdt;
  private int fhgt;
  private int fasc;

  Color gridcolor=Color.black;
  Color backcolor=Color.white;

  private double ln=Math.log(10);

  private int FMAX=19000;
  private int FMIN=9000;

  private int NRANG=75;
  private int FRANG=180;
  private int ERANG=180+NRANG*45;
  private int FRAMEHGT=225;

  HotBox hotbox;

  private class MousePress extends MouseAdapter {
    public void mousePressed(MouseEvent evt) {
      locatePoint(evt.getX(),evt.getY());
    }
  }


  private void getMetrics() {
    this.fm=this.getFontMetrics(this.font);
    fhgt=fm.getHeight();
    fasc=fm.getAscent();
    fwdt=fm.stringWidth("00:00");
  }

  public void addNotify() {
    super.addNotify();
    getMetrics();
    setBackground(backcolor);
    imgPwr=createImage(time.MAXFRAME,FRAMEHGT);
    imgVel=createImage(time.MAXFRAME,FRAMEHGT);
    imgWdt=createImage(time.MAXFRAME,FRAMEHGT);
    imgFrq=createImage(time.MAXFRAME,1);
    imgNse=createImage(time.MAXFRAME,1);
  }

  public TimeCanvas(int bmnum,int mbeam,int nrang,HotBox hotbox) { 
    this.mbeam=mbeam;
    this.NRANG=nrang;
    this.ERANG=180+this.NRANG*45;
    this.hotbox=hotbox;
    this.addMouseListener(new MousePress());
    hbar=new Scrollbar(Scrollbar.HORIZONTAL);
    font=new Font("Helvetica",Font.PLAIN,12);
    hbar.addAdjustmentListener(this);
    hbar.setValues(0,s.width,0,s.width);
    zoom=new Panel();
    plus=new Button("+");
    minus=new Button("-");
    zoom.setLayout(new FlowLayout(FlowLayout.LEFT));
    zoom.add(minus);
    zoom.add(plus);       
    plus.addActionListener(this);
    minus.addActionListener(this);
    time=new TimeData(bmnum);
  }

private void drawGrid(Graphics g) {
    int x,y;
    int wd;
    if (g==null) return;
 
    int bx=fwdt+2-offset;
    int by=fhgt*2+fasc+2;

    wd=width-2*fwdt-5-(fwdt/2);
    int ht=s.height-fhgt*3-fasc-5;

    g.setColor(gridcolor);
    g.drawRect(bx,by,wd,ht);
    g.drawRect(bx,2,wd,10);
    g.drawRect(bx,2+fhgt,wd,10);

    if ((rstep !=0) && ((ht/rstep) > 3)) {
      for (int rng=0;rng<rstep;rng++) {
        y=by+(ht*rng/rstep);
        g.drawLine(bx+wd-2,y,bx+wd,y);
        g.drawLine(bx,y,bx+4,y);
      }
    }    

    if ((tstep !=0) && ((wd*tstep/flength) > 10)) {
      for (int tme=0;tme<flength;tme+=tstep) {
        x=(wd*tme)/flength;
        g.drawLine(bx+x,by,bx+x,by+ht);
      }
    }

   if ((ttstep !=0) && ((wd*ttstep/flength) > 3)) {
      for (int tme=0;tme<flength;tme+=ttstep) {
        x=(wd*tme)/flength;
        g.drawLine(bx+x,by,bx+x,by+4);
        g.drawLine(bx+x,by+ht,bx+x,by+ht-2);
      }
    }
    
   /* now label the scales */
 
   g.setFont(this.font);
   g.drawString("Freq.",2-offset,2+fasc);
   g.drawString("Nois.",2-offset,2+fhgt+fasc);

   if ((rstep !=0) && ((ht/rstep) > fhgt)) {
      for (int rng=0;rng<=rstep;rng++) {
        y=ht*(rstep-rng)/rstep;
        g.drawString(Integer.toString(rng),2-offset,
        by+y+fasc-(fhgt/2));
       g.drawString(Integer.toString(rng),bx+wd+2,
        by+y+fasc-(fhgt/2));
      }
    } else {
      g.drawString(Integer.toString(FRANG),2-offset,by+ht+fasc-(fhgt/2));
      g.drawString(Integer.toString(ERANG),2-offset,by+fasc-(fhgt/2));
      g.drawString(Integer.toString(FRANG),bx+wd+2,by+ht+fasc-(fhgt/2));
      g.drawString(Integer.toString(ERANG),bx+wd+2,by+fasc-(fhgt/2));
    }  

    if ((tstep !=0) && ((wd*tstep/flength) > fwdt)) {
      int hr,mt;
      for (int tme=0;tme<=flength;tme+=tstep) {
        hr=((fstart+tme)/3600) % 24;
        mt=((fstart+tme)/60) % 60;
        x=(wd*tme)/flength;
        if ((hr < 10) && (mt<10)) 
          g.drawString(new String("0"+hr+":0"+mt),bx+x-(fwdt/2),by+ht+fasc+2);
        else if (hr<10)
          g.drawString(new String("0"+hr+":"+mt),bx+x-(fwdt/2),by+ht+fasc+2);
        else if (mt<10)
          g.drawString(new String(hr+":0"+mt),bx+x-(fwdt/2),by+ht+fasc+2);
        else
          g.drawString(new String(hr+":"+mt),bx+x-(fwdt/2),by+ht+fasc+2);
 
      }
    }
  }

  public void replot() {
    if (img==null) return;
    Graphics bg=img.getGraphics();
    if (bg==null) return;

    bg.setColor(backcolor);
    bg.fillRect(0,0,s.width,s.height);

    int bx=fwdt+2-offset;
    int by=fhgt*2+fasc+2;

    int wd=width-2*fwdt-5-(fwdt/2);
    int ht=s.height-fhgt*3-fasc-5;
    if ((prm==0) && (imgVel !=null)) bg.drawImage(imgVel,bx,by,wd,ht,null);
    if ((prm==1) && (imgPwr !=null)) bg.drawImage(imgPwr,bx,by,wd,ht,null);
    if ((prm==2) && (imgWdt !=null)) bg.drawImage(imgWdt,bx,by,wd,ht,null);
    if (imgFrq !=null) bg.drawImage(imgFrq,bx,2,wd,10,null);
    if (imgNse !=null) bg.drawImage(imgNse,bx,2+fhgt,wd,10,null);

    drawGrid(bg);
    bg.dispose();
  }


 public void render() {
   
    if (s.width>width) {
      width=s.width;
      hbar.setValues(0,width,0,width);
      offset=0;
    } else {
      if ((offset+s.width)>width) offset=width-s.width;
      hbar.setValues(offset,s.width,0,width);
    }
    replot();
  
 }

  public void adjustmentValueChanged(AdjustmentEvent evt) {
    offset=hbar.getValue();
    replot(); 
    repaint();
  }

  public void actionPerformed(ActionEvent evt) {
    String arg=evt.getActionCommand();
    if (arg.equals("+")) {
      width=width*2;
      if (width>10000) width=10000;
      hbar.setValues(offset,s.width,0,width);
      replot();
      repaint();
    } else if (arg.equals("-")) {
      width=width/2;
      if (width<s.width) width=s.width;
      if ((offset+s.width)>width) offset=width-s.width;
      hbar.setValues(offset,s.width,0,width);
      replot();
      repaint();
    }
  }

  public void remapNseFrq(int i) {
    double nlog;
    byte plot;
    Graphics ng=imgNse.getGraphics();
    Graphics fg=imgFrq.getGraphics();

    int tval=(int) this.time.time[i] % (24*3600);

    int xa=(tval-fstart)*time.MAXFRAME/flength;
    int xb=(tval-fstart+120)*time.MAXFRAME/flength;
    if (xa==xb) xb++;
  
    plot=(byte) ((255*(this.time.tfreq[i]-FMIN)/(FMAX-FMIN))-128);
    fg.setColor(ColorKey.color(1,plot,10));
    fg.drawLine(xa,0,xb,0);



    nlog= (Math.log( (double) (this.time.noise[i]/200.0))/ln)/3.0;
    if (nlog<0.0) nlog=0;
    if (nlog>1.0)  nlog=1.0;
    plot=(byte) (255*nlog-128);
 

    ng.setColor(ColorKey.color(1,plot,10));
    ng.drawLine(xa,0,xb,0);


    fg.dispose();
    ng.dispose();
  }



  public void remapBeam(int i) {
    Graphics pg=imgPwr.getGraphics();
    Graphics vg=imgVel.getGraphics();
    Graphics wg=imgWdt.getGraphics();
    int ya,yb,y;
   
    int tval=(int) this.time.time[i] % (24*3600);

    int xa=(tval-fstart)*time.MAXFRAME/flength;
    int xb=(tval-fstart+120)*time.MAXFRAME/flength;
    if (xa==xb) xb++;
   
    for (int r=0;r<this.time.nrang[i];r++) {
      byte plot=0;
      if (this.time.dflg[i][r] !=0) {
        ya=this.time.frang[i]+r*this.time.rsep[i];
        yb=this.time.frang[i]+(r+1)*this.time.rsep[i];
        ya=FRAMEHGT-FRAMEHGT*(ya-FRANG)/(ERANG-FRANG);
        yb=FRAMEHGT-FRAMEHGT*(yb-FRANG)/(ERANG-FRANG);
        if (this.time.dval[i][r][0]>vrng) plot=127;
        else if (this.time.dval[i][r][0]<-vrng) plot=-128;
        else plot=(byte)(127*this.time.dval[i][r][0]/vrng);
        if ((this.time.dflg[i][r]==2) && (gflg==true)) vg.setColor(Color.gray);
        else vg.setColor(ColorKey.color(0,plot,bxcar));
        for (y=yb;y<=ya;y++) vg.drawLine(xa,y,xb,y);

        if (this.time.dval[i][r][1]>prng) plot=127;
        else if (this.time.dval[i][r][1]<0) plot=-128; 
        else plot=(byte) ((255*this.time.dval[i][r][1]/prng)-128);   
        pg.setColor(ColorKey.color(1,plot,bxcar));
        for (y=yb;y<=ya;y++) pg.drawLine(xa,y,xb,y);

        if (this.time.dval[i][r][2]>wrng) plot=127;
        else if (this.time.dval[i][r][2]<0) plot=-128;
        else plot= (byte) ((255*this.time.dval[i][r][2]/wrng)-128);
        wg.setColor(ColorKey.color(1,plot,bxcar));
        for (y=yb;y<=ya;y++) wg.drawLine(xa,y,xb,y);
      } else {
	/* draw white space on beam grid */
        ya=this.time.frang[i]+r*this.time.rsep[i];
        yb=this.time.frang[i]+(r+1)*this.time.rsep[i];
        ya=FRAMEHGT-FRAMEHGT*(ya-FRANG)/(ERANG-FRANG);
        yb=FRAMEHGT-FRAMEHGT*(yb-FRANG)/(ERANG-FRANG);
	
        vg.setColor(Color.white);
        for (y=yb;y<=ya;y++) vg.drawLine(xa,y,xb,y);
        pg.setColor(Color.white);
        for (y=yb;y<=ya;y++) pg.drawLine(xa,y,xb,y);
        wg.setColor(Color.white);
        for (y=yb;y<=ya;y++) wg.drawLine(xa,y,xb,y);
	
      }
    }
    wg.dispose();
    vg.dispose();
    pg.dispose();

  }

  public void erase() {
    Graphics pg=imgPwr.getGraphics();
    Graphics vg=imgVel.getGraphics();
    Graphics wg=imgWdt.getGraphics();
    Graphics ng=imgNse.getGraphics();
    Graphics fg=imgFrq.getGraphics();

    pg.setColor(backcolor);
    pg.fillRect(0,0,time.MAXFRAME,FRAMEHGT);
    vg.setColor(backcolor);
    vg.fillRect(0,0,time.MAXFRAME,FRAMEHGT);
    wg.setColor(backcolor); 
    wg.fillRect(0,0,time.MAXFRAME,FRAMEHGT);
    ng.setColor(backcolor); 
    ng.fillRect(0,0,time.MAXFRAME,1);
    fg.setColor(backcolor); 
    fg.fillRect(0,0,time.MAXFRAME,1);
  }


  public void remap() {
    int i;
    i=time.start;
    while (i != time.end) { 
      remapBeam(i);
      remapNseFrq(i);
      i++;
      if (i==time.MAXFRAME) i=0;
    }
  }

  public void plotData(RadarData data) {
    int index;
    index=time.mapData(data);
    remapNseFrq(index);
    remapBeam(index);
    replot(); 
    repaint();
  }


  public void setPrm(int prm) {
    if (prm==this.prm) return;
    this.prm=prm;
    replot();
    repaint();
  } 


 public void setGridColor(Color gridcolor) {
    if (gridcolor==this.gridcolor) return;
    this.gridcolor=gridcolor;
    replot();
    repaint();
  }

 public void setBackColor(Color backcolor) {
    if (backcolor==this.backcolor) return;
    this.backcolor=backcolor;
    erase();
    remap();
    replot();
    repaint();
  }

  public double getRange() {
    if (prm==0) return vrng;
    if (prm==1) return prng;
    return wrng;
  }

  public void setGflg(boolean state) {
    if (gflg==state) return;
    gflg=state;
    remap();
    replot();
    repaint();
  }


 public void setBxCar(int bxcar) {
    if (bxcar==this.bxcar) return;
    if (bxcar<0) bxcar=0;
    if (bxcar>50) bxcar=50;
    this.bxcar=bxcar;
    remap();
    replot();
    repaint();
  }

 public void setRange(double rng) {
    int i;
    if (prm==0) {
      if (rng<10.0) rng=10.0;
      if (rng>5000.0) rng=5000.0;
      vrng=rng;
      for (i=0;i<mbeam;i++) this.remapBeam(i);
      remap();
      replot();
      repaint();    
    } else if (prm==1) {
      if (rng<1.0) rng=1.0;
      if (rng>100.0) rng=100.0;
      prng=rng;
      for (i=0;i<mbeam;i++) this.remapBeam(i);
      remap();
      replot();
      repaint();
    } else {
      if (rng<1.0) rng=1.0;
      if (rng>5000.0) rng=5000.0;
      for (i=0;i<mbeam;i++) this.remapBeam(i);
      wrng=rng;
      remap();
      replot();
      repaint();
    } 
  }

  private void locatePoint(int x,int y) {
    int wd; 
    double tme;
    String tstr=null;
    Time tobj=new Time();
    int tval,rng;
    int bx=fwdt+2-offset;
    int by=fhgt*2+fasc+2;
 
    wd=width-2*fwdt-5-(fwdt/2);
    int ht=s.height-fhgt*3-fasc-5;

    if ((x<bx) || (x>(bx+wd))) return;
    if ((y<by) || (y>(by+ht))) return;

    int xt=time.start;
    int xl,xr;  
    while (xt != time.end) { 
      tval=(int) time.time[xt] % (24*3600);
      xl=wd*(tval-fstart)/flength;
      xr=wd*(tval+120-fstart)/flength;
      if (xr==xl) xr++;
      if (xr>wd) xr=wd;
   
      if ((x>=(bx+xl)) && (x<=(bx+xr))) break;
      xt++;
      if (xt==time.MAXFRAME) xt=0;
    }
 
    if (xt !=time.end) {
       int yb,yt;
       tme=time.time[xt];
       for (rng=0;rng<time.nrang[xt];rng++) {
         yb=this.time.frang[xt]+rng*this.time.rsep[xt];
         yt=this.time.frang[xt]+(rng+1)*this.time.rsep[xt];
         yb=ht*(yb-FRANG)/(ERANG-FRANG);
         yt=ht*(yt-FRANG)/(ERANG-FRANG);
         if (yt>ht) yt=ht;
         if ((y>=(by+ht-yt)) && (y<=(by+ht-yb))) break;
       }
       
      int rkm=time.frang[xt]+time.rsep[xt]*rng;
      System.err.println("bm="+time.bmnum+", rng="+rng+" ("+rkm+" km.)");

      MapPoint geo=GeoMap.geo(0,time.rpos,
			      time.frang[xt],time.rsep[xt],time.rxrise[xt],
                              300,mbeam,time.bmnum,rng);
      if (geo !=null) System.err.println("lat="+geo.lat+", lon="+geo.lon);

      if (time.dflg[xt][rng]==0) {
          System.err.println("No Data");
      } else {
         if ((prm==0) && (time.dflg[xt][rng]==2))
         System.err.println("Ground Scatter");
         else System.err.println("Value="+time.dval[xt][rng][prm]);
      }
      tobj.setEpoch(time.time[xt]);
      tstr=TimeUTC.getString(tobj.year,tobj.month,tobj.day,
                 tobj.hour,tobj.minute,(int) tobj.second);
      if (geo !=null) 
        hotbox.set(time.bmnum,rng,(byte) time.dflg[xt][rng],
                   time.dval[xt][rng][prm],
                   rkm,geo.lat,geo.lon,time.tfreq[xt],time.noise[xt],
                   tstr);
      else 
        hotbox.set(time.bmnum,rng,(byte) time.dflg[xt][rng],
                   time.dval[xt][rng][prm],
                   rkm,0,0,time.tfreq[xt],time.noise[xt],
                   tstr);
    } else {
      tme=fstart+flength*(x-bx)/wd;
      rng=(NRANG+1)-(NRANG+1)*(y-by)/ht;
      tstr=new String("Unknown");
      hotbox.set(time.bmnum,rng,(byte) 0,0,
                 0,0,0,0,0,tstr); 
    }
  }
 
}




