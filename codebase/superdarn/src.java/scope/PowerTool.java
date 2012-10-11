/* PowerTool.java
   ==============
   Author: R.J.Barnes
*/

/*
 $License$
*/



import java.applet.*;
import java.awt.*;
import java.awt.event.*;


public class PowerTool extends CloseFrame {

    private TextField xmin;
    private TextField xmax;
    private TextField ymax;
    PowerCanvas power;
    PlotInfo info;


    private class TextHandler implements ActionListener {
	public void actionPerformed(ActionEvent evt) {
	    try {
		int txmin=(new Integer(xmin.getText().trim())).intValue();
	     	int txmax=(new Integer(xmax.getText().trim())).intValue();
                int tymax=(new Integer(ymax.getText().trim())).intValue();
		
                if (txmin<0) txmin=0;
                if (txmin>1000) txmin=1000;
                if (txmin>info.xmax) txmin=info.xmax-1;

		if (txmax<0) txmin=0;
                if (txmax>1000) txmax=1000;
                if (txmax<info.xmin) txmax=info.xmin+1;

		if (tymax<0) tymax=0;
                if (tymax>1000) tymax=1000;


                info.xmin=txmin;
		info.xmax=txmax;
		info.ymin=0;
                info.ymax=tymax;

                info.xmajor=(info.xmax-info.xmin)/info.xstep;
                if (info.xmajor==0) info.xmajor=1;
		info.xminor=(info.xmax-info.xmin)/(5*info.xstep);
                if (info.xminor==0) info.xminor=info.xmajor;

	
	    } catch (NumberFormatException ex) {
		System.err.println("Invalid entry in field");
	    }
	    xmin.setText(Integer.toString(info.xmin));
	    xmax.setText(Integer.toString(info.xmax));
            ymax.setText(Integer.toString(info.ymax));
            power.render();
            power.repaint();
 

	}
    }


    public PowerTool(String name,PowerCanvas power,PlotInfo info) {
	this.setTitle(name);
	this.info=info;
        this.power=power;
	TextHandler textHandler=new TextHandler();


	xmin=new TextField(6);
	xmax=new TextField(6);
        ymax=new TextField(6);

	xmin.setText(Integer.toString(info.xmin));
	xmax.setText(Integer.toString(info.xmax));
        ymax.setText(Integer.toString(info.ymax));

	xmin.addActionListener(textHandler);
	xmax.addActionListener(textHandler);
        ymax.addActionListener(textHandler);


	Panel a=new Panel();
	a.setLayout(new GridLayout(3,2,0,0));
        a.add(new Label("X min:"));
        a.add(xmin);
	a.add(new Label("X max:"));
        a.add(xmax);
        a.add(new Label("Y max:"));
        a.add(ymax);

        this.add("Center",a);
	this.pack();

    }
}
