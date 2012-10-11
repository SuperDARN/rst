/* GeoWindow.java
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

public class GeoWindow extends CloseFrame implements Runnable,ActionListener {
  boolean stop=false;
  private Thread runner;
  RadarData data;

  private GeoCanvas geo;
  private ColorBar colorbar;
  private GeoTool tool;

  private TextField timestr;
  private TextField station;
  private TextField beam; 
  private TextField noisestr;
  private TextField freq;
 
  private Button tbtn;


  
  public GeoWindow(String name,RadarData data,HotBox hotbox,
		   PolyVector meridian,PolyVector globe,Transform trf,
                   int mbeam) {
    boolean stop=false;
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
    geo=new GeoCanvas(hotbox,meridian,globe,trf,mbeam);
    tool=new GeoTool(name,geo,colorbar);

    this.add("North",timestr);
    this.add("South",p);
    this.add("Center",geo);
    this.add("East",colorbar); 
    this.pack();
    this.setTitle(name);
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

    geo.plotData(data);
    

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
    stop=true;
    this.dispose();
  }

  public void actionPerformed(ActionEvent evt) {
    String arg=evt.getActionCommand();
    if (arg.equals("Tool Box")) {
      tool.setVisible(true);
    }
  }

 
}
   




