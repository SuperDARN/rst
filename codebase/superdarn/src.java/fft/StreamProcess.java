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
  boolean data=true;
  Message handler=new Message();

  InputStream in=null;
  SocketControl s=null;
  TextField log=null;
  FFTData fftdata=null;
 
  byte buffer[];
  private int count=0;
  
  public StreamProcess(TextField log,FFTData fftdata,SocketControl s) {
    this.log=log;
    this.fftdata=fftdata;
    this.s=s;
    this.data=true;
  }
 
  public void open() {
    if (this.isAlive()==true) return;
    if (s.isOpen() == true) s.closeSocket(); 
    this.count=0;
    this.in=s.openSocket();
    if (this.in==null) {
      log.setText("Connection Failed");
      data=false;
    } else log.setText("Connection Open");
    this.start();
  }

  public void close() {
    if (this.isAlive()==true) data=false;
    if (s.isOpen() ==true) s.closeSocket();
    log.setText("Connection Closed");
  }
 
  public void run() {
    while (data==true) {

      /* listen for broadcasts from the server */ 

      System.err.println("Waiting for Packet");
      buffer=handler.read(this.in);
      if (buffer == null) break;

      /* report to system log */
      System.err.println("Processing packet:"+this.count);
      log.setText("Processing packet:"+this.count);
      
      fftdata.decode(buffer);
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










