/* AvgPowerWindow.java
   =================== 
   Author: R.J.Barnes
*/

/*
 $License$
*/




import java.lang.*;
import java.awt.*;
import java.awt.event.*;
import java.util.*;

public class AvgPowerWindow extends CloseFrame 
                         implements Runnable,ActionListener {
  boolean stop=false;
  private Thread runner;
 
  private AvgPowerCanvas power;
  ScopeData scopedata;
  PlotInfo plotinfo;

  private Button tbtn;
  private AvgPowerTool tool;

  public AvgPowerWindow(String name,ScopeData scopedata) {

    this.scopedata=scopedata;
    power=new AvgPowerCanvas();
    plotinfo=new PlotInfo();
    plotinfo.ymin=0;
    plotinfo.ymax=10;
    plotinfo.ymajor=1;
    tbtn=new Button("Tool Box");
    tbtn.addActionListener(this);

    Panel p=new Panel();
    p.setLayout(new FlowLayout(FlowLayout.CENTER,2,2));
    p.add(tbtn);

    this.add("Center",power);
    this.add("South",p);
    this.pack();
    this.setTitle(name);

    tool=new AvgPowerTool(name,power,plotinfo);

    runner=new Thread(this);
    runner.start();   
  }


  public void run() {
     for(;;) {
         scopedata.poll();
         power.plotData(scopedata,plotinfo);
	 if (stop==true) break;
     }
  }

  public void kill() {
    this.setVisible(false);
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
   




