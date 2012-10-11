/* World.java
   ==========
   Author R.J.Barnes
*/

/*
 $License$
*/




import java.awt.*;
import java.net.*;
import java.io.*;
import java.util.*;

public class World implements Runnable {
  boolean stop=false;
  PolyVector store=null;
  Thread loader=null;
  URL fileURL=null;
  String file=null;
  boolean done=false;
  
  World(URL fileURL,PolyVector store) {
    this.fileURL=fileURL;
    this.store=store;
  }

  World(String file,PolyVector store) {
    this.file=file;
    this.store=store;
  }

  void load() {
    if (done == true) return;
    if (loader !=null) stop=true;
    loader=new Thread(this);
    stop=false;
    loader.start();
  }


  public void close() {
    if (loader !=null) stop=true;
    loader=null;
  }



  public void run() {
     int side;
     int s=0;
     float lat,lon;

     InputStream inURL=null;
     InputStream in=null;
     DataInputStream data=null;

     /* System.err.println("Loading Co-ordinate set in background..."); */
     if ((fileURL==null) && (file==null)) return;
     try {    
       if (fileURL !=null) {
         inURL=fileURL.openStream();
         data=new DataInputStream(inURL);
       } else {
         in=new FileInputStream(file);
         data=new DataInputStream(in);
       }
     } catch (IOException e) {
       System.err.println("Failed to open data file.");
       return;
     }

     try {
       store.reset();
       while ((side=data.readInt())>0) {
	 if (stop==true) break;
         s=(int) data.readByte()+1;
        
         lat=data.readFloat();
         lon=data.readFloat();
         
         store.addVector(s+1,new MapPoint(lat,lon));
         s=0;
         
         for (int i=1;i<side;i++) {
           lat=data.readFloat();
           lon=data.readFloat();
           store.addVector(s,new MapPoint(lat,lon));
           s=0;
         }
       }
    } catch (IOException e) {
	/* System.err.println("Finished loading coastline."); */
    }
    store.complete();
    try {
      if (in !=null) in.close();
      if (inURL !=null) inURL.close();

    } catch (IOException e) {
	/* System.err.println("Failed to close files."); */
    }
    in=null;
    inURL=null;
    this.done=true;
  }



}

  


