/* TestAACGM.java
   ============== 
   By R.J.Barnes */


/*
 $License$
*/








import java.util.*;
import java.lang.*;
import java.io.*;

public class AACGM {

    public double glat=0;
    public double glon=0;
    public double height=0;
    public double mlat=0;
    public double mlon=0;
    public double rho=0;
    public boolean flag=false;

    AACGMCoeff coeff;
    static int i_err64=0;
    static double height_old[]={-1,-1};
    static double first_coeff_old=-1;
    static double cint[][][]=new double[121][3][2]; 
    static double ylmval[]=new double[121];
     
  
    class Complex {
      double x,y;
    };



    public AACGM() {
      this.coeff=new AACGMCoeff();
        
    }

    public AACGM(AACGM aacgm) {
      this.coeff=aacgm.coeff;        
    }


    public AACGM(DataInputStream in) throws IOException {
      this.coeff=new AACGMCoeff(in);
        
    }

    int rylm(double colat,double lon,int order) {
   
      double d1;
      Complex z1=new Complex();
      Complex z2=new Complex();

 
      /* Local variables */
      Complex q_fac=new Complex();
      Complex q_val=new Complex();
      int l, m;
      int la,lb,lc,ld,le,lf;

      double cos_theta,sin_theta;  
      double ca,cb;
      double fac;
      double cos_lon, sin_lon;

      cos_theta = Math.cos(colat);
      sin_theta = Math.sin(colat);

      cos_lon = Math.cos(lon);
      sin_lon = Math.sin(lon);

      d1 = -sin_theta;
      z2.x = cos_lon;
      z2.y = sin_lon;

      z1.x = d1 * z2.x;
      z1.y = d1 * z2.y;

      q_fac.x = z1.x;
      q_fac.y = z1.y;

      ylmval[0] = 1;
      ylmval[2] = cos_theta;

      for (l = 1; l <= (order-1); l++) {
	  la = (l - 1) * l + 1;
	  lb = l * (l + 1) + 1;
	  lc = (l + 1) * (l + 2) + 1;

	  ca =  ((double) (l * 2 + 1)) / (l + 1);
	  cb =  ((double) l) / (l + 1);

	  ylmval[lc-1] = ca * cos_theta * ylmval[lb-1] - cb * ylmval[la-1];
      }

      q_val.x = q_fac.x;
      q_val.y = q_fac.y;

      ylmval[3] = q_val.x;
      ylmval[1] = -q_val.y;
      for (l = 2; l <= order; l++) {

	  d1 = l*2 - 1.;
	  z2.x = d1 * q_fac.x;
	  z2.y = d1 * q_fac.y;
	  z1.x = z2.x * q_val.x - z2.y * q_val.y;
	  z1.y = z2.x * q_val.y + z2.y * q_val.x;
	  q_val.x = z1.x;
	  q_val.y = z1.y;

	  la = l*l + (2*l) + 1;
	  lb = l*l + 1;

	  ylmval[la-1] = q_val.x;
	  ylmval[lb-1] = -q_val.y;
      }

      for (l = 2; l <= order; l++) {

	  la = l*l;
  	  lb = l*l - 2*(l - 1);

	  lc = l*l + (2*l);
	  ld = l*l + 2;

	  fac = l*2 - 1;

	  ylmval[lc-1] = fac * cos_theta * ylmval[la-1];
	  ylmval[ld-1] = fac * cos_theta * ylmval[lb-1];
      }

      for (m = 1; m <= (order-2); m++) {

	  la = (m+1)*(m+1);
          lb = (m+2)*(m+2)-1;
          lc = (m+3)*(m+3)-2;
	
	  ld = la - (2*m);
	  le = lb - (2*m);
	  lf = lc - (2*m);

	  for (l = m + 2; l <= order; l++) {
	      ca =  ((double) (2*l - 1)) / (l - m);
	      cb =  ((double) (l+m - 1)) / (l - m);

	      ylmval[lc-1] = ca * cos_theta *ylmval[lb-1] - cb *ylmval[la-1];
	      ylmval[lf-1] = ca * cos_theta *ylmval[le-1] - cb *ylmval[ld-1];

	      la = lb;
	      lb = lc;
	      lc = lb + (2*l) + 2;

	      ld = la - (2*m);
	      le = lb - (2*m);
	      lf = lc - (2*m);

	  }
      }
      return 0;
    } 


    double cgm_to_altitude(double r_height_in,double r_lat_in) 
              throws AACGMException {
      double eradius = 6371.2;
      double unim=1;
      double  r1;
      double ra;
      int error=0;

     /* Compute the corresponding altitude adjusted dipole latitude. */
     /* Computing 2nd power */

      r1 = Math.cos(r_lat_in*Math.PI/180.0);
      ra = (r_height_in/ eradius+1)*(r1*r1);
      if (ra > unim) {
	  ra = unim;
          throw new AACGMException("Invalid latitude for conversion");
      }

      r1 = Math.acos(Math.sqrt(ra));
   
      return AACGMMath.sgn(r1,r_lat_in)*180.0/Math.PI;
    }


    double altitude_to_cgm(double r_height_in,double  r_lat_alt) {
   
      double eradius =6371.2;
      double eps =1e-9;
      double unim =0.9999999;

      double r1;
      double r0, ra;

      /* Computing 2nd power */
      r1 = Math.cos(r_lat_alt*Math.PI/180.0);
      ra = r1 * r1;
      if (ra < eps) ra = eps;
      r0 = (r_height_in/eradius+1) / ra;
      if (r0 < unim) r0 = unim;
  
      r1 = Math.acos(Math.sqrt(1/r0));
     
      return AACGMMath.sgn(r1, r_lat_alt)*180.0/Math.PI;

    } 

    void convert_geo_coord(double lat,double lon,double hgt,
                           int flag,int order) 
      throws AACGMException{

      

      int i1;
      int i, j, l, m, k;
       
      double colat_temp;
      double lon_output;
    
      double lat_adj=0;
      double lat_alt=0;
      double colat_input; 
   
      double alt_var_cu=0, lon_temp=0, alt_var_sq=0, alt_var_qu=0;
      double colat_output=0, r=0, x=0, y=0, z=0;
      double alt_var=0;
      double lon_input=0;

      if (lon<0) lon+=360.0;  

      if (first_coeff_old != coeff.coef[0][0][0][0]) {
	  height_old[0] = -1.0;
	  height_old[1] = -1.0;
      }
      first_coeff_old= coeff.coef[0][0][0][0];
      
      if ((hgt < 0) || (hgt > 7200)) 
         throw new AACGMException("Invalid height");
      else if ((flag < 0) || (flag > 1)) 
         throw new AACGMException("Invalid conversion");
      else if (Math.abs(lat) >90.) 
         throw new AACGMException("Invalid latitude");
      else if ((lon<0) || (lon >360)) 
          throw new AACGMException("Invalid longitude");

      if (hgt != height_old[flag]) {
	  alt_var= hgt/7200.0;
	  alt_var_sq = alt_var * alt_var;
	  alt_var_cu = alt_var * alt_var_sq;
	  alt_var_qu = alt_var * alt_var_cu;

	  for (i=0; i<3; i++) {
	      for (j=0; j<121;j++) {
	          cint[j][i][flag] =coeff.coef[j][i][0][flag]+
                  coeff.coef[j][i][1][flag]*alt_var+
                  coeff.coef[j][i][2][flag]*alt_var_sq+
                  coeff.coef[j][i][3][flag]*alt_var_cu+
                  coeff.coef[j][i][4][flag]*alt_var_qu;
	      }
	  } 
	  height_old[flag] = hgt;
    
      }

      x = 0;
      y = 0;
      z = 0;

      lon_input =lon*Math.PI/180.0;

      if (flag == 0) colat_input = (90-lat)*Math.PI/180.0;
      else {
	  
        lat_adj=cgm_to_altitude(hgt, lat);
        colat_input= (90. - lat_adj)*Math.PI/180;

      }
      
      rylm(colat_input,lon_input,order);
      
      for (l = 0; l <= order; l++) {
        for (m = -l; m <= l; m++) {

	      k = l * (l+1) + m+1;
	      x += cint[k-1][0][flag]*ylmval[k-1];
              y += cint[k-1][1][flag]*ylmval[k-1];
              z += cint[k-1][2][flag]*ylmval[k-1];
	  }
      }
      r = Math.sqrt(x * x + y * y + z * z);
      if ((r< 0.9) || (r > 1.1)) 
        throw new AACGMException("Solution not found");
   
      z /= r;
      x /= r;
      y /= r;

      if (z > 1.) colat_temp=0;
      else if (z< -1.) colat_temp =Math.PI;
      else colat_temp= Math.acos(z);
  
      if ((Math.abs(x) < 1e-8) && (Math.abs(y) < 1e-8)) lon_temp =0;
      else lon_temp = Math.atan2(y,x);

      lon_output = lon_temp;

      if (flag == 0) {

	  lat_alt =90 - colat_temp*180/Math.PI;
       
	  lat_adj=altitude_to_cgm(hgt, lat_alt);
	    
	  colat_output = (90. - lat_adj) * Math.PI/180;

      } else colat_output = colat_temp;

      if (flag==0) {
        mlat =  (double) (90 -colat_output*180/Math.PI);
        mlon  = (double) (lon_output*180/Math.PI);
        rho=1.0;
      } else {
        glat =  (double) (90 -colat_output*180/Math.PI);
        glon  = (double) (lon_output*180/Math.PI);
        rho=1.0;
      }

    }


    void setGeo(double lat,double lon,double height) 
      throws AACGMException {        
        this.glat=lat;
        this.glon=lon;
        this.height=height;
        this.flag=true;
        convert_geo_coord(lat,lon,height,0,10);
    }


    void setMag(double lat,double lon,double height) 
      throws AACGMException {        
        this.mlat=lat;
        this.mlon=lon;
        this.height=height;
        this.flag=false;
        convert_geo_coord(lat,lon,height,1,10);
    }

    double getGeoLat() {
	return this.glat;
    }

    double getGeoLon() {
	return this.glon;
    }

    double getMagLat() {
	return this.mlat;
    }

    double getMagLon() {
	return this.mlon;
    }

    double getHeight() {
	return this.height;
    }

    boolean getFlag() {
	return this.flag;
    } 

  

  


                   
}
