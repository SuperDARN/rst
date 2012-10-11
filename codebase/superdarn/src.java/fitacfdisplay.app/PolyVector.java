/* PolyVector.java
   ===============
   Author R.J.Barnes
*/

/*
 $License$
*/




import java.util.*;

public class PolyVector {
  public boolean active=true;

  public Vector info;
  public Vector data;


 public PolyVector() {
    info=new Vector(10,10);
    data=new Vector(10,10);
  }

  public PolyVector(int size) {
    info=new Vector(size,10);
    data=new Vector(size,10);
  }

  public void truncate() {
    /* remove the last polygon from the stack */
    int i;
    if (this.info.size()==0) return;

    int num;
    PolyInfo info;
    num=this.info.size();
    info=(PolyInfo) this.info.elementAt(num-1);  
    
    /* remove objects redundant vectors */
    for (i=this.data.size();i>=info.off;i--) this.data.removeElementAt(i);
    info.num=0;
  }      



  public void addVector(int flg,Object object) { 
    int num;
    PolyInfo info;

    if (active==false) return;
    if (flg !=0) {
      /* create a new polygon */
      int off;
      off=this.data.size();
      this.info.addElement(new PolyInfo(flg-1,off));
    }


    num=this.info.size();
    info=(PolyInfo) this.info.elementAt(num-1);
    info.num++;
    this.data.addElement(object);
  
  }



  public void reset() {
    this.active=true;
    this.info.removeAllElements();
    this.data.removeAllElements();
  
  }

  public synchronized void complete() {
    this.active=false;
    this.notify();
  }

  public synchronized void poll() {
    try {
      this.wait(); 
    } catch (InterruptedException e) {
      System.err.println("PolyVector poll interrupted");
    }
  }

}






