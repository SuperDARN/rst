/* Scope.java
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

public class ScopeApp {


  public static void main(String args[]) {
     String hosts=null;
     String ports=null;
     String names=null;

     String fname=null;

     fname=new String("radar.cfg");
     File tf=new File(fname);
     if (tf.exists()==false) fname=null;

     int i,j;

     for (i=0;i<args.length;i++) {
	if (args[i].startsWith("-p")==true) {
	    ports=args[i+1];
            i++;
	} else if (args[i].startsWith("-n")==true) {
	    names=args[i+1];
            i++;
	} else if (args[i].startsWith("-f")==true) {
	  fname=args[i+1];
	  i++;
	} else hosts=args[i];
     }
 
     if (fname !=null) {
	/* load configuration from a file */
      try {
        String line=null,arg=null;
        FileInputStream in=null;
        BufferedReader txt=null;
        in=new FileInputStream(fname);
        txt=new BufferedReader(new InputStreamReader(in));
        
        while (true) {
          line=txt.readLine();
          if (line==null) break;
         
        
          for (i=0;(i<line.length()) &&
                    ((line.charAt(i)==' ') || (line.charAt(i) == '\t'));i++);
          if (i==line.length()) continue;
          if (line.charAt(i)=='#') continue;
          arg=line.substring(i);
          System.err.println(arg);
          if (arg.startsWith("hosts")==true) {
             j=arg.indexOf("=");
             hosts=arg.substring(j+1);
	  }
          if (arg.startsWith("ports")==true) {
             j=arg.indexOf("=");
             ports=arg.substring(j+1);
	  }
          if (arg.startsWith("names")==true) {
             j=arg.indexOf("=");
             names=arg.substring(j+1);
	  }
	}

      } catch (IOException e) {
        System.err.println("Failed to open configuration file.");
      }
    }

    if (hosts==null) hosts=new String("superdarn-qnx");	
    if (ports==null) ports=new String("1025");
    if (names==null) names=new String("Wallops");
    new ScopeAppWindow(hosts,ports,names); 
  }     
 
}












