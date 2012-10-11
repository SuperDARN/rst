/* ScopeTool.java
   ==============
   Author: R.J.Barnes
*/

/*
 $License$
*/



import java.applet.*;
import java.awt.*;
import java.awt.event.*;


public class ScopeTool extends CloseFrame {

    private TextField xmin;
    private TextField xmax;
    private TextField ymax;

    ScopeCanvas scope;
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

		if (tymax<1) tymax=1;
                if (tymax>10000) tymax=10000;


                info.xmin=txmin;
		info.xmax=txmax;
		info.ymin=-tymax;
                info.ymax=tymax;

                info.xmajor=(info.xmax-info.xmin)/info.xstep;
                if (info.xmajor==0) info.xmajor=1;
		info.xminor=(info.xmax-info.xmin)/(5*info.xstep);
                if (info.xminor==0) info.xminor=info.xmajor;

		info.ymajor=(info.ymax-info.ymin)/info.ystep;
                if (info.ymajor==0) info.ymajor=1;
                info.yminor=(info.ymax-info.ymin)/(5*info.ystep);
                if (info.yminor==0) info.yminor=info.ymajor;

	    } catch (NumberFormatException ex) {
		System.err.println("Invalid entry in field");
	    }
	    xmin.setText(Integer.toString(info.xmin));
	    xmax.setText(Integer.toString(info.xmax));
            ymax.setText(Integer.toString(info.ymax));
            scope.render();
            scope.repaint();

	}
    }


    public ScopeTool(String name,ScopeCanvas scope,PlotInfo info) {
	this.setTitle(name);
	this.scope=scope;
	this.info=info;

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
