/* BitArray.java  
   ============= 
   Author:R.J.Barnes
*/

/*
  $License$
*/





import java.applet.*;
import java.awt.*;

public class BitArray extends Canvas {
  int HEIGHT=10;
  int WIDTH=10;
  int bits;
  Color bgcolor=Color.lightGray;
  Color fgcolor=Color.red;
  boolean bit[];
  
  private void drawBit(Graphics g) {
    Rectangle r=this.getBounds();
    int dx=(r.width-WIDTH*bits)/2;
    int dy=(r.height-HEIGHT)/2;
    for (int b=0;b<bits;b++) {
      if (bit[b]==false) g.setColor(bgcolor);
       else g.setColor(fgcolor);
      g.fillOval(2+b*WIDTH+dx,2+dy,6,6);
      g.setColor(Color.black);
      g.drawOval(2+b*WIDTH+dx,2+dy,6,6);
    }
  }

  public void paint(Graphics g) {   
    if (g==null) return;
    this.drawBit(g);
  }

  public BitArray(int bits,Color bgcolor,Color fgcolor) {
    this.bits=bits;
    this.bit=new boolean[bits];
    this.bgcolor=bgcolor;
    this.fgcolor=fgcolor;
    this.setSize(WIDTH*bits,HEIGHT);
  }

  public void setBit(int b,boolean state) {
    bit[b]=state;
  }

 
}
 






