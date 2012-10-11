/* DeltaRead.java
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

public class DeltaRead extends Thread {

  private ReadBlock block;
  public int max=0;
  URL url;
  String file=null;
  CSDData csd;
  TimeCanvas canvas;

  private boolean readBit(byte t[],int n,int off) {
    return ((t[off+(n/8)] & (1<< (n %8))) !=0);
  }

  private int readTop(byte t[],int off) {
    return (int) ((t[off+9] >>> 4) & 0x0f);
  } 
 
  public DeltaRead(URL url,TimeCanvas canvas) {
    this.url=url;
    this.canvas=canvas;
    csd=new CSDData();
    block=new ReadBlock(url);
    this.start();
  }

  public DeltaRead(String file,TimeCanvas canvas) {
    this.file=file;
    this.canvas=canvas;
    csd=new CSDData();
    block=new ReadBlock(file);
    this.start();
  }

  void decodeHeader() {
    csd.pmax=MathConvert.dbl(block.buffer,8);
    csd.vmax=MathConvert.dbl(block.buffer,16);
    csd.wmax=MathConvert.dbl(block.buffer,24);
    csd.stid=MathConvert.shrt(block.buffer,32);

  }
 
  private void decodeBlock() {
    double time;
    int rngNum=0;
    int index=0;
    int step=0;    
    int offset=8+2*10+2*4;
    int nrng,nbyte,bm,chn;
    int num=csd.num;

    csd.time[num]=MathConvert.dbl(block.buffer,0);
    csd.scan[num]=MathConvert.shrt(block.buffer,8);
    csd.cp[num]=MathConvert.shrt(block.buffer,10);
    csd.inttsc[num]=MathConvert.shrt(block.buffer,12);
    csd.inttus[num]=MathConvert.lng(block.buffer,14);
    csd.frang[num]=MathConvert.shrt(block.buffer,18);
    csd.rsep[num]=MathConvert.shrt(block.buffer,20);
    csd.rxrise[num]=MathConvert.shrt(block.buffer,22);
    csd.tfreq[num]=MathConvert.shrt(block.buffer,24);
    csd.noise[num]=MathConvert.lng(block.buffer,26);
    csd.atten[num]=MathConvert.shrt(block.buffer,30);
    csd.nave[num]=MathConvert.shrt(block.buffer,32);
    csd.nrang[num]=MathConvert.shrt(block.buffer,34);

    nrng=csd.nrang[num];
    nbyte=(nrng+7)/8;
    
    csd.stored=block.buffer[offset+nbyte+nbyte];
    chn=block.buffer[offset+nbyte+nbyte+1];
    bm=block.buffer[offset+nbyte+nbyte+2];
    
    csd.channel[num]=(short) (chn);    
    csd.bmnum[num]=(short) (bm);
    
    for (int i=0;i<nrng;i++) {
      if (readBit(block.buffer,i,offset+nbyte)== true) rngNum++;
    }

    for (int i=0;i<nrng;i++) {
      if (readBit(block.buffer,i,offset+nbyte)== true) {
        if (readBit(block.buffer,i,offset)==false) csd.map[num][i][0]=1;
          else csd.map[num][i][0]=2;
         step=0;                

         if ((csd.stored &0x01) !=0) {
           csd.map[num][i][2]=(byte) 
           (MathConvert.bit(block.buffer[offset+nbyte+nbyte+2+index])-128);
           step+=rngNum;
         }
         if ((csd.stored &0x02) !=0) {
           csd.map[num][i][1]=(byte) 
           (MathConvert.bit(block.buffer[offset+nbyte+nbyte+2+step+index])-128);
           step+=rngNum;
         }
         if ((csd.stored & 0x04) !=0) {
           csd.map[num][i][3]=(byte)
           (MathConvert.bit(block.buffer[offset+nbyte+nbyte+2+step+index])-128);
           step+=rngNum;
         }
         index++;
      } else csd.map[num][i][0]=0;
    }
    csd.num++;
  }


  public void run() { /* extract data from the delta file */
    int i=0;
    block.load();
    for (i=0;i<block.max;i++) {
      block.read(i);
      if (block.id[i]==-1) {
        decodeHeader();
      } else if (block.id[i]=='d') {
        canvas.time.mapData(csd);
        decodeBlock();
        if ((csd.num>=31) || (csd.scan[csd.num]==1)) {
           canvas.time.mapData(csd);
           csd.num=0;
	}
      }
    }
  
    canvas.remap();
    canvas.replot();
    canvas.repaint();
  }
}







