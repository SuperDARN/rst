/*FanTool.java
  Author R.J.Barnes
  =================*/

/*
 $License$
*/




import java.applet.*;
import java.awt.*;
import java.awt.event.*;


public class FanTool extends CloseFrame implements 
                                          ActionListener,ItemListener {

  private Button pbtn;
  private Button vbtn;
  private Button wbtn;
 
  private TextField rfld;

  private Checkbox cbox;
  private Checkbox bbox;
  private Checkbox gfbox;
 
  private CheckboxGroup gridGroup=new CheckboxGroup();
  private Checkbox gridOff;
  private Checkbox grid5;
  private Checkbox grid10;
  private Checkbox grid15;

  private Choice gcol;
  private Choice bcol;

  FanCanvas fan;
  ColorBar color;

  private class RngText implements ActionListener {
    public void actionPerformed(ActionEvent evt) {
      try {
        double rng=(new Double(rfld.getText().trim())).doubleValue();
        if (rng<1) rng=1;
        rfld.setText(""+rng);
        color.setRange(rng);
        fan.setRange(rng);
      } catch (NumberFormatException ex) {
        System.err.println("Invalid entry in field");
      }
    }
  }

  private class GridColor implements ItemListener {
    public void itemStateChanged(ItemEvent evt) {
       if (evt.getItem().equals("black")) fan.setGridColor(Color.black);
       if (evt.getItem().equals("blue")) fan.setGridColor(Color.blue);
       if (evt.getItem().equals("red")) fan.setGridColor(Color.red);
       if (evt.getItem().equals("yellow")) fan.setGridColor(Color.yellow);
       if (evt.getItem().equals("green")) fan.setGridColor(Color.green);
       if (evt.getItem().equals("white")) fan.setGridColor(Color.white);
       if (evt.getItem().equals("grey")) fan.setGridColor(Color.gray);
    }
  }

  private class BackColor implements ItemListener {
    public void itemStateChanged(ItemEvent evt) {
       if (evt.getItem().equals("black")) fan.setBackColor(Color.black);
       if (evt.getItem().equals("blue")) fan.setBackColor(Color.blue);
       if (evt.getItem().equals("red")) fan.setBackColor(Color.red);
       if (evt.getItem().equals("yellow")) fan.setBackColor(Color.yellow);
       if (evt.getItem().equals("green")) fan.setBackColor(Color.green);
       if (evt.getItem().equals("white")) fan.setBackColor(Color.white);
       if (evt.getItem().equals("grey")) fan.setBackColor(Color.gray);
    }
  }


  public FanTool(String name,FanCanvas fan,ColorBar color) {
  
     /* This is the control panel */
    this.setTitle(name);
    this.fan=fan;
    this.color=color;

    pbtn=new Button("Lambda Power");
    vbtn=new Button("Velocity");
    wbtn=new Button("Spectral Width");
  
    pbtn.addActionListener(this);
    vbtn.addActionListener(this);
    wbtn.addActionListener(this);

    rfld=new TextField(6);
    rfld.addActionListener(new RngText());   
    rfld.setText(""+fan.getRange());


    cbox=new Checkbox("Mark Key");
    bbox=new Checkbox("Linear Key");  
    gfbox=new Checkbox("Ground Scatter");

    cbox.addItemListener(this);
    bbox.addItemListener(this);
    gfbox.addItemListener(this);

    gridOff=new Checkbox("Off",gridGroup,true);
    grid5=new Checkbox("5",gridGroup,false);
    grid10=new Checkbox("10",gridGroup,false);
    grid15=new Checkbox("15",gridGroup,false);

    gridOff.addItemListener(this);
    grid5.addItemListener(this);
    grid10.addItemListener(this);
    grid15.addItemListener(this);
 
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
    b.setLayout(new GridLayout(6,1,0,0));

    b.add(cbox);
    b.add(bbox);
 
    b.add(new Label("Show:"));
    b.add(gfbox);
    b.add(new Label("Range Grid:"));
    Panel g=new Panel();
    g.setLayout(new GridLayout(1,4,0,0));
    g.add(gridOff);
    g.add(grid5);
    g.add(grid10);
    g.add(grid15);
    b.add(g);

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
        fan.setBxCar(10);
      } else {
        color.setBxCar(0); 
        fan.setBxCar(0);   
      }
    } else if (evt.getItem().equals("Ground Scatter")) {
       fan.setGflg(state);
    } else if ((evt.getItem().equals("Off")) && (state==true)) {
       fan.setGrid(0);
    } else if ((evt.getItem().equals("5")) && (state==true)) {
       fan.setGrid(5);
    } else if ((evt.getItem().equals("10")) && (state==true)) {
       fan.setGrid(10);
    } else if ((evt.getItem().equals("15")) && (state==true)) {
       fan.setGrid(15);
    }

  }
 
  public void actionPerformed(ActionEvent evt) {
    String arg=evt.getActionCommand();
    if (arg.equals("Lambda Power")) {
      fan.setPrm(1);
      color.setPrm(1); 
      rfld.setText(""+fan.getRange());
    } else if (arg.equals("Velocity")) {
      fan.setPrm(0);
      color.setPrm(0);
      rfld.setText(""+fan.getRange());

    } else if (arg.equals("Spectral Width")) {
      fan.setPrm(2);
      color.setPrm(2);
      rfld.setText(""+fan.getRange());
    }
  }




}
