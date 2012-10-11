/* ConnectFrame.java
   ================= 
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

public class ConnectFrame extends CloseFrame implements ActionListener {
  TextField log;
  DataMap dmap;
  RadarData data;
  StreamProcess process=null;
  SocketControl socket=null;
  int stream;
  StatusWindow status;
  InfoWindow info;
  

  String version=new String("1.0");

  public void kill() {
    this.setVisible(false);
    info.setVisible(false);
    status.kill();
    if ((process !=null) && (process.isAlive()==true)) process.close();
    data=null;
    dmap=null;
    process=null;
    this.dispose();
  }

  public ConnectFrame(String name,String host,URL portId,int stream,
		      int channel) {
    
     log=new TextField("Connection Closed");
     log.setEditable(false);
 
     Button open=new Button("Open Connection");
     Button close=new Button("Close Connection");
     open.addActionListener(this);
     close.addActionListener(this);
     
     Button sbtn=new Button("Status Window");
     sbtn.addActionListener(this);

     Button ibtn=new Button("Info");
     ibtn.addActionListener(this);   

     Panel cpanel=new Panel();
     cpanel.setLayout(new FlowLayout());
     cpanel.add(open);
     cpanel.add(close);
  
     Panel dpanel=new Panel();
     dpanel.setLayout(new FlowLayout());
     dpanel.add(sbtn);
     dpanel.add(ibtn);
 
     this.add("North",log);
     this.add("Center",cpanel);
     this.add("South",dpanel);
     this.pack();  
     this.setVisible(false);
     dmap=new DataMap();
     data=new RadarData(dmap,channel);
     socket=new SocketControl(host,portId);
     status=new StatusWindow(name,data);
     info=new InfoWindow(name,version);
     this.setTitle(name);
     
     this.stream=stream;
     process=new StreamProcess(log,data,socket,stream);
     process.open();
  }

  public void actionPerformed(ActionEvent evt) {
    String arg=evt.getActionCommand();
    if (arg.equals("Open Connection")) {
      if ((process==null) || (process.isAlive()==false)) {
        process=new StreamProcess(log,data,socket,stream);
        process.open();
      }
    } else if (arg.equals("Close Connection")) {
      if ((process !=null) && (process.isAlive()==true)) process.close();
      process=null;
    } else if (arg.equals("Status Window")) {
      status.setVisible(true);
    } else if (arg.equals("Info")) {
      info.setVisible(true);
    }
  }
 
}












