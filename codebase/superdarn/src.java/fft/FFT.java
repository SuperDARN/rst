/* FFT.java
   ==========
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

public class FFT extends Applet implements ActionListener {
  String host=null;
  String name[]=new String[32];
  URL portId[]=new URL[32];

  int maxhost=0;

  RadarFrame radar[]=new RadarFrame[32];

  public void init() {
    int i=0,j=0,k=0,l=0;
    Button openRadar=null;
    URL codeBase=getCodeBase();
    String port=null;
    String delta=null;
    String ports=this.getParameter("ports");
    String names=this.getParameter("names");
   
    if (ports==null) ports=new String("port_id");
    if (names==null) names=new String("Unknown");

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

    maxhost=k;
    for (i=0;i<maxhost;i++) 
      if (name[i]==null) name[i]=new String("Unknown ("+i+")");
    
   
    this.setLayout(new GridLayout(l,1,4,4));
     
    for (i=0;i<maxhost;i++) {
      if (portId[i]==null) continue;
      openRadar=new Button(name[i]);
      openRadar.addActionListener(this);
      this.add(openRadar);
    }

  }

  public void actionPerformed(ActionEvent evt) {
    int i;
    String s=evt.getActionCommand();
    for (i=0;i<maxhost;i++) 
      if ((name[i] !=null) && (s.equals(name[i]))) {
        if (radar[i]==null) radar[i]=
          new RadarFrame(name[i],host,portId[i]);
        radar[i].setVisible(true);
      }
  }


  public void destroy() {
    int i;
    for (i=0;i<maxhost;i++) if (radar[i] !=null) radar[i].kill();
  }
 
}












