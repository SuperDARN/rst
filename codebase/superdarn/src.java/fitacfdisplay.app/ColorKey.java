/* ColorKey.java
   =============
  Author R.J.Barnes
*/

/*
 $License$
*/




import java.lang.*;
import java.awt.*;

public class ColorKey { /* the palette object */
  static private Color map[][]=new Color[2][256];

  static { /* build the palette table */
    int r,g,b;
    int c;
    int i;
   
    /* This is for the -ve to +ve and the 0 to +ve color scale */ 

    for (i=0;i<64;i++) { /* green-yellow, blue-cyan */
      r=i*4;
      b=0;
      g=255;
      map[0][i]=new Color(r,g,b);
      b=255;
      g=i*4;
      r=0;
      map[1][i]=new Color(r,g,b);
    }
    for (i=64;i<128;i++) { /* yellow-red, cyan-green */ 
      r=255; 
      b=0;
      g=255-4*(i-64);
      map[0][i]=new Color(r,g,b);
      b=255-4*(i-64);
      g=255;
      r=0;
      map[1][i]=new Color(r,g,b);
    }   
    for (i=128;i<192;i++) { /* blue-cyan, green yellow */ 
      r=0; 
      b=255;
      g=4*(i-128);
      map[0][i]=new Color(r,g,b);
      b=0;
      g=255;
      r=4*(i-128);
      map[1][i]=new Color(r,g,b);
    } 
    for (i=192;i<256;i++) { /* blue-cyan, yellow to red */
      r=4*(i-192); 
      b=255;
      g=255;
      map[0][i]=new Color(r,g,b);
      b=0;
      g=255-(i-192)*4;
      r=255;
      map[1][i]=new Color(r,g,b);
    }     
  }      

  static public Color color(int set,byte index,int bxcar) {
    if ((set<0) || (set>1)) return Color.black;
    if (bxcar==0) return map[set][128+index];
    int i=bxcar*(128+index)/256;
     
    return map[set][(256*i)/bxcar]; 
  }
}


