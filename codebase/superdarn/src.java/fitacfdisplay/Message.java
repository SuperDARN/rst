/* Message.java
   ============ 
   Author: R.J.Barnes
*/

/*
 $License$
*/



import java.lang.*;
import java.io.*;

/* Class that handles the IPC over the socket stream */
public class Message {

   private byte buffer[];
   public int ErrNum;
   public static final int IO_ERR=0x00;
   public static final int OUT_OF_SYNC=0x01;
      
   private static byte msg_id[]={0x49,0x08,0x1e,0x00}; 
   private byte sze_buf[]=new byte[4];
   
   public byte[] read(InputStream in) {
      
     long cnt=0;
     long num;
     int idbyte;
     long size=0;
     ErrNum=0;
   
     try {

       while (cnt<4) {
         idbyte=in.read();
         if (idbyte<0) break;
         if (idbyte != msg_id[ (int) cnt]) {
           ErrNum=OUT_OF_SYNC;
           break;
         }
         cnt++;
       }
       if (cnt <4) return null;
       cnt=0;
       while (cnt<4) {
         num=in.read(sze_buf,(int) cnt, (int) (4-cnt));
         if (num < 0) break;
         cnt+=num;       
       }
       if (cnt <4) return null;
       size=MathConvert.lng(sze_buf);
       cnt=0;

       buffer=new byte[(int) size];
       while (cnt<size) {
         num=in.read(buffer,(int) cnt,(int) (size-cnt));
         if (num<0) break;
         cnt+=num;
       }
       if (cnt<size) return null;
       return buffer;
     } catch (IOException e) {
          return null;
     }
  }


  public long write(OutputStream out,byte buffer[],long size) {
     
    try {
      out.write(msg_id);
      sze_buf[0]=(byte) (size & 0xff);
      sze_buf[1]=(byte) ((size & 0xff00)>>8);
      sze_buf[2]=(byte) ((size & 0xff0000)>>16);
      sze_buf[3]=(byte) ((size & 0xff000000)>>24);
      out.write(sze_buf);
      out.write(buffer,0,(int) size);
      out.flush();
      return size;
    } catch (IOException e) {
        return -1;
    }
  }
}


