/* CloseFrame.java
   =============== 
   Author: R.J.Barnes
*/


/*
 $License$
*/




import java.awt.*;
import java.awt.event.*;

public class CloseFrame extends Frame {
  
  private class WindowCtrl extends WindowAdapter {
    public void windowClosing(WindowEvent evt) {
     setVisible(false);
    }
  }

  public CloseFrame() {
     setBackground(Color.lightGray);
     addWindowListener(new WindowCtrl());
  }

}

   

