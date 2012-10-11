/* FitData.java
   -===========
   Author: R.J.Barnes
*/

/*
 $License$
*/




class FitNoise {
  double vel;
  double skynoise;
  double lag0;
}


class FitRange {
  double v;
  double v_err;
  double p_0;
  double p_l;
  double p_l_err; 
  double p_s;
  double p_s_err;
  double w_l;
  double w_l_err;
  double w_s;
  double w_s_err;
  double phi0;
  double phi0_err;
  double sdev_l;
  double sdev_s;
  double sdev_phi;
  int qflg,gsct;
  int nump;

}

class FitElv {
  double normal;
  double low;
  double high;
}

public class FitData {
  RadarRevision revision=new RadarRevision();
  FitNoise noise=new FitNoise();
  FitRange rng[]=new FitRange[RadarData.MAX_RANGE];
  FitRange xrng[]=new FitRange[RadarData.MAX_RANGE];
  FitElv elv[]=new FitElv[RadarData.MAX_RANGE];

  public FitData() {
    int n;
    for (n=0;n<RadarData.MAX_RANGE;n++) {
        rng[n]=new FitRange();
        xrng[n]=new FitRange();
        elv[n]=new FitElv();
    }
  }

  public boolean decode(DataMap dmap)  {
     int c,x;
     boolean flag=false;
     DataMapScalar scl;
     DataMapArray arr;
     short slist[]=new short[RadarData.MAX_RANGE];

     for (c=0;c<dmap.snum;c++) {
       scl=(DataMapScalar) dmap.scalar.elementAt(c);

       if ((scl.name.equals("fitacf.revision.major")) && 
           (scl.type==DataMap.DATACHAR)) {
	   revision.major=((Character) scl.value).charValue();
	   /* System.err.println(scl.name+" = "+revision.major);*/
	   flag=true;
       } 
       if ((scl.name.equals("fitacf.revision.minor")) && 
           (scl.type==DataMap.DATACHAR)) {
	   revision.minor=((Character) scl.value).charValue();
           /* System.err.println(scl.name+" = "+revision.minor);*/
	   flag=true;
       } 

       if ((scl.name.equals("noise.sky")) && 
           (scl.type==DataMap.DATAFLOAT)) {
	   noise.skynoise=((Float) scl.value).floatValue();
           /* System.err.println(scl.name+" = "+noise.skynoise); */
	   flag=true;
       } 

       if ((scl.name.equals("noise.lag0")) && 
           (scl.type==DataMap.DATAFLOAT)) {
	   noise.lag0=((Float) scl.value).floatValue();
           /* System.err.println(scl.name+" = "+noise.lag0); */
	   flag=true;
       } 

       if ((scl.name.equals("noise.vel")) && 
           (scl.type==DataMap.DATAFLOAT)) {
	   noise.vel=((Float) scl.value).floatValue();
           /* System.err.println(scl.name+" = "+noise.vel); */
	   flag=true;
       } 
     }
     
     for (c=0;c<RadarData.MAX_RANGE;c++) {
       rng[c].qflg=0;
       xrng[c].qflg=0;
     }

     for (c=0;c<dmap.anum;c++) {
         arr=(DataMapArray) dmap.array.elementAt(c);

         if ((arr.name.equals("pwr0")) && (arr.type==DataMap.DATAFLOAT) &&
           (arr.dim==1)) {
           for (x=0;x<arr.rng[0];x++) 
             rng[x].p_0=((Float) arr.value[x]).floatValue();

           /* System.err.println(arr.name+" = ");
	   for (x=0;x<arr.rng[0];x++) System.err.println(rng[x].p_0);
	   */
	   flag=true;
	 }     


         if ((arr.name.equals("slist")) && (arr.type==DataMap.DATASHORT) &&
           (arr.dim==1)) {
	   for (x=0;x<arr.rng[0];x++) {
             slist[x]=((Short) arr.value[x]).shortValue();
             rng[slist[x]].qflg=1;
	   }             
           /* System.err.println(arr.name+" = "); 
	   for (x=0;x<arr.rng[0];x++) System.err.println(slist[x]);
	   */
	   flag=true;
	 }     
     }

    for (c=0;c<dmap.anum;c++) {
         arr=(DataMapArray) dmap.array.elementAt(c);

         if ((arr.name.equals("nlag")) && (arr.type==DataMap.DATASHORT) &&
           (arr.dim==1)) {
           for (x=0;x<arr.rng[0];x++) 
             rng[slist[x]].nump=((Short) arr.value[x]).shortValue();

           /* System.err.println(arr.name+" = "); 
	   for (x=0;x<arr.rng[0];x++) System.err.println(rng[slist[x]].nump);
	   */
	   flag=true;
	 }    

         if ((arr.name.equals("qflg")) && (arr.type==DataMap.DATACHAR) &&
           (arr.dim==1)) {
           for (x=0;x<arr.rng[0];x++) 
             rng[slist[x]].qflg=((Character) arr.value[x]).charValue();

           /* System.err.println(arr.name+" = "); 
	   for (x=0;x<arr.rng[0];x++) System.err.println(rng[slist[x]].qflg);
	   */
	   flag=true;
	 }   

         if ((arr.name.equals("gflg")) && (arr.type==DataMap.DATACHAR) &&
           (arr.dim==1)) {
           for (x=0;x<arr.rng[0];x++) 
             rng[slist[x]].gsct=((Character) arr.value[x]).charValue();

           /* System.err.println(arr.name+" = "); 
	   for (x=0;x<arr.rng[0];x++) System.err.println(rng[slist[x]].gsct);
	   */
	   flag=true;
	 }   

         if ((arr.name.equals("p_l")) && (arr.type==DataMap.DATAFLOAT) &&
           (arr.dim==1)) {
           for (x=0;x<arr.rng[0];x++) 
             rng[slist[x]].p_l=((Float) arr.value[x]).floatValue();

           /* System.err.println(arr.name+" = "); 
	   for (x=0;x<arr.rng[0];x++) System.err.println(rng[slist[x]].p_l);
	   */
	   flag=true;
	 }   

         if ((arr.name.equals("p_l_e")) && (arr.type==DataMap.DATAFLOAT) &&
           (arr.dim==1)) {
           for (x=0;x<arr.rng[0];x++) 
             rng[slist[x]].p_l_err=((Float) arr.value[x]).floatValue();

           /* System.err.println(arr.name+" = ");
	      for (x=0;x<arr.rng[0];x++) 
                System.err.println(rng[slist[x]].p_l_err);
	   */
	   flag=true;
	 }   


         if ((arr.name.equals("p_s")) && (arr.type==DataMap.DATAFLOAT) &&
           (arr.dim==1)) {
           for (x=0;x<arr.rng[0];x++) 
             rng[slist[x]].p_s=((Float) arr.value[x]).floatValue();

           /* System.err.println(arr.name+" = "); 
	      for (x=0;x<arr.rng[0];x++) System.err.println(rng[slist[x]].p_s);
	   */
	   flag=true;
	 }   

         if ((arr.name.equals("p_s_e")) && (arr.type==DataMap.DATAFLOAT) &&
           (arr.dim==1)) {
           for (x=0;x<arr.rng[0];x++) 
             rng[slist[x]].p_s_err=((Float) arr.value[x]).floatValue();

           /* System.err.println(arr.name+" = ");
	   for (x=0;x<arr.rng[0];x++) 
	       System.err.println(rng[slist[x]].p_s_err); 
	   */
	   flag=true;
	 }   

         if ((arr.name.equals("v")) && (arr.type==DataMap.DATAFLOAT) &&
           (arr.dim==1)) {
           for (x=0;x<arr.rng[0];x++) 
             rng[slist[x]].v=((Float) arr.value[x]).floatValue();

           /* System.err.println(arr.name+" = "); 
	      for (x=0;x<arr.rng[0];x++) System.err.println(rng[slist[x]].v); 
            */
	   flag=true;
	 }  

         if ((arr.name.equals("v_e")) && (arr.type==DataMap.DATAFLOAT) &&
           (arr.dim==1)) {
           for (x=0;x<arr.rng[0];x++) 
             rng[slist[x]].v_err=((Float) arr.value[x]).floatValue();

	   /* System.err.println(arr.name+" = ");
	      for (x=0;x<arr.rng[0];x++) 
                System.err.println(rng[slist[x]].v_err);
	   */
	   flag=true;
	 }  



         if ((arr.name.equals("w_l")) && (arr.type==DataMap.DATAFLOAT) &&
           (arr.dim==1)) {
           for (x=0;x<arr.rng[0];x++) 
             rng[slist[x]].w_l=((Float) arr.value[x]).floatValue();
	   /*
             System.err.println(arr.name+" = ");
	     for (x=0;x<arr.rng[0];x++) System.err.println(rng[slist[x]].w_l);
	   */
	   flag=true;
	 }  

         if ((arr.name.equals("w_l_e")) && (arr.type==DataMap.DATAFLOAT) &&
           (arr.dim==1)) {
           for (x=0;x<arr.rng[0];x++) 
             rng[slist[x]].w_l_err=((Float) arr.value[x]).floatValue();
	   
           /*  System.err.println(arr.name+" = ");
	     for (x=0;x<arr.rng[0];x++) 
             System.err.println(rng[slist[x]].w_l_err);
	   */
	   flag=true;
	 }  


         if ((arr.name.equals("w_s")) && (arr.type==DataMap.DATAFLOAT) &&
           (arr.dim==1)) {
           for (x=0;x<arr.rng[0];x++) 
             rng[slist[x]].w_s=((Float) arr.value[x]).floatValue();

           /* System.err.println(arr.name+" = ");
	     for (x=0;x<arr.rng[0];x++) System.err.println(rng[slist[x]].w_s);
	   */
	   flag=true;
	 }  

         if ((arr.name.equals("w_s_e")) && (arr.type==DataMap.DATAFLOAT) &&
           (arr.dim==1)) {
           for (x=0;x<arr.rng[0];x++) 
             rng[slist[x]].w_s_err=((Float) arr.value[x]).floatValue();

	   /*
             System.err.println(arr.name+" = ");
	     for (x=0;x<arr.rng[0];x++) 
             System.err.println(rng[slist[x]].w_s_err);
	   */
	   flag=true;
	 }  

         if ((arr.name.equals("sd_l")) && (arr.type==DataMap.DATAFLOAT) &&
           (arr.dim==1)) {
           for (x=0;x<arr.rng[0];x++) 
             rng[slist[x]].sdev_l=((Float) arr.value[x]).floatValue();
	   /*
             System.err.println(arr.name+" = ");
	     for (x=0;x<arr.rng[0];x++) 
               System.err.println(rng[slist[x]].sdev_l);
	   */
	   flag=true;
	 }  

         if ((arr.name.equals("sd_s")) && (arr.type==DataMap.DATAFLOAT) &&
           (arr.dim==1)) {
           for (x=0;x<arr.rng[0];x++) 
             rng[slist[x]].sdev_s=((Float) arr.value[x]).floatValue();
	   /*
             System.err.println(arr.name+" = ");
	     for (x=0;x<arr.rng[0];x++) 
                System.err.println(rng[slist[x]].sdev_s);
	   */
	   flag=true;
	 }

         if ((arr.name.equals("sd_phi")) && (arr.type==DataMap.DATAFLOAT) &&
           (arr.dim==1)) {
           for (x=0;x<arr.rng[0];x++) 
             rng[slist[x]].sdev_phi=((Float) arr.value[x]).floatValue();
	   
           /* System.err.println(arr.name+" = ");
	     for (x=0;x<arr.rng[0];x++) 
             System.err.println(rng[slist[x]].sdev_phi);
	   */
	   flag=true;
	 }

         if ((arr.name.equals("x_qflg")) && (arr.type==DataMap.DATACHAR) &&
           (arr.dim==1)) {
           for (x=0;x<arr.rng[0];x++) 
             xrng[slist[x]].qflg=((Character) arr.value[x]).charValue();
	   /*
             System.err.println(arr.name+" = ");
	     for (x=0;x<arr.rng[0];x++) 
                   System.err.println(xrng[slist[x]].qflg);
	   */
	   flag=true;
	 }   

         if ((arr.name.equals("x_gflg")) && (arr.type==DataMap.DATACHAR) &&
           (arr.dim==1)) {
           for (x=0;x<arr.rng[0];x++) 
             xrng[slist[x]].gsct=((Character) arr.value[x]).charValue();
	   /*
           System.err.println(arr.name+" = ");
	   for (x=0;x<arr.rng[0];x++) System.err.println(xrng[slist[x]].gsct);
	   */
	   flag=true;
	 }   

         if ((arr.name.equals("x_p_l")) && (arr.type==DataMap.DATAFLOAT) &&
           (arr.dim==1)) {
           for (x=0;x<arr.rng[0];x++) 
             xrng[slist[x]].p_l=((Float) arr.value[x]).floatValue();
	   /*
           System.err.println(arr.name+" = ");
	   for (x=0;x<arr.rng[0];x++) System.err.println(xrng[slist[x]].p_l);
	   */
	   flag=true;
	 }   

         if ((arr.name.equals("x_p_l_e")) && (arr.type==DataMap.DATAFLOAT) &&
           (arr.dim==1)) {
           for (x=0;x<arr.rng[0];x++) 
             xrng[slist[x]].p_l_err=((Float) arr.value[x]).floatValue();
	   /*
           System.err.println(arr.name+" = ");
	   for (x=0;x<arr.rng[0];x++) 
             System.err.println(xrng[slist[x]].p_l_err);
	   */
	   flag=true;
	 }   


         if ((arr.name.equals("x_p_s")) && (arr.type==DataMap.DATAFLOAT) &&
           (arr.dim==1)) {
           for (x=0;x<arr.rng[0];x++) 
             xrng[slist[x]].p_s=((Float) arr.value[x]).floatValue();
	   /*
           System.err.println(arr.name+" = ");
	   for (x=0;x<arr.rng[0];x++) 
             System.err.println(xrng[slist[x]].p_s);
	   */
	   flag=true;
	 }   

         if ((arr.name.equals("x_p_s_e")) && (arr.type==DataMap.DATAFLOAT) &&
           (arr.dim==1)) {
           for (x=0;x<arr.rng[0];x++) 
             xrng[slist[x]].p_s_err=((Float) arr.value[x]).floatValue();
	   /*
           System.err.println(arr.name+" = ");
	   for (x=0;x<arr.rng[0];x++) 
             System.err.println(xrng[slist[x]].p_s_err);
	   */
	   flag=true;
	 }   

         if ((arr.name.equals("x_v")) && (arr.type==DataMap.DATAFLOAT) &&
           (arr.dim==1)) {
           for (x=0;x<arr.rng[0];x++) 
             xrng[slist[x]].v=((Float) arr.value[x]).floatValue();
	   /*
           System.err.println(arr.name+" = ");
	   for (x=0;x<arr.rng[0];x++) System.err.println(xrng[slist[x]].v);
	   */
	   flag=true;
	 }  

         if ((arr.name.equals("x_v_e")) && (arr.type==DataMap.DATAFLOAT) &&
           (arr.dim==1)) {
           for (x=0;x<arr.rng[0];x++) 
             xrng[slist[x]].v_err=((Float) arr.value[x]).floatValue();
	   /*
           System.err.println(arr.name+" = ");
	   for (x=0;x<arr.rng[0];x++) System.err.println(xrng[slist[x]].v_err);
	   */
	   flag=true;
	 }  



         if ((arr.name.equals("x_w_l")) && (arr.type==DataMap.DATAFLOAT) &&
           (arr.dim==1)) {
           for (x=0;x<arr.rng[0];x++) 
             xrng[slist[x]].w_l=((Float) arr.value[x]).floatValue();
	   /*
           System.err.println(arr.name+" = ");
	   for (x=0;x<arr.rng[0];x++) System.err.println(xrng[slist[x]].w_l);
	   */
	   flag=true;
	 }  

         if ((arr.name.equals("x_w_l_e")) && (arr.type==DataMap.DATAFLOAT) &&
           (arr.dim==1)) {
           for (x=0;x<arr.rng[0];x++) 
             xrng[slist[x]].w_l_err=((Float) arr.value[x]).floatValue();
	   /*
           System.err.println(arr.name+" = ");
	   for (x=0;x<arr.rng[0];x++) 
              System.err.println(xrng[slist[x]].w_l_err);
	   */
	   flag=true;
	 }  


         if ((arr.name.equals("x_w_s")) && (arr.type==DataMap.DATAFLOAT) &&
           (arr.dim==1)) {
           for (x=0;x<arr.rng[0];x++) 
             xrng[slist[x]].w_s=((Float) arr.value[x]).floatValue();
	   /*
           System.err.println(arr.name+" = ");
	   for (x=0;x<arr.rng[0];x++) System.err.println(xrng[slist[x]].w_s);
	   */
	   flag=true;
	 }  

         if ((arr.name.equals("x_w_s_e")) && (arr.type==DataMap.DATAFLOAT) &&
           (arr.dim==1)) {
           for (x=0;x<arr.rng[0];x++) 
             xrng[slist[x]].w_s_err=((Float) arr.value[x]).floatValue();
	   /*
           System.err.println(arr.name+" = ");
	   for (x=0;x<arr.rng[0];x++) 
              System.err.println(xrng[slist[x]].w_s_err);
	   */
	   flag=true;
	 }  


         if ((arr.name.equals("phi0")) && (arr.type==DataMap.DATAFLOAT) &&
           (arr.dim==1)) {
           for (x=0;x<arr.rng[0];x++) 
             rng[slist[x]].phi0=((Float) arr.value[x]).floatValue();
	   /*
           System.err.println(arr.name+" = ");
	   for (x=0;x<arr.rng[0];x++) System.err.println(rng[slist[x]].phi0);
	   */
	   flag=true;
	 }  

         if ((arr.name.equals("phi0_e")) && (arr.type==DataMap.DATAFLOAT) &&
           (arr.dim==1)) {
           for (x=0;x<arr.rng[0];x++) 
             rng[slist[x]].phi0_err=((Float) arr.value[x]).floatValue();
	   /*
           System.err.println(arr.name+" = ");
	   for (x=0;x<arr.rng[0];x++) 
              System.err.println(rng[slist[x]].phi0_err);
	   */
	   flag=true;
	 }  


         if ((arr.name.equals("elv")) && (arr.type==DataMap.DATAFLOAT) &&
           (arr.dim==1)) {
           for (x=0;x<arr.rng[0];x++) 
             elv[slist[x]].normal=((Float) arr.value[x]).floatValue();
	   /*
           System.err.println(arr.name+" = ");
	   for (x=0;x<arr.rng[0];x++) 
              System.err.println(elv[slist[x]].normal);
	   */
	   flag=true;
	 }
  
         if ((arr.name.equals("elv_low")) && (arr.type==DataMap.DATAFLOAT) &&
           (arr.dim==1)) {
           for (x=0;x<arr.rng[0];x++) 
             elv[slist[x]].low=((Float) arr.value[x]).floatValue();
	   /*
           System.err.println(arr.name+" = ");
	   for (x=0;x<arr.rng[0];x++) 
              System.err.println(elv[slist[x]].low);
	   */
	   flag=true;
	 }  
         if ((arr.name.equals("elv_high")) && (arr.type==DataMap.DATAFLOAT) &&
           (arr.dim==1)) {
           for (x=0;x<arr.rng[0];x++) 
             elv[slist[x]].high=((Float) arr.value[x]).floatValue();
	   /*
           System.err.println(arr.name+" = ");
	   for (x=0;x<arr.rng[0];x++) 
              System.err.println(elv[slist[x]].high);
	   */
	   flag=true;
	 }  

         if ((arr.name.equals("x_sd_l")) && (arr.type==DataMap.DATAFLOAT) &&
           (arr.dim==1)) {
           for (x=0;x<arr.rng[0];x++) 
             xrng[slist[x]].sdev_l=((Float) arr.value[x]).floatValue();
	   /*
           System.err.println(arr.name+" = ");
	   for (x=0;x<arr.rng[0];x++) 
              System.err.println(xrng[slist[x]].sdev_l);
	   */
	   flag=true;
	 }  

         if ((arr.name.equals("x_sd_s")) && (arr.type==DataMap.DATAFLOAT) &&
           (arr.dim==1)) {
           for (x=0;x<arr.rng[0];x++) 
             xrng[slist[x]].sdev_s=((Float) arr.value[x]).floatValue();
	   /*
           System.err.println(arr.name+" = ");
	   for (x=0;x<arr.rng[0];x++) 
              System.err.println(xrng[slist[x]].sdev_s);
	   */
	   flag=true;
	 }  

         if ((arr.name.equals("x_sd_phi")) && (arr.type==DataMap.DATAFLOAT) &&
           (arr.dim==1)) {
           for (x=0;x<arr.rng[0];x++) 
             xrng[slist[x]].sdev_phi=((Float) arr.value[x]).floatValue();
	   /*
           System.err.println(arr.name+" = ");
	   for (x=0;x<arr.rng[0];x++) 
              System.err.println(xrng[slist[x]].sdev_phi);
	   */
	   flag=true;
	 }  




 
     }



     return flag;
  }



}

