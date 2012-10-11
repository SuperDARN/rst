/* HotBox.java
   ===========
   Author R.J.Barnes
*/

/*
 $License$
*/





import java.lang.*;
import java.awt.*;
import java.util.*;
import java.text.*;


public class HotBox extends CloseFrame {

 private TextField tmefld;
  private TextField bfld;
  private TextField rfld;
  private TextField gfld;
  private TextField vfld;
  private TextField latfld;
  private TextField lngfld;
  private TextField rkmfld;
  private TextField frqfld;
  private TextField nsefld;
  
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

  public HotBox(String name) {
    tmefld=new TextField(32);
    bfld=new TextField(3);
    rfld=new TextField(3);
    gfld=new TextField(4);
    vfld=new TextField(12);
    latfld=new TextField(16);
    lngfld=new TextField(16);
    rkmfld=new TextField(6);
    frqfld=new TextField(16);
    nsefld=new TextField(16);
    bfld.setEditable(false);
    rfld.setEditable(false);
    gfld.setEditable(false);
    vfld.setEditable(false);
    latfld.setEditable(false);
    lngfld.setEditable(false);
    rkmfld.setEditable(false);
    frqfld.setEditable(false);
    nsefld.setEditable(false);
    tmefld.setEditable(false);    

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

    this.layout(new Label("Beam:"),0,1,1,1);
    this.layout(new Label("Range:"),2,1,1,1); 
    this.layout(new Label("Range (km):"),2,2,1,1);

    this.layout(new Label("Latitude:"),0,3,1,1);
    this.layout(new Label("Longitude:"),2,3,1,1);

    this.layout(new Label("Gflg:"),0,4,1,1);
    this.layout(new Label("Value:"),2,4,1,1); 
  
    this.layout(new Label("Frequency:"),0,5,1,1);
    this.layout(new Label("Noise:"),2,5,1,1); 

    this.layout(tmefld,0,0,4,1);  

    this.layout(bfld,1,1,1,1);
    this.layout(rfld,3,1,1,1);
    this.layout(rkmfld,3,2,1,1);

    this.layout(latfld,1,3,1,1);
    this.layout(lngfld,3,3,1,1);

    this.layout(gfld,1,4,1,1); 
    this.layout(vfld,3,4,1,1);
    this.layout(frqfld,1,5,1,1);
    this.layout(nsefld,3,5,1,1);
    
    this.pack();
    this.setTitle(name);

  }


  public void set(int bm,int rng,byte flag,double value,int rkm,
                  double lat,double lon,int freq,float noise,String time) {
    if (flag==0) {
      vfld.setText("no data");
      gfld.setText("-");
    } else {
     if (flag==2) gfld.setText("Yes");
     else gfld.setText("No");
     vfld.setText(Double.toString(value));
    } 

    NumberFormat fmt=NumberFormat.getInstance();
    fmt.setMaximumFractionDigits(2);
    fmt.setMinimumFractionDigits(2);
    fmt.setMinimumIntegerDigits(3);


    bfld.setText(Integer.toString(bm));
    rfld.setText(Integer.toString(rng));
    lngfld.setText(fmt.format(lon));
    latfld.setText(fmt.format(lat));
    rkmfld.setText(Integer.toString(rkm));
    frqfld.setText(Integer.toString(freq));
    nsefld.setText(fmt.format(noise));
    tmefld.setText(time);
  }
}




