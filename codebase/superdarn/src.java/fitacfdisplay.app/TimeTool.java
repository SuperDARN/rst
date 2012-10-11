/* TimeTool.java
   =============
   Author R.J.Barnes
*/

/*
 $License$
*/





import java.applet.*;
import java.awt.*;
import java.awt.event.*;


public class TimeTool extends CloseFrame implements 
                                             ActionListener,ItemListener {

  private Button pbtn;
  private Button vbtn;
  private Button wbtn;
 
  private TextField rfld;

  private Checkbox cbox;
  private Checkbox bbox;
  private Checkbox gfbox;
 
  private Choice gcol;
  private Choice bcol;

  TimeCanvas time;
  ColorBar color;


  private class RngText implements ActionListener {
    public void actionPerformed(ActionEvent evt) {
      try {
        double rng=(new Double(rfld.getText().trim())).doubleValue();
        if (rng<1) rng=1;
        rfld.setText(""+rng);
        color.setRange(rng);
        time.setRange(rng);
      } catch (NumberFormatException ex) {
        System.err.println("Invalid entry in field");
      }
    }
  }

  private class GridColor implements ItemListener {
    public void itemStateChanged(ItemEvent evt) {
      
       if (evt.getItem().equals("black")) time.setGridColor(Color.black);
       if (evt.getItem().equals("blue")) time.setGridColor(Color.blue);
       if (evt.getItem().equals("red")) time.setGridColor(Color.red);
       if (evt.getItem().equals("yellow")) time.setGridColor(Color.yellow);
       if (evt.getItem().equals("green")) time.setGridColor(Color.green);
       if (evt.getItem().equals("white")) time.setGridColor(Color.white);
       if (evt.getItem().equals("grey")) time.setGridColor(Color.gray);
      
    }
  }


  private class BackColor implements ItemListener {
    public void itemStateChanged(ItemEvent evt) {
      
       if (evt.getItem().equals("black")) time.setBackColor(Color.black);
       if (evt.getItem().equals("blue")) time.setBackColor(Color.blue);
       if (evt.getItem().equals("red")) time.setBackColor(Color.red);
       if (evt.getItem().equals("yellow")) time.setBackColor(Color.yellow);
       if (evt.getItem().equals("green")) time.setBackColor(Color.green);
       if (evt.getItem().equals("white")) time.setBackColor(Color.white);
       if (evt.getItem().equals("grey")) time.setBackColor(Color.gray);
      
    }
  }


  public TimeTool(String name,TimeCanvas time,ColorBar color) {
  
     /* This is the control panel */

    this.setTitle(name);
    this.time=time;
    this.color=color;

    pbtn=new Button("Lambda Power");
    vbtn=new Button("Velocity");
    wbtn=new Button("Spectral Width");
  
    pbtn.addActionListener(this);
    vbtn.addActionListener(this);
    wbtn.addActionListener(this);

    rfld=new TextField(12);
    rfld.addActionListener(new RngText());   
    rfld.setText(""+time.getRange());


    cbox=new Checkbox("Mark Key");
    bbox=new Checkbox("Linear Key");  
    gfbox=new Checkbox("Ground Scatter");

    cbox.addItemListener(this);
    bbox.addItemListener(this);
    gfbox.addItemListener(this);
 
    gcol=new Choice();
    gcol.addItem("black");
    gcol.addItem("blue");
    gcol.addItem("red");
    gcol.addItem("yellow");
    gcol.addItem("green");
    gcol.addItem("white");
    gcol.addItem("grey");
    gcol.select("black");  
    gcol.addItemListener(new GridColor());
 

    bcol=new Choice();
    bcol.addItem("black");
    bcol.addItem("blue");
    bcol.addItem("red");
    bcol.addItem("yellow");
    bcol.addItem("green");
    bcol.addItem("white");
    bcol.addItem("grey");
    bcol.select("white");  
    bcol.addItemListener(new BackColor());
 
 
    cbox.setState(false);
    bbox.setState(true);
    gfbox.setState(true);

    Panel a=new Panel();
    a.setLayout(new GridLayout(3,2,0,0));
    a.add(pbtn);
    a.add(new Label("Key Limit:"));
    a.add(vbtn);
    a.add(rfld);
    a.add(wbtn);
   
    Panel b=new Panel();
    b.setLayout(new GridLayout(4,1,0,0));

    b.add(cbox);
    b.add(bbox);
    b.add(new Label("Show:"));
    b.add(gfbox);
    
    Panel c=new Panel();
    c.setLayout(new GridLayout(2,2,0,0));
    c.add(new Label("Grid Colour:"));
    c.add(gcol);
    c.add(new Label("Background Colour:"));
    c.add(bcol);

  
    this.setLayout(new BorderLayout());
    this.add("North",a);
    this.add("Center",b);
    this.add("South",c);

    this.pack();    
  }


  public void itemStateChanged(ItemEvent evt) {
    boolean state=(evt.getStateChange()==ItemEvent.SELECTED); 
    if (evt.getItem().equals("Mark Key")) {
      if (state==true) color.setStep(10);
      else color.setStep(0);
    } else if (evt.getItem().equals("Linear Key")) {
      if (state==false) {
        color.setBxCar(10);
        time.setBxCar(10);
      } else {
        color.setBxCar(0); 
        time.setBxCar(0);   
      }
    } else if (evt.getItem().equals("Ground Scatter")) {
      time.setGflg(state);
    }

  }
 
      


  public void actionPerformed(ActionEvent evt) {
    String arg=evt.getActionCommand();
    if (arg.equals("Lambda Power")) {
      time.setPrm(1);
      color.setPrm(1); 
      rfld.setText(""+time.getRange());
    } else if (arg.equals("Velocity")) {
      time.setPrm(0);
      color.setPrm(0);
      rfld.setText(""+time.getRange());

    } else if (arg.equals("Spectral Width")) {
      time.setPrm(2);
      color.setPrm(2);
      rfld.setText(""+time.getRange());
    }
  }




}
