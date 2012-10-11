/* StreamProcess.java
   ==================
   Author: R.J.Barnes
*/


/*
 $License$
*/




import java.applet.*;
import java.awt.*;
import java.io.*;
import java.net.*;

class StreamProcess extends Thread {
  boolean dflag=true;
  Message handler=new Message();

  InputStream in=null;
  OutputStream out=null;
  RadarData data=null; 
  SocketControl s=null;
  TextField log=null;

  int stream; 
  byte buffer[];
  private int count=0;
  
  public StreamProcess(TextField log,RadarData data,SocketControl s,
                       int stream) {
    this.data=data;
    this.log=log;
    this.s=s;
    this.dflag=true;
    this.stream=stream;
  }
 
  public void open() {
    if (this.isAlive()==true) return;
    if (s.isOpen() == true) s.closeSocket(); 
    this.count=0;
    if (s.openSocket()==false) {
      log.setText("Connection Failed");
      dflag=false;
    } else {
      log.setText("Connection Open");  
      this.in=s.getInput();
      this.out=s.getOutput();  
      try { 
        out.write((byte) stream);
      } catch (IOException e) {
          log.setText("Connection Failed");
          System.err.println("Failed to set stream");
          s.closeSocket();
          return;
      }
      this.start();
    }
  }

  public void close() {
    if (this.isAlive()==true) dflag=false;
    if (s.isOpen() ==true) s.closeSocket();
    log.setText("Connection Closed");
  }
 
  public void run() {
    while (dflag==true) {

      /* listen for broadcasts from the server */ 

      /* System.err.println("Waiting for Packet"); */
      buffer=handler.read(this.in);
      if (buffer == null) break;

      /* report to system log */
      System.err.println("Processing packet:"+this.count);
      log.setText("Processing packet:"+this.count);
      /* decode the message here */

      data.decode(buffer,0);
      this.count++;
    }
    
    /* If we get here then the connection has died and this 
       thread will die with it */

    if (s.isOpen()) {
      s.closeSocket();
      log.setText("Connection Reset");
    }
    System.err.println("Stream processor thread died:Connection lost");
  }
  
}    










