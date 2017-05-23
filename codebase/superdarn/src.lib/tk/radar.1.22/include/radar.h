/* radar.h
   =======
   Author: R.J.Barnes
*/

/* 
 LICENSE AND DISCLAIMER
 
 Copyright (c) 2012 The Johns Hopkins University/Applied Physics Laboratory
 
 This file is part of the Radar Software Toolkit (RST).
 
 RST is free software: you can redistribute it and/or modify
 it under the terms of the GNU Lesser General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 any later version.
 
 RST is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU Lesser General Public License for more details.
 
 You should have received a copy of the GNU Lesser General Public License
 along with RST.  If not, see <http://www.gnu.org/licenses/>.
 
 
 
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
