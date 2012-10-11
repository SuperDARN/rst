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
  FanWindow fan;
  GeoWindow geo;
  TimeWindow time;


  HotBox hotbox;

  InfoWindow info;
 
  DeltaRead delta;

  String version=new String("3.0");

  public void kill() {
    this.setVisible(false);
    info.setVisible(false);
    status.kill();
    fan.kill();
    geo.kill();
    time.kill();


    if ((process !=null) && (process.isAlive()==true)) process.close();
    data=null;
    dmap=null;
    process=null;
    this.dispose();
  }

  public ConnectFrame(String name,String host,int port,int stream,
		      int channel,int bmnum,int mbeam,int nrang,
		      PolyVector meridian,
		      PolyVector globe,Transform trf,
		      String deltaFile) {
    
     log=new TextField("Connection Closed");
     log.setEditable(false);
 
     Button open=new Button("Open Connection");
     Button close=new Button("Close Connection");
     open.addActionListener(this);
     close.addActionListener(this);
     
 
     Button sbtn=new Button("Status Window");
     sbtn.addActionListener(this);

     Button fbtn=new Button("Fan Plot");
     fbtn.addActionListener(this);

     Button gbtn=new Button("Geographic Plot");
     gbtn.addActionListener(this);

     Button tbtn=new Button("Time Plot");
     tbtn.addActionListener(this);

     Button hbtn=new Button("Hot Box");
     hbtn.addActionListener(this);

     Button ibtn=new Button("Info");
     ibtn.addActionListener(this);   

     Panel cpanel=new Panel();
     cpanel.setLayout(new FlowLayout());
     cpanel.add(open);
     cpanel.add(close);
  
     Panel dpanel=new Panel();
     dpanel.setLayout(new FlowLayout());
     dpanel.add(fbtn);
     dpanel.add(gbtn);
     dpanel.add(tbtn);
     dpanel.add(sbtn);
     dpanel.add(hbtn);
     dpanel.add(ibtn);
 
     this.add("North",log);
     this.add("Center",cpanel);
     this.add("South",dpanel);
     this.pack();  
     this.setVisible(false);
     dmap=new DataMap();
     data=new RadarData(dmap,channel);
     socket=new SocketControl(host,port);

     hotbox=new HotBox(name);

     status=new StatusWindow(name,data,mbeam);
     fan=new FanWindow(name,data,hotbox,mbeam);
     geo=new GeoWindow(name,data,hotbox,meridian,globe,trf,mbeam);
     time=new TimeWindow(name,data,hotbox,bmnum,mbeam,nrang);
 
     info=new InfoWindow(name,version);
     this.setTitle(name);

     if (deltaFile !=null) System.err.println(deltaFile);
     if (deltaFile !=null) delta=new DeltaRead(deltaFile,time.time); 

     
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
    } else if (arg.equals("Fan Plot")) {
      fan.setVisible(true);
    } else if (arg.equals("Geographic Plot")) {
      geo.setVisible(true);
    } else if (arg.equals("Time Plot")) {
      time.setVisible(true);
    } else if (arg.equals("Hot Box")) {
      hotbox.setVisible(true);
    } else if (arg.equals("Info")) {
      info.setVisible(true);
    }
  }
 
}












