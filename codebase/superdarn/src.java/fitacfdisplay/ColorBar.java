/* ColorBar.java
   =============
   Author R.J.Barnes
*/

/*
 $License$
*/




import java.applet.*;
import java.awt.*;

public class ColorBar extends Canvas {
  int MAX_WIDTH=6; /* the maximum length of a label String */
  private double prng=30.0;
  private double vrng=1000.0;
  private double wrng=500.0;
 
  Dimension d;
  private int prm=0;
  private int step=0;
  private int bxcar=0;
  private int fwdt;
  private int fhgt;
  private int fasc;
  
  private Font font;

  private void getMetrics() {
    FontMetrics fm=this.getFontMetrics(this.font);
    fhgt=fm.getHeight();
    fasc=fm.getAscent();
    fwdt=MAX_WIDTH*fm.charWidth('X');
  }

  private void drawKey(Graphics g) {
    Dimension r=getSize();
    int h,dh;
 
    int dx=(r.width-d.width-8-fwdt)/2;
    int dy=(r.height-d.height-2*fhgt)/2+fasc;
    dh=d.height/256;
    dh=(dh < 1) ? 1 : dh;
    if (dh>1) {
      for (int i=0;i<256;i++) {
        h=(d.height*i)/256;
        if (prm==0) g.setColor(ColorKey.color(0,(byte) (128-i),bxcar));
        else g.setColor(ColorKey.color(1,(byte) (128-i),bxcar));
        g.fillRect(dx+4,dy+h,d.width,dh);
      }
    } else {
      for (int i=0;i<256;i++) {
        h=(d.height*i)/256;
        if (prm==0) g.setColor(ColorKey.color(0,(byte) (128-i),bxcar));
        else g.setColor(ColorKey.color(1,(byte) (128-i),bxcar));
        g.drawLine(dx+4,dy+h,dx+4+d.width,dy+h);
      }
    }
    
    g.setColor(Color.black);
    g.drawRect(dx+4,dy,d.width,d.height-1);
    if (prm==0) g.drawLine(dx+4,dy+d.height/2,dx+d.width,dy+d.height/2);

    if (step !=0) {
       for (int i=0;i<step;i++) {
         h=i*d.height/step;
         g.drawLine(dx+4,dy+h,dx+d.width+4,dy+h);
       }
    }
    g.setFont(this.font);

    /* draw the labels to the right of the key */
    if (prm==0) {
       g.drawString(Double.toString(vrng),dx+d.width+8,dy);
       g.drawString(Double.toString(-vrng),dx+d.width+8,dy+d.height+fasc);
    } else if (prm==1) {
       g.drawString(Double.toString(prng),dx+d.width+8,dy);
       g.drawString(Double.toString(0),dx+d.width+8,dy+d.height+fasc);
    } else {
       g.drawString(Double.toString(wrng),dx+d.width+8,dy);
       g.drawString(Double.toString(0),dx+d.width+4,dy+d.height+fasc);
    }
  }

  public void addNotify() {
    super.addNotify();
    getMetrics();
    setSize(d.width+fwdt+8,d.height+2*fhgt);

  }

  public void paint(Graphics g) {   
    if (g==null) return;
    this.drawKey(g);
  }

  public ColorBar(int width,int height) {
    d=new Dimension(width,height);
    this.font=new Font("Helvetica",Font.PLAIN,12);
  }

  public void setPrm(int prm) {
    if (prm==this.prm) return;
    this.prm=prm;
    this.repaint();
  } 

  public void setStep(int step) {
    if (step==this.step) return;
    if (step <0) step=0;
    if (step >50) step=50;
    this.step=step;
    this.repaint();
  }

  public void setBxCar(int bxcar) {
    if (bxcar==this.bxcar) return;
    if (bxcar<0) bxcar=0;
    if (bxcar>50) bxcar=50;
    this.bxcar=bxcar;
    this.repaint();
  }

  public void setRange(double rng) {
    if (prm==0) {
      if (rng<10.0) rng=10.0;
      if (rng>5000.0) rng=5000.0;
      vrng=rng;
      this.repaint();
    } else if (prm==1) {
      if (rng<1.0) rng=1.0;
      if (rng>100.0) rng=100.0;
      prng=rng;
      this.repaint();
    } else {
      if (rng<1.0) rng=1.0;
      if (rng>5000.0) rng=5000.0;
      wrng=rng;
      this.repaint();
    } 
  }

}
 






