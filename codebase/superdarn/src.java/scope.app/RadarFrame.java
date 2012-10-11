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
  ScopeData scopedata=new ScopeData();
  StreamProcess process=null;
  SocketControl socket=null;
  ScopeWindow scope;
  PowerWindow power;
  AvgPowerWindow average;
  TextViewer text;
  InfoWindow info;
  

  String version=new String("1.0");

  public void kill() {
    this.setVisible(false);
    info.setVisible(false);
    scope.kill();
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
     
     Button sbtn=new Button("Scope Display");
     sbtn.addActionListener(this);

     Button pbtn=new Button("Power Display");
     pbtn.addActionListener(this);

     Button abtn=new Button("Average Power Display");
     abtn.addActionListener(this);


     Button tbtn=new Button("Text List");
     tbtn.addActionListener(this);


     Button ibtn=new Button("Info");
     ibtn.addActionListener(this);   

     Panel cpanel=new Panel();
     cpanel.setLayout(new FlowLayout());
     cpanel.add(open);
     cpanel.add(close);
  
     Panel dpanel=new Panel();
     dpanel.setLayout(new FlowLayout());
     dpanel.add(sbtn);
     dpanel.add(pbtn);
     dpanel.add(abtn);
     dpanel.add(tbtn);
     dpanel.add(ibtn);
 
     this.add("North",log);
     this.add("Center",cpanel);
     this.add("South",dpanel);
     this.pack();  
     this.setVisible(false);
     socket=new SocketControl(host,port);
     process=new StreamProcess(log,scopedata,socket);
     process.open();

     scope=new ScopeWindow(name,scopedata);
     power=new PowerWindow(name,scopedata);
     average=new AvgPowerWindow(name,scopedata);
     info=new InfoWindow(name,version);
     text=new TextViewer(name,scopedata);
     this.setTitle(name);

  }

  public void actionPerformed(ActionEvent evt) {
    String arg=evt.getActionCommand();
    if (arg.equals("Open Connection")) {
      if ((process==null) || (process.isAlive()==false)) {
        process=new StreamProcess(log,scopedata,socket);
        process.open();
      }
    } else if (arg.equals("Close Connection")) {
      if ((process !=null) && (process.isAlive()==true)) process.close();
      process=null;
    } else if (arg.equals("Scope Display")) {
      scope.setVisible(true);
    } else if (arg.equals("Power Display")) {
	power.setVisible(true);
    } else if (arg.equals("Average Power Display")) {
	average.setVisible(true);
    } else if (arg.equals("Text List")) {
      text.setVisible(true);
    } else if (arg.equals("Info")) {
      info.setVisible(true);
    }

  }
 
}









