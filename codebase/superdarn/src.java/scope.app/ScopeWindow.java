/* ScopeWindow.java
   ================= 
   Author: R.J.Barnes
*/

/*
 $License$
*/




import java.lang.*;
import java.awt.*;
import java.awt.event.*;
import java.util.*;

public class ScopeWindow extends CloseFrame 
                         implements Runnable,ActionListener {
  boolean stop=false;
  private Thread runner;
 
  private ScopeCanvas scope;
  ScopeData scopedata;
  PlotInfo plotinfo;

  private Button tbtn;
  private ScopeTool tool;

  public ScopeWindow(String name,ScopeData scopedata) {

    this.scopedata=scopedata;
    scope=new ScopeCanvas();
    plotinfo=new PlotInfo();

    tbtn=new Button("Tool Box");
    tbtn.addActionListener(this);

    Panel p=new Panel();
    p.setLayout(new FlowLayout(FlowLayout.CENTER,2,2));
    p.add(tbtn);

    this.add("Center",scope);
    this.add("South",p);
    this.pack();
    this.setTitle(name);

    tool=new ScopeTool(name,scope,plotinfo);

    runner=new Thread(this);
    runner.start();   
  }


  public void run() {
     for(;;) {
         scopedata.poll();
         scope.plotData(scopedata,plotinfo);
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
   




