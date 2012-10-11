/* ReadBlock.java
   ==============
   Author R.J.Barnes
*/

/*
 $License$
*/





import java.lang.*;
import java.io.*;
import java.net.*;
import java.util.*;

public class ReadBlock {

  private URL url; 
  private String file;
  private byte sze_buf[]=new byte[4];
  private byte id_buf[]=new byte[4];
  private byte memBuf[]=null;
  private InputStream in=null;
  public byte buffer[]=null;

  public long index[]=new long[80000];
  public int id[]=new int[80000];
  public int size[]=new int[80000];
 
  public int max=0;
  public int last=0;

  public ReadBlock(URL url) {
    this.url=url;
    this.max=0;
    this.last=0;
  }


  public ReadBlock(String file) {
    this.file=file;
    this.max=0;
    this.last=0;
  }


  private void storeFile(int fsze) { /* store in memory */
    int cnt=0;
    int num,i;
    int memPtr=0;
    int size;
    int id;   
    InputStream inx=null;

    System.err.println("Loading file into memory");
    this.max=0;
   
    try {
      if (url !=null) inx=url.openStream(); 
      else inx=new FileInputStream(file);     
      while (true) {
        cnt=0;
        while (cnt<fsze) {
          num=inx.read(memBuf,(int) cnt,(int) (fsze-cnt));
          if (num<0) break;
          cnt+=num;
        }
        if (cnt<fsze) break;
      }
      inx.close();
      inx=null;
      System.err.println("Completed Loading file.");
    } catch (IOException e) {
      System.err.println("Failed to read record:"+e.toString());
      return;
    }
    System.err.println("Building index table.");
    
    while(memPtr<memBuf.length) {
      synchronized (this) {
        for (i=0;i<4;i++) id_buf[i]=memBuf[memPtr+i];
        memPtr+=4;
        for (i=0;i<4;i++) sze_buf[i]=memBuf[memPtr+i];
        memPtr+=4;
        id=MathConvert.lng(id_buf);
        size=MathConvert.lng(sze_buf);
        this.size[max]=size;
        this.id[max]=id;
        this.index[max]=memPtr;
        max++;   
        memPtr+=size;
        this.notify();
      }
    }
    System.err.println("Completed building index table");
  }     

  private void buildIndex() { /* too big for memory */

    long cnt=0;
    long num;
    long filePtr=0;
    int size;
    int id;   
    InputStream inx=null;

    System.err.println("Building index table");
    this.max=0;
   
    try {
      if (url !=null) inx=url.openStream(); 
      else inx=new FileInputStream(file);         
      while (true) {
        synchronized (this) {
          cnt=0;
          while(cnt < 4) {
            num=inx.read(id_buf,(int) cnt,(int) (4-cnt));
            if (num<0) break;
            cnt+=num;  
          }  
          if (cnt<4) break;
          filePtr+=4;
          id=MathConvert.lng(id_buf);
          cnt=0;
          while (cnt<4) {
            num=inx.read(sze_buf,(int) cnt,(int) (4-cnt));
            if (num<0) break;
            cnt+=num;
          }
          if (cnt <4) break;
          filePtr+=4;
          size=MathConvert.lng(sze_buf);
          if (size <0) break;

          this.size[max]=size;
          this.id[max]=id;
          this.index[max]=filePtr;

          max++;   
          filePtr+=size;
          cnt=0;
          while (cnt<size) {
            num=inx.skip(size-cnt);
            if (num<0) break;
            cnt+=num;
          }
          if (cnt<size) break;
          this.notify();
        }
      }
      inx.close();
      inx=null;
      System.err.println("Completed building index table.");
      return;
    } catch (IOException e) {
      System.err.println("Failed to read record:"+e.toString());
      return;
    }
  }   

  public byte[] readBlock(int block) {
    if (block >=max) return null;
    if (block <0) return null;
    int memPtr;

    /* re-reading the same block */

    if ((block ==last) && (buffer !=null)) return buffer;
    memPtr=(int) this.index[block];
    buffer=new byte[this.size[block]];
    for (int i=0;i<this.size[block];i++) buffer[i]=memBuf[memPtr+i];
    return buffer;
  } 

  public byte[] readFile(int block) {
    long cnt;
    long num;
    long skip;
    int size;
    int id;

    if (block >=max) return null;
    if (block <0) return null;

    /* re-reading the same block */

    if ((block ==last) && (buffer !=null)) return buffer;

    try {

      if ((block <= last) || (in==null)) {
 
        /* rewinding or re-opening the file */
        if (in !=null) in.close();
        in=null;
        skip=this.index[block];
      } else skip=this.index[block]-(this.index[last]+this.size[last]);
      
      if (in ==null) in=url.openStream();
      if (skip>0) {
        cnt=0;
        while(cnt<skip) {
          num=in.skip(skip-cnt);
          if (num<0) break;
          cnt+=num;
        }
        if (cnt<skip) {
          in.close();
          in=null;
          return null;
        }
      }

      cnt=0;
      size=this.size[block];

      cnt=0;
      buffer=new byte[(int) size];
      if (buffer ==null) return null;
      while (cnt<size) {
        num=in.read(buffer,(int) cnt,(int) (size-cnt));
        if (num<0) break;
        cnt+=num;
      }
      if (cnt<size) {
        in.close();
        in=null;
        return null;
      }  
      last=block;
      return buffer;
    } catch (IOException e) {
      System.err.println("Failed to read record:"+e.toString());
      return null;
    }
  }

  synchronized public byte[] read(int block) {
    if (memBuf !=null) return readBlock(block);
    return readFile(block);
  }

  public boolean load() {
    URLConnection cnx;
    File fle;
    int size=0;
   
    try {
        if (url !=null) {
          cnx=url.openConnection();
          size=cnx.getContentLength(); 
          cnx=null;
	} else {
	    fle=new File(file);
            size=(int) fle.length();
	}
    }   catch (IOException e) {
      System.err.println("Failed to read File size:"+e.toString());
    }
    System.err.println("Size of file:"+size);
    if (size<=0) {
      return false;
    }
    memBuf=new byte[size];
    if (memBuf == null) buildIndex();
    else storeFile(size);
    return true;
  }
}





