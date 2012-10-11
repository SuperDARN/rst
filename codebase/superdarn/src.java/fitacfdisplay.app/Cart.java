/* Cart.java 
   =========
   Author R.J.Barnes
*/

/*
 $License$
*/




import java.awt.*;
import java.lang.*;

public class Cart{

  float x,y;

  public Cart() {
    this.x=0;
    this.y=0;
  }

  public Cart(Cart p) {
    this.x=p.x;
    this.y=p.y;
  }

  public Cart(double x,double y) {
    this.x=(float) x;
    this.y=(float) y;
  }

}


