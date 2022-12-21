/* radar.h
   =======
   Author: R.J.Barnes

 Copyright (c) 2012 The Johns Hopkins University/Applied Physics Laboratory


This file is part of the Radar Software Toolkit (RST).

RST is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see <https://www.gnu.org/licenses/>.

Modifications:
  E.G.Thomas 2021-08: added support for new hdw file fields
  E.G.Thomas 2022-03: added support for tdiff calibration files
*/ 




#ifndef _RADAR_H
#define _RADAR_H

struct RadarSite {
  int status;
  double tval;
  double geolat,geolon,alt;
  double boresite,bmoff,bmsep;
  double vdir;
  double tdiff[2];
  double phidiff;
  double interfer[3];
  double recrise;
  double atten;
  int maxatten;
  int maxrange;
  int maxbeam;
};

struct RadarTdiff {
  int method;
  int channel;
  double freq[2];
  double tval[2];
  double tdiff;
  double tdiff_err;
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
  int tnum;
  struct RadarTdiff *tdiff;
};

struct RadarNetwork {
  int rnum;
  struct Radar *radar;
};

struct RadarSite *RadarEpochGetSite(struct Radar *ptr,double tval);
struct RadarSite *RadarYMDHMSGetSite(struct Radar *ptr,int yr,
                                     int mo,int dy,int hr,int mt,int sc);
struct RadarTdiff *RadarEpochGetTdiff(struct Radar *ptr,double tval,
                                      int method,int channel,int tfreq);
struct RadarTdiff *RadarYMDHMSGetTdiff(struct Radar *ptr,int yr,
                                       int mo,int dy,int hr,int mt,int sc,
                                       int method,int channel,int tfreq);


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
int RadarLoadTdiff(char *tdiffpath,struct RadarNetwork *ptr);
struct RadarSite *RadarGetSite(struct RadarNetwork *ptr,
                               int yr,int mo,int dy,int hr,int mt,int sc,
                               int stid);

#endif
