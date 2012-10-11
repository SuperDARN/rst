/* FanWindow.java
   ==============
   Author R.J.Barnes
*/

/*
 $License$
*/





import java.lang.*;
import java.awt.*;
import java.awt.event.*;
import java.util.*;

public class FanWindow extends CloseFrame implements Runnable,ActionListener {
  boolean stop=false;
  private Thread runner;
  RadarData data;

  private FanCanvas fan;
  private ColorBar colorbar;
  private FanTool tool;

  private TextField timestr;
  private TextField station;
  private TextField beam; 
  private TextField noisestr;
  private TextField freq;
  private Button tbtn;
 
  public FanWindow(String name,RadarData data,HotBox hotbox,int mbeam) {
    this.setTitle(name);
    this.data=data;
    timestr=new TextField(20);
    station=new TextField(15);
    beam=new TextField(4);
    noisestr=new TextField(10);
    freq=new TextField(10);
 
    timestr.setEditable(false);
    station.setEditable(false);
    beam.setEditable(false);
    noisestr.setEditable(false);
    freq.setEditable(false);

    tbtn=new Button("Tool Box");
    tbtn.addActionListener(this);

    Panel p=new Panel();
    p.setLayout(new FlowLayout(FlowLayout.CENTER,2,2));
    p.add(new Label("Beam:"));
    p.add(beam);
    p.add(new Label("Frequency:"));
    p.add(freq);
    p.add(new Label("Noise:"));
    p.add(noisestr);
    p.add(tbtn);
   
    colorbar=new ColorBar(16,256);
    fan=new FanCanvas(hotbox,mbeam);
    tool=new FanTool(name,fan,colorbar);

    this.add("North",timestr);
    this.add("South",p);
    this.add("Center",fan);
    this.add("East",colorbar); 
    this.pack();
    runner=new Thread(this);
    runner.start();   
  }

  public void plotData() {
    int status,i;

    station.setText(StationID.toString(data.prm.stid));
    beam.setText(Integer.toString(data.prm.bmnum));
    freq.setText(Integer.toString(data.prm.tfreq));
    noisestr.setText(Integer.toString((int) data.prm.noise.search));
    
    timestr.setText(TimeUTC.getString(data.prm.time.yr,data.prm.time.mo,
                 data.prm.time.dy,data.prm.time.hr,
                 data.prm.time.mt,data.prm.time.sc));
  
    fan.plotData(data);
    

  }

  public void run() {
     for(;;) {
      data.poll();
      this.plotData();
      if (stop==true) break;
     }
  }

  public void kill() {
    this.setVisible(false);
    tool.setVisible(false);
    stop=false;
    this.dispose();
  }

  public void actionPerformed(ActionEvent evt) {
    String arg=evt.getActionCommand();
    if (arg.equals("Tool Box")) {
      tool.setVisible(true);
    }
  }


 
}
   




