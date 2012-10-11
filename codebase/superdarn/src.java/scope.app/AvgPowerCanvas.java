/* AvgPowerCanvas.java
   ===================
   Author: R.J.Barnes
*/


/*
 $License$
*/



import java.applet.*;
import java.awt.*;
import java.awt.event.*;
import java.util.*;

public class AvgPowerCanvas extends BaseCanvas {

  PlotInfo plt=null;
  ScopeData scopedata=null;

  private Font font;
  FontMetrics fm;
 
  private int fhgt=0;
  private int fasc=0;

  Color pwrcolor=Color.white;


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

  public AvgPowerCanvas() {
    setSize(720,500);
    font=new Font("Helvetica",Font.PLAIN,12);

  }

  public void plotData(ScopeData scopedata,PlotInfo plt) {
    int ox,oy,wd,ht;
    int ax,bx;
    double ay,by;
    double apwr,bpwr;
    int x,y;
    String txt=null;
    int twdt;

    this.scopedata=scopedata;
    this.plt=plt;


    Graphics bg=null;
    if (img !=null) bg=img.getGraphics();
    if (bg==null) return;
    bg.setColor(backcolor);
    bg.fillRect(0,0,s.width,s.height);

    txt=Integer.toString(plt.ymin);
    twdt=fm.stringWidth("10^"+txt);
    txt=Integer.toString(plt.ymax);
    x=fm.stringWidth("10^"+txt);
    if (x>twdt) twdt=x;

    ox=twdt+14;
    oy=fhgt+9;
    wd=s.width-2*twdt-28;
    ht=s.height-2*fhgt-18;


    /* plot the grid */

    bg.setColor(minorcolor);
    for (x=plt.xmin;x<=plt.xmax;x+=plt.xminor) {
	ax=wd*(x-plt.xmin)/(plt.xmax-plt.xmin);
	bg.drawLine(ox+ax,oy,ox+ax,oy+ht);
    }
    for (y=plt.ymin;y<=plt.ymax;y+=plt.yminor) {
        ay=ht*(y-plt.ymin)/(plt.ymax-plt.ymin);
        bg.drawLine(ox,oy+ht-(int) ay,ox+wd,oy+ht-(int) ay);
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
	txt=Integer.toString(y);
	twdt=fm.stringWidth("10^"+txt);
        bg.drawString("10^"+txt,ox+wd+4,oy+ht-(int) ay+5);
        bg.drawString("10^"+txt,ox-twdt-4,oy+ht-(int) ay+5);
    }



    /* plot the data */


    if (scopedata.buffer !=null) {  
      for (x=plt.xmin+1;(x<scopedata.nsamp) && (x<plt.xmax);x++) {  
	  ax=wd*(x-1-plt.xmin)/(plt.xmax-plt.xmin);
	  bx=wd*(x-plt.xmin)/(plt.xmax-plt.xmin);
          apwr=scopedata.pwr[x-1];
          bpwr=scopedata.pwr[x];

          apwr=Math.log(apwr)/Math.log(10);
          bpwr=Math.log(bpwr)/Math.log(10);
          ay=ht*(apwr-plt.ymin)/(plt.ymax-plt.ymin);
          by=ht*(bpwr-plt.ymin)/(plt.ymax-plt.ymin);
          if (ay<0) ay=0;
          if (by<0) by=0;
          if (ay>ht) ay=ht;
          if (by>ht) by=ht;

          bg.setColor(pwrcolor);
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
    double apwr,bpwr;
    int x,y;
    String txt=null;
    int twdt;

    if (img==null) return;
    Graphics bg=img.getGraphics();
    if (bg==null) return;
    if (plt==null) return;
    if (scopedata==null) return;

    bg.setColor(backcolor);
    bg.fillRect(0,0,s.width,s.height);

    txt=Integer.toString(plt.ymin);
    twdt=fm.stringWidth("10^"+txt);
    txt=Integer.toString(plt.ymax);
    x=fm.stringWidth("10^"+txt);
    if (x>twdt) twdt=x;

    ox=twdt+14;
    oy=fhgt+9;
    wd=s.width-2*twdt-28;
    ht=s.height-2*fhgt-18;

    /* plot the grid */

    bg.setColor(minorcolor);
    for (x=plt.xmin;x<=plt.xmax;x+=plt.xminor) {
	ax=wd*(x-plt.xmin)/(plt.xmax-plt.xmin);
	bg.drawLine(ox+ax,oy,ox+ax,oy+ht);
    }
    for (y=plt.ymin;y<=plt.ymax;y+=plt.yminor) {
        ay=ht*(y-plt.ymin)/(plt.ymax-plt.ymin);
        bg.drawLine(ox,oy+ht-(int) ay,ox+wd,oy+ht-(int) ay);
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
	txt=Integer.toString(y);
	twdt=fm.stringWidth("10^"+txt);
        bg.drawString("10^"+txt,ox+wd+4,oy+ht-(int) ay+5);
        bg.drawString("10^"+txt,ox-twdt-4,oy+ht-(int) ay+5);
    }



    /* plot the data */


    if (scopedata.buffer !=null) {  
      for (x=plt.xmin+1;(x<scopedata.nsamp) && (x<plt.xmax);x++) {  
	  ax=wd*(x-1-plt.xmin)/(plt.xmax-plt.xmin);
	  bx=wd*(x-plt.xmin)/(plt.xmax-plt.xmin);
          apwr=scopedata.pwr[x-1];
          bpwr=scopedata.pwr[x];
          apwr=Math.log(apwr)/Math.log(10);
          bpwr=Math.log(bpwr)/Math.log(10);
          ay=ht*(apwr-plt.ymin)/(plt.ymax-plt.ymin);
          by=ht*(bpwr-plt.ymin)/(plt.ymax-plt.ymin);
          if (ay<0) ay=0;
          if (by<0) by=0;
          if (ay>ht) ay=ht;
          if (by>ht) by=ht;

          bg.setColor(pwrcolor);
          bg.drawLine(ox+ax,oy+ht-(int) ay,ox+bx,oy+ht-(int) by);
      }
    }

    bg.setColor(fgcolor);
    bg.drawRect(ox,oy,wd,ht);

    bg.dispose();


  }


}

