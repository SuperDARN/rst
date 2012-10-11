/* TimeValue.java
   ============== 
   By R.J.Barnes */


/*
 $License$
*/





import java.util.*;
import java.lang.*;

public class TimeValue {
    static final int YEAR_SEC=(365*24*3600);
    static final int LYEAR_SEC=(366*24*3600);
    static final int DAY_SEC=(24*3600);

    public int year;
    public int month;
    public int day;
    public int hour;
    public int minute;
    public double second;

    public TimeValue(int year,int month,int day,
                      int hour,int minute,double second) {

	this.year=year;
        this.month=month;
        this.day=day;
        this.hour=hour;
        this.minute=minute;
        this.second=second;
    }

    public void setTime(int year,int month,int day,
                      int hour,int minute,double second) {

        this.year=year;
        this.month=month;
        this.day=day;
        this.hour=hour;
        this.minute=minute;
        this.second=second;
    }


    public void setTime(int hour,int minute,double second) {
        this.hour=hour;
        this.minute=minute;
        this.second=second;
    }

    public void setDate(int year,int month,int day) {
        this.year=year;
        this.month=month;
        this.day=day; 
    }



    public void setYrSec(int year,int time) {

       int yd;
       int dt; 
       int n;
       int jday[];

       int nday[] = {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334};
       int lday[] = {0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335};

       this.year=year;

       if ((year % 4)==0) jday=lday; 
       else jday=nday;  

       yd=time/(24*60*60); /* get the day of the year */

       for (n=0;(n<12) && (yd>=jday[n]);n++);
 
       month=n; /* got the month */
       if (n>0) day=1+yd-jday[n-1]; /* got the date */
       else day=yd+1; 

       dt=time % (24*60*60);
       hour=dt/(60*60);
       minute=(dt % (60*60))/60;
       second=dt % 60;     

    }

    int getYrSec() {
      int t;
      int jday[] = {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334};
      int mday[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};


     if (day > mday[month-1])
	if (month != 2) return -1;
	else if (day != (mday[1] +1) || ((year % 4) != 0)) return -1;

      t = jday[month-1] + day - 1;

      if ( (month > 2) && ((year % 4) == 0)) t = t+1;
      return (((t*24 + hour)*60 + minute)*60)+(int) second;
    }


    public void setEpoch(double epoch) {

      int yrsec=0;
      int sec;
      int i=0;

      /* find the year */

      while ((yrsec<=epoch) && (i<1e6)) {
        if ((i % 4)==2) yrsec+=LYEAR_SEC;
        else yrsec+=YEAR_SEC;
        i++;
      }
      if (((i-1) % 4)==2) epoch-=yrsec-LYEAR_SEC;
      else epoch-=yrsec-YEAR_SEC;
      year=i+1969;
      this.setYrSec(year,(int) epoch);
      second=second+(epoch-(int) epoch);
    }

    double getEpoch() {
      double tme;
       int lpyear,ryear;
       int yrsec;
       if (year<1970) return -1;
       yrsec=getYrSec();
       if (yrsec==-1) return -1;
       tme=yrsec+(second-(int) second);
    
       /* work out number of seconds from 1970 to start of year */

      lpyear=(year-1969)/4;
      ryear=(year-1970)-lpyear;

      tme+=(lpyear*LYEAR_SEC)+(ryear*YEAR_SEC); 
      return tme;
    }

    int getYear() {
	return this.year;
    }

    int getMonth() {
	return this.month;
    }

    int getDay() {
	return this.day;
    }

    int getHour() {
	return this.hour;
    }

    int getMinute() {
	return this.minute;
    }

    double getSecond() {
	return this.second;
    }

}





