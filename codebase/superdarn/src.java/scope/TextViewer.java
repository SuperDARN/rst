/* TextViewer.java
   =============== 
   Author: R.J.Barnes
*/


/*
 $License$
*/

import java.applet.*;
import java.awt.*;
import java.awt.event.*;
import java.net.*;
import java.io.*;
import java.text.*;



public class TextViewer extends CloseFrame implements Runnable,ActionListener {

  boolean stop=false;
  private Thread runner;
  public TextArea txt;
  Button clr;
  ScopeData scopedata;


  public void actionPerformed(ActionEvent evt) {
    String arg=evt.getActionCommand();
    txt.setText("");
  }

    public TextViewer(String title,ScopeData scopedata) {
 
    this.scopedata=scopedata;
    txt=new TextArea("",40,60,TextArea.SCROLLBARS_VERTICAL_ONLY);
    clr=new Button("Clear");
    this.add("Center",txt);
    Panel p=new Panel();
    p.add(clr);
    this.add("South",p);  
    clr.addActionListener(this);

    this.pack();
    runner=new Thread(this);
    runner.start();   
  }

  public void run() {
     int n;
     double pwr;
     NumberFormat fmt=NumberFormat.getInstance();

     fmt.setMaximumFractionDigits(2);
        
     for(;;) {
         scopedata.poll();
	 if (stop==true) break;
         txt.append("Samples:"+fmt.format(scopedata.nsamp)+"\n");
         txt.append("SAMPLE\tI\tQ\tPWR\n");
         for (n=0;n<scopedata.nsamp;n++) {
             pwr=scopedata.buffer[2*n]*scopedata.buffer[2*n]+
                 scopedata.buffer[2*n+1]*scopedata.buffer[2*n+1];
	     txt.append(fmt.format(n)+"\t"+
                        fmt.format(scopedata.buffer[2*n])+"\t"+
                        fmt.format(scopedata.buffer[2*n+1])+"\t"+
                        fmt.format(pwr)+"\n");
	 }
         txt.append("\n\n");


     }
  }


}



    

     
    
   
