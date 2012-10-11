/* Time.java
   =========
   Author R.J.Barnes
*/

/*
 $License$
*/




public class Time {

  int DAY_SEC=(24*3600); 
  int YEAR_SEC=(365*24*3600);
  int LYEAR_SEC=(366*24*3600);

  int year;
  int month;
  int day;
  int hour;
  int minute;
  double second;

  public void setYrSec(int year,int time) {
    int yd;
    int dt;
    int n;
    int nday[]={0,31,59,90,120,151,181,212,243,273,304,334};
    int lday[]={0,31,60,91,121,152,182,213,244,274,305,335};
    int jday[];

    if ((year % 4)==0) jday=lday;
      else jday=nday;

    yd=time/(24*60*60);
    for (n=0;(n<12) && (yd>=jday[n]);n++);
   
    this.year=year; 
    this.month=n;
    
    if (n>0) this.day=1+yd-jday[n-1];
    else this.day=yd+1;
     
    dt=time % (24*60*60);
    this.hour=dt/(60*60);
    this.minute=(dt % (60*60))/60;
    this.second=(double) (dt % 60);
  }

  public int getYrSec() {

    int t;
    int jday[] = {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334};
    int mday[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    if ( (this.month < 1) || (this.month > 12) || 
         (this.hour < 0) ||  (this.hour > 23) || 
         (this.day <1) || (this.minute < 0) || 
         (this.minute > 59) || (this.second < 0) || 
         (this.second >= 60) )
	   return -1;

    if (this.day > mday[this.month-1])
      if (this.month != 2) return -1;
      else if (day != (mday[1] +1) || ((this.year % 4) != 0)) return -1;

    t = jday[this.month-1] + this.day - 1;

    if ( (this.month > 2) && ((this.year % 4) == 0)) t = t+1;

    return (((t*24 + this.hour)*60 + this.minute)*60)+(int) this.second;
  }


  double getEpoch() {
    double tme;
    int lpyear,ryear;
    int yrsec;
    if (this.year<1970) return -1;
    yrsec=getYrSec();
    if (yrsec==-1) return -1;
    tme=yrsec+(this.second-(int) this.second);
    
    /* work out number of seconds from 1970 to start of year */

    lpyear=(this.year-1969)/4;
    ryear=(this.year-1970)-lpyear;

    tme+=(lpyear*LYEAR_SEC)+(ryear*YEAR_SEC); 
    return tme;
  }


  void setEpoch(double tme) {
    int yrsec=0;
    int sec;
    int i=0;
    int yr;

    /* find the year */

    while ((yrsec<=tme) && (i<1e6)) {
      if ((i % 4)==2) yrsec+=LYEAR_SEC;
      else yrsec+=YEAR_SEC;
      i++;
    }
    if (((i-1) % 4)==2) tme-=yrsec-LYEAR_SEC;
    else tme-=yrsec-YEAR_SEC;
    yr=i+1969;
    setYrSec(yr,(int) tme);

    this.second=this.second+(tme-(int) tme);
  }
 
}


   





