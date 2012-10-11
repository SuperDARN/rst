/* TimeData.java
   =============
   Author R.J.Barnes
*/

/*
 $License$
*/





import java.applet.*;
import java.awt.*;
import java.awt.event.*;

public class TimeData {

  boolean active=false;
  boolean scan=true;
  public int bmnum;
  public int MAXFRAME=720;
  public int start=0;
  public int end=0;
  public double dval[][][];
  public int dflg[][];
  public int tfreq[];
  public float noise[];
  public int nrang[];
  public int frang[];
  public int rsep[];
  public int rxrise[];
  public double time[];

  public RadarPos rpos;

  public TimeData(int bmnum) {
    this.bmnum=bmnum;
  }

  public int mapData(RadarData data) {
   int index;
   Time tme=new Time();
   if (active==false) {
     dval=new double[MAXFRAME][RadarData.MAX_RANGE][3];
     dflg=new int[MAXFRAME][RadarData.MAX_RANGE];
     tfreq=new int [MAXFRAME];
     noise=new float [MAXFRAME];
     nrang=new int [MAXFRAME];
     frang=new int [MAXFRAME];
     rsep=new int [MAXFRAME];
     rxrise=new int[MAXFRAME];
     time=new double[MAXFRAME];
     active=true;
   }

   if ((scan==true) && (data.prm.scan<0)) return end;
    
    tfreq[end]=data.prm.tfreq;
    noise[end]=data.prm.noise.search;
    nrang[end]=data.prm.nrang;
    frang[end]=data.prm.frang;
    rsep[end]=data.prm.rsep;
    rxrise[end]=data.prm.rxrise;
    
    if (rpos==null) rpos=new RadarPos(data.prm.stid);
    tme.year=data.prm.time.yr;
    tme.month=data.prm.time.mo;
    tme.day=data.prm.time.dy;
    tme.hour=data.prm.time.hr;
    tme.minute=data.prm.time.mt;
    tme.second=data.prm.time.sc;

    this.time[end]=tme.getEpoch();
   
    /* convert to epoch time */

    for (int r=0;r<data.prm.nrang;r++) {
      if ((data.fit.rng[r].qflg !=0) && (data.fit.rng[r].p_l>1.0)) {
	dval[end][r][0]=data.fit.rng[r].v;
	dval[end][r][1]=data.fit.rng[r].p_l;
	dval[end][r][2]=data.fit.rng[r].w_l;
	if (data.fit.rng[r].gsct==0) dflg[end][r]=1;
        else dflg[end][r]=2;
      } else dflg[end][r]=0;
    }
    index=end;
    end++;    
    if (end==MAXFRAME) end=0;
    if (end==start) start++;
    if (start==MAXFRAME) start=0;
    return index;
  }

  public int mapData(CSDData csd) {
    int index=0;
    int num=0;
    if (active==false) {
      dval=new double[MAXFRAME][RadarData.MAX_RANGE][3];
      dflg=new int[MAXFRAME][RadarData.MAX_RANGE];
      tfreq=new int [MAXFRAME];
      noise=new float [MAXFRAME];
      nrang=new int [MAXFRAME];
      frang=new int [MAXFRAME];
      rsep=new int [MAXFRAME];
      rxrise=new int [MAXFRAME];
      time=new double[MAXFRAME];
      active=true;
    }
    if (rpos==null) rpos=new RadarPos(csd.stid);

    for (num=0;num<csd.num;num++) {
	if (csd.bmnum[num] !=bmnum) continue;
        if ((scan==true) && (csd.scan[num]<0)) continue;
  
        time[end]=csd.time[num];
 
        tfreq[end]=csd.tfreq[num];
        noise[end]=(float) csd.noise[num];
        nrang[end]=csd.nrang[num];
        frang[end]=csd.frang[num];
        rsep[end]=csd.rsep[num];
        rxrise[end]=0;

        for (int r=0;r<csd.nrang[num];r++) {
          if (csd.map[num][r][0] !=0) {
            dval[end][r][1]=(csd.map[num][r][2]+128.0)*csd.pmax/256.0;
            dval[end][r][0]=csd.map[num][r][1]*csd.vmax/128.0;
            dval[end][r][2]=(csd.map[num][r][3]+128.0)*csd.wmax/256.0;
            if (csd.map[num][r][0]==1) dflg[end][r]=1;
            else dflg[end][r]=2;
          }   else dflg[end][r]=0;
        }
        index=end;
        end++;    
        if (end==MAXFRAME) end=0;
        if (end==start) start++;
        if (start==MAXFRAME) start=0;
    }
    return index;
  }
}

