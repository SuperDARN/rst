/* RadarFrame.java
   =============== 
   Author: R.J.Barnes
*/


/*
 $License$
*/




import java.applet.*;
import java.awt.*;
import java.awt.event.*;
import java.io.*;
import java.net.*;

public class RadarFrame extends CloseFrame implements ActionListener {

  TextField log;
  FFTData fftdata=new FFTData();
  StreamProcess process=null;
  SocketControl socket=null;
  FFTWindow fft;
  InfoWindow info;
  

  String version=new String("1.0");

  public void kill() {
    this.setVisible(false);
    info.setVisible(false);
    fft.kill();
    if ((process !=null) && (process.isAlive()==true)) process.close();
    process=null;
    this.dispose();
  }

  public RadarFrame(String name,String host,int port) {
    
     log=new TextField("Connection Closed");
     log.setEditable(false);
 
     Button open=new Button("Open Connection");
     Button close=new Button("Close Connection");
     open.addActionListener(this);
     close.addActionListener(this);
     
     Button fbtn=new Button("FFT Display");
     fbtn.addActionListener(this);

     Button ibtn=new Button("Info");
     ibtn.addActionListener(this);   

     Panel cpanel=new Panel();
     cpanel.setLayout(new FlowLayout());
     cpanel.add(open);
     cpanel.add(close);
  
     Panel dpanel=new Panel();
     dpanel.setLayout(new FlowLayout());
     dpanel.add(fbtn);
     dpanel.add(ibtn);
 
     this.add("North",log);
     this.add("Center",cpanel);
     this.add("South",dpanel);
     this.pack();  
     this.setVisible(false);
     socket=new SocketControl(host,port);
     process=new StreamProcess(log,fftdata,socket);
     process.open();

     fft=new FFTWindow(name,fftdata);
     info=new InfoWindow(name,version);
     this.setTitle(name);

  }

  public void actionPerformed(ActionEvent evt) {
    String arg=evt.getActionCommand();
    if (arg.equals("Open Connection")) {
      if ((process==null) || (process.isAlive()==false)) {
        process=new StreamProcess(log,fftdata,socket);
        process.open();
      }
    } else if (arg.equals("Close Connection")) {
      if ((process !=null) && (process.isAlive()==true)) process.close();
      process=null;
    } else if (arg.equals("FFT Display")) {
      fft.setVisible(true);
    } else if (arg.equals("Info")) {
      info.setVisible(true);
    }
  }
 
}












