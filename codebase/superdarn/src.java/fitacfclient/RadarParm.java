/* RadarData.java
   ==============
   Author: R.J.Barnes
*/

/*
 $License$
*/




class RadarRevision {
    int major;
    int minor;
}

class RadarOrigin {
    int code;
    String time;
    String command;
}

class RadarTime {
   short yr,mo,dy,hr,mt,sc,us;
}

class RadarNoise {
  float mean;
  float search;
}

class RadarStat {
    short agc;
    short lopwr;
}

class RadarIntt {
    short sc;
    short us;
}

public class RadarParm {
 
  RadarRevision revision=new RadarRevision();

  short cp;
  short stid;

  RadarTime time=new RadarTime();

  RadarOrigin origin=new RadarOrigin();

  short txpow;
  short nave;
  short atten;
  short lagfr;
  short smsep;
  short  ercod;

  RadarStat stat=new RadarStat();
  RadarNoise noise=new RadarNoise();
    
  short channel;
  short bmnum;
  float bmazm;
  short scan;
 
  short rxrise;

  RadarIntt intt=new RadarIntt();

  short txpl;
  short mpinc;
  short mppul;
  short mplgs;
  short nrang;
  short frang;
  short rsep;
  short xcf;
  short tfreq;
  short offset; 

  int mxpwr;
  int lvmax;

  short pulse[]=new short[RadarData.PULSE_SIZE];
  short lag[][]=new short[RadarData.LAG_SIZE][2];  
  String combf;  

  int getChannel(DataMap dmap) {
     int c;  
     int chn=-1;
     DataMapScalar scl;

     for (c=0;c<dmap.snum;c++) {
       scl=(DataMapScalar) dmap.scalar.elementAt(c);

       if ((scl.name.equals("channel")) && (scl.type==DataMap.DATASHORT)) {
	   chn=((Short) scl.value).shortValue();
           System.err.println(chn);
       } 
     }
     return chn;
  }

  public boolean decode(DataMap dmap)  {
     int c,x;
     boolean flag=false;
     DataMapScalar scl;
     DataMapArray arr;
  

     for (c=0;c<dmap.snum;c++) {
       scl=(DataMapScalar) dmap.scalar.elementAt(c);

       if ((scl.name.equals("radar.revision.major")) && 
           (scl.type==DataMap.DATACHAR)) {
	   revision.major=((Character) scl.value).charValue();
           System.err.println(scl.name+" = "+revision.major);
	   flag=true;
       } 
       if ((scl.name.equals("radar.revision.minor")) && 
           (scl.type==DataMap.DATACHAR)) {
	   revision.minor=((Character) scl.value).charValue();
           System.err.println(scl.name+" = "+revision.minor);

	   flag=true;
       } 

       if ((scl.name.equals("origin.code")) && 
           (scl.type==DataMap.DATACHAR)) {
	   origin.code=((Character) scl.value).charValue();
           System.err.println(scl.name+" = "+origin.code);
	   flag=true;
       } 

       if ((scl.name.equals("origin.time")) && 
           (scl.type==DataMap.DATASTRING)) {
	   origin.time=new String((String) scl.value);
           System.err.println(scl.name+" = "+origin.time);
	   flag=true;
       }

       if ((scl.name.equals("origin.command")) && 
           (scl.type==DataMap.DATASTRING)) {
	   origin.time=new String((String) scl.value);
           System.err.println(scl.name+" = "+origin.command);
	   flag=true;
       }


       if ((scl.name.equals("cp")) && (scl.type==DataMap.DATASHORT)) {
	   cp=((Short) scl.value).shortValue();
           System.err.println(scl.name+" = "+cp);
	   flag=true;
       }  

       if ((scl.name.equals("stid")) && (scl.type==DataMap.DATASHORT)) {
	   stid=((Short) scl.value).shortValue();
           System.err.println(scl.name+" = "+stid);
	   flag=true;
       }  

       if ((scl.name.equals("time.yr")) && (scl.type==DataMap.DATASHORT)) {
	   time.yr=((Short) scl.value).shortValue();
           System.err.println(scl.name+" = "+time.yr);
	   flag=true;
       }  
       if ((scl.name.equals("time.mo")) && (scl.type==DataMap.DATASHORT)) {
	   time.mo=((Short) scl.value).shortValue();
           System.err.println(scl.name+" = "+time.mo);
	   flag=true;
       }  
       if ((scl.name.equals("time.dy")) && (scl.type==DataMap.DATASHORT)) {
	   time.dy=((Short) scl.value).shortValue();
           System.err.println(scl.name+" = "+time.dy);
	   flag=true;
       }  
       if ((scl.name.equals("time.hr")) && (scl.type==DataMap.DATASHORT)) {
	   time.hr=((Short) scl.value).shortValue();
           System.err.println(scl.name+" = "+time.hr);
	   flag=true;
       }  
       if ((scl.name.equals("time.mt")) && (scl.type==DataMap.DATASHORT)) {
	   time.mt=((Short) scl.value).shortValue();
           System.err.println(scl.name+" = "+time.mt);
	   flag=true;
       }  
       if ((scl.name.equals("time.sc")) && (scl.type==DataMap.DATASHORT)) {
	   time.sc=((Short) scl.value).shortValue();
           System.err.println(scl.name+" = "+time.sc);
	   flag=true;
       }  
       if ((scl.name.equals("time.us")) && (scl.type==DataMap.DATASHORT)) {
	   time.us=((Short) scl.value).shortValue();
           System.err.println(scl.name+" = "+time.us);
	   flag=true;
       }  

       if ((scl.name.equals("txpow")) && (scl.type==DataMap.DATASHORT)) {
	   txpow=((Short) scl.value).shortValue();
           System.err.println(scl.name+" = "+txpow);
	   flag=true;
       }  
       if ((scl.name.equals("nave")) && (scl.type==DataMap.DATASHORT)) {
	   nave=((Short) scl.value).shortValue();
           System.err.println(scl.name+" = "+nave);
	   flag=true;
       }  
       if ((scl.name.equals("atten")) && (scl.type==DataMap.DATASHORT)) {
	   atten=((Short) scl.value).shortValue();
           System.err.println(scl.name+" = "+atten);
	   flag=true;
       } 
       if ((scl.name.equals("lagfr")) && (scl.type==DataMap.DATASHORT)) {
	   lagfr=((Short) scl.value).shortValue();
           System.err.println(scl.name+" = "+lagfr);
	   flag=true;
       } 
       if ((scl.name.equals("smsep")) && (scl.type==DataMap.DATASHORT)) {
	   smsep=((Short) scl.value).shortValue();
           System.err.println(scl.name+" = "+smsep);
	   flag=true;
       } 
       if ((scl.name.equals("ercod")) && (scl.type==DataMap.DATASHORT)) {
	   ercod=((Short) scl.value).shortValue();
           System.err.println(scl.name+" = "+ercod);
	   flag=true;
       } 
       if ((scl.name.equals("stat.agc")) && (scl.type==DataMap.DATASHORT)) {
	   stat.agc=((Short) scl.value).shortValue();
           System.err.println(scl.name+" = "+stat.agc);
	   flag=true;
       } 

       if ((scl.name.equals("stat.lopwr")) && (scl.type==DataMap.DATASHORT)) {
	   stat.lopwr=((Short) scl.value).shortValue();
           System.err.println(scl.name+" = "+stat.lopwr);
	   flag=true;
       } 

       if ((scl.name.equals("noise.search")) && 
           (scl.type==DataMap.DATAFLOAT)) {
	   noise.search=((Float) scl.value).floatValue();
           System.err.println(scl.name+" = "+noise.search);
	   flag=true;
       } 
       if ((scl.name.equals("noise.mean")) && 
           (scl.type==DataMap.DATAFLOAT)) {
	   noise.mean=((Float) scl.value).floatValue();
           System.err.println(scl.name+" = "+noise.mean);
	   flag=true;
       } 

 

       if ((scl.name.equals("channel")) && (scl.type==DataMap.DATASHORT)) {
	   channel=((Short) scl.value).shortValue();
           System.err.println(scl.name+" = "+channel);
	   flag=true;
       } 
       if ((scl.name.equals("bmnum")) && (scl.type==DataMap.DATASHORT)) {
	   bmnum=((Short) scl.value).shortValue();
           System.err.println(scl.name+" = "+bmnum);
	   flag=true;
       } 


       if ((scl.name.equals("bmazm")) && 
           (scl.type==DataMap.DATAFLOAT)) {
	   bmazm=((Float) scl.value).floatValue();
           System.err.println(scl.name+" = "+bmazm);
	   flag=true;
       } 

       if ((scl.name.equals("scan")) && (scl.type==DataMap.DATASHORT)) {
	   scan=((Short) scl.value).shortValue();
           System.err.println(scl.name+" = "+scan);
	   flag=true;
       } 

       if ((scl.name.equals("offset")) && (scl.type==DataMap.DATASHORT)) {
	   offset=((Short) scl.value).shortValue();
           System.err.println(scl.name+" = "+offset);
	   flag=true;
       } 

       if ((scl.name.equals("rxrise")) && (scl.type==DataMap.DATASHORT)) {
	   rxrise=((Short) scl.value).shortValue();
           System.err.println(scl.name+" = "+rxrise);
	   flag=true;
       } 


       if ((scl.name.equals("intt.sc")) && (scl.type==DataMap.DATASHORT)) {
	   intt.sc=((Short) scl.value).shortValue();
           System.err.println(scl.name+" = "+intt.sc);
	   flag=true;
       } 

       if ((scl.name.equals("intt.us")) && (scl.type==DataMap.DATASHORT)) {
	   intt.us=((Short) scl.value).shortValue();
           System.err.println(scl.name+" = "+intt.us);

	   flag=true;
       } 

       if ((scl.name.equals("txpl")) && (scl.type==DataMap.DATASHORT)) {
	   txpl=((Short) scl.value).shortValue();
           System.err.println(scl.name+" = "+txpl);
	   flag=true;
       }
 
       if ((scl.name.equals("mpinc")) && (scl.type==DataMap.DATASHORT)) {
	   mpinc=((Short) scl.value).shortValue();
           System.err.println(scl.name+" = "+mpinc);
	   flag=true;
       } 

       if ((scl.name.equals("mppul")) && (scl.type==DataMap.DATASHORT)) {
	   mppul=((Short) scl.value).shortValue();
           System.err.println(scl.name+" = "+mppul);
	   flag=true;
       } 
       if ((scl.name.equals("mplgs")) && (scl.type==DataMap.DATASHORT)) {
	   mplgs=((Short) scl.value).shortValue();
           System.err.println(scl.name+" = "+mplgs);
	   flag=true;
       } 

       if ((scl.name.equals("nrang")) && (scl.type==DataMap.DATASHORT)) {
	   nrang=((Short) scl.value).shortValue();
           System.err.println(scl.name+" = "+nrang);
	   flag=true;
       }

       if ((scl.name.equals("frang")) && (scl.type==DataMap.DATASHORT)) {
	   frang=((Short) scl.value).shortValue();
           System.err.println(scl.name+" = "+frang);
	   flag=true;
       }

       if ((scl.name.equals("rsep")) && (scl.type==DataMap.DATASHORT)) {
	   rsep=((Short) scl.value).shortValue();
           System.err.println(scl.name+" = "+rsep);
	   flag=true;
       }

       if ((scl.name.equals("xcf")) && (scl.type==DataMap.DATASHORT)) {
	   xcf=((Short) scl.value).shortValue();
           System.err.println(scl.name+" = "+xcf);
	   flag=true;
       }

       if ((scl.name.equals("tfreq")) && (scl.type==DataMap.DATASHORT)) {
	   tfreq=((Short) scl.value).shortValue();
           System.err.println(scl.name+" = "+tfreq);
	   flag=true;
       }

       if ((scl.name.equals("mxpwr")) && (scl.type==DataMap.DATALONG)) {
	   mxpwr=((Integer) scl.value).intValue();
           System.err.println(scl.name+" = "+mxpwr);
	   flag=true;
       }

       if ((scl.name.equals("lvmax")) && (scl.type==DataMap.DATALONG)) {
	   lvmax=((Integer) scl.value).intValue();
           System.err.println(scl.name+" = "+lvmax);
	   flag=true;
       }

       if ((scl.name.equals("combf")) && (scl.type==DataMap.DATASTRING)) {
	   combf=new String((String) scl.value);
           System.err.println(scl.name+" = "+combf);
	   flag=true;
       }

     }

     for (c=0;c<dmap.anum;c++) {
       arr=(DataMapArray) dmap.array.elementAt(c);
       if ((arr.name.equals("ptab")) && (arr.type==DataMap.DATASHORT) &&
           (arr.dim==1)) {
           for (x=0;x<arr.rng[0];x++) 
             pulse[x]=((Short) arr.value[x]).shortValue();

           System.err.println(arr.name+" = ");
	   for (x=0;x<arr.rng[0];x++) System.err.println(pulse[x]);


	   flag=true;
       }

       if ((arr.name.equals("ltab")) && (arr.type==DataMap.DATASHORT) &&
           (arr.dim==2)) {
           for (x=0;x<arr.rng[1];x++) {
             lag[x][0]=((Short) arr.value[2*x]).shortValue();
             lag[x][1]=((Short) arr.value[2*x+1]).shortValue();
	   }
           System.err.println(arr.name+" = ");
	   for (x=0;x<arr.rng[1];x++) 
             System.err.println(lag[x][0]+","+lag[x][1]);


	   flag=true;
       }
     }

     return flag;
  }

}

