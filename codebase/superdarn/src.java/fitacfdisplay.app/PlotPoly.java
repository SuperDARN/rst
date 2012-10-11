/* PlotPoly.java
   =============
   Author R.J.Barnes
*/

/*
 $License$
*/




import java.awt.*;
import java.util.*;

public class PlotPoly extends PolyVector {
   public PolyVector root;

  public PlotPoly(PolyVector root) {
    this.root=root;
  }

  void fill(Graphics g,Rectangle bb,int xoff,int yoff,int width,int height,
            int type) {
    int num;
    int i,j;   
    Cart c;
    int x,y;
    PolyInfo info;
    Polygon p;
    num=this.info.size();
 
    for (i=0;i<num;i++) {
      info=(PolyInfo) this.info.elementAt(i);
      if (info.type !=type) continue;
      p=new Polygon();
      for (j=0;j<info.num;j++) {
        c=(Cart) this.data.elementAt(info.off+j);
        x=(int) (c.x*width)-xoff;
        y=(int) (c.y*height)-yoff;
        p.addPoint(x,y);
      }
      g.fillPolygon(p);
    }
  }

 void draw(Graphics g,Rectangle bb,int xoff,int yoff,int width,int height,
            int type) {
    int num;
    int i,j;   
    Cart c;
    int x,y;
    PolyInfo info;
    Polygon p;
    num=this.info.size();
 
    for (i=0;i<num;i++) {
      info=(PolyInfo) this.info.elementAt(i);
      if (info.type !=type) continue;
      p=new Polygon();
      for (j=0;j<info.num;j++) {
        c=(Cart) this.data.elementAt(info.off+j);
        x=(int) (c.x*width)-xoff;
        y=(int) (c.y*height)-yoff;
        p.addPoint(x,y);
      }
      g.drawPolygon(p);
    }
  }

  public void clip(PolyVector clip) {
    

    /* clips the PolyVector */
    float dx,dy,dt;
    float ma,mb,mc,md;
    float determ;
    float ia,ib,ic,id;
    float sx,sy,tx,ty;
    float kl,jl;
    boolean wp,wq;

    PolyInfo cinfo;
    PolyInfo info;
    int m,r,s,n,j,p,q;
    int flg;
    Cart a,b,pp,qp,c;
    PolyVector tmp;

    int i;
    if (this.active==true) {
      /* System.err.println("Waiting for plot polygon array to complete"); */
      this.poll();
    }
    /* System.err.println("recalculating clipped polygon.."); */


    cinfo=(PolyInfo) clip.info.elementAt(0);
    for (m=1;m<=cinfo.num;m++) {
      tmp=new PolyVector();
      r=m-1;
      s=(m==cinfo.num) ? 0 : m;
      a=(Cart) clip.data.elementAt(r);
      b=(Cart) clip.data.elementAt(s);     
      
      dx=b.x-a.x;
      dy=b.y-a.y;

      dt=(float) Math.sqrt(dx*dx+dy*dy);
      ma=dx/dt;
      mb=-dy/dt;
      mc=dy/dt;
      md=dx/dt;

      determ=(float) 1.0/(ma*md-mb*mc);
    
      ia=md/determ;
      ib=-mb/determ;
      ic=-mc/determ;
      id=ma/determ;

   
      
      for(j=0;j<this.info.size();j++) {
      
         info=(PolyInfo) this.info.elementAt(j);
         flg=info.type+1;
         if (info.num<3) continue;
         for (n=1;n<=info.num;n++) {
          
           p=n-1;
           q=(n==info.num) ? 0 : n;

           pp=(Cart) this.data.elementAt(info.off+p);
           qp=(Cart) this.data.elementAt(info.off+q);
           ty=ic*(pp.x-a.x)+id*(pp.y-a.y);
           wp=(ty>0);
           ty=ic*(qp.x-a.x)+id*(qp.y-a.y);
           wq=(ty>0);
           if (wp && wq) {
	     if (p==0) {
               tmp.addVector(flg,new Cart(pp));
               flg=0;
             }
             if (q !=0) {
               tmp.addVector(flg,new Cart(qp));
               flg=0;
             }
	   } else if (wp !=wq) {
	     if ((wp) && (p==0)) {
               tmp.addVector(flg,new Cart(pp));
               flg=0;
	     }

             sx=ia*(pp.x-a.x)+ib*(pp.y-a.y);
             sy=ic*(pp.x-a.x)+id*(pp.y-a.y);
             tx=ia*(qp.x-a.x)+ib*(qp.y-a.y);
             ty=ic*(qp.x-a.x)+id*(qp.y-a.y);

             if (tx !=sx) {
               kl=(ty-sy)/(tx-sx);
               jl=sy-sx*kl;
               c=new Cart(ma*-jl/kl+a.x,mc*-jl/kl+a.y);
	     } else c=new Cart(ma*sx+a.x,mc*sx+a.y);
               
             tmp.addVector(flg,c);
             flg=0;
             
             if ((wq) && (q !=0)) {
               tmp.addVector(flg,new Cart(qp));
               flg=0;
	     }

	   }


           
         }
         /* truncate useless polygons here */ 
         if (flg==0) {
           info=(PolyInfo) tmp.info.elementAt(tmp.info.size()-1);
           if (info.num<3) tmp.truncate();
         }
      }
      
      this.info=tmp.info;
      this.data=tmp.data;
    
    }
    /* System.err.println("recalculation clipped polygon complete."); */

  }

  public void convert(Transform convert) {
    int i;
    if (root.active==true) {
      /* System.err.println("Waiting for polygon array to fill"); */
      root.poll();
    }
    /* System.err.println("recalculating plot polygon vector.."); */
    convert.run(this);
    /* System.err.println("recalculation plot polygon complete."); */
  }

  

}

