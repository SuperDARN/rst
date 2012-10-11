/* FitACFWindow.java
   ==================== 
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

public class FitACFWindow extends CloseFrame implements ActionListener {

  double scale=2.8;

  String host[]=new String[32];
  String name[]=new String[32];
  int port[]=new int[32];
  int stream[]=new int[32];
  int maxhost=0;

  int channel[]=new int[32];

  int beam[]=new int[32];
  int mbeam[]=new int[32];
  int nrang[]=new int[32];


  String deltaFile[]=new String[32];
  Stereo trf[]=new Stereo[32];

  PolyVector meridian;
  PolyVector globe;
  World world;


  ConnectFrame connex[]=new ConnectFrame[32];

 
  public FitACFWindow(String hosts,String ports,String names,String streams,
                      String channels,String beams,String mbeams,String nrangs,String deltas,
		      String sf,String mapname) {
    int i=0,j=0,k=0,l=0;
    Button open=null;
   
    if (hosts==null) hosts=new String("209.128.50.18");	
    if (ports==null) ports=new String("1024");
    if (names==null) names=new String("Unknown");
    if (streams==null) streams=new String("1");
    if (channels==null) channels=new String("a");
    if (beams==null) beams=new String("8");
    if (mbeams==null) mbeams=new String("16");
    if (nrangs==null) nrangs=new String("75");

    if (mapname==null) mapname=new String("mapdata");
    if (sf==null) sf=new String("2.8");   
     
    globe=new PolyVector(10);
    world=new World(mapname,globe);
  

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
    

    i=k=j=0;
    String bmstr;
    do {
	i=beams.indexOf(",",i);
	if (i !=-1) bmstr=beams.substring(j,i);
	else bmstr=beams.substring(j);
	try {
	    beam[k]=(new Integer(bmstr.trim())).intValue();
	} catch (NumberFormatException ex) {
	    beam[k]=8;
	}
	k++;
	i++;
	j=i;
    } while (i !=0);

    for (i=0;i<k;i++) {
	System.err.println("Summary Beam="+beam[i]);
    }

    i=k=j=0;
    String mbmstr;
    do {
	i=mbeams.indexOf(",",i);
	if (i !=-1) mbmstr=mbeams.substring(j,i);
	else mbmstr=mbeams.substring(j);
	try {
	    mbeam[k]=(new Integer(mbmstr.trim())).intValue();
	} catch (NumberFormatException ex) {
	    mbeam[k]=16;
	}
	k++;
	i++;
	j=i;
    } while (i !=0);

    for (i=0;i<k;i++) {
	System.err.println("Maximum Beams="+mbeam[i]);
    }


    i=k=j=0;
    String nrstr;
    do {
	i=nrangs.indexOf(",",i);
	if (i !=-1) nrstr=nrangs.substring(j,i);
	else nrstr=nrangs.substring(j);
	try {
	    nrang[k]=(new Integer(nrstr.trim())).intValue();
	} catch (NumberFormatException ex) {
	    nrang[k]=75;
	}
	k++;
	i++;
	j=i;
    } while (i !=0);

    for (i=0;i<k;i++) {
	System.err.println("Number of Ranges="+nrang[i]);
    }



    i=k=j=0;

    if (deltas !=null) {
      do {
        i=deltas.indexOf(",",i);
        if (i !=-1) deltaFile[k]=deltas.substring(j,i);
        else deltaFile[k]=deltas.substring(j);
        k++;
        i++;
        j=i;
      } while (i !=0);
      i=k=j=0;
    }

    try {
       scale=(new Double(sf.trim())).doubleValue();
    } catch (NumberFormatException ex) {
       scale=2.8;
    }

   
    this.setLayout(new GridLayout(l,1,4,4));

    this.meridian=new PolyVector(10);
    Meridian.make(meridian,15,15);

    world.load();
     
    for (i=0;i<maxhost;i++) {
      if (port[i]==0) continue;
      trf[i]=new Stereo(scale,0,0);
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
        if (connex[i]==null) connex[i]=
          new ConnectFrame(name[i],host[i],port[i],stream[i],
			   channel[i],beam[i],mbeam[i],nrang[i],
			   meridian,globe,trf[i],deltaFile[i]);
        connex[i].setVisible(true);
      }
  }


  public void destroy() {
    int i;
    for (i=0;i<maxhost;i++) if (connex[i] !=null) connex[i].kill();
  }
 
}












