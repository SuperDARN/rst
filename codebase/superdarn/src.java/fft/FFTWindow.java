/* FFTWindow.java
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

public class FFTWindow extends CloseFrame 
                         implements Runnable,ActionListener {
  boolean stop=false;
  private Thread runner;
 
  private FFTCanvas fft;
  FFTData fftdata;
  PlotInfo plotinfo;

  private Button tbtn;
  private FFTTool tool;

  public FFTWindow(String name,FFTData fftdata) {

    this.fftdata=fftdata;
    fft=new FFTCanvas();
    plotinfo=new PlotInfo();

    tbtn=new Button("Tool Box");
    tbtn.addActionListener(this);

    Panel p=new Panel();
    p.setLayout(new FlowLayout(FlowLayout.CENTER,2,2));
    p.add(tbtn);

    this.add("Center",fft);
    this.add("South",p);
    this.pack();
    this.setTitle(name);

    tool=new FFTTool(name,fft,plotinfo);

    runner=new Thread(this);
    runner.start();   
  }


  public void run() {
     for(;;) {
         fftdata.poll();
         fft.plotData(fftdata,plotinfo);
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
   




