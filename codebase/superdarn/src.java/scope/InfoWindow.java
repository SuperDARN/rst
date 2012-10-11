/* InfoWindow.java
   =============== 
   Author: R.J.Barnes
*/


/*
 $License$
*/




import java.lang.*;
import java.awt.*;
import java.awt.event.*;
import java.util.*;

public class InfoWindow extends CloseFrame {


 private GridBagLayout gridbag=new GridBagLayout();
  private GridBagConstraints constrain=new GridBagConstraints();

  public void layout(Component component,int gx,int gy,int wx,int wy) {
    constrain.gridx=gx;
    constrain.gridy=gy;
    constrain.gridwidth=wx;
    constrain.gridheight=wy;
    gridbag.setConstraints(component,constrain);
    this.add(component);
  } 


  public InfoWindow(String name,String version) {
    this.setTitle(name);
    this.setLayout(gridbag);
    constrain.gridx=0;
    constrain.gridy=0;
    constrain.gridwidth=0;
    constrain.gridwidth=0;
    constrain.fill=GridBagConstraints.BOTH; 
    constrain.ipadx=0;
    constrain.ipady=0;
    constrain.insets=new Insets(2,2,2,2);
    constrain.anchor=GridBagConstraints.CENTER;
    constrain.weightx=0;
    constrain.weighty=0;

    this.layout(new Label("Purpose:"),0,0,1,1);
    this.layout(new Label("Version:"),0,1,1,1);
    this.layout(new Label("Author:"),0,2,1,1);

    this.layout(new Label("SuperDARN Oscilloscope"),1,0,1,1);
    this.layout(new Label(version),1,1,1,1);
    this.layout(new Label("R.J.Barnes"),1,2,1,1);
    this.pack();
  }
}
