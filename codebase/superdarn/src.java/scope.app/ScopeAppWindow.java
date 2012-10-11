/* ScopeAppWindow.java
   =================== 
   Author: R.J.Barnes
*/

/*
 $License$
*/





import java.awt.*;
import java.awt.event.*;
import java.io.*;
import java.net.*;

public class ScopeAppWindow extends CloseFrame implements ActionListener {


  String host[]=new String[32];
  String name[]=new String[32];
  int port[]=new int[32];
  int maxhost=0;


  RadarFrame radar[]=new RadarFrame[32];

 
  public ScopeAppWindow(String hosts,String ports,String names) {
    int i=0,j=0,k=0,l=0;
    Button open=null;
   
    if (hosts==null) hosts=new String("209.128.50.18");	
    if (ports==null) ports=new String("1024");
    if (names==null) names=new String("Unknown");

    do {
      i=hosts.indexOf(",",i);
      if (i !=-1) host[k]=hosts.substring(j,i);
      else host[k]=hosts.substring(j);
      k++;
      i++;
      j=i;
    } while (i !=0);
    for (i=0;i<k;i++) {
      System.err.println("Radar Hosts="+host[i]);
    }
    i=k=j=0;

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
    String prtstr;
    do {
	i=ports.indexOf(",",i);
	if (i !=-1) prtstr=ports.substring(j,i);
	else prtstr=ports.substring(j);
	try {
	    port[k]=(new Integer(prtstr.trim())).intValue();
	} catch (NumberFormatException ex) {
	    port[k]=1024;
	}
	k++;
	i++;
	j=i;
    } while (i !=0);

    for (i=0;i<k;i++) {
	System.err.println("Port Numbers="+port[i]);
    }


    maxhost=k;
    for (i=0;i<maxhost;i++) 
      if (name[i]==null) name[i]=new String("Unknown ("+i+")");
    
   
    this.setLayout(new GridLayout(l,1,4,4));

  
     
    for (i=0;i<maxhost;i++) {
      if (port[i]==0) continue;
      open=new Button(name[i]);
      open.addActionListener(this);
      this.add(open);
    }
    this.pack();
    this.setVisible(true);
  }

  public void actionPerformed(ActionEvent evt) {
    int i;
    String s=evt.getActionCommand();
    for (i=0;i<maxhost;i++) 
      if ((name[i] !=null) && (s.equals(name[i]))) {
        if (radar[i]==null) radar[i]=
          new RadarFrame(name[i],host[i],port[i]);
        radar[i].setVisible(true);
      }
  }


  public void destroy() {
    int i;
    for (i=0;i<maxhost;i++) if (radar[i] !=null) radar[i].kill();
  }
 
}












