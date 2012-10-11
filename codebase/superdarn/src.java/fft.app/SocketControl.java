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


  public InputStream openSocket() {
    this.s=null;
    this.in=null;
    this.state=false;
  

    if ((host==null) || (port==0)) {
      System.err.println("Null pointers passed to openSocket"+host+":"+port);
      return null;
    }

    try {
      this.s=new Socket(host,port);
      this.in=s.getInputStream();
      this.state=true;
    } catch (IOException e) {
      System.err.println("Failed to open socket:"+e.toString());
      return null;
    }
    System.err.println("Socket is Open");
    return in;
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
  












