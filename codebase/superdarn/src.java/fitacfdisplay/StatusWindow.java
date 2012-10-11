/* StatusWindow.java
   =================
   Author: R.J.Barnes
*/

/*
 $License$
*/




import java.lang.*;
import java.awt.*;
import java.util.*;

public class StatusWindow extends CloseFrame implements Runnable {
 
  boolean stop=false;
  private Thread runner;
  RadarData data;
  private int mbeam=16;
  private TextField time;
  private TextField cp;
  private TextField combf;
  private TextField station;
  private TextField beam; 
  private TextField channel;
  private TextField intt;
  private TextField noise;
  private TextField freq;
  private TextField frang;
  private TextField rsep;
  private TextField nrang;
  private TextField ercod;
  private TextField seqno;
  private BitArray atten;
  private BitArray lopwr;
  private BitArray agcstat;

  private GridBagLayout gridbag=new GridBagLayout();
  private GridBagConstraints constrain=new GridBagConstraints();


  public void layout(Component component,int gx,int gy,int wx,int wy) {
    constrain.gridx=gx;
    constrain.gridy=gy;
    constrain.gridwidth=wx;
    constrain.gridheight=wy;
    gridbag.setConstraints(component,constrain);
    this.add(component);
  } 

  public StatusWindow(String name,RadarData data,int mbeam) {

    this.data=data;
    this.mbeam=mbeam;
    time=new TextField(20);
    cp=new TextField(10);
    combf=new TextField(15);
    station=new TextField(15);
    beam=new TextField(10);
    channel=new TextField(10);
    intt=new TextField(10);
    noise=new TextField(10);
    freq=new TextField(10);
    frang=new TextField(10);
    rsep=new TextField(10);
    nrang=new TextField(10);
    ercod=new TextField(10);
    seqno=new TextField(10);
 
    atten=new BitArray(8,Color.lightGray,Color.red);
    lopwr=new BitArray(mbeam,Color.black,Color.yellow);
    agcstat=new BitArray(mbeam,Color.black,Color.yellow);


    time.setEditable(false);
    cp.setEditable(false);
    combf.setEditable(false);
    station.setEditable(false);
    beam.setEditable(false);
    channel.setEditable(false);
    intt.setEditable(false);
    noise.setEditable(false);
    freq.setEditable(false);
    frang.setEditable(false);
    nrang.setEditable(false);
    rsep.setEditable(false);
    ercod.setEditable(false);
    seqno.setEditable(false);

    this.setLayout(gridbag);
    constrain.gridx=0;
    constrain.gridy=0;
    constrain.gridwidth=0;
    constrain.gridwidth=0;
    constrain.fill=GridBagConstraints.BOTH; 
    constrain.ipadx=0;
    constrain.ipady=0;
    constrain.insets=new Insets(2,2,2,2);
    constrain.anchor=GridBagConstraints.CENTER;
    constrain.weightx=0;
    constrain.weighty=0;

    this.layout(new Label("Station:"),0,1,1,1);


   this.layout(new Label("Program ID:"),0,2,1,1);
    this.layout(new Label("Comment:"),0,3,1,1);

    this.layout(new Label("Beam:"),0,4,1,1);
    this.layout(new Label("Channel:"),0,5,1,1);
    this.layout(new Label("Integration Time:"),0,6,1,1);
   
    this.layout(new Label("Noise:"),0,7,1,1);
    this.layout(new Label("Frequency:"),0,8,1,1);
    this.layout(new Label("First Range: (km)"),0,9,1,1);
    this.layout(new Label("Range Separation: (km)"),0,10,1,1);
    this.layout(new Label("Number of Ranges"),0,11,1,1);

    this.layout(new Label("Error Code:"),0,12,1,1);
    this.layout(new Label("Sequences:"),0,13,1,1);
    this.layout(new Label("Attenuation:"),0,14,1,1);
    this.layout(new Label("LOWPWR Status:"),0,15,2,1);
    this.layout(new Label("AGC Status:"),0,17,2,1);
   
   
    this.layout(time,0,0,2,1);
    this.layout(station,1,1,1,1);

    this.layout(cp,1,2,1,1);
    this.layout(combf,1,3,1,1);

    this.layout(beam,1,4,1,1);
    this.layout(channel,1,5,1,1);
    this.layout(intt,1,6,1,1);

    this.layout(noise,1,7,1,1);
    this.layout(freq,1,8,1,1);
    this.layout(frang,1,9,1,1);
    this.layout(rsep,1,10,1,1);
    this.layout(nrang,1,11,1,1);
   
    this.layout(ercod,1,12,1,1);
    this.layout(seqno,1,13,1,1);
    this.layout(atten,1,14,1,1);   
    this.layout(lopwr,0,16,2,1);
    this.layout(agcstat,0,18,2,1);

    this.pack();
    this.setTitle(name);
    runner=new Thread(this);
    runner.start();   
  }

  public void plotData() {
    int status,i;

    station.setText(StationID.toString(data.prm.stid));
    cp.setText(Integer.toString(data.prm.cp));
    if (data.prm.combf !=null) combf.setText(data.prm.combf);
    else combf.setText("Not Defined");

    beam.setText(Integer.toString(data.prm.bmnum));
    channel.setText(Integer.toString(data.prm.channel));
    freq.setText(Integer.toString(data.prm.tfreq));
    noise.setText(Integer.toString((int) data.prm.noise.search));
    seqno.setText(Integer.toString(data.prm.nave));
    ercod.setText(Integer.toString(data.prm.ercod));
    frang.setText(Integer.toString(data.prm.frang));
    rsep.setText(Integer.toString(data.prm.rsep));
    nrang.setText(Integer.toString(data.prm.nrang));
    intt.setText(Integer.toString(data.prm.intt.sc));

    
    time.setText(TimeUTC.getString(data.prm.time.yr,data.prm.time.mo,
                 data.prm.time.dy,data.prm.time.hr,
                 data.prm.time.mt,data.prm.time.sc));
   
  
    status=data.prm.stat.lopwr;  
    for (i=0;i<mbeam;i++) {
     if ((status &1) !=0) lopwr.setBit(i,true);
       else lopwr.setBit(i,false);
     status=status>>1;
    }
    status=data.prm.stat.agc;  
    for (i=0;i<mbeam;i++) {
     if ((status & 1) !=0) agcstat.setBit(i,true);
       else agcstat.setBit(i,false);
     status=status>>1;
    }
 
    for (i=0;i<8;i++) atten.setBit(i,false);
    for (i=0;i<data.prm.atten;i++) atten.setBit(i,true);

    lopwr.repaint();
    agcstat.repaint();
    atten.repaint();
  }

  public void run() {
     for(;;) {
      data.poll();
      this.plotData();
      if (stop==true) break;
     }
  }

  public void kill() {
    stop=true;
    this.setVisible(false);
    this.dispose();
  }
 
}
   




