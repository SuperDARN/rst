/* radar.h
   =======
   Author: R.J.Barnes
*/

/* 
   See license.txt
*/




#ifndef _RADAR_H
#define _RADAR_H

struct RadarSite { 
  double tval;
  double geolat,geolon,alt;
  double boresite,bmsep;
  double vdir;
  double atten;
  double tdiff;
  double phidiff;
  double interfer[3];
  double recrise;
  int maxatten;
  int maxrange;
  int maxbeam;
};

struct Radar {
  int id;
  int status;
  int cnum;
  char **code;
  char *name;
  char *operator;
  char *hdwfname;
  double st_time;
  double ed_time;
  int snum;
  struct RadarSite *site;
};

struct RadarNetwork {
  int rnum;
  struct Radar *radar;
};

struct RadarSite *RadarEpochGetSite(struct Radar *ptr,double tval);
struct RadarSite *RadarYMDHMSGetSite(struct Radar *ptr,int yr,
				     int mo,int dy,int hr,int mt,
                                     int sc);


struct Radar *RadarGetRadar(struct RadarNetwork *ptr,int stid);
int RadarGetID(struct RadarNetwork *ptr,char *code);
int RadarGetCodeNum(struct RadarNetwork *ptr,int stid);
char *RadarGetCode(struct RadarNetwork *ptr,int stid,int cnum);
char *RadarGetName(struct RadarNetwork *ptr,int stid);
char *RadarGetOperator(struct RadarNetwork *ptr,int stid);
int RadarGetStatus(struct RadarNetwork *ptr,int stid);


void RadarFree(struct RadarNetwork *ptr);
struct RadarNetwork *RadarLoad(FILE *fp); 
int RadarLoadHardware(char *hdwpath,struct RadarNetwork *ptr); 
struct RadarSite *RadarGetSite(struct RadarNetwork *ptr,
                               int yr,int mo,int dy,int hr,int mt,int sc,
                               int stid);

#endif
