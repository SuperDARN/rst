/* FitACFClient.java
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

public class FitACFClient extends Applet implements ActionListener {
  String host=null;
  String name[]=new String[32];
  URL portId[]=new URL[32];
  int stream[]=new int[32];
  int maxhost=0;

  int channel[]=new int[32];

  ConnectFrame connex[]=new ConnectFrame[32];

  public void init() {
    int i=0,j=0,k=0,l=0;
    Button open=null;
    URL codeBase=getCodeBase();
    String port=null;
    String delta=null;
    String ports=this.getParameter("ports");
    String names=this.getParameter("names");
    String streams=this.getParameter("streams");

    String channels=this.getParameter("channels");



    if (ports==null) ports=new String("port_id");
    if (names==null) names=new String("Unknown");
    if (streams==null) streams=new String("1");

    if (streams==null) streams=new String("1");


    host=codeBase.getHost();
  
    do {
      i=names.indexOf(",",i);
      if (i !=-1) name[k]=names.substring(j,i);
      else name[k]=names.substring(j);
      k++;
      i++;
      j=i;
    } while (i !=0);

    for (i=0;i<k;i++) {
      System.err.println("Radar Names="+name[i]);
    }
    i=k=j=0;
    
    do {
      i=ports.indexOf(",",i);
      if (i !=-1) port=ports.substring(j,i);
      else port=ports.substring(j);
      try {
        portId[k]=new URL(codeBase,port);
        l++;
      } catch (MalformedURLException e) {
        System.err.println("Failed to URL");
      }
      k++;
      i++;
      j=i;
    } while (i !=0);

    i=k=j=0;
    String ststr;
    do {
      i=streams.indexOf(",",i);
      if (i !=-1) ststr=streams.substring(j,i);
      else ststr=streams.substring(j);
      try {
         stream[k]=(new Integer(ststr.trim())).intValue();
      } catch (NumberFormatException ex) {
        stream[k]=1;
      }
      k++;
      i++;
      j=i;
    } while (i !=0);

    for (i=0;i<k;i++) {
      System.err.println("Streams="+stream[i]);
    }

    i=k=j=0;
    String chstr;
    do {
      i=channels.indexOf(",",i);
      if (i !=-1) chstr=channels.substring(j,i);
      else chstr=channels.substring(j);
      channel[k]=0;
      if (chstr.charAt(0)=='a') channel[k]=1;
      if (chstr.charAt(0)=='b') channel[k]=2;
      k++;
      i++;
      j=i;
    } while (i !=0);

    for (i=0;i<k;i++) {
      System.err.println("Channels="+channel[i]);
    }


    maxhost=k;
    for (i=0;i<maxhost;i++) 
      if (name[i]==null) name[i]=new String("Unknown ("+i+")");
    
   
    this.setLayout(new GridLayout(l,1,4,4));
     
    for (i=0;i<maxhost;i++) {
      if (portId[i]==null) continue;
      open=new Button(name[i]);
      open.addActionListener(this);
      this.add(open);
    }

  }

  public void actionPerformed(ActionEvent evt) {
    int i;
    String s=evt.getActionCommand();
    for (i=0;i<maxhost;i++) 
      if ((name[i] !=null) && (s.equals(name[i]))) {
        if (connex[i]==null) connex[i]=
          new ConnectFrame(name[i],host,portId[i],stream[i],
			   channel[i]);
        connex[i].setVisible(true);
      }
  }


  public void destroy() {
    int i;
    for (i=0;i<maxhost;i++) if (connex[i] !=null) connex[i].kill();
  }
 
}












