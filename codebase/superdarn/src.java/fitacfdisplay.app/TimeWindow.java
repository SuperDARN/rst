/* TimeWindow.java
   ===============
   Author R.J.Barnes
*/

/*
 $License$
*/





import java.lang.*;
import java.awt.*;
import java.awt.event.*;
import java.util.*;

public class TimeWindow extends CloseFrame implements Runnable,ActionListener {
  boolean stop=false;
  private Thread runner;
  RadarData data;
  int bmnum;

  public ColorBar colorbar;
  public TimeCanvas time;
  public TimeTool tool;
 
  private TextField timestr;
  private TextField station;
  private TextField bmstr;
  private Button tbtn;
 
 
  public TimeWindow(String name,RadarData data,HotBox hotbox,int bmnum,
                    int mbeam,int nrang) {

   
    this.data=data;
    this.bmnum=bmnum;
    timestr=new TextField(20);
    station=new TextField(15);
    bmstr=new TextField(2);
    
    timestr.setEditable(false);
    station.setEditable(false);
    bmstr.setEditable(false);     

    tbtn=new Button("Tool Box");
    tbtn.addActionListener(this);

   
    bmstr.setText(""+bmnum);

    colorbar=new ColorBar(16,256);
    time=new TimeCanvas(bmnum,mbeam,nrang,hotbox);    
    tool=new TimeTool(name,time,colorbar);

    Panel t=new Panel();
    t.setLayout(new FlowLayout(FlowLayout.CENTER));
    t.add(timestr);
    t.add(bmstr);  
    t.add(time.zoom);
    t.add(tbtn);

    Panel p=new Panel();
    p.setSize(time.s.width,time.s.height);
    p.setLayout(new BorderLayout());
    p.add("Center",time);
    p.add("South",time.hbar);

    this.add("North",t);
    this.add("East",colorbar); 
    this.add("Center",p); 
    this.pack();

    this.setTitle(name);
    runner=new Thread(this);
    runner.start();   
  }

  public void plotData() {
    int status,i;
    if (data.prm.bmnum !=bmnum) return;
    station.setText(StationID.toString(data.prm.stid));
    timestr.setText(TimeUTC.getString(data.prm.time.yr,data.prm.time.mo,
				      data.prm.time.dy,data.prm.time.hr,
				      data.prm.time.mt,data.prm.time.sc));
    time.plotData(data);
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
   




