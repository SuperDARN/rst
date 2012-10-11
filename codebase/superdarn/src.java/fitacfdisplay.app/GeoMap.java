/* GeoMap.java
   ===========
   Author R.J.Barnes
*/

/*
 $License$
*/





import java.lang.*;

public class GeoMap {

  static Polar gpos=new Polar(0,0,0);

  private static double cosd(double d) { 
   return Math.cos(d*Math.PI/180.0); 
  }

  private static double sind(double d) { 
    return Math.sin(d*Math.PI/180.0); 
  }

  private static double tand(double d) {
    return Math.tan(d*Math.PI/180.0);
  }

  private static double acosd(double x) {
    return Math.acos(x)*180.0/Math.PI;
  }
  
  private static double asind(double x) {
    return Math.asin(x)*180.0/Math.PI;
  }

  private static double atand(double x) { 
    return Math.atan(x)*180.0/Math.PI; 
  }

  private static double atan2d(double x,double y) {
     return Math.atan2(x,y)*180.0/Math.PI; 
  }

  private static double slantRange(int lagfr,int smsep,
		   double rxris,double range_edge,
		   int range_gate) {
    return (lagfr-rxris+(range_gate-1)*smsep+range_edge)*0.15;
  }

  private static void fldpnt(double rrho,double rlat,double rlon,double ral,
		      double rel,double r) {
  
     double rx,ry,rz,sx,sy,sz,tx,ty,tz;
     double sinteta;
  
     /* convert from global spherical to global cartesian*/

     sinteta=sind(90.0-rlat);
     rx=rrho*sinteta*cosd(rlon);
     ry=rrho*sinteta*sind(rlon);
     rz=rrho*cosd(90.0-rlat);

     sx=-r*cosd(rel)*cosd(ral);
     sy=r*cosd(rel)*sind(ral);
     sz=r*sind(rel);

     tx  =  cosd(90.0-rlat)*sx + sind(90.0-rlat)*sz;
     ty  =  sy;
     tz  = -sind(90.0-rlat)*sx + cosd(90.0-rlat)*sz;
     sx  =  cosd(rlon)*tx - sind(rlon)*ty;
     sy  =  sind(rlon)*tx + cosd(rlon)*ty;
     sz  =  tz;

     tx=rx+sx;
     ty=ry+sy;
     tz=rz+sz;

     /* convert from cartesian back to global spherical*/
     gpos.rho=Math.sqrt((tx*tx)+(ty*ty)+(tz*tz));
     gpos.lat=90.0-acosd(tz/(gpos.rho));
     if ((tx==0) && (ty==0)) gpos.lon=0;
     else gpos.lon=atan2d(ty,tx);
  }

  private static double geocnvrt(double gdlat,double gdlon,
			  double xal,double xel) {

    double kxg,kyg,kzg,kxr,kyr,kzr;
    double rrad,rlat,rlon,del;
    double a=6378.16;
    double b,e2;

    b=a*(1.0-1.0/298.25);

    kxg=cosd(xel)*sind(xal);
    kyg=cosd(xel)*cosd(xal);
    kzg=sind(xel);

    rlat=atand( (b*b)/(a*a)*tand(gdlat));
    del=gdlat-rlat;

    kxr=kxg;
    kyr=kyg*cosd(del)+kzg*sind(del);
    return atan2d(kxr,kyr);
  }

  private static void fldPnth(double gdlat,double gdlon,
               double psi,double bore,double fh,
               double r) {

    double rrad,rlat,rlon;
    double tan_azi,azi,rel,xel,fhx,xal,rrho,ral,xh;
    double dum;
    double frad;  
    double a=6378.16;
    double b,e2;

    b=a*(1.0-1.0/298.25);
    e2=(a*a)/(b*b)-1;
 
    if (fh<=150) xh=fh;
    else {
      if (r<=600) xh=115;
      else if ((r>600) && (r<800)) xh=(r-600)/200*(fh-115)+115;
      else xh=fh;
    }

    if (r<150) xh=(r/150.0)*115.0;

    rlat=atand( (b*b) / (a*a) *tand(gdlat));
    rlon=gdlon;
    if (rlon>180) rlon=rlon-360;
    rrad=a/Math.sqrt(1.0+e2*sind(rlat)*sind(rlat));
    rrho=rrad;
    frad=rrad;
 

    do {
      gpos.rho=frad+xh;
  
      rel=asind( ((gpos.rho*gpos.rho) - (rrad*rrad) - (r*r)) / (2*rrad*r));
      xel=rel;
      if (((cosd(psi)*cosd(psi))-(sind(xel)*sind(xel)))<0) tan_azi=1e32;
      else tan_azi=Math.sqrt( (sind(psi)*sind(psi))/
                ((cosd(psi)*cosd(psi))-(sind(xel)*sind(xel))));
      if (psi>0) azi=atand(tan_azi)*1.0;
      else azi=atand(tan_azi)*-1.0;
      xal=azi+bore;
      ral=geocnvrt(gdlat,gdlon,xal,xel);

      fldpnt(rrho,rlat,rlon,ral,rel,r);

      frad=a/Math.sqrt(1.0+e2*sind(gpos.lat)*sind(gpos.lat));

      fhx=gpos.rho-frad; 
    } while(Math.abs(fhx-xh) > 0.5);
  } 

  public static MapPoint geo(int center,RadarPos pos,int frang,
			  int rsep,int rxrise,double height,
			  int mbeam,int bcrd,int rcrd) {
    double psi,d,rx;
    double re=6356.779;
    int lagfr=20*frang/3;
    int smsep=20*rsep/3;
    double offset=7.5;
    double bm_edge=0;
    double range_edge=0;

    if (pos==null) return null;
    if ((frang==0) || (rsep==0)) return null; 
    if (center==0) {
      bm_edge=-pos.bmwidth*0.5;
      range_edge=-0.5*smsep;
    }

    if (rxrise==0) rx=pos.rxris;
    else rx=rxrise;
    offset=mbeam/2.0-0.5;
    psi=pos.bmwidth*(bcrd-offset)+bm_edge;
    d=slantRange(lagfr,smsep,rx,range_edge,rcrd+1);
    
    

    if (height < 90) height=
      (int) (-re+Math.sqrt((re*re)+2*d*re*sind(height)+(d*d)));
    fldPnth(pos.gdlat,pos.gdlon,psi,pos.boresite,height,d);
    return new MapPoint(gpos.lat,gpos.lon); 
  }
}
 
