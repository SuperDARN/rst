/* FFTCanvas.java
   ================
   Author R.J.Barnes
*/


/*
 $License$
*/



import java.applet.*;
import java.awt.*;
import java.awt.event.*;
import java.util.*;

public class FFTCanvas extends BaseCanvas {

  FFTData fftdata=null;
  PlotInfo plt=null;
 

  private Font font;
  FontMetrics fm;
 
  private int fhgt=0;
  private int fasc=0;

  Color majorcolor=Color.gray;
  Color minorcolor=Color.darkGray;
  Color fgcolor=Color.white;
  Color backcolor=Color.black;
  
  private void getMetrics() {
    this.fm=this.getFontMetrics(this.font);
    fhgt=fm.getHeight();
    fasc=fm.getAscent();
  }


  public void addNotify() {
    super.addNotify();
    getMetrics();
    setBackground(backcolor);
  }

  public FFTCanvas() {
    setSize(720,500);
    font=new Font("Helvetica",Font.PLAIN,12);

  }

  public void plotData(FFTData fftdata,PlotInfo plt) {
    int ox,oy,wd,ht;
    int ax,bx;
    double ay,by;
    int x;
    double y;
    String txt=null;
    int twdt;

    this.fftdata=fftdata;
    this.plt=plt;

    Graphics bg=null;
    if (img !=null) bg=img.getGraphics();
    if (bg==null) return;
    bg.setColor(backcolor);
    bg.fillRect(0,0,s.width,s.height);

 
    txt=Double.toString(plt.ymin);
    twdt=fm.stringWidth("10^"+txt);
    txt=Double.toString(plt.ymax);
    x=fm.stringWidth("10^"+txt);
    if (x>twdt) twdt=x;

    ox=twdt+14;
    oy=fhgt+9;
    wd=s.width-2*twdt-28;
    ht=s.height-2*fhgt-18;

  

 

    bg.setColor(minorcolor);
    for (x=plt.xmin;x<=plt.xmax;x+=plt.xminor) {
	ax=wd*(x-plt.xmin)/(plt.xmax-plt.xmin);
	bg.drawLine(ox+ax,oy,ox+ax,oy+ht);
    }
    bg.setColor(majorcolor);
    for (x=plt.xmin;x<=plt.xmax;x+=plt.xmajor) {
        ax=wd*(x-plt.xmin)/(plt.xmax-plt.xmin);
        bg.drawLine(ox+ax,oy,ox+ax,oy+ht);
    }
    for (y=plt.ymin;y<=plt.ymax;y+=plt.ymajor) {
        ay=ht*(y-plt.ymin)/(plt.ymax-plt.ymin);
        bg.drawLine(ox,oy+ht-(int) ay,ox+wd,oy+ht-(int) ay);
    }

    bg.setColor(fgcolor);
    for (x=plt.xmin;x<=plt.xmax;x+=plt.xmajor) {
        ax=wd*(x-plt.xmin)/(plt.xmax-plt.xmin);
        bg.drawLine(ox+ax,oy,ox+ax,oy-2);
        bg.drawLine(ox+ax,oy+ht,ox+ax,oy+ht+2);
        txt=Integer.toString(x);
	twdt=fm.stringWidth(txt);
        bg.drawString(txt,ox+ax-twdt/2,oy+ht+4+fasc);
	bg.drawString(txt,ox+ax-twdt/2,oy-4);
    }
    for (y=plt.ymin;y<=plt.ymax;y+=plt.ymajor) {
        ay=ht*(y-plt.ymin)/(plt.ymax-plt.ymin);
        bg.drawLine(ox,oy+ht-(int) ay,ox-2,oy+ht-(int) ay);
	bg.drawLine(ox+wd,oy+ht-(int) ay,ox+wd+2,oy+ht-(int) ay);
	txt=Double.toString(y);
	twdt=fm.stringWidth("10^"+txt);
        bg.drawString("10^"+txt,ox+wd+4,oy+ht-(int) ay+5);
        bg.drawString("10^"+txt,ox-twdt-4,oy+ht-(int) ay+5);
    }



    /* plot the data */


    if (fftdata.buffer !=null) {  
      for (x=plt.xmin+1;(x<fftdata.nsamp) && (x<plt.xmax);x++) {  
	  ax=wd*(x-1-plt.xmin)/(plt.xmax-plt.xmin);
	  bx=wd*(x-plt.xmin)/(plt.xmax-plt.xmin);
          y=Math.log(fftdata.buffer[x-1])/Math.log(10);
          ay=ht*(y-plt.ymin)/(plt.ymax-plt.ymin);
          y=Math.log(fftdata.buffer[x])/Math.log(10);
          by=ht*(y-plt.ymin)/(plt.ymax-plt.ymin);
          if (ay<0) ay=0;
          if (by<0) by=0;
          if (ay>ht) ay=ht;
          if (by>ht) by=ht;
          bg.drawLine(ox+ax,oy+ht-(int) ay,ox+bx,oy+ht-(int) by);

      }
    }

    bg.setColor(fgcolor);
    bg.drawRect(ox,oy,wd,ht);

    bg.dispose();
    repaint();
  }

 public void render() {
    int ox,oy,wd,ht;
    int ax,bx;
    double ay,by;
    int x;
    double y;
    String txt=null;
    int twdt;

    int b;
    if (img==null) return;
    Graphics bg=img.getGraphics();
    if (bg==null) return;
    if (plt==null) return;
    if (fftdata==null) return;

    bg.setColor(backcolor);
    bg.fillRect(0,0,s.width,s.height);

    txt=Double.toString(plt.ymin);
    twdt=fm.stringWidth("10^"+txt);
    txt=Double.toString(plt.ymax);
    x=fm.stringWidth("10^"+txt);
    if (x>twdt) twdt=x;

    ox=twdt+14;
    oy=fhgt+9;
    wd=s.width-2*twdt-28;
    ht=s.height-2*fhgt-18;

  
    bg.setColor(minorcolor);
    for (x=plt.xmin;x<=plt.xmax;x+=plt.xminor) {
	ax=wd*(x-plt.xmin)/(plt.xmax-plt.xmin);
	bg.drawLine(ox+ax,oy,ox+ax,oy+ht);
    }
    bg.setColor(majorcolor);
    for (x=plt.xmin;x<=plt.xmax;x+=plt.xmajor) {
        ax=wd*(x-plt.xmin)/(plt.xmax-plt.xmin);
        bg.drawLine(ox+ax,oy,ox+ax,oy+ht);
    }
    for (y=plt.ymin;y<=plt.ymax;y+=plt.ymajor) {
        ay=ht*(y-plt.ymin)/(plt.ymax-plt.ymin);
        bg.drawLine(ox,oy+ht-(int) ay,ox+wd,oy+ht-(int) ay);
    }

    bg.setColor(fgcolor);
    for (x=plt.xmin;x<=plt.xmax;x+=plt.xmajor) {
        ax=wd*(x-plt.xmin)/(plt.xmax-plt.xmin);
        bg.drawLine(ox+ax,oy,ox+ax,oy-2);
        bg.drawLine(ox+ax,oy+ht,ox+ax,oy+ht+2);
        txt=Integer.toString(x);
	twdt=fm.stringWidth(txt);
        bg.drawString(txt,ox+ax-twdt/2,oy+ht+4+fasc);
	bg.drawString(txt,ox+ax-twdt/2,oy-4);
    }
    for (y=plt.ymin;y<=plt.ymax;y+=plt.ymajor) {
        ay=ht*(y-plt.ymin)/(plt.ymax-plt.ymin);
        bg.drawLine(ox,oy+ht-(int) ay,ox-2,oy+ht-(int) ay);
	bg.drawLine(ox+wd,oy+ht-(int) ay,ox+wd+2,oy+ht-(int) ay);
	txt=Double.toString(y);
	twdt=fm.stringWidth("10^"+txt);
        bg.drawString("10^"+txt,ox+wd+4,oy+ht-(int) ay+5);
        bg.drawString("10^"+txt,ox-twdt-4,oy+ht-(int) ay+5);
    }



    /* plot the data */


    if (fftdata.buffer !=null) {  
      for (x=plt.xmin+1;(x<fftdata.nsamp) && (x<plt.xmax);x++) {  
	  ax=wd*(x-1-plt.xmin)/(plt.xmax-plt.xmin);
	  bx=wd*(x-plt.xmin)/(plt.xmax-plt.xmin);
          y=Math.log(fftdata.buffer[x-1])/Math.log(10);
          ay=ht*(y-plt.ymin)/(plt.ymax-plt.ymin);
          y=Math.log(fftdata.buffer[x])/Math.log(10);
          by=ht*(y-plt.ymin)/(plt.ymax-plt.ymin);
          if (ay<0) ay=0;
          if (by<0) by=0;
          if (ay>ht) ay=ht;
          if (by>ht) by=ht;
          bg.drawLine(ox+ax,oy+ht-(int) ay,ox+bx,oy+ht-(int) by);

      }
    }

    bg.setColor(fgcolor);
    bg.drawRect(ox,oy,wd,ht);

    bg.dispose();


 }
  

}

