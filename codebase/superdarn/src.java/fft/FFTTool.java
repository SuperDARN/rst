/* FFTTool.java
   ==============
   Author: R.J.Barnes
*/

/*
 $License$
*/



import java.applet.*;
import java.awt.*;
import java.awt.event.*;


public class FFTTool extends CloseFrame {

    private TextField xmin;
    private TextField xmax;
    private TextField ymin;
    private TextField ymax;

    FFTCanvas fft;
    PlotInfo info;


    private class TextHandler implements ActionListener {
	public void actionPerformed(ActionEvent evt) {
	    try {
		int txmin=(new Integer(xmin.getText().trim())).intValue();
	     	int txmax=(new Integer(xmax.getText().trim())).intValue();
		double tymin=(new Double(ymin.getText().trim())).floatValue();
                double tymax=(new Double(ymax.getText().trim())).floatValue();
		
                if (txmin<0) txmin=0;
                if (txmin>1000) txmin=1000;
                if (txmin>info.xmax) txmin=info.xmax-1;

		if (txmax<0) txmin=0;
                if (txmax>1000) txmax=1000;
                if (txmax<info.xmin) txmax=info.xmin+1;

		if (tymin<-10000) tymin=-10000;
                if (tymin>10000) tymin=10000;
                if (tymin>info.ymax) tymin=info.ymax-1;

		if (tymax<-10000) tymax=-10000;
                if (tymax>10000) tymax=10000;
                if (tymax<info.ymin) tymax=info.ymin+1;

                info.xmin=txmin;
		info.xmax=txmax;
		info.ymin=tymin;
                info.ymax=tymax;

                info.xmajor=(info.xmax-info.xmin)/info.xstep;
                if (info.xmajor==0) info.xmajor=1;
		info.xminor=(info.xmax-info.xmin)/(4*info.xstep);
                if (info.xminor==0) info.xminor=info.xmajor;

	
           

	    } catch (NumberFormatException ex) {
		System.err.println("Invalid entry in field");
	    }
            fft.render();
            fft.repaint();
	    xmin.setText(Integer.toString(info.xmin));
	    xmax.setText(Integer.toString(info.xmax));
	    ymin.setText(Double.toString(info.ymin));
            ymax.setText(Double.toString(info.ymax));


	}
    }


    public FFTTool(String name,FFTCanvas fft,PlotInfo info) {
	this.setTitle(name);
	this.fft=fft;
	this.info=info;

	TextHandler textHandler=new TextHandler();


	xmin=new TextField(6);
	xmax=new TextField(6);
	ymin=new TextField(6);
        ymax=new TextField(6);

	xmin.setText(Integer.toString(info.xmin));
	xmax.setText(Integer.toString(info.xmax));
	ymin.setText(Double.toString(info.ymin));
        ymax.setText(Double.toString(info.ymax));

	xmin.addActionListener(textHandler);
	xmax.addActionListener(textHandler);
	ymin.addActionListener(textHandler);
        ymax.addActionListener(textHandler);


	Panel a=new Panel();
	a.setLayout(new GridLayout(4,2,0,0));
        a.add(new Label("X min:"));
        a.add(xmin);
	a.add(new Label("X max:"));
        a.add(xmax);
	a.add(new Label("Y min:"));
        a.add(ymin);
        a.add(new Label("Y max:"));
        a.add(ymax);

        this.add("Center",a);
	this.pack();

    }
}
