/* BaseCanvas.java
   ===============
   Author R.J.Barnes
*/

/*
 $License$
*/




import java.applet.*;
import java.awt.*;

public class BaseCanvas extends Canvas {

  Image img=null;
  Dimension s=new Dimension();

  public BaseCanvas() {
    setSize(400,400);
    s.width=400;
    s.height=400;
  }  

  public BaseCanvas(int width,int height) {
    setSize(width,height);
    s.width=width;
    s.height=height;
  }  

  public void makeImage() {
   Dimension t=getSize();
   img=createImage(t.width,t.height);
   if (img==null) return;
   s.width=t.width;
   s.height=t.height;
   Graphics bg=img.getGraphics();

   if (bg==null) return;
   
    /* this is the region for rendering stuff for the screen */
    bg.dispose();
  }

  public void render() {
    /* dummy method for rendering the display */
  }

  public void update(Graphics g) {
    paint(g);
  }
     
  public void paint(Graphics g) {
    Dimension t=getSize();
    if ((img==null) || (t.width !=s.width) || (t.height !=s.height)) {
      makeImage();
      render();
    }
    if (img !=null) g.drawImage(img,0,0,null);
  }
}



