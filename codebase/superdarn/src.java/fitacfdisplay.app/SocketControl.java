/* SocketControl.java
   ================== 
   Author: R.J.Barnes
*/

/*
 $License$
*/





import java.awt.*;
import java.lang.*;
import java.io.*;
import java.net.*;

public class SocketControl {
  boolean state=false;
  InputStream in=null;
  OutputStream out=null;
  Socket s=null;
 
  String host;
  int port;

  public void dispose() {
    this.closeSocket();
    s=null;
    in=null;
  }

  public SocketControl(String host,int port) {
   this.host=host;
   this.port=port;
  }


  public InputStream getInput() {
      return this.in;
  }

  public OutputStream getOutput() {
      return this.out;
  }
 


  public boolean openSocket() {
    this.s=null;
    this.in=null;
    this.out=null;
    this.state=false;
 
    if ((host==null) || (port==0)) {
      System.err.println("Null pointers passed to openSocket");
      return false;
    }

    try {
      this.s=new Socket(host,port);
      this.in=s.getInputStream();
      this.out=s.getOutputStream();
      this.state=true;
    } catch (IOException e) {
      System.err.println("Failed to open socket:"+e.toString());
      return false;
    }
    System.err.println("Socket is Open");
    return true;
  }

  public void closeSocket() {
     try {
       if (in !=null) in.close();
       if (s !=null) s.close();
       this.state=false;
     } catch (IOException e) {
       System.err.println("Failed to close Socket:"+e.toString());
       return;
     }
     System.err.println("Socket is Closed");
  }

  public boolean isOpen() {
    return state;
  }

}
  












