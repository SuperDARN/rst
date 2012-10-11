/* GeoTool.java
   ============
   Author R.J.Barnes
*/

/*
 $License$
*/





import java.applet.*;
import java.awt.*;
import java.awt.event.*;


public class GeoTool extends CloseFrame implements 
                                           ActionListener,ItemListener {

  private Button pbtn;
  private Button vbtn;
  private Button wbtn;
 
  private TextField rfld;

  private Checkbox cbox;
  private Checkbox bbox;
  private Checkbox gfbox;
  private Checkbox mbox;
  private Checkbox ctbox; 
  private Checkbox fcbox; 


  private CheckboxGroup gridGroup=new CheckboxGroup();
  private Checkbox gridOff;
  private Checkbox grid5;
  private Checkbox grid10;
  private Checkbox grid15;

  private Choice gcol;
  private Choice mcol;
  private Choice ccol;
  private Choice bcol;

  GeoCanvas geo;
  ColorBar color;


  private class RngText implements ActionListener {
    public void actionPerformed(ActionEvent evt) {
      try {
        double rng=(new Double(rfld.getText().trim())).doubleValue();
        if (rng<1) rng=1;
        rfld.setText(""+rng);
        color.setRange(rng);
        geo.setRange(rng);
      } catch (NumberFormatException ex) {
        System.err.println("Invalid entry in field");
      }
    }
  }

  private class GridColor implements ItemListener {
    public void itemStateChanged(ItemEvent evt) {
       if (evt.getItem().equals("black")) geo.setGridColor(Color.black);
       if (evt.getItem().equals("blue")) geo.setGridColor(Color.blue);
       if (evt.getItem().equals("red")) geo.setGridColor(Color.red);
       if (evt.getItem().equals("yellow")) geo.setGridColor(Color.yellow);
       if (evt.getItem().equals("green")) geo.setGridColor(Color.green);
       if (evt.getItem().equals("white")) geo.setGridColor(Color.white);
       if (evt.getItem().equals("grey")) geo.setGridColor(Color.gray);
    }
  }


 private class MeridianColor implements ItemListener {
    public void itemStateChanged(ItemEvent evt) {
       if (evt.getItem().equals("black")) geo.setMeridianColor(Color.black);
       if (evt.getItem().equals("blue")) geo.setMeridianColor(Color.blue);
       if (evt.getItem().equals("red")) geo.setMeridianColor(Color.red);
       if (evt.getItem().equals("yellow")) geo.setMeridianColor(Color.yellow);
       if (evt.getItem().equals("green")) geo.setMeridianColor(Color.green);
       if (evt.getItem().equals("white")) geo.setMeridianColor(Color.white);
       if (evt.getItem().equals("grey")) geo.setMeridianColor(Color.gray);
    }
  }

  private class GlobeColor implements ItemListener {
    public void itemStateChanged(ItemEvent evt) {
       if (evt.getItem().equals("black")) geo.setGlobeColor(Color.black);
       if (evt.getItem().equals("blue")) geo.setGlobeColor(Color.blue);
       if (evt.getItem().equals("red")) geo.setGlobeColor(Color.red);
       if (evt.getItem().equals("yellow")) geo.setGlobeColor(Color.yellow);
       if (evt.getItem().equals("green")) geo.setGlobeColor(Color.green);
       if (evt.getItem().equals("white")) geo.setGlobeColor(Color.white);
       if (evt.getItem().equals("grey")) geo.setGlobeColor(Color.gray);
    }
  }

  private class BackColor implements ItemListener {
    public void itemStateChanged(ItemEvent evt) {
       if (evt.getItem().equals("black")) geo.setBackColor(Color.black);
       if (evt.getItem().equals("blue")) geo.setBackColor(Color.blue);
       if (evt.getItem().equals("red")) geo.setBackColor(Color.red);
       if (evt.getItem().equals("yellow")) geo.setBackColor(Color.yellow);
       if (evt.getItem().equals("green")) geo.setBackColor(Color.green);
       if (evt.getItem().equals("white")) geo.setBackColor(Color.white);
       if (evt.getItem().equals("grey")) geo.setBackColor(Color.gray);
    }
  }




  public GeoTool(String name,GeoCanvas geo,ColorBar color) {
  
     /* This is the control panel */
    this.setTitle(name);
    this.geo=geo;
    this.color=color;

    pbtn=new Button("Lambda Power");
    vbtn=new Button("Velocity");
    wbtn=new Button("Spectral Width");
  
    pbtn.addActionListener(this);
    vbtn.addActionListener(this);
    wbtn.addActionListener(this);

    rfld=new TextField(12);
    rfld.addActionListener(new RngText());   
    rfld.setText(""+geo.getRange());

    cbox=new Checkbox("Mark Key");
    bbox=new Checkbox("Linear Key");  
    gfbox=new Checkbox("Ground Scatter");
    mbox=new Checkbox("Meridian Marks");
    ctbox=new Checkbox("Continents");
    fcbox=new Checkbox("Filled Continents");

    cbox.addItemListener(this);
    bbox.addItemListener(this);
    gfbox.addItemListener(this);
    mbox.addItemListener(this);
    ctbox.addItemListener(this);
    fcbox.addItemListener(this);

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
  
    mcol=new Choice();
    mcol.addItem("black");
    mcol.addItem("blue");
    mcol.addItem("red");
    mcol.addItem("yellow");
    mcol.addItem("green");
    mcol.addItem("white");
    mcol.addItem("grey");
    mcol.select("grey");  
    mcol.addItemListener(new MeridianColor());
  
    ccol=new Choice();
    ccol.addItem("black");
    ccol.addItem("blue");
    ccol.addItem("red");
    ccol.addItem("yellow");
    ccol.addItem("green");
    ccol.addItem("white");
    ccol.addItem("grey");
    ccol.select("grey");  
    ccol.addItemListener(new GlobeColor());
  

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
    mbox.setState(true);
    ctbox.setState(true);
    fcbox.setState(true);


    Panel a=new Panel();
    a.setLayout(new GridLayout(3,2,0,0));
    a.add(pbtn);
    a.add(new Label("Key Limit:"));
    a.add(vbtn);
    a.add(rfld);
    a.add(wbtn);

    Panel b=new Panel();
    b.setLayout(new GridLayout(9,1,0,0));

    b.add(cbox);
    b.add(bbox);
 
    b.add(new Label("Show:"));
    b.add(gfbox);
    b.add(mbox);
    b.add(ctbox);
    b.add(fcbox);
    b.add(new Label("Range Grid:"));
    Panel g=new Panel();
    g.setLayout(new GridLayout(1,4,0,0));
    g.add(gridOff);
    g.add(grid5);
    g.add(grid10);
    g.add(grid15);
    b.add(g);


    Panel c=new Panel();
    c.setLayout(new GridLayout(4,2,0,0));
    c.add(new Label("Grid Colour:"));
    c.add(gcol);
    c.add(new Label("Meridian Colour:"));
    c.add(mcol);
    c.add(new Label("Continent Colour:"));
    c.add(ccol);
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
        geo.setBxCar(10);
      } else {
        color.setBxCar(0); 
        geo.setBxCar(0);   
      }
    } else if (evt.getItem().equals("Ground Scatter")) {
       geo.setGflg(state);
    } else if (evt.getItem().equals("Meridian Marks")) {
       geo.setMeridian(state);
    } else if (evt.getItem().equals("Continents")) {
       geo.setGlobe(state);
    } else if (evt.getItem().equals("Filled Continents")) {
       geo.setfillGlobe(state);
    } else if ((evt.getItem().equals("Off")) && (state==true)) {
       geo.setGrid(0);
    } else if ((evt.getItem().equals("5")) && (state==true)) {
       geo.setGrid(5);
    } else if ((evt.getItem().equals("10")) && (state==true)) {
       geo.setGrid(10);
    } else if ((evt.getItem().equals("15")) && (state==true)) {
       geo.setGrid(15);
    }

  }
 
  public void actionPerformed(ActionEvent evt) {
    String arg=evt.getActionCommand();
    if (arg.equals("Lambda Power")) {
      geo.setPrm(1);
      color.setPrm(1); 
      rfld.setText(""+geo.getRange());
    } else if (arg.equals("Velocity")) {
      geo.setPrm(0);
      color.setPrm(0);
      rfld.setText(""+geo.getRange());

    } else if (arg.equals("Spectral Width")) {
      geo.setPrm(2);
      color.setPrm(2);
      rfld.setText(""+geo.getRange());
    }
  }




}
